#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h> 
#include "entities.h"
#include "UI.c"

int checkClient(sqlite3 *db, char *customerNumber) {
  printf("%s\n", "CHECK CLIENT");

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
    char *customerContacts = askString("Enter your contacts:\n");

    clientID = createClient(db, customerFirstName, customerLastName, customerContacts, customerNumber);

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

  if (idClient < 0) return;

  printf("%s\n", "creating a new order. please fill in next questions:");

  printf("what is the type of order:\n");
  char *arrayTypeOrder[2] = {"sewing", "repair"};
  int idService = askVariant(arrayTypeOrder, 2);

  char *clothes = askString("type what clothes it is:");

  char *comment = askString("type some comments:");

  int costOrder = askNumber("type the cost of the order:");

  createOrder(db, clothes, comment, idService, idClient, idTailor, idStorageUnit);
}

void checkTime(sqlite3 *db, int orderId) {
  printf("%s\n", "CHECK TIME");
  getHoursSinceOrderCreation(db, orderId);
}

void getOrderById(sqlite3 *db, int orderId) {
  printf("%s\n", "GET ORDER");
  getOrder(db, orderId);
}

void cancelOrder(sqlite3 *db, int orderId){
  printf("%s\n", "CANCLE ORDER");

  int passedTimeFromCreating = getHoursSinceOrderCreation(db, orderId);

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

void checkOrderStatus(sqlite3 *db, int orderId) {
  printf("%s\n", "CHECK ORDER");

  switch(getOrderStatus(db, orderId)) {
    case orderStatus_created:
      printf("your order is created\n");
      break;

    case orderStatus_inProcess:
      printf("Your order is not ready yet, please come later\n");
      break;

    case orderStatus_completed:
      char *yesNo[2] = {"yes", "no"};
      printf("Your order is ready now, do you want to get it?\n");

      switch (askVariant(yesNo, 2)) {
        case 1:
         getOrderById(db, orderId);
         break;
        case 2:
         printf("Ok, but you have only .. days to take it\n");
         break;
        default:
         printf("ERROR answer!\n");
      }
      break;

    case orderStatus_unclaimed:
      printf("you're an awful client! you forgot about your order and now it is in a long term storage!\n");
      printf("we will need from two to five days to retrieve it from there!\n");
      break;
  }
}

void completeOrder(sqlite3 *db, int orderId) {
	int passedTimeFromCreating = getHoursSinceOrderCreation(db, orderId);
	
	if (passedTimeFromCreating > 24) {
      updateOrderComplete(db, orderId);
    } else {
	  printf("It's impossible to change the status of order, it's still in progress\n");
	  return;	
	}
}
