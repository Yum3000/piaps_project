#include <stdio.h>
#include <sqlite3.h> 
#include <string.h>
#include "entities.h"

# define maxCountTailors 2

sqlite3 *initDB(sqlite3 *db) {
   int rc;

   rc = sqlite3_open("atelier.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
   return db;
}

void addNewClerk(sqlite3 *db, int id, char *firstName, char *lastName){
  sqlite3_stmt *res;  // компилируемое выражение

    // выполняемый код SQL
    char *sql = "INSERT INTO clerks (id, firstName, lastName) VALUES (?, ?, ?)";
     
    // готовим выражение
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
    if (rc == SQLITE_OK) 
    {
         
        // привязываем параметры
        sqlite3_bind_int(res, 1, id);
        sqlite3_bind_text(res, 2, firstName, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 3, lastName, -1, SQLITE_STATIC);

        // выполняем выражение
        int step = sqlite3_step(res);
        // если выражение успешно выполнено
        if (step == SQLITE_DONE) 
        {
            printf("new clerk inserted\n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }
    // удаляем скомпилированное выражение
    sqlite3_finalize(res);

}

int createOrder(sqlite3 *db, char *typeClothes, char *comments, int cost, int idService, int idClient, int idTailor, int idStorageUnit, int unclaimedRequested){
    sqlite3_stmt *orderStmt;
    int currOrderId = -1;

    char *orderStmtSQL = "INSERT INTO orders (typeClothes, comments, cost, idService, idClient, idTailor, idStorageUnit, unclaimedRequested) VALUES (?, ?, ?, ?, ?, ?, ?, ?) returning id";
    int rc = sqlite3_prepare_v2(db, orderStmtSQL, -1, &orderStmt, 0);
     
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(orderStmt, 1, typeClothes, -1, SQLITE_STATIC);
        sqlite3_bind_text(orderStmt, 2, comments, -1, SQLITE_STATIC);
        sqlite3_bind_int(orderStmt, 3, cost);
        sqlite3_bind_int(orderStmt, 4, idService);
        sqlite3_bind_int(orderStmt, 5, idClient);
        sqlite3_bind_int(orderStmt, 6, idTailor);
        sqlite3_bind_int(orderStmt, 7, idStorageUnit);
        sqlite3_bind_int(orderStmt, 8, unclaimedRequested);

        
      int step = sqlite3_step(orderStmt);

      if (step == SQLITE_ROW) {
        currOrderId = sqlite3_column_int(orderStmt, 0);
      }

      //if (step != SQLITE_DONE) {
      //    return -1;
      //}
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(orderStmt);

    return currOrderId;
}

int getFreeTailorID(sqlite3 *db) {
  int tailorID = -1;

  sqlite3_stmt *statement;
  char *sql = "SELECT id FROM tailors t WHERE NOT EXISTS (SELECT id FROM orders WHERE idTailor = t.id) LIMIT 1";
  int rc = sqlite3_prepare_v2(db, sql, -1, &statement, 0);

  if (rc == SQLITE_OK) {
    if (sqlite3_step(statement) == SQLITE_ROW) {
      tailorID = sqlite3_column_int(statement, 0);
    }
  }

  sqlite3_finalize(statement);

  return tailorID;
}

int getFreeStorageUnitID(sqlite3 *db) {
  int storageID = -1;

  sqlite3_stmt *statement;
  char *sql = "SELECT id FROM storageUnits as t WHERE NOT EXISTS (SELECT id FROM orders WHERE idStorageUnit = t.id) LIMIT 1";
  int rc = sqlite3_prepare_v2(db, sql, -1, &statement, 0);

  if (rc == SQLITE_OK) {
    if (sqlite3_step(statement) == SQLITE_ROW) {
      storageID = sqlite3_column_int(statement, 0);
    }
  }

  sqlite3_finalize(statement);

  return storageID;
}

void getOrder(sqlite3 *db, int orderId){
    sqlite3_stmt *res;

    char *sql = "SELECT * FROM orders WHERE id = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
    if (rc == SQLITE_OK) 
    {
      sqlite3_bind_int(res, 1, orderId);
    }
    
    int step = sqlite3_step(res);

    if (step == SQLITE_ROW) {
        int orderId = sqlite3_column_int(res, 0);
        char *created = (char *)sqlite3_column_text(res, 1);
        char *typeClothes = (char *)sqlite3_column_text(res, 2);
        char *comments = (char *)sqlite3_column_text(res, 3);
        int cost = sqlite3_column_int(res, 4);
        printf("\nInformation about your order:\nid: %d, created: %s, type of clothes: %s, comments: %s, cost: %d\n", orderId, created, typeClothes, comments, cost);
       
    } 

    sqlite3_finalize(res);
}

int checkClientId(sqlite3 *db, char *mobileNumber) {
    sqlite3_stmt *res;
    
    char *sql = "SELECT id FROM clients WHERE mobileNumber = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
      sqlite3_bind_text(res, 1, mobileNumber, -1, SQLITE_STATIC);
    }
    
    int step = sqlite3_step(res);
    
    int idClient = -1;

    if (step == SQLITE_ROW) {
        idClient = sqlite3_column_int(res, 0);
        printf("We found you in atelier system, your id is: %d\n", idClient);
    } else {
      printf("There is no such customer in system\n");
    }

    sqlite3_finalize(res);
    return idClient;
}

int getHoursSinceOrderCreation(sqlite3 *db, int orderId) {
  sqlite3_stmt *res;
  char *sql = "SELECT ROUND((JULIANDAY(CURRENT_TIMESTAMP) - JULIANDAY(created)) * 24) FROM orders where id = ?";
  
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(res, 1, orderId);
  }
    
    int step = sqlite3_step(res);
    
    int timeDifference = -1;

    if (step == SQLITE_ROW) {
        timeDifference = sqlite3_column_int(res, 0);
        //printf("Difference between NOW and your order's created time: %d\n", timeDifference);
    } else {
      //printf("There is no such order in system\n");
    }

    sqlite3_finalize(res);
    return timeDifference;
}

enum orderStatus getOrderStatus(sqlite3 *db, int orderId) {
    int hoursInProgress = 1;
    int hoursCompleted = 24;
    // 6 месяцев
    int hoursUnclaimed = 24*30*6;

    int hoursSinceCreation = getHoursSinceOrderCreation(db, orderId);

    if (hoursSinceCreation > hoursUnclaimed) {
        return orderStatus_unclaimed;
    } else if (hoursSinceCreation > hoursCompleted) {
        return orderStatus_completed;
    } else if (hoursSinceCreation > hoursInProgress) {
        return orderStatus_inProcess;
    } else if (hoursSinceCreation == -1) {
	  return orderStatus_doesNotExist;	
	}

    return orderStatus_created;
}

void deleteOrder(sqlite3 *db, int orderId){
  sqlite3_stmt *res;
  char *sql = "DELETE FROM orders WHERE id = ?";
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

  if (rc == SQLITE_OK) {
      sqlite3_bind_int(res, 1, orderId);

      if (sqlite3_step(res) == SQLITE_DONE) {
          //printf("order is canceled!\n");
      } 
  } else {
      fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
  }

    sqlite3_finalize(res);
}

int createClient(sqlite3 *db, char *firstName, char *lastName, char *clientContacts, char *mobileNumber) {
  int clientID = -1;
  sqlite3_stmt *res;
  char *sql = "INSERT INTO clients (firstName, lastName, clientContacts, mobileNumber) VALUES (?, ?, ?, ?) returning id";
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
  if (rc == SQLITE_OK) {
      sqlite3_bind_text(res, 1, firstName, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 2, lastName, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 3, clientContacts, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 4, mobileNumber, -1, SQLITE_STATIC);

      int step = sqlite3_step(res);

      if (step == SQLITE_ROW) {
		  //printf("write id client!!\n");
        clientID = sqlite3_column_int(res, 0);
      }

      //if (step != SQLITE_DONE) {
		//  printf("not done !!!!\n");
          //return -1;
      //}
    } else {
      fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);

    return clientID;
}

int logInSystem(sqlite3 *db, char *login, char *password) {
	
	sqlite3_stmt *res;
    char *sql = "SELECT id FROM clerks WHERE login = ? AND password = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
      sqlite3_bind_text(res, 1, login, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 2, password, -1, SQLITE_STATIC);
    }
    
    int step = sqlite3_step(res);
    
    int idClerk = -1;

    if (step == SQLITE_ROW) {
        idClerk = sqlite3_column_int(res, 0);
        printf("We found you in atelier system, your id is: %d\n", idClerk);
    } else {
      printf("There is no such clerk in system\n");
    }

    sqlite3_finalize(res);
    return idClerk;
}

void updateOrdersStorageUnit(sqlite3 *db) {
	sqlite3_stmt *res;
	char *sql = "UPDATE orders SET idStorageUnit = NULL WHERE ROUND(JULIANDAY(CURRENT_TIMESTAMP) - JULIANDAY(created)) > 183";
	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	
	if (rc == SQLITE_OK) {
		
      if (sqlite3_step(res) == SQLITE_DONE) {
        //printf("free storage unit!!\n");
      } 
      
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
	
}

void setUnclaimedRequested(sqlite3 *db, int orderId) {
    sqlite3_stmt *res;
	char *sql = "UPDATE orders SET unclaimedRequested = 1 WHERE id = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(res, 1, orderId);

        if (sqlite3_step(res) == SQLITE_DONE) {
            printf("We will contact you later.\n");
        } 
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
}

void freeTheTailors(sqlite3 *db) {
  	sqlite3_stmt *res;
	char *sql = "UPDATE orders SET idTailor = NULL WHERE (JULIANDAY(CURRENT_TIMESTAMP) - JULIANDAY(created)) > 1";
	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	
	if (rc == SQLITE_OK) {
		
      if (sqlite3_step(res) == SQLITE_DONE) {
        //printf("free tailors!!\n");
      } 
      
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
}
