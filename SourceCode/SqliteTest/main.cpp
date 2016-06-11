#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

int main()
{

    sqlite3 *db;
    char *errMsg = NULL;

    if (sqlite3_open_v2("example.db3", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
    {
        return 0;
    }
    char* createsql = "CREATE TABLE Contact(ID INTEGER PRIMARY KEY,Name TEXT,PhoneNumber TEXT);";

    int rc = sqlite3_exec(db, createsql, 0, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        printf ("CREATE Error!\n");
    }
    else
    {
        printf ("CREATE Success!\n");
    }





    //char* insert = "INSERT INTO Contact(ID, Name ,PhoneNumber) VALUES (17,'asd','das');";

    //rc = sqlite3_exec(db, insert, 0, 0, &errMsg);

   // if (rc != SQLITE_OK)
   // {
   //     printf ("INSERT Error!\n");
   // }
   // else
   // {
   //     printf ("INSERT Success!\n");
   // }


    int rows, cols;
    char **result;
    sqlite3_stmt *stmt;

    char* select = "SELECT * FROM Contact";

    rc = sqlite3_prepare(db, select, 100, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf ("SELECT Error!\n");
    }
    else
    {
        printf ("SELECT Success!\n");

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            printf("%s|", sqlite3_column_text(stmt, 0));
            printf("%s|", sqlite3_column_text(stmt, 1));
            printf("%s|\n", sqlite3_column_text(stmt, 2));

        }

    }


    sqlite3_finalize(stmt);
    sqlite3_close(db);




    /*
     if (sqlite3_open_v2("example.db3", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)) {
         return 0;
     }

     sqlite3_exec(db, createsql, 0, 0, &errMsg);

     sqlite3_exec(db, insertsql, 0, 0, &errMsg);
     printf("%d\n", sqlite3_last_insert_rowid(db));

     sqlite3_get_table(db , querysql, &result , &rows, &cols, &errMsg);

     for (int i=0;i<rows;i++) {
         for (int j=0;j<cols;j++) {
             printf("%s\t", result[i*cols+j]);
         }
         printf("\n");
     }

     sqlite3_free_table(result);

     sqlite3_close(db);
     /* 新增一筆資料 */
    return 0;
}
