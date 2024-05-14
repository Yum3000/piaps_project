#include <stdio.h>
#include <sqlite3.h> 
#include <string.h>
#include "entities.h"

# define maxCountTailors 2

sqlite3 *initDB(sqlite3 *db) {
   int rc;

   rc = sqlite3_open("atelier.db", &db);

    if (rc != SQLITE_OK)
    {
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


void addNewOrder(sqlite3 *db, enum orderStatus status, char *typeClothes, char *comments, int idService, int idClient, int idTailor){
    // Создаем заказ
    sqlite3_stmt *createNewOrder;

    char *createNewOrderSQL = "INSERT INTO orders (status, typeClothes, comments, idService, idClient, idTailor) VALUES (?, ?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, createNewOrderSQL, -1, &createNewOrder, 0);
     
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(createNewOrder, 1, orderStatusName(status), -1, SQLITE_STATIC);
        sqlite3_bind_text(createNewOrder, 2, typeClothes, -1, SQLITE_STATIC);
        sqlite3_bind_text(createNewOrder, 3, comments, -1, SQLITE_STATIC);
        sqlite3_bind_int(createNewOrder, 4, idService);
        sqlite3_bind_int(createNewOrder, 5, idClient);
        sqlite3_bind_int(createNewOrder, 6, idTailor);

        if (sqlite3_step(createNewOrder) == SQLITE_DONE) {
            printf("new order inserted\n");
        }
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(createNewOrder);


    // Обновляем статус портного
    sqlite3_stmt *updateTailor;
    char *updateTailorSQL = "UPDATE tailors SET status='busy' WHERE id = ?";
    rc = sqlite3_prepare_v2(db, updateTailorSQL, -1, &updateTailor, 0);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(updateTailor, 1, idTailor);

        if (sqlite3_step(updateTailor) == SQLITE_DONE) {
            printf("tailor's going to work on your order - BUSY\n");
        } 
    } else {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(updateTailor);

    printf("\norder created successfully!\n");

}

int getFreeTailor(sqlite3 *db){

    int idOfFreeTailor = -1;
    //freeTailors = malloc(maxCountTailors * sizeof(int));
    //if(freeTailors == NULL){
        //printf("checkTailors - insufficient memory space.\n");
    //}

  sqlite3_stmt *res;  // компилируемое выражение

  char *sql = "SELECT id FROM tailors t WHERE NOT EXISTS (SELECT id FROM orders WHERE idTailor = t.id) LIMIT 1";
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) 
    {
        //numOfFreeTailors = 0;
        // перебираем все строки из результата
        while (sqlite3_step(res) == SQLITE_ROW) 
        {
            if (idOfFreeTailor == -1) {
              idOfFreeTailor = sqlite3_column_int(res, 0); 
            }
            //printf("id: %d", numOfFreeTailor);
            //printf("  status: %s", sqlite3_column_text(res, 1));
            //printf("  currOrderID: %d\n", sqlite3_column_int(res, 2));
        } 
    }


  // удаляем скомпилированное выражение
  sqlite3_finalize(res);
  return idOfFreeTailor;
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

char *checkOrder(sqlite3 *db, int orderId){
    sqlite3_stmt *res;

    char *sql = "SELECT status FROM orders WHERE id = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
    if (rc == SQLITE_OK) 
    {
      sqlite3_bind_int(res, 1, orderId);
    }
    
    int step = sqlite3_step(res);
    char *status, *stat; 
    
    if (step == SQLITE_ROW) {
        stat = (char *)sqlite3_column_text(res, 0);

        status = strdup(stat);

        printf("Status of your order:\n%s\n", status);
    } 

    sqlite3_finalize(res);

    return status;
}

int checkClientId(sqlite3 *db, char *mobileNumber){
    sqlite3_stmt *res;
    
    char *sql = "SELECT id FROM clients WHERE firstName = ? AND lastName = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    printf("rc: %d\n", rc);

    if (rc == SQLITE_OK) {
      //sqlite3_bind_text(res, 1, firstName, -1, SQLITE_STATIC);
      //sqlite3_bind_text(res, 2, lastName, -1, SQLITE_STATIC);
    }
    
    int step = sqlite3_step(res);
    
    printf("STEP: %d\n", step);

    int idClient = -1;

    if (step == SQLITE_ROW) {
        idClient = sqlite3_column_int(res, 0);
        printf("Your id:%d\n", idClient);
    } else {
      printf("There is no such customer in system\n");
    }

    sqlite3_finalize(res);
    return idClient;
}

int checkTimeCreatedOrder(sqlite3 *db, int orderId) {
  
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

void addNewClient(sqlite3 *db, char *firstLastName, char *clientContacts){
  sqlite3_stmt *res;  // компилируемое выражение

  char *sql = "INSERT INTO clients (firstLastName, clientContacts) VALUES (?, ?)";
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
  if (rc == SQLITE_OK) 
  {
        sqlite3_bind_text(res, 1, firstLastName, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 2, clientContacts, -1, SQLITE_STATIC);

        int step = sqlite3_step(res);

        if (step == SQLITE_DONE) 
        {
            printf("New client was registered\n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);

}
