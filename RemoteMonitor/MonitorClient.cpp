#include "MonitorClient.h"
#include "ui_MonitorClient.h"
#include "../RemoteMonitorServer/transferData.h"

MonitorClient::MonitorClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::MonitorClient)
{
    ui->setupUi(this);

    connect(ui->sendDataButton, SIGNAL(clicked()), this, SLOT(sendDataToServer()));

} //end of constructor

MonitorClient::~MonitorClient()
{
    delete ui;

} //end of destructor

void MonitorClient::sendDataToServer()
{
	UserPackage userPackage;
	QString     messageStr, userNameStr, userAgeStr, timeRecordStr;
	QDateTime   dateTimeObj;
	QByteArray  timeDataArray;

	memset(&userPackage, 0, sizeof(UserPackage));
	userPackage.imageBuffer = NULL;

	dateTimeObj   = QDateTime::currentDateTime();
	timeRecordStr = dateTimeObj.toString("yyyy-MM-dd hh:mm:ss");
	timeDataArray = timeRecordStr.toLatin1();

	strcpy(userPackage.userName, "Stanley Chang");
	userPackage.userAge = 41;
	strcpy(userPackage.timeRecord, timeDataArray.data());

	qDebug() << "userPackage.timeRecord: " << userPackage.timeRecord << endl;

	userNameStr = QString(QLatin1String(userPackage.userName));
	userAgeStr  = QString::number(userPackage.userAge);

	messageStr = "User Name: " + userNameStr + "\nUser Age: " + userAgeStr + "\nTime Record: " + timeRecordStr;
	QMessageBox::information(this, "Client", messageStr);

	//QMessageBox::information(this, "Client", "Send data to server");

	/*Mat sourceImage = imread("/home/stanleychang/stanleychangFiles/images/Yuju1-z.jpg");

	imshow("GFriend Yuju", sourceImage);
	waitKey(0);*/

} //end of function sendDataToServer
