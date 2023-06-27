#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "mysqlServer.h"
#include "transferData.h"

using namespace cv;

int main(int argc, char *argv[])
{
	UserPackage userPackage;

	memset(&userPackage, 0, sizeof(UserPackage));

	//MySQL Processing
	{
		MYSQL mysqlObj;

		const char *queryString = "SELECT * FROM staff";

		/*char userName[]           = "Larson Chang";
		int userAge               = 55;
		char timeRecord[]         = "2022-11-23 19:22:12";
		char sqlInsertString[256] = {0};

		sprintf(sqlInsertString, "INSERT INTO staff(userName, userAge, timeRecord) VALUES('%s', %d, '%s')",
			userName, userAge, timeRecord);*/

		char sqlInsertString[1024] = {0};

		strcpy(userPackage.userName, "Leonel Messi");
		userPackage.userAge = 35;
		strcpy(userPackage.timeRecord, "2023-03-22 10:29:35");

		sprintf(sqlInsertString, "INSERT INTO staff(userName, userAge, timeRecord) VALUES('%s', %d, '%s')",
			userPackage.userName, userPackage.userAge, userPackage.timeRecord);

		mysqlServerInit(&mysqlObj);

		displayAllRecords(&mysqlObj, queryString);

		//addNewRecord(&mysqlObj, sqlInsertString);
		//displayAllRecords(&mysqlObj, queryString);

		mysqlServerClose(&mysqlObj);
	}

	//OpenCV Processing
	{
		Mat sourceImage, outputImage;
		int imageWidth, imageHeight, imageChannel;

		userPackage.imageBuffer = NULL;

		sourceImage = imread("/home/stanleychang/stanleychangFiles/images/Yuju1-z.jpg");
		outputImage = sourceImage.clone();

		imageWidth   = outputImage.cols;
		imageHeight  = outputImage.rows;
		imageChannel = outputImage.channels();

		userPackage.imageBuffer = (UserChannel *)malloc(sizeof(UserChannel) * imageWidth * imageHeight);

		printf("Image Size: %d x %d. Image Channel: %d\n", imageWidth, imageHeight, imageChannel);

		//Copy source image to image buffer
		for(int y = 0; y < imageHeight; y++)
		{
			for(int x = 0; x < imageWidth; x++)
			{
				userPackage.imageBuffer[(y * imageWidth) + x].bValue = sourceImage.at<Vec3b>(y, x)[0];
				userPackage.imageBuffer[(y * imageWidth) + x].gValue = sourceImage.at<Vec3b>(y, x)[1];
				userPackage.imageBuffer[(y * imageWidth) + x].rValue = sourceImage.at<Vec3b>(y, x)[2];
			}
		}

		//Convert image buffer to gray
		for(int y = 0; y < imageHeight; y++)
		{
			for(int x = 0; x < imageWidth; x++)
			{
				unsigned char averageValue, bValue, gValue, rValue;

				bValue = userPackage.imageBuffer[(y * imageWidth) + x].bValue;
				gValue = userPackage.imageBuffer[(y * imageWidth) + x].gValue;
				rValue = userPackage.imageBuffer[(y * imageWidth) + x].rValue;

				averageValue = (bValue + gValue + rValue) / 3;

				userPackage.imageBuffer[(y * imageWidth) + x].bValue = averageValue;
				userPackage.imageBuffer[(y * imageWidth) + x].gValue = averageValue;
				userPackage.imageBuffer[(y * imageWidth) + x].rValue = averageValue;
			}
		}

		//Assign image buffer to output image
		for(int y = 0; y < imageHeight; y++)
		{
			for(int x = 0; x < imageWidth; x++)
			{
				outputImage.at<Vec3b>(y, x)[0] = userPackage.imageBuffer[(y * imageWidth) + x].bValue;
				outputImage.at<Vec3b>(y, x)[1] = userPackage.imageBuffer[(y * imageWidth) + x].gValue;
				outputImage.at<Vec3b>(y, x)[2] = userPackage.imageBuffer[(y * imageWidth) + x].rValue;
			}
		}

		//imshow("GFriend Image (Original)", sourceImage);
		imshow("GFriend Yuju Image (Gray)", outputImage);
		waitKey(0);

		if(userPackage.imageBuffer != NULL)
		{
			free(userPackage.imageBuffer);

			userPackage.imageBuffer = NULL;
		}
	}

	exit(EXIT_SUCCESS);

} //end of function main
