#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h> 
#include "entities.h"
#include "UI.c"


void login(sqlite3 *db) {
	char *login = askString("Enter login:");
    char *password = askString("Enter password:");
    int idClerk = logInSystem(db, login, password);
    if (idClerk == -1) {
		exit(1);
	}
	free(login);
	free(password);
	return;
  
}

int checkClient(sqlite3 *db, char *customerNumber) {

  int clientID = checkClientId(db, customerNumber);

  if (clientID == -1) {
    printf("%s\n", "Sorry, we didn't find you in atelier system");
    printf("Do you want to register now?\n");

    char *arrayAnswers[2] = {"yes", "no"};
    int answer = askVariant(arrayAnswers, 2);

    if (answer == 2) {
      return -1;
    }

    char *customerFirstName = askString("Enter your first name:\n");
    char *customerLastName = askString("Enter your last name:\n");
    
    printf("Enter your contacts\n");
    
    char customerContacts[100];

    scanf(" %[^\n]s", customerContacts);

    clientID = createClient(db, customerFirstName, customerLastName, customerContacts, customerNumber);

    free(customerFirstName);
    free(customerLastName);

    if (clientID > 0) {
      printf("New client was registered\n");
    }
  }

  return clientID;
}

void createNewOrder(sqlite3 *db) {
  int idTailor = getFreeTailorID(db);

  if (idTailor < 0) {
    printf("You can't make an order - there is no free tailors, come back later\n");
    return;
  }

  int idStorageUnit = getFreeStorageUnitID(db);

  if (idStorageUnit < 0) {
    printf("You can't make an order - there is no free storage units, come back later\n");
    return;
  }

  printf("To make a new order enter mobile number: \n");

  char *customerNumber = askMobileNumber();

  int idClient = checkClient(db, customerNumber);
  
  if (idClient < 0) {
	  free(customerNumber);
	  return;
  }

  printf("%s\n", "Creating a new order. Please fill in next questions:");

  printf("what is the type of order:\n");
  char *arrayTypeOrder[2] = {"sewing", "repair"};
  int idService = askVariant(arrayTypeOrder, 2);

  char *clothes = askString("What clothes it is:");

  char *comment = askString("Add some comments:");

  int costOrder = askNumber("The cost of the order:");

  int currOrderId = createOrder(db, clothes, comment, costOrder, idService, idClient, idTailor, idStorageUnit, 0);
  printf("Order created successfully! Your order id: %d\n", currOrderId);
  free(customerNumber);
  free(clothes);
  free(comment);
}

void cancelOrder(sqlite3 *db, int orderId){

  int passedTimeFromCreating = getHoursSinceOrderCreation(db, orderId);
  
  char *yesNo[2] = {"yes", "no"};

  if (passedTimeFromCreating > 1) {
    printf("You can't cancel the order, it's turned more than 1 hour\n");
  } else if (passedTimeFromCreating < 1) {
    printf("You can cancel the order, it's turned less than 1 hour\n");
    printf("Do you really want to cancel the order?\n");
    
    switch (askVariant(yesNo, 2)) {
      case 1:
       deleteOrder(db, orderId);
       sleep(2);
       printf("The order was canceled.\n");
       break;
      case 2:
       break;
      default:
       printf("ERROR answer!\n");
    }

  } else {
    printf("Some error");
  }

  return;
}

void checkOrderStatus(sqlite3 *db, int orderId) {

  char *yesNo[2] = {"yes", "no"};

  switch(getOrderStatus(db, orderId)) {
    case orderStatus_created:
      printf("your order is created\n");
      break;

    case orderStatus_inProcess:
      printf("Your order is not ready yet, please come later\n");
      break;

    case orderStatus_completed:

      printf("Your order is ready now, do you want to get it?\n");

      switch (askVariant(yesNo, 2)) {
        case 1:
         getOrder(db, orderId);
         sleep(2);
         printf("Thank you for the order!\n");
         deleteOrder(db, orderId);
         break;
        case 2:
         printf("Ok, see you later.\n");
         break;
        default:
         printf("ERROR answer!\n");
      }
      break;

    case orderStatus_unclaimed:
      printf("You're an awful client! You forgot about your order and now it is in a long term storage!\n");
      printf("We will need from two to five days to retrieve it from there!\n");
      printf("Do you want to leave request?\n");
      
      switch (askVariant(yesNo, 2)) {
        case 1:
         leaveRequestUnclaimedOrder(db, orderId);
         break;
        case 2:
         break;
        default:
         printf("ERROR answer!\n");
      }
      break;
      
    case orderStatus_doesNotExist:
      printf("There is no such order in system\n");
      break;
      
  }
}

void checkStorageUnits(sqlite3 *db){
  updateOrdersStorageUnit(db);
}

void leaveRequestUnclaimedOrder(sqlite3 *db, int orderId) {
  	setUnclaimedRequested(db, orderId);
}

void setTailorsFree(sqlite3 *db){
  freeTheTailors(db);
}
