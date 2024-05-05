#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "dataBase.c"

//#include<windows.h> // sleep() for Windows
#include<unistd.h> // sleep() for Linux

enum orderStatus {processed, working, ready};


void clear(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}

char *askString(char *string) {
  printf("%s\n", string);
  
  char *answer = malloc(100);
  scanf("%99s", answer);

  return answer;
}

int askNumber(char *string){
  printf("%s\n", string);
  int *answerNumber = malloc(100);
  scanf("%99d", answerNumber);
  // исправить ограничение на string
  return answerNumber;
}

int askVariant(char **array, int countElements) {
  int choice = 0;
  for (int i = 0; i < countElements; i++) {
    printf("%d. %s\n", i+1, array[i]);
  }
  scanf("%d", &choice);

  return choice;
}

void askAnyKey() {
  char *anyKey = malloc(5);
  printf("To return to main menu press any key...\n");
  //gets(anyKey);
  scanf("%5s", anyKey);
  // очищать буфер??
  //getchar(); // wait for ENTER
  //getc(stdin);
  printf("ok...\n");
  return;
}

//orderStatus getNumStatus(char *status) {
  //if (status
//}

void createNewOrder(sqlite3 *db)
{
  //char *customerName = malloc(50);
  printf("To make a new order enter your first ans last name: \n");
  char customerFirstName[50];
  char customerLastName[50];
  scanf("%49s %49s", customerFirstName, customerLastName);	

  //scanf("%49s", customerName);
  printf("Your full name in createNewOrder: %s    %s\n", customerFirstName, customerLastName);
  checkClient(db, customerFirstName, customerLastName);
  int numOfFreeTailor = checkTailor(db);
  

  sleep(1);

  if (numOfFreeTailor > 0){
    printf("You can make an order\n");
    printf("Id of free tailor: %d\n", numOfFreeTailor);

  } else if (numOfFreeTailor == -1) {
    printf("You can't make an order - there is no free tailors, come back later\n");
    return;
  } else {
    printf("Error!\n");
    exit(1);
  }

	//if ()// check if there is a free space for new orders(if not)
	// аналогично портным
	//{
		//cout << "creating a new order is not possible, all the space is taken" << endl;
		//return;
	//}

  char *answer, *clothes, *comms, *currTime, *maxDate;
  int *costOrder;
  int idService;
  char *arrayTypeOrder[2] = {"sewing", "repair"};
  sleep(1);
  printf("%s\n", "creating a new order. please fill in next questions:");
  sleep(1);
  printf("what is the type of order:\n");

  idService = askVariant(arrayTypeOrder, 2);

  clothes = askString("type what clothes it is:");
  comms = askString("type some comments:");
  //currTime = askString("type current time:");
  maxDate = askString("type date of max storing the order:");
  costOrder = askNumber("type the cost of the order:");
  //printf("%d\n", costOrder);

  int idClient = 1;


  addNewOrder(db, "processed", clothes, comms, idService, idClient, numOfFreeTailor);
  

	// next we are transferring all the data into the data base
	// status/number: not ready/previous number+1

	//return;
}

void checkOrderStatus(sqlite3 *db, int orderId){

  char *arrayAnswers[2] = {"yes", "no"};
  int answer;
  printf("%s\n", "CHECK ORDER");
  enum orderStatus test = processed;
  printf("%d\n", test);
  char *currOrderStatus = checkOrder(db, orderId);
  if (strcmp(currOrderStatus, "processed") == 0) {
    printf("Your order is not ready yet, please come later\n");
    return;
  } else if (strcmp(currOrderStatus, "ready") == 0) {
    printf("Your order is ready now, do you want to get it?\n");
    answer = askVariant(arrayAnswers, 2);

    if (answer == 1) {
      getOrderById(db, orderId);
    } else if (answer == 2){
      printf("Ok, but you have only .. days to take it\n");
    } else {
      printf("ERROR answer!\n");
    }
    return;
  }

}

void checkClient(sqlite3 *db, char *firstName, char *lastName){
  printf("%s\n", "CHECK CLIENT");
  int answer;
  char *arrayAnswers[2] = {"yes", "no"};
  int currId = checkClientId(db, firstName, lastName);
  printf("!!! %d\n", currId);
  if (currId == -1) {
    printf("%s\n", "Sorry, we didn't find you in atelier system");
    printf("Do you want to register now?\n");
    answer = askVariant(arrayAnswers, 2);
    if (answer == 2) {
      return;
    } else if (answer == 1) {
      char *customerName = askString("Enter your first and last name\n");
      char *customerContacts = askString("Enter your contacts");
      addNewClient(db, customerName, customerContacts);
    }
  } else {
    printf("%s%d\n", "We found you in atelier system, your id is:", currId);
  }
  return;
}

void getOrderById(sqlite3 *db, int orderId){
  printf("%s\n", "GET ORDER");
  getOrder(db, orderId);
}

void cancelOrder(sqlite3 *db, int orderId){
  printf("%s\n", "DELETE ORDER");
  char *currOrderStatus;
  checkOrderStatus(db, orderId);
}

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