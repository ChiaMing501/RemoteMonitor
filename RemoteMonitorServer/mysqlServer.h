#ifndef MYSQL_SERVER_H
#define MYSQL_SERVER_H

#include <stdio.h>
#include <unistd.h>

#include "/usr/include/mysql/mysql.h"

void mysqlServerInit(MYSQL *mysqlObj);
void mysqlServerClose(MYSQL *mysqlObj);
int  displayAllRecords(MYSQL *mysqlObjPtr, const char *userSqlQueryString);
void addNewRecord(MYSQL *mysqlObjPtr, const char *userSqlQueryString);

#endif
