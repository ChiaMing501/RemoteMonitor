#include "MonitorClient.h"
#include "ui_MonitorClient.h"

MonitorClient::MonitorClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::MonitorClient)
{
    ui->setupUi(this);

    connect(ui->sendDataButton, SIGNAL(clicked()), this, SLOT(sendDataToServer()));
    connect(ui->createDataButton, SIGNAL(clicked()), this, SLOT(createUserPackage()));

    ui->sendDataButton->setVisible(true);

    memset(&userPackage, 0, sizeof(UserPackage));
	userPackage.imageBuffer = NULL;

	messageStr    = "";
	userNameStr   = "";
	userAgeStr    = "";
	timeRecordStr = "";

} //end of constructor

MonitorClient::~MonitorClient()
{
    delete ui;

} //end of destructor

void MonitorClient::createUserPackage()
{
	QDateTime  dateTimeObj;
	QByteArray timeDataArray;

	Mat sourceImage, outputImage;
	int imageWidth, imageHeight, imageChannels;

	sourceImage = imread("/home/stanleychang/stanleychangFiles/images/Yuju1-z.jpg");

	imageWidth    = sourceImage.cols;
	imageHeight   = sourceImage.rows;
	imageChannels = sourceImage.channels();

	qDebug() << "Image Size: " << imageWidth << " x " << imageHeight << "; Image Channels: " << imageChannels << endl;

#ifdef RECORD_IMAGE
	outputImage.create(imageHeight, imageWidth, CV_8UC3);

	userPackage.imageBuffer = (UserChannel *)malloc(sizeof(UserChannel) * imageWidth * imageHeight);

	for(int y = 0; y < imageHeight; y++)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			userPackage.imageBuffer[(y * imageWidth) + x].bValue = sourceImage.at<Vec3b>(y, x)[B_CHANNEL];
			userPackage.imageBuffer[(y * imageWidth) + x].gValue = sourceImage.at<Vec3b>(y, x)[G_CHANNEL];
			userPackage.imageBuffer[(y * imageWidth) + x].rValue = sourceImage.at<Vec3b>(y, x)[R_CHANNEL];
		}
	}

	for(int y = 0; y < imageHeight; y++)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			outputImage.at<Vec3b>(y, x)[B_CHANNEL] = userPackage.imageBuffer[(y * imageWidth) + x].bValue;
			outputImage.at<Vec3b>(y, x)[G_CHANNEL] = userPackage.imageBuffer[(y * imageWidth) + x].gValue;
			outputImage.at<Vec3b>(y, x)[R_CHANNEL] = userPackage.imageBuffer[(y * imageWidth) + x].rValue;
		}
	}

	imshow("GFriend Yuju", outputImage);
	waitKey(0);
#endif

	dateTimeObj   = QDateTime::currentDateTime();
	timeRecordStr = dateTimeObj.toString("yyyy-MM-dd hh:mm:ss");
	timeDataArray = timeRecordStr.toLatin1();

	strcpy(userPackage.userName, "Yuju1-z.jpg");
	userPackage.userAge = 25;
	strcpy(userPackage.timeRecord, timeDataArray.data());

	QMessageBox::information(this, "Client Message", "User information is recorded.");

} //end of function createUserPackage

void MonitorClient::sendDataToServer()
{
#ifdef USER_INFO_DEBUG
	qDebug() << "userPackage.timeRecord: " << userPackage.timeRecord << endl;

	userNameStr = QString(QLatin1String(userPackage.userName));
	userAgeStr  = QString::number(userPackage.userAge);
	messageStr  = "User Name: " + userNameStr + "\nUser Age: " + userAgeStr + "\nTime Record: " + timeRecordStr;

	QMessageBox::information(this, "Client User Information", messageStr);
#endif

	int clientSocketId = 0;
	int reValue        = 0;
	int dataLength     = 0;

	struct sockaddr_in clientAddressObj;

	clientSocketId = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocketId < 0)
	{
		//fprintf(stderr, "Client [%d]: socket() failed !!\n", getpid());
		qCritical() << "Client [" << QString::number(getpid()) << "]: socket() failed !!" << endl;

		return;
	}

	clientAddressObj.sin_family      = AF_INET;
	clientAddressObj.sin_port        = htons(SERVICE_PORT);
	clientAddressObj.sin_addr.s_addr = inet_addr("127.0.0.1");

	dataLength = sizeof(clientAddressObj);

	//connect() of Linux socket API and QT conflicked, so the use of ::connect() is needed for Linux socket API
	reValue = ::connect(clientSocketId, (struct sockaddr *)&clientAddressObj, dataLength);
	if(reValue == -1)
	{
		//fprintf(stderr, "Client [%d]: connect() failed !!\n", getpid());
		qCritical() << "Client [" << QString::number(getpid()) << "]: connect() failed !!" << endl;

		return;
	}

	write(clientSocketId, &userPackage, sizeof(UserPackage));
	//printf("Client [%d] sent data to server.\n", getpid());
	qDebug() << "Client [" << QString::number(getpid()) << "] sent user info to server.\n" << endl;

	//close() of Linux socket API and QT conflicked, so the use of ::close() is needed for Linux socket API
	::close(clientSocketId);
	qDebug() << "Client [" << QString::number(getpid()) << "] closed the socket.\n" << endl;

	if(userPackage.imageBuffer != NULL)
	{
		free(userPackage.imageBuffer);

		userPackage.imageBuffer = NULL;
	}

} //end of function sendDataToServer
