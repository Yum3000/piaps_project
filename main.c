#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "dataBase.c"
#include "useCase.c"
#include "UI.c"


//#include<windows.h> // sleep() for Windows
#include<unistd.h> // sleep() for Linux




//orderStatus getNumStatus(char *status) {
  //if (status
//}

int main(){

  printf("%s\n", "Welcome to the atelier!");
  sqlite3 *db; 
  //sqlite3_stmt *res;  // компилируемое выражение
  //int rc = initDB(db);

    int rc  = sqlite3_open("atelier.db", &db);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        return 1;
    }

    
  //addNewClerk(db, 1, "Mary", "Cooper");
  
  char *arrayMenu[5] = {"make a new order", "check order status", "get the ready order", "cancel the order", "exit"};
  printf("%s\n", "what do you need?");

  int answer, orderId;
  while (1) {
  //clear();
  answer = askVariant(arrayMenu, 5);
    switch(answer){
      case 1: clear();
              createNewOrder(db);
              askAnyKey();
              //return;
              break;
      case 2: printf("Enter the number of your order: \n");
              scanf("%d", &orderId);
              checkOrderStatus(db, orderId);
              askAnyKey();
              //return;
              break;
      case 3: printf("Enter the number of your order: \n");
              scanf("%d", &orderId);
              getOrderById(db, orderId);
              askAnyKey();
              //return;
              break;
      case 4: printf("Enter the number of your order: \n");
              scanf("%d", &orderId);
              cancelOrder(db, orderId);
              askAnyKey();
              //return;
              break;
      case 5: return 0;
      default: printf("%s\n", "Error! Try again");
    }
  }
  printf("Don't forget to close the db!\n");
  sqlite3_close(db);
  //clear();
  //free
  return 0;
}
