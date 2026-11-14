#include "common.h"
#include <ctype.h>

float loyaltyDiscount(float totalSpent) {
    if (totalSpent >= 50000) return 10.0;
    if (totalSpent >= 20000) return 5.0;
    if (totalSpent >= 5000)  return 2.0;
    return 0.0;
}

int findCustomer(int id, Customer *out) {
    FILE *fp = fopen(CUSTOMER_FILE, "rb");
    if (!fp) return 0;
    Customer c;
    while (fread(&c, sizeof(Customer), 1, fp) == 1) {
        if (c.id == id) { if (out) *out = c; fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

int findCustomerByPhone(const char *phone, Customer *out) {
    FILE *fp = fopen(CUSTOMER_FILE, "rb");
    if (!fp) return 0;
    Customer c;
    while (fread(&c, sizeof(Customer), 1, fp) == 1) {
        if (strcmp(c.phone, phone) == 0) { if (out) *out = c; fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

int updateCustomerSpent(int id, float amount) {
    if (amount < 0) return 0;
    FILE *fp = fopen(CUSTOMER_FILE, "rb+");
    if (!fp) return 0;

    Customer c;
    while (fread(&c, sizeof(Customer), 1, fp) == 1) {
        if (c.id == id) {
            c.totalSpent += amount;
            fseek(fp, -(long)sizeof(Customer), SEEK_CUR);   
            int ok = fwrite(&c, sizeof(Customer), 1, fp) == 1;
            fclose(fp);
            return ok;
        }
    }
    fclose(fp);
    return 0;
}

static void addCustomer(void) {
    Customer c;
    memset(&c, 0, sizeof(c));

    printf("Customer ID (positive number): ");
    if (scanf("%d", &c.id) != 1 || c.id <= 0) {
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Invalid ID.\n");
        return;
    }
    if (findCustomer(c.id, NULL)) { printf("A customer with ID %d already exists.\n", c.id); return; }

    printf("Name: ");
    scanf(" %49[^\n]", c.name);

    printf("Phone (10-15 digits): ");
    scanf(" %15s", c.phone);
    int len = (int)strlen(c.phone);
    if (len < 10 || len > 15) { printf("Invalid phone number.\n"); return; }
    for (int i = 0; i < len; i++)
        if (!isdigit((unsigned char)c.phone[i])) { printf("Phone must contain digits only.\n"); return; }
    if (findCustomerByPhone(c.phone, NULL)) { printf("That phone number is already registered.\n"); return; }

    c.totalSpent = 0;
    FILE *fp = fopen(CUSTOMER_FILE, "ab");
    if (!fp) { printf("Could not open %s.\n", CUSTOMER_FILE); return; }
    fwrite(&c, sizeof(Customer), 1, fp);
    fclose(fp);
    printf("Customer '%s' registered (ID %d).\n", c.name, c.id);
}

static void lookupCustomer(void) {
    int mode;
    Customer c;
    int found = 0;
    printf("Look up by 1) ID  2) Phone : ");
    if (scanf("%d", &mode) != 1) { int ch; while ((ch = getchar()) != '\n' && ch != EOF); return; }

    if (mode == 1) {
        int id; printf("ID: ");
        if (scanf("%d", &id) == 1) found = findCustomer(id, &c);
    } else if (mode == 2) {
        char ph[16]; printf("Phone: ");
        if (scanf(" %15s", ph) == 1) found = findCustomerByPhone(ph, &c);
    } else { printf("Invalid choice.\n"); return; }

    if (!found) { printf("Customer not found.\n"); return; }
    printf("ID: %d | Name: %s | Phone: %s | Total spent: Rs.%.2f | Loyalty discount: %.0f%%\n",
           c.id, c.name, c.phone, c.totalSpent, loyaltyDiscount(c.totalSpent));
}

static void listCustomers(void) {
    FILE *fp = fopen(CUSTOMER_FILE, "rb");
    Customer c; int count = 0;
    if (!fp) { printf("No customers registered yet.\n"); return; }
    printf("\n%-6s %-22s %-16s %12s %9s\n", "ID", "Name", "Phone", "Total Spent", "Discount");
    printf("--------------------------------------------------------------------\n");
    while (fread(&c, sizeof(Customer), 1, fp) == 1) {
        printf("%-6d %-22.22s %-16s %12.2f %8.0f%%\n",
               c.id, c.name, c.phone, c.totalSpent, loyaltyDiscount(c.totalSpent));
        count++;
    }
    fclose(fp);
    if (count == 0) printf("No customers registered yet.\n");
}

void registercustomer() {
    int ch;
    do {
        printf("\n--- CUSTOMER MANAGEMENT ---\n");
        printf("1. Register new customer\n");
        printf("2. Look up customer\n");
        printf("3. List all customers\n");
        printf("0. Back\n");
        printf("Select One: ");

        int r = scanf("%d", &ch);
        if (r == EOF) return;
        if (r != 1) { int c; while ((c = getchar()) != '\n' && c != EOF); ch = -1; }

        switch (ch) {
            case 1: addCustomer();    break;
            case 2: lookupCustomer(); break;
            case 3: listCustomers();  break;
            case 0: break;
            default: printf("Invalid Choice!\n");
        }
    } while (ch != 0);
}
