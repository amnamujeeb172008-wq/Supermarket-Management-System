#include "common.h"
#include <stdio.h>
#include <string.h>

void billing(){
    int id, qty;
    int cartCount = 0;
    float subtotal = 0.0;
    int cartIDs[10];
    char cartNames[10][50];
    float cartPrices[10];
    int cartQtys[10];
    float cartTotals[10];
    char cartTags[10][20];

    printf("\n==== POS BILLING SYSTEM====\n");
    while(cartCount<10){
        printf("Enter the product ID (or 0 to finish): ");
        scanf("%d" , &id);
        if(id==0){
            break;
        }
        printf("Enter the Qunatity: ");
        scanf("%d" , &qty);
        if(qty<=0){
            printf("Error: Quantity must be greater than zero.\n");
            continue;
        }
        FILE*fp = fopen("inventory.dat", "rb");
        if(fp == NULL){
            printf("Error: Could not open inventory file.\n");
            break;
        }
        Product p;
        int found = 0;
        while(fread(&p, sizeof(Product),1,fp)==1){
            if(p.id == id){
                found = 1;
                break;
            }
        }
        fclose(fp);
        if(!found){
            printf("Error: product ID %d not found.\n", id);
            continue;
        }
        if(qty > p.quantity){
            printf("Error: Requested quantity exceeds the avialable stock (%d available).\n", p.quantity);
            continue;
        }
        int daysRemaining = daysLeft(p.expiry);
        if(daysRemaining<=0){
            printf("Error: product has been expired and cannot be sold!\n");
            continue;
        }
        float itemPrice = p.price;
        char tag[20] = "";
        if(daysRemaining>=1 && daysRemaining<=NEAR_EXPIRY_DAYS){
            itemPrice = p.price*(1.0 - NEAR_EXPIRY_DISCOUNT);
            strcpy(tag, " (Clearance)");
            printf("Near-expiry item: clearance discount applied.\n");
        }
        float lineTotal = itemPrice*qty;
        subtotal+= lineTotal;
        cartIDs[cartCount] = p.id;
        strcpy(cartNames[cartCount], p.name);
        strcat(cartNames[cartCount], tag);
        cartPrices[cartCount] = itemPrice*qty;
        cartQtys[cartCount] = qty;
        cartCount++;
        FILE *f_inv = fopen(INVENTORY_FILE,"rb+");
        if(f_inv!=NULL){
            while(fread(&p, sizeof(Product),1,f_inv)==1){
                if(p.id==id){
                    p.quantity -= qty;
                    fseek(f_inv, -((long)sizeof(Product)), SEEK_CUR);
                    fwrite(&p, sizeof(Product), 1, f_inv);
                    break;
                }
            }
            fclose(f_inv);
        }
        printf("items added successfully!\n");
    }
    if(cartCount==0){
        printf("No items Purchased. Billing cancelled.\n");
        return;
    }
    int customerID;
    float finalTotal = subtotal;
    float loyaltyDiscAmt = 0.0;
    Customer cust;
    int isMember = 0;
    
    printf("\nEnter customer ID (or 0 for guest):");
    scanf("%d" , &customerID);
    if(customerID != 0){
        if(findCustomer(customerID, &cust)){
            isMember = 1;
            float discountPercent = loyaltyDiscount(cust.totalSpent);
            loyaltyDiscAmt = subtotal*discountPercent;
            finalTotal-= loyaltyDiscAmt;
            printf("Customer found: %s. Loyalty Discount Applied: %.0f%%\n", cust.name, discountPercent);
        }else{
            printf("Customer ID not found. proceeding as non-member.\n");
        }
    }
    char promo[20];
    float promoDisc = 0.0;
    printf("Enter promotion code (or type NONE): ");
    scanf("%s", promo);
    if(strcmp(promo, "SAVE2026") == 0){
        promoDisc = 100.0;
        finalTotal-=promoDisc;
        if(finalTotal<0) finalTotal=0;
        printf("promo code 'SAVE2026' applied! Rs.100 off.\n");
    }
    printf("\n======== RECEIPT =========\n");
    for(int i=0; i<cartCount; i++){
        printf("%d. %s x%d - Total: Rs.%.2f\n",i+1, cartNames[i], cartQtys[i], cartPrices[i]);
    }
    printf("\n----------------------------\n");
    printf("subtotal          : Rs.%.2f\n", subtotal);
    if(isMember){
        printf("Loyalty Discount  : -Rs. %.2f\n", loyaltyDiscAmt);
    }
    if(promoDisc>0){
        printf("promo Discount  : -Rs. %.2f\n", promoDisc);
    }
    printf("Final Payable     : Rs. %.2f\n",finalTotal);
    printf("==============================\n");
    if(isMember){
        updateCustomerSpent(customerID, finalTotal);
    }
    FILE *f_sales = fopen("sales.txt", "a");
    if(f_sales != NULL){
        fprintf(f_sales, "Bill Total: Rs. %.2f, Items Sold: %d\n", finalTotal, cartCount);
        fclose(f_sales);
    }
}
void salesreport(){
    FILE *f_sales = fopen("sales.txt" , "r");
    if(f_sales == NULL){
        printf("\nNo sales records found yet!\n");
        return;
    }
    float totalRevenue = 0.0;
    int totalTransactions = 0;
    float billTotal;
    int itemsSold;

    printf("\n========== SALES REPORT ===========\n");
    while(fscanf(f_sales, "Bill Total: Rs. %f, Items Sold: %d", &billTotal, &itemsSold)== 2){
        totalRevenue += billTotal;
        totalTransactions++;
        printf("Transaction %d: Rs. %.2f (%d items)\n", totalTransactions,billTotal, itemsSold);
    }
    fclose(f_sales);
    printf("-------------------------------\n");
    printf("Total Transactisons : %d\n", totalTransactions);
    printf("Total Revenue       : Rs. %.2f\n", totalRevenue);
    printf("================================\n");
}
    