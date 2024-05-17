#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h> 
#include "entities.h"
#include "UI.c"

int checkClient(sqlite3 *db, char *customerNumber){
  printf("%s\n", "CHECK CLIENT");
  int answer;
  char *arrayAnswers[2] = {"yes", "no"};
  int currId = checkClientId(db, customerNumber);
  if (currId == -1) {
    printf("%s\n", "Sorry, we didn't find you in atelier system");
    printf("Do you want to register now?\n");
    answer = askVariant(arrayAnswers, 2);
    if (answer == 2) {
      return 1;
    } else if (answer == 1) {
      char *customerFirstName = askString("Enter your first name:\n");
      char *customerLastName = askString("Enter your last name:\n");
      char *customerContacts = askString("Enter your contacts:\n");
      addNewClient(db, customerFirstName, customerLastName, customerContacts, customerNumber);
    }
  }
  return 0;
}

void createNewOrder(sqlite3 *db)
{
  char number[30];
  char temp;
  
  printf("To make a new order enter your mobile number: \n");
	
  scanf("%c",&temp); // temp statement to clear buffer
  fgets(number,30,stdin);
  
  char *customerNumber = formatNumber(number);
	
  if (checkClient(db, customerNumber) == 1) return;
  
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
  } else if (strcmp(currOrderStatus, "completed") == 0) {
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

void passOrderToTailor(sqlite3 *db, int orderId){
	char *currOrderStatus = checkOrder(db, orderId);
  	if (strcmp(currOrderStatus, "created") == 0) {
		if (checkTimeCreatedOrder(db, orderId) == 0) {
			transferOrder(db, orderId);
		}
	} else {
	  printf("You can't pass order to tailor!\n");	
	}
	return;
}

void completeOrder(sqlite3 *db, int orderId){
	int passedTimeFromCreating = checkTimeCreatedOrder(db, orderId);
	
	if (passedTimeFromCreating > 24) {
      updateOrderComplete(db, orderId);
    } else {
	  printf("It's impossible to change the status of order, it's still in progress\n");
	  return;	
	}
}
