#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "mysqlServer.h"

using namespace cv;

int main(int argc, char *argv[])
{
	//MySQL Processing
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

		//addNewRecord(&mysqlObj, sqlInsertString);
		//displayAllRecords(&mysqlObj, queryString);

		mysqlServerClose(&mysqlObj);
	}

	//OpenCV Processing
	{
		Mat sourceImage = imread("/home/stanleychang/stanleychangFiles/images/Yuju1-z.jpg");

		imshow("GFriend Image", sourceImage);
		waitKey(0);
	}

	exit(EXIT_SUCCESS);

} //end of function main
