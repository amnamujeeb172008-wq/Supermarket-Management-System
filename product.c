#include "common.h"
int idexists(int id){
    FILE *fptr;
    fptr=fopen("inventory.dat","rb");
    Product p;
    if(fptr!=NULL){
        while(fread(&p,sizeof(Product),1,fptr)==1) {
        if (p.id==id){
            fclose(fptr);
            return 1;
        }
    }
    }
    else{
    return 0; }
}
void addproduct() {
    Product p;
    printf("***ADD PRODUCT***\n");
    printf("Enter Product ID:");
    scanf("%d", &p.id);
    if(idexists(p.id)){
    printf("Product With This ID Already Exists!\n");
    return ;
    }
    while(getchar()!='\n');
    printf("\nEnter Product Name: ");
    scanf("%[^\n]", p.name);
    printf("\nEnter Price Per Unit: ");
    scanf("%f", &p.price);
    printf("\nEnter Quantity In Stock: ");
    scanf("%d", &p.quantity);
    printf("\nEnter Expiry Date (Day,Month,Year): ");
    scanf("%d %d %d", &p.expiry.day,&p.expiry.month,&p.expiry.year);

    FILE *fptr;
    fptr=fopen("inventory.dat", "ab");
    if(fptr!=NULL){
        fwrite(&p,sizeof(Product),1,fptr);
        fclose(fptr);
    }
    else{
        printf("Error: File can not be opened!\n");
        return;
    }
    printf("Product Successfully Stored In Inventory File!\n");
}
void displayproduct() { 
    Product p;
    FILE *fptr;
    fptr=fopen("inventory.dat","rb");
    if(fptr!=NULL){
        printf("***PRODUCT INVENTORY***\n");
        printf("%-5s | %-20s | %-10s | %-8s | %-12s\n", "ID", "NAME", "PRICE", "QUANTITY", "EXPIRY DATE");
        printf("---------------------------------------------------------------------------------------\n");
        while(fread(&p,sizeof(Product),1,fptr)==1) {
        printf("%-5d | %-20s | %-10.2f | %-8d | %02d/%02d/%4d\n", p.id,p.name,p.price,p.quantity,p.expiry.day,p.expiry.month,p.expiry.year);
        }
        fclose(fptr);
    }
    else{
        printf("The Product Inventory Is Empty!");
        return;
    }
}
void searchproduct() { 
    Product p;
    int p_id;
    int found=0;
    printf("***SEARCH PRODUCT***\n");
    printf("Enter Product ID: ");
    scanf("%d",&p_id);
    FILE *fptr;
    fptr=fopen("inventory.dat","rb");
    if(fptr!=NULL){
        while(fread(&p,sizeof(Product),1,fptr)==1){
            if(p.id==p_id){
                found=1;
                printf("%-5s | %-20s | %-10s | %-8s | %-12s\n", "ID", "NAME", "PRICE", "QUANTITY", "EXPIRY DATE");
                printf("---------------------------------------------------------------------------------------\n");
                printf("%-5d | %-20s | %-10.2f | %-8d | %02d/%02d/%4d\n", p.id,p.name,p.price,p.quantity,p.expiry.day,p.expiry.month,p.expiry.year);
            }
        } fclose(fptr);
        if(found==0){
            printf("Product Not Found!\n");
        }
    }
    else{
        printf("The Product Inventory Is Empty!");
        return;
    }
}
void updateprice() {
   Product p;
   int p_id;
   int found=0;
   printf("***UPDATE PRICE***\n");
   printf("Enter Product ID: ");
   scanf("%d",&p_id);
   FILE *fptr;
   fptr=fopen("inventory.dat","rb+");
   if(fptr==NULL){
    printf("Error:File Does Not Exists\n");
    return;
   }
   while(fread(&p,sizeof(Product),1,fptr)==1){
    if(p_id==p.id){
        found=1;
         printf("Enter Updated Price: ");
        scanf("%f",&p.price);
        fseek(fptr,-(long)sizeof(Product),SEEK_CUR);
        fwrite(&p,sizeof(Product),1,fptr);
        printf("Price Updated Successfully!\n");
        break;
    }
   }
   fclose(fptr);
   if(found==0){
   printf("Product With This ID Not Found!\n"); }
}


void deleteproduct() {
    Product p;
    int p_id;
    int found=0;
    printf("***DELETE PRODUCT***\n");
    printf("Enter Product ID: ");
    scanf("%d",&p_id);
    FILE *fptr;
    fptr=fopen("inventory.dat","rb");
    if(fptr==NULL){
        printf("Error:File Does Not Exists\n");
        return;
    }
    FILE *fp;
    fp=fopen("temp.dat","wb");
    if(fp==NULL){
        printf("Error Creating Temporary File!\n");
        fclose(fptr);
        return;
    }
    while(fread(&p,sizeof(Product),1,fptr)==1){
        if(p.id==p_id){
            found=1;
        }
        else{
            fwrite(&p,sizeof(Product),1,fp);
        }
    }
    fclose(fptr);
    fclose(fp);
    if(found==0){
        printf("Product With This ID Not Found\n");
        remove("temp.dat");
    }
    else{
        if (remove("inventory.dat") != 0) {
            printf("Error: Could not remove old inventory file (file locked)!\n");
            remove("temp.dat"); 
            return;
        }
        if (rename("temp.dat", "inventory.dat") != 0) {
            printf("Error: Could not rename temporary file!\n");
            return;
        }
        printf("The Product With ID %d Deleted Successfully!\n", p_id); }
}
void lowstockalert() {
Product p;
FILE *fptr;
int found=0;
fptr=fopen("inventory.dat","rb");
if(fptr==NULL){
    printf("Error:File Does Not Exists\n");
    return;
}
printf("***LOW STOCK ALERT***\n");
printf("\n---Products Below Threshold---\n");
printf("%-5s | %-20s | %-10s | %-8s | %-12s\n", "ID", "NAME", "PRICE", "QUANTITY", "EXPIRY DATE");
printf("---------------------------------------------------------------------------------------\n");
while(fread(&p,sizeof(Product),1,fptr)==1){
    if(p.quantity<LOW_STOCK){
        found=1;
        printf("%-5d | %-20s | %-10.2f | %-8d | %02d/%02d/%4d\n", p.id,p.name,p.price,p.quantity,p.expiry.day,p.expiry.month,p.expiry.year);
    }
}
fclose(fptr);
if(found==0){
    printf("No products are currently below the threshold of %d.\n",LOW_STOCK);
}
}