#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "mysqlServer.h"
#include "transferData.h"

using namespace cv;

void opencvProcessing(const UserPackage *uPackage);
void mysqlProcessing(const UserPackage *uPackage);
void writeUserImage(const Mat *outputImage);
void showUserImage(const char *filePath, const int width, const int height);

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

						mysqlProcessing(&userPackage);
						opencvProcessing(&userPackage);
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

	if(userPackage.imageBuffer != NULL)
	{
		free(userPackage.imageBuffer);

		userPackage.imageBuffer = NULL;
	}

	exit(EXIT_SUCCESS);

} //end of function main

//Show user image according to the matched data sent from Client
void opencvProcessing(const UserPackage *uPackage)
{
	Mat  userImage, processedImage;
	int  imageWidth, imageHeight, imageChannel;
	char imageDatabasePath[512];
	char userNameStr[1024], userAgeStr[1024], timeRecordStr[1024];
	char imageTitle[512];

	memset(imageDatabasePath, 0, sizeof(imageDatabasePath));
	strcpy(imageDatabasePath, "/home/stanleychang/stanleychangFiles/images/");
	strcat(imageDatabasePath, uPackage->userName);
	strcat(imageDatabasePath, ".jpg");
	printf("Server [%d]: Image Database Path: %s\n", getpid(), imageDatabasePath);

	userImage = imread(imageDatabasePath);

	imageWidth   = userImage.cols;
	imageHeight  = userImage.rows;
	imageChannel = userImage.channels();
	printf("Server [%d]: Image Size: %d x %d. Image Channel: %d\n", getpid(), imageWidth, imageHeight, imageChannel);

	processedImage.create(imageHeight, imageWidth, CV_8UC3);

	for(int y = 0; y < imageHeight; y++)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			processedImage.at<Vec3b>(y, x)[B_CHANNEL] = 255 - userImage.at<Vec3b>(y, x)[B_CHANNEL];
			processedImage.at<Vec3b>(y, x)[G_CHANNEL] = 255 - userImage.at<Vec3b>(y, x)[G_CHANNEL];
			processedImage.at<Vec3b>(y, x)[R_CHANNEL] = 255 - userImage.at<Vec3b>(y, x)[R_CHANNEL];
		}
	}

	sprintf(userNameStr, "User Name: %s", uPackage->userName);
	putText(processedImage, userNameStr, Point(25, 50), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2, 8);

	sprintf(userAgeStr, "User Age: %d", uPackage->userAge);
	putText(processedImage, userAgeStr, Point(25, 90), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2, 8);

	sprintf(timeRecordStr, "Time Record: %s", uPackage->timeRecord);
	putText(processedImage, timeRecordStr, Point(25, 130), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2, 8);

	sprintf(imageTitle, "Server [%d]: Real Time Matched User Image", getpid());
	imshow(imageTitle, processedImage);
	waitKey(0);

	writeUserImage(&processedImage);
	showUserImage("UserImage.data", imageWidth, imageHeight);

} //end of function opencvProcessing

//Write data which is sent by Client to MySQL table
void mysqlProcessing(const UserPackage *uPackage)
{
	MYSQL mysqlObj;

	const char *queryString    = "SELECT * FROM staff";
	char sqlInsertString[1024] = {0};
	int maxUserNumber          = 0;

	mysqlServerInit(&mysqlObj);

	maxUserNumber = displayAllRecords(&mysqlObj, queryString);
	//printf("Max User Number: %d\n", maxUserNumber);

	sprintf(sqlInsertString, "INSERT INTO staff(userNumber, userName, userAge, timeRecord) VALUES(%d, '%s', %d, '%s')",
		maxUserNumber + 1, uPackage->userName, uPackage->userAge, uPackage->timeRecord);

	printf("#######################################################################\n");
	addNewRecord(&mysqlObj, sqlInsertString);
	displayAllRecords(&mysqlObj, queryString);

	mysqlServerClose(&mysqlObj);

} //end of function mysqlProcessing

//Record the user image with info text into a file
void writeUserImage(const Mat *outputImage)
{
	int width, height;
	int fileDescriptor;
	int nWrite;
	UserPackage tmpPackage;

	tmpPackage.imageBuffer = NULL;

	width  = outputImage->cols;
	height = outputImage->rows;

	tmpPackage.imageBuffer = (UserChannel *)malloc(sizeof(UserChannel) * width * height);

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			tmpPackage.imageBuffer[y * width + x].bValue = 255 - outputImage->at<Vec3b>(y, x)[B_CHANNEL];
			tmpPackage.imageBuffer[y * width + x].gValue = 255 - outputImage->at<Vec3b>(y, x)[G_CHANNEL];
			tmpPackage.imageBuffer[y * width + x].rValue = 255 - outputImage->at<Vec3b>(y, x)[R_CHANNEL];
		}
	}

	fileDescriptor = open("UserImage.data", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	nWrite         = write(fileDescriptor, tmpPackage.imageBuffer, sizeof(UserChannel) * width * height);

	if(nWrite == -1)
	{
		fprintf(stderr, "Server [%d]: wirte() for user image data failed !!\n", getpid());

		if(tmpPackage.imageBuffer != NULL)
		{
			free(tmpPackage.imageBuffer);

			tmpPackage.imageBuffer = NULL;
		}

		return;
	}

	printf("Server [%d]: write() is successful, the written data is %d bytes.\n", getpid(), nWrite);

	if(tmpPackage.imageBuffer != NULL)
	{
		free(tmpPackage.imageBuffer);

		tmpPackage.imageBuffer = NULL;
	}

} //end of function writeUserImage

//Read the user image with info text from file and show it
void showUserImage(const char *filePath, const int width, const int height)
{
	Mat         inputImage;
	UserPackage inputPackage;
	int         fileDescriptor;
	int         nRead;
	char        imageInfo[512];

	inputPackage.imageBuffer = NULL;

	inputImage.create(height, width, CV_8UC3);
	inputPackage.imageBuffer = (UserChannel *)malloc(sizeof(UserChannel) * width * height);

	fileDescriptor = open(filePath, O_RDONLY);
	nRead          = read(fileDescriptor, inputPackage.imageBuffer, sizeof(UserChannel) * width * height);

	if(nRead == -1)
	{
		fprintf(stderr, "Server [%d]: read() for user image data failed !!\n", getpid());

		if(inputPackage.imageBuffer != NULL)
		{
			free(inputPackage.imageBuffer);

			inputPackage.imageBuffer = NULL;
		}

		return;
	}

	printf("Server [%d]: read() is successful, the read data is %d bytes.\n", getpid(), nRead);

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			inputImage.at<Vec3b>(y, x)[B_CHANNEL] = inputPackage.imageBuffer[y * width + x].bValue;
			inputImage.at<Vec3b>(y, x)[G_CHANNEL] = inputPackage.imageBuffer[y * width + x].gValue;
			inputImage.at<Vec3b>(y, x)[R_CHANNEL] = inputPackage.imageBuffer[y * width + x].rValue;
		}
	}

	sprintf(imageInfo, "Server [%d]: Database Image", getpid());
	imshow(imageInfo, inputImage);
	waitKey(0);

	if(inputPackage.imageBuffer != NULL)
	{
		free(inputPackage.imageBuffer);

		inputPackage.imageBuffer = NULL;
	}

} //end of function showUserImage
