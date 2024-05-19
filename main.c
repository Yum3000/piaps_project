#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "dataBase.c"
#include "useCase.c"
#include "UI.c"

//#include<windows.h> // sleep() for Windows
#include<unistd.h> // sleep() for Linux

int main(){
  printf("%s\n", "Welcome to the atelier!");
  sqlite3 *db; 

  int rc  = sqlite3_open("atelier.db", &db);

  if (rc != SQLITE_OK) {
      sqlite3_close(db);
      return 1;
  }

  char *arrayMenu[5] = {"make a new order", "check order status", "cancel the order", "complete order", "exit"};
  printf("%s\n", "what do you need?");

  int answer;
  int orderId;

  while (1) {
  clear();
  answer = askVariant(arrayMenu, 5);
    switch(answer){
      case 1: clear();
              createNewOrder(db);
              askEnter();
              break;
      case 2: printf("Enter the number of your order: \n");
              scanf("%d", &orderId);
              checkOrderStatus(db, orderId);
              askEnter();
              break;
      case 3: printf("Enter the number of your order: \n");
              scanf("%d", &orderId);
              cancelOrder(db, orderId);
              askEnter();
              break;
      case 4: printf("Enter the number of order to complete: \n");
              scanf("%d", &orderId);
              completeOrder(db, orderId);
      case 5: return 0;
      default: printf("%s\n", "Error! Try again");
    }
  }

  printf("Don't forget to close the db!\n");

  sqlite3_close(db);

  return 0;
}
