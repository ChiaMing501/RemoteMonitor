#include "mysqlServer.h"

void mysqlServerInit(MYSQL *mysqlObj)
{
	MYSQL *reValue;

	reValue = mysql_init(mysqlObj);
	if(reValue == NULL)
	{
		fprintf(stderr, "mysql_init() failed: %s\n", mysql_error(mysqlObj));

		exit(EXIT_FAILURE);
	}

	reValue = mysql_real_connect(mysqlObj, "localhost", "stanleychang", "StanleyChang@501", "remoteMonitor", 0, NULL, 0);
	if(reValue == NULL)
	{
		fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(mysqlObj));

		exit(EXIT_FAILURE);
	}

	printf("MySQL Connection Success.\n");

} //end of function mysqlServerInit

void mysqlServerClose(MYSQL *mysqlObj)
{
	mysql_close(mysqlObj);

	printf("Close MySQL Connection.\n");

} //end of function mysqlServerClose

int displayAllRecords(MYSQL *mysqlObjPtr, const char *userSqlQueryString)
{
	int reValueNum;
	int fieldNum;
	int maxId;

	MYSQL_RES   *mysqlResObj;
	MYSQL_FIELD *mysqlFieldObj;
	MYSQL_ROW   mysqlRowObj;

	printf("\nDisplay all records:\n");

	reValueNum = mysql_query(mysqlObjPtr, userSqlQueryString);
	if(reValueNum != 0)
	{
		fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(mysqlObjPtr));

		exit(EXIT_FAILURE);
	}

	fieldNum = mysql_field_count(mysqlObjPtr);
	printf("Number of fields: %d\n", fieldNum);

	mysqlResObj = mysql_use_result(mysqlObjPtr);
	if(mysqlResObj == NULL)
	{
		fprintf(stderr, "mysql_use_result() failed: %s\n", mysql_error(mysqlObjPtr));

		exit(EXIT_FAILURE);
	}

	printf("================================================\n");

	while((mysqlFieldObj = mysql_fetch_field(mysqlResObj)) != NULL)
	{
		printf("%s  ", mysqlFieldObj->name);
	}
	printf("\n");

	printf("++++++++++++++++++++++++++++++++++++++++++++\n");

	while((mysqlRowObj = mysql_fetch_row(mysqlResObj)) != NULL)
	{
		int i;

		for(i = 0; i < fieldNum; i++)
		{
			printf("%s  ", mysqlRowObj[i]);
		}
		printf("\n");

		maxId = atoi(mysqlRowObj[0]);
	}

	printf("================================================\n");

	printf("\n\n");

	return maxId;

} //end of function displayAllRecords

void addNewRecord(MYSQL *mysqlObjPtr, const char *userSqlQueryString)
{
	unsigned long reValueNum;

	printf("\nAdd new record.\n");

	reValueNum = mysql_query(mysqlObjPtr, userSqlQueryString);
	if(reValueNum != 0)
	{
		fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(mysqlObjPtr));

		exit(EXIT_FAILURE);
	}

	reValueNum = mysql_affected_rows(mysqlObjPtr);
	printf("Number of affected files: %ld\n", reValueNum);

	printf("\n\n");

} //end of function addNewRecord
