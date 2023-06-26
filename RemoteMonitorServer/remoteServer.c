#include <stdio.h>
#include <unistd.h>

#include "mysqlServer.h"

int main(int argc, char *argv[])
{
	MYSQL mysqlObj;

	const char *queryString = "SELECT * FROM staff";

	char userName[]           = "Larson Chang";
	int userAge               = 55;
	char timeRecord[]         = "2022-11-23 19:22:12";
	char sqlInsertString[256] = {0};

	sprintf(sqlInsertString, "INSERT INTO staff(userName, userAge, timeRecord) VALUES('%s', %d, '%s')",
		userName, userAge, timeRecord);

	mysqlServerInit(&mysqlObj);

	displayAllRecords(&mysqlObj, queryString);

	/*addNewRecord(&mysqlObj, sqlInsertString);
	displayAllRecords(&mysqlObj, queryString);*/

	mysqlServerClose(&mysqlObj);

	exit(EXIT_SUCCESS);

} //end of function main
