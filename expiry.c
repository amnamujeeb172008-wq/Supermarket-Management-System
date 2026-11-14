#include "common.h"
#include <time.h>

#define TEMP_FILE "temp.dat"

int daysLeft(Date d) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    t->tm_hour = 12; t->tm_min = 0; t->tm_sec = 0;
    time_t today = mktime(t);

    struct tm e = {0};
    e.tm_mday = d.day;
    e.tm_mon  = d.month - 1;
    e.tm_year = d.year - 1900;
    e.tm_hour = 12;
    e.tm_isdst = -1;
    time_t expiry = mktime(&e);

    if (today == (time_t)-1 || expiry == (time_t)-1) return -99999;

    double days = difftime(expiry, today) / 86400.0;
    return (int)(days >= 0 ? days + 0.5 : days - 0.5);  
}

int validDate(Date d) {
    static const int dim[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    time_t now = time(NULL);
    int currentYear = localtime(&now)->tm_year + 1900;

    if (d.year < currentYear || d.year > 9999) return 0;
    if (d.month < 1 || d.month > 12)           return 0;

    int maxDay = dim[d.month - 1];
    if (d.month == 2 && ((d.year % 4 == 0 && d.year % 100 != 0) || d.year % 400 == 0))
        maxDay = 29;
    return d.day >= 1 && d.day <= maxDay;
}

static void printHeader(const char *lastCol) {
    printf("%-5s %-20s %6s %9s %-11s %s\n", "ID", "Name", "Quantity", "Price", "Expiry", lastCol);
    printf("--------------------------------------------------------------------\n");
}

static int listNearExpiry(void) {
    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (!fp) { printf("Could not open %s.\n", INVENTORY_FILE); return 0; }

    Product p; int count = 0;
    printf("\n=== NEAR EXPIRY (within %d days) - available for clearance ===\n", NEAR_EXPIRY_DAYS);
    printHeader("Days left / clearance price");
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        int dl = daysLeft(p.expiry);
        if (dl >= 0 && dl <= NEAR_EXPIRY_DAYS) {
            printf("%-5d %-20.20s %6d %9.2f %02d/%02d/%-6d %d day(s)  NEAR EXPIRY -> Rs.%.2f (-30%%)\n",
                   p.id, p.name, p.quantity, p.price,
                   p.expiry.day, p.expiry.month, p.expiry.year,
                   dl, p.price * (1.0f - NEAR_EXPIRY_DISCOUNT));
            count++;
        }
    }
    fclose(fp);
    if (count == 0) printf("No near-expiry products.\n");
    return count;
}

static int listExpired(void) {
    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (!fp) { printf("Could not open %s.\n", INVENTORY_FILE); return 0; }

    Product p; int count = 0;
    printf("\n=== EXPIRED - locked from sale ===\n");
    printHeader("Status");
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        if (daysLeft(p.expiry) < 0) {
            printf("%-5d %-20.20s %6d %9.2f %02d/%02d/%-6d EXPIRED\n",
                   p.id, p.name, p.quantity, p.price,
                   p.expiry.day, p.expiry.month, p.expiry.year);
            count++;
        }
    }
    fclose(fp);
    if (count == 0) printf("No expired products.\n");
    return count;
}

static void moveExpired(void) {
    FILE *in = fopen(INVENTORY_FILE, "rb");
    if (!in) { printf("Could not open %s.\n", INVENTORY_FILE); return; }
    FILE *keep = fopen(TEMP_FILE, "wb");
    FILE *exp  = fopen(EXPIRED_FILE, "ab");
    if (!keep || !exp) {
        printf("Could not open working files.\n");
        if (keep) fclose(keep);
        if (exp)  fclose(exp);
        fclose(in);
        return;
    }

    Product p; int moved = 0;
    while (fread(&p, sizeof(Product), 1, in) == 1) {
        if (daysLeft(p.expiry) < 0) { fwrite(&p, sizeof(Product), 1, exp);  moved++; }
        else                        { fwrite(&p, sizeof(Product), 1, keep); }
    }
    fclose(in); fclose(keep); fclose(exp);

    if (moved > 0) {
        remove(INVENTORY_FILE);                 /* needed on Windows */
        if (rename(TEMP_FILE, INVENTORY_FILE) != 0)
            printf("Error: could not replace %s (data is safe in %s).\n", INVENTORY_FILE, TEMP_FILE);
        else
            printf("%d expired product(s) moved to %s.\n", moved, EXPIRED_FILE);
    } else {
        remove(TEMP_FILE);
        printf("Nothing to move.\n");
    }
}

static void viewExpiredArchive(void) {
    FILE *fp = fopen(EXPIRED_FILE, "rb");
    Product p; int count = 0;
    printf("\n=== ISOLATED STOCK (%s) ===\n", EXPIRED_FILE);
    if (!fp) { printf("No expired.dat yet.\n"); return; }
    printHeader("Status");
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        printf("%-5d %-20.20s %6d %9.2f %02d/%02d/%-6d ISOLATED\n",
               p.id, p.name, p.quantity, p.price,
               p.expiry.day, p.expiry.month, p.expiry.year);
        count++;
    }
    fclose(fp);
    if (count == 0) printf("Empty.\n");
}

void expirymanagement() {
    int ch;
    do {
        printf("\n--- EXPIRY MANAGEMENT ---\n");
        printf("1. List near-expiry items\n");
        printf("2. List expired items\n");
        printf("3. Move expired items to %s\n", EXPIRED_FILE);
        printf("4. View isolated (expired.dat) items\n");
        printf("0. Back\n");
        printf("Select One: ");

        int r = scanf("%d", &ch);
        if (r == EOF) return;
        if (r != 1) { int c; while ((c = getchar()) != '\n' && c != EOF); ch = -1; }

        switch (ch) {
            case 1: listNearExpiry(); break;
            case 2: listExpired();    break;
            case 3: {
                int n = listExpired();
                if (n > 0) {
                    char ans;
                    printf("\nMove these %d item(s) out of active stock? (y/n): ", n);
                    if (scanf(" %c", &ans) == 1 && (ans == 'y' || ans == 'Y')) moveExpired();
                    else printf("Cancelled.\n");
                }
                break;
            }
            case 4: viewExpiredArchive(); break;
            case 0: break;
            default: printf("Invalid Choice!\n");
        }
    } while (ch != 0);
}
