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

void createOrder(sqlite3 *db, char *typeClothes, char *comments, int idService, int idClient, int idTailor, int idStorageUnit){
    sqlite3_stmt *orderStmt;

    char *orderStmtSQL = "INSERT INTO orders (typeClothes, comments, idService, idClient, idTailor, idStorageUnit) VALUES (?, ?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, orderStmtSQL, -1, &orderStmt, 0);
     
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(orderStmt, 1, typeClothes, -1, SQLITE_STATIC);
        sqlite3_bind_text(orderStmt, 2, comments, -1, SQLITE_STATIC);
        sqlite3_bind_int(orderStmt, 3, idService);
        sqlite3_bind_int(orderStmt, 4, idClient);
        sqlite3_bind_int(orderStmt, 5, idTailor);
        sqlite3_bind_int(orderStmt, 6, idStorageUnit);

        if (sqlite3_step(orderStmt) == SQLITE_DONE) {
            printf("new order inserted\n");
        }
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(orderStmt);


    // Обновляем статус портного
    //sqlite3_stmt *updateTailor;
    //char *updateTailorSQL = "UPDATE tailors SET status='busy' WHERE id = ?";
    //rc = sqlite3_prepare_v2(db, updateTailorSQL, -1, &updateTailor, 0);

    //if (rc == SQLITE_OK) {
        //sqlite3_bind_int(updateTailor, 1, idTailor);

        //if (sqlite3_step(updateTailor) == SQLITE_DONE) {
            //printf("tailor's going to work on your order - BUSY\n");
        //} 
    //} else {
        //fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    //}

    //sqlite3_finalize(updateTailor);

    printf("order created successfully!\n");
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
        char *status = (char *)sqlite3_column_text(res, 1);
        char *orderDate = (char *)sqlite3_column_text(res, 2);
        char *typeClothes = (char *)sqlite3_column_text(res, 3);
        char *comments = (char *)sqlite3_column_text(res, 4);
        int idService = sqlite3_column_int(res, 5);
        int idClient = sqlite3_column_int(res, 6);
        int idTailor = sqlite3_column_int(res, 7);
        printf("Information about your order:\n%d,%s,%s,%s,%s,%d,%d,%d\n", orderId, status, orderDate, typeClothes, comments, idService, idClient, idTailor);
       
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
        printf("Difference between NOW and your order's created time: %d\n", timeDifference);
    } else {
      printf("There is no such order in system\n");
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
          printf("order is canceled!\n");
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
        clientID = sqlite3_column_int(res, 0);
      }

      if (step != SQLITE_DONE) {
          return -1;
      }
    } else {
      fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);

    return clientID;
}

void transferOrder(sqlite3 *db, int orderId) {
  	sqlite3_stmt *updateStatus;
    char *updateStatusSQL = "UPDATE orders SET status='inProcess' WHERE id = ?";
    int rc = sqlite3_prepare_v2(db, updateStatusSQL, -1, &updateStatus, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(updateStatus, 1, orderId);

        if (sqlite3_step(updateStatus) == SQLITE_DONE) {
            printf("tailor's working on your order RIGHT NOW\n");
        } 
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(updateStatus);
}

void updateOrderComplete(sqlite3 *db, int orderId){
  
  // получить id портного из заказа
  sqlite3_stmt *res;
  char *sql = "SELECT idTailor FROM orders WHERE id = ?";
  
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

  if (rc == SQLITE_OK) {
    sqlite3_bind_int(res, 1, orderId);
  }
    
  int step = sqlite3_step(res);
  
  int idTailor = -1;
    
  if (step == SQLITE_ROW) {
      idTailor = sqlite3_column_int(res, 0);
      printf("We found id of your tailor: %d\n", idTailor);
  } else {
    printf("There is no such order\n");
  }

  sqlite3_finalize(res);
	
	sqlite3_stmt *updateStatus;
    char *updateStatusSQL = "UPDATE orders SET status='completed', idTailor=NULL WHERE id = ?";
    rc = sqlite3_prepare_v2(db, updateStatusSQL, -1, &updateStatus, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(updateStatus, 1, orderId);

        if (sqlite3_step(updateStatus) == SQLITE_DONE) {
            printf("tailor has completed your order, it will be in storage\n");
        } 
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(updateStatus);
}
