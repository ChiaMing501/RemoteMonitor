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
	userPackage.imageBuffer = NULL;

	int clientSocketId = 0;
	int serverSocketId = 0;
	int reValue        = 0;
	int dataLength     = 0;
	int activeId       = 0;
	int nRead          = 0;

	fd_set selectFd;
	fd_set setFd;

	struct sockaddr_in clientAddressObj;
	struct sockaddr_in serverAddressObj;

	serverSocketId = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocketId < 0)
	{
		fprintf(stderr, "Server [%d]: socket() failed !!\n", getpid());

		exit(EXIT_FAILURE);
	}

	serverAddressObj.sin_family      = AF_INET;
	serverAddressObj.sin_port        = htons(SERVICE_PORT);
	serverAddressObj.sin_addr.s_addr = htonl(INADDR_ANY);

	dataLength = sizeof(serverAddressObj);

	reValue = bind(serverSocketId, (struct sockaddr *)&serverAddressObj, dataLength);
	if(reValue == -1)
	{
		fprintf(stderr, "Server [%d]: bind() failed !!\n", getpid());

		exit(EXIT_FAILURE);
	}

	reValue = listen(serverSocketId, 5);
	if(reValue == -1)
	{
		fprintf(stderr, "Server [%d]: listen() failed !!\n", getpid());

		exit(EXIT_FAILURE);
	}

	FD_ZERO(&setFd);
	FD_SET(serverSocketId, &setFd);

	while(1)
	{
		printf("Server [%d] is waiting for clients...\n", getpid());

		selectFd = setFd;

		reValue = select(FD_SETSIZE, &selectFd, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
		if(reValue < 1)
		{
			fprintf(stderr, "Server [%d]: select() failed !!\n", getpid());

			exit(EXIT_FAILURE);
		}

		for(activeId = 0; activeId < FD_SETSIZE; activeId++)
		{
			if(FD_ISSET(activeId, &selectFd) != 0)
			{
				if(activeId == serverSocketId)
				{
					dataLength = sizeof(clientAddressObj);

					//If compiler g++ is used, the last type of parameter (int *) must converted by (socklen_t *)
					clientSocketId = accept(serverSocketId, (struct sockaddr *)&clientAddressObj, (socklen_t *)&dataLength);
					if(clientSocketId == -1)
					{
						fprintf(stderr, "Server [%d]: accept() failed !!\n", getpid());

						exit(EXIT_FAILURE);
					}

					FD_SET(clientSocketId, &setFd);
					printf("Server [%d] add new client (%d).\n", getpid(), clientSocketId);
				}
				else
				{
					ioctl(activeId, FIONREAD, &nRead);

					if(nRead != 0)
					{
						read(activeId, &userPackage, sizeof(UserPackage));

						usleep(500000);

						printf("Server [%d] got data from client (%d) ==> User Name: %s, User Age: %d, Time Record: %s\n",
							getpid(), activeId, userPackage.userName, userPackage.userAge, userPackage.timeRecord);
					}
					else
					{
						close(activeId);
						FD_CLR(activeId, &setFd);
						printf("Server [%d] remove client (%d).\n", getpid(), activeId);
					}
				}
			}
		}
	}

#ifdef USE_MYSQL
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
#endif

#ifdef USE_OPENCV
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
#endif

	exit(EXIT_SUCCESS);

} //end of function main
