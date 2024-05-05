#include <stdio.h>
#include <sqlite3.h> 
#include <string.h>

# define maxCountTailors 2

sqlite3 *initDB(sqlite3 *db) {

   char *zErrMsg = 0;
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


void addNewOrder(sqlite3 *db, char *status, char *typeClothes, char *comments, int idService, int idClient, int idTailor){
    sqlite3_stmt *res;  // компилируемое выражение

    // выполняемый код SQL
    char *sql = "INSERT INTO orders (status, typeClothes, comments, idService, idClient, idTailor) VALUES (?, ?, ?, ?, ?, ?)";
     
    // готовим выражение
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
     
    if (rc == SQLITE_OK) 
    {
         
        // привязываем параметры
        //sqlite3_bind_int(res, 1, id);
        //sqlite3_bind_null(res, 1);
        sqlite3_bind_text(res, 1, status, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 2, typeClothes, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 3, comments, -1, SQLITE_STATIC);
        sqlite3_bind_int(res, 4, idService);
        sqlite3_bind_int(res, 5, idClient);
        sqlite3_bind_int(res, 6, idTailor);

        // выполняем выражение
        int step = sqlite3_step(res);
        // если выражение успешно выполнено
        if (step == SQLITE_DONE) 
        {
            printf("new order inserted\n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);


    sqlite3_stmt *res2;
    char *sql2 = "UPDATE tailors SET status='busy' WHERE id = ?";
    int rc2 = sqlite3_prepare_v2(db, sql2, -1, &res2, 0);

    if (rc2 == SQLITE_OK) 
    {
        sqlite3_bind_int(res2, 1, idTailor);

        // выполняем выражение
        int step = sqlite3_step(res2);
        // если выражение успешно выполнено
        if (step == SQLITE_DONE) 
        {
            printf("tailor's going to work on your order - BUSY\n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }


    printf("\norder created successfully!\n");
    // удаляем скомпилированное выражение
    sqlite3_finalize(res2);

    sqlite3_stmt *res3;  // компилируемое выражение

    // выполняемый код SQL
    char *sql3 = "SELECT id FROM orders WHERE typeClothes = ? AND comments = ?";
     
    // готовим выражение
    int rc3 = sqlite3_prepare_v2(db, sql3, -1, &res3, 0);
     
    if (rc3 == SQLITE_OK) 
    {
      sqlite3_bind_text(res3, 1, typeClothes, -1, SQLITE_STATIC);
      sqlite3_bind_text(res3, 2, comments, -1, SQLITE_STATIC);
    }
    
    int step = sqlite3_step(res3);
 
    int currOrderID = -1;
    int currentOrderId = -1;

    if (step == SQLITE_ROW) {
        currentOrderId = sqlite3_column_int(res3, 0);
        //currentOrderId = strdup(currOrderID);
        printf("%d: ", currOrderID);
        printf("clerk gets your order!\n");
        
    } 

    sqlite3_finalize(res3);


    sqlite3_stmt *res4;
    char *sql4 = "UPDATE orders SET orderDate = CURRENT_TIMESTAMP WHERE id = ?";
    int rc4 = sqlite3_prepare_v2(db, sql4, -1, &res4, 0);

    if (rc2 == SQLITE_OK) 
    {
        sqlite3_bind_int(res4, 1, currentOrderId);

        // выполняем выражение
        int step = sqlite3_step(res4);
        // если выражение успешно выполнено
        if (step == SQLITE_DONE) 
        {
            printf("current order time is writing \n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }


    printf("\norder created successfully!\n");
    // удаляем скомпилированное выражение
    sqlite3_finalize(res4);

    sqlite3_stmt *res5;
    char *sql5 = "UPDATE tailors SET currOrderID = ? WHERE id = ?";
    int rc5 = sqlite3_prepare_v2(db, sql5, -1, &res4, 0);

    if (rc5 == SQLITE_OK) 
    {
        sqlite3_bind_int(res5, 1, currOrderID);
        sqlite3_bind_int(res5, 2, idTailor);

        int step = sqlite3_step(res5);

        if (step == SQLITE_DONE) 
        {
            printf("now tailor gets your order!\n");
        } 
    } 
    else
    {
        // выводим сообщение об ошибке
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res5);
}

int checkTailor(sqlite3 *db){

    int numOfFreeTailor = -1;
    //freeTailors = malloc(maxCountTailors * sizeof(int));
    //if(freeTailors == NULL){
        //printf("checkTailors - insufficient memory space.\n");
    //}

  sqlite3_stmt *res;  // компилируемое выражение

  char *sql = "SELECT * FROM tailors WHERE status='free'";
  int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) 
    {
        //numOfFreeTailors = 0;
        // перебираем все строки из результата
        while (sqlite3_step(res) == SQLITE_ROW) 
        {
            if (numOfFreeTailor == -1) {
              numOfFreeTailor = sqlite3_column_int(res, 0); 
            }
            //printf("id: %d", numOfFreeTailor);
            //printf("  status: %s", sqlite3_column_text(res, 1));
            //printf("  currOrderID: %d\n", sqlite3_column_int(res, 2));
        } 
    }


  // удаляем скомпилированное выражение
  sqlite3_finalize(res);
  return numOfFreeTailor;
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

int checkClientId(sqlite3 *db, char *firstName, char *lastName){
    sqlite3_stmt *res;
    
    printf("From DB Your name is: %s %s\n", firstName, lastName);

    char *sql = "SELECT id FROM clients WHERE firstName = ? AND lastName = ?";
     
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    printf("rc: %d\n", rc);

    if (rc == SQLITE_OK) {
      sqlite3_bind_text(res, 1, firstName, -1, SQLITE_STATIC);
      sqlite3_bind_text(res, 2, lastName, -1, SQLITE_STATIC);
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