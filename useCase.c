#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h> 
#include "entities.h"
#include "UI.c"

void checkClient(sqlite3 *db, char *customerNumber){
  printf("%s\n", "CHECK CLIENT");
  int answer;
  char *arrayAnswers[2] = {"yes", "no"};
  int currId = checkClientId(db, customerNumber);
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

void createNewOrder(sqlite3 *db)
{
  printf("To make a new order enter your mobile number: \n");
  char *customerNumber = malloc(50);

  scanf("%49s %49s", customerNumber);	

  char *newNum = formatNumber(customerNumber);

  printf("YOUR NUMBER: %s\n", newNum);

  //checkClient(db, customerNumber);
  int numOfFreeTailor = getFreeTailor(db);
  

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

  char *clothes;
  char *comms;
  char *maxDate;
  int costOrder;
  int idService;
  char *arrayTypeOrder[2] = {"sewing", "repair"};
  sleep(1);
  printf("%s\n", "creating a new order. please fill in next questions:");
  sleep(1);
  printf("what is the type of order:\n");

  idService = askVariant(arrayTypeOrder, 2);

  clothes = askString("type what clothes it is:");
  comms = askString("type some comments:");
  maxDate = askString("type date of max storing the order:");
  costOrder = askNumber("type the cost of the order:");
  printf("%d %s\n", costOrder, maxDate);

  int idClient = 1;


  addNewOrder(db, orderStatus_created, clothes, comms, idService, idClient, numOfFreeTailor);
  

	// next we are transferring all the data into the data base
	// status/number: not ready/previous number+1

	//return;
}

void checkTime(sqlite3 *db, int orderId) {
  printf("%s\n", "CHECK TIME");
  checkTimeCreatedOrder(db, orderId);
}

void getOrderById(sqlite3 *db, int orderId){
  printf("%s\n", "GET ORDER");
  getOrder(db, orderId);
}

void cancelOrder(sqlite3 *db, int orderId){
  printf("%s\n", "CANCLE ORDER");

  int passedTimeFromCreating = checkTimeCreatedOrder(db, orderId);

  if (passedTimeFromCreating > 1) {
    printf("You can't cancel the order, it's turned more than 1 hour\n");
  } else if (passedTimeFromCreating < 1) {
    printf("You can cancel the order, it's turned less than 1 hour\nCANCEL HERE\n");
    deleteOrder(db, orderId);
  } else {
    printf("SOME ERROR");
  }

  return;
}

void checkOrderStatus(sqlite3 *db, int orderId){
  char *arrayAnswers[2] = {"yes", "no"};
  int answer;

  printf("%s\n", "CHECK ORDER");

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
