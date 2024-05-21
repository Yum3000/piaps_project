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
  
  checkStorageUnits(db);
  setTailorsFree(db);
  
  login(db);
  sleep(3); // заменить

  char *arrayMenu[4] = {"make a new order", "get the order", "cancel the order", "exit"};

  int answer;
  int orderId;
  

  while (1) {
  clear();
  answer = askVariant(arrayMenu, 4);
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
      case 4: printf("Goodbye!");
              sqlite3_close(db);
              return 0;
      default: printf("%s\n", "Error! Try again");
    }
  }



  return 0;
}
