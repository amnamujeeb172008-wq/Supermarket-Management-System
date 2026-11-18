#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOW_STOCK 10

#define NEAR_EXPIRY_DAYS      7      
#define NEAR_EXPIRY_DISCOUNT  0.30f  
#define INVENTORY_FILE "inventory.dat"
#define EXPIRED_FILE   "expired.dat"
#define CUSTOMER_FILE  "customers.dat"

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int id;
    char name[50];
    float price;
    int quantity;
    Date expiry;
} Product;

typedef struct {
    int id;
    char name[50];
    char phone[16];
    float totalSpent;
} Customer;

void addproduct();
void displayproduct();
void searchproduct();
void updateprice();
void deleteproduct();
void lowstockalert();
void billing();
void expirymanagement();
void registercustomer();
void salesreport();

int   daysLeft(Date d);                       
int   validDate(Date d);                      
int   findCustomer(int id, Customer *c);      
int   findCustomerByPhone(const char *phone, Customer *c);
float loyaltyDiscount(float totalSpent);      
int   updateCustomerSpent(int id, float amount);

#endif
