#include "common.h"

int main(){
    int choice;
    do{
        printf("\n****SUPERMARKET MANGEMENT SYSTEM****\n");
        printf("1. Add Product\n");
        printf("2. Display All Products\n");
        printf("3. Search Product\n");
        printf("4. Update Product Price\n");
        printf("5. Delete Product\n");
        printf("6. Billing\n");
        printf("7. Alert Low Stock\n");
        printf("8. Expiry Mangement\n");
        printf("9. Customer Mangement\n");
        printf("10. Sales Report\n");
        printf("0. Exit\n");
        printf("Select One:\n");
        scanf("%d", &choice);
        switch (choice) {
            case 1: addproduct(); break;
            case 2: displayproduct(); break;
            case 3: searchproduct(); break;
            case 4: updateprice(); break;
            case 5: deleteproduct(); break;
            //case 6: billing(); break;
            case 7: lowstockalert(); break;
            case 8: expirymanagement(); break;
            case 9: registercustomer(); break;
            //case 10: salesreport(); break;
            case 0: printf("ThankYou Goodbye!"); break;
            default: printf("Invalid Choice!\n");
            printf("Please Select again:\n");
            scanf("%d", &choice);
        }
}while(choice!=0);
return 0;
}
