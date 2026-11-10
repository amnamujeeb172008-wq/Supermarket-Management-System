#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOW_STOCK 10

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

#endif