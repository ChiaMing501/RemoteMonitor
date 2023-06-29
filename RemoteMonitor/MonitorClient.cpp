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

	dateTimeObj   = QDateTime::currentDateTime();
	timeRecordStr = dateTimeObj.toString("yyyy-MM-dd hh:mm:ss");
	timeDataArray = timeRecordStr.toLatin1();

	strcpy(userPackage.userName, "Yuju-Cheng");
	userPackage.userAge = 27;
	strcpy(userPackage.timeRecord, timeDataArray.data());

	qDebug() << "Client [" << QString::number(getpid()) << "] catched user information: User Name: " << userPackage.userName <<
		"; User Age: " << QString::number(userPackage.userAge) << "; Time Record: " << userPackage.timeRecord << endl;

} //end of function createUserPackage

void MonitorClient::sendDataToServer()
{
	int clientSocketId = 0;
	int reValue        = 0;
	int dataLength     = 0;

	struct sockaddr_in clientAddressObj;

	clientSocketId = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocketId < 0)
	{
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
		qCritical() << "Client [" << QString::number(getpid()) << "]: connect() failed !!" << endl;

		return;
	}

	write(clientSocketId, &userPackage, sizeof(UserPackage));
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
