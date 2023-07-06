#include "MonitorClient.h"
#include "ui_MonitorClient.h"

MonitorClient::MonitorClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::MonitorClient)
{
    ui->setupUi(this);

    connect(ui->sendDataButton, SIGNAL(clicked()), this, SLOT(sendDataToServer()));
    connect(ui->createDataButton, SIGNAL(clicked()), this, SLOT(createUserPackage()));
    connect(this, SIGNAL(sendTimeSignal(QString)), this, SLOT(setTimeStr(QString)));

    connect(ui->displayButton, SIGNAL(clicked()), this, SLOT(displayLiveView()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopLiveView()));

    capturePtr = nullptr;
	framePtr   = nullptr;
	timerPtr   = nullptr;

	capturePtr = new VideoCapture(0);
	framePtr   = new Mat();
	timerPtr   = new QTimer(this);

    ui->sendDataButton->setVisible(false);
    ui->createDataButton->setVisible(false);
    ui->faceDetectionCheckBox->setChecked(true);

    memset(&userPackage, 0, sizeof(UserPackage));
	userPackage.imageBuffer = NULL;

	messageStr    = "";
	userNameStr   = "";
	userAgeStr    = "";
	timeRecordStr = "";

	liveViewActionObj.isDetected    = false;
	liveViewActionObj.isTransmitted = false;
	liveViewActionObj.detectedCount = 0;

	ui->timeLineEdit->setReadOnly(true);

	pthread_create(&timeThreadId, NULL, showCurrentTime, (void *)this);
	pthread_detach(timeThreadId);

} //end of constructor

MonitorClient::~MonitorClient()
{
    delete ui;

    if(capturePtr != nullptr)
	{
		delete capturePtr;
		capturePtr = nullptr;
	}

	if(framePtr != nullptr)
	{
		delete framePtr;
		framePtr = nullptr;
	}

	if(timerPtr != nullptr)
	{
		timerPtr->stop();
		capturePtr->release();

		delete timerPtr;
		timerPtr = nullptr;
	}

} //end of destructor

void MonitorClient::displayLiveView()
{
	/*capturePtr = new VideoCapture(0);
	framePtr   = new Mat();*/

	capturePtr->open(0);

	//Disable continuous auto focus (on: 1, off: 0)
	capturePtr->set(CAP_PROP_AUTOFOCUS, 0);

	//Set manual focus step (0 - 255)
	capturePtr->set(CAP_PROP_FOCUS, 200); // 200

	//640กั360 (16:9), 320x240 (4:3)
	capturePtr->set(CAP_PROP_FRAME_WIDTH, 320);
	capturePtr->set(CAP_PROP_FRAME_HEIGHT, 240);

	//Frame rate
	//capturePtr->set(CAP_PROP_FPS, 30);

	//Shutter time
	//Manual --> Ex: -4 (means 2^-4 = 1/16 = 80 ms)
	/*capturePtr->set(CAP_PROP_AUTO_EXPOSURE, 1);
	capturePtr->set(CAP_PROP_EXPOSURE, 600);
	capturePtr->set(CAP_PROP_GAIN, 16);*/

	if(capturePtr->isOpened())
	{
		double frameRate = capturePtr->get(CV_CAP_PROP_FPS);

		*capturePtr >> *framePtr;

		if(!framePtr->empty())
		{
			QImage qImage = cvMat2QImage(*framePtr);

			ui->liveViewLabel->setPixmap(QPixmap::fromImage(qImage));

			//timerPtr = new QTimer(this);
			timerPtr->setInterval(1000 / frameRate);
			connect(timerPtr, SIGNAL(timeout()), this, SLOT(nextFrame()));
			timerPtr->start();
		}

		//QMessageBox::information(this, "Live View", "Start");
		//ui->liveViewLineEdit->setText("Live View Status ==> Started");
	}

} //end of function displayLiveView

void MonitorClient::stopLiveView()
{
    timerPtr->stop();

    //QMessageBox::information(this, "Live View", "Stop");
    //ui->liveViewLineEdit->setText("Live View Status ==> Stopped");

    disconnect(timerPtr, SIGNAL(timeout()), this, SLOT(nextFrame()));
    capturePtr->release();

} //end of function stopLiveView

void MonitorClient::nextFrame()
{
    *capturePtr >> *framePtr;

    //Face Detection
    if(ui->faceDetectionCheckBox->isChecked() == true)
    {
    	Mat outputImage;
    	int faceWidth, faceHeight;

    	skinCollector(*framePtr, outputImage);
    	faceDetection(*framePtr, outputImage, &faceWidth, &faceHeight);

    	//qDebug() << "Face is detected. ( rect size: " << faceWidth << "x" << faceHeight << " ) " << endl;

    	if((faceWidth >= 45) && (faceHeight >= 50))
    	{
    		liveViewActionObj.detectedCount++;

    		if((liveViewActionObj.isDetected == false) && (liveViewActionObj.detectedCount >= 90))
    		{
    			liveViewActionObj.isDetected    = true;
    			liveViewActionObj.isTransmitted = true;
    			liveViewActionObj.detectedCount = 0;

    			qDebug() << "Face is detected. ( rect size: " << faceWidth << "x" << faceHeight << " ) " << endl;
    		}
    	}
    	else
    	{
    		liveViewActionObj.isDetected    = false;
    		liveViewActionObj.detectedCount = 0;
    	}
    }

    //User Data Transmission by Socket
    if(liveViewActionObj.isTransmitted == true)
    {
    	qDebug() << "Transmit data to Server." << endl;

    	liveViewActionObj.isTransmitted = false;

    	createUserPackage();
    	sendDataToServer();
    }

    if(capturePtr->isOpened())
    {
        int width  = 0;
        int height = 0;

        width  = saturate_cast<int>(capturePtr->get(CAP_PROP_FRAME_WIDTH));
        height = saturate_cast<int>(capturePtr->get(CAP_PROP_FRAME_HEIGHT));

        Mat outputFrame(height, width, CV_8UC3);

        //toGrayImage(*framePtr, outputFrame, height, width);
        //toNegativeImage(*framePtr, outputFrame, height, width);

        qImage  = cvMat2QImage(*framePtr);
        //qImage  = cvMat2QImage(outputFrame);
        qPixmap = QPixmap::fromImage(qImage);

        qPixmap = qPixmap.scaled(ui->liveViewLabel->width(), ui->liveViewLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->liveViewLabel->setPixmap(qPixmap);
    }

} //end of function nextFrame

QImage MonitorClient::cvMat2QImage(const cv::Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }

} //end of function cvMat2QImage

void MonitorClient::toNegativeImage(const Mat &sourceImage, Mat &outputImage, const int imageHeight, const int imageWidth)
{
    for(int y = 0; y < imageHeight; y++)
    {
        for(int x = 0; x < imageWidth; x++)
        {
            uchar pixel_B = sourceImage.at<Vec3b>(y, x)[B_CHANNEL];
            uchar pixel_G = sourceImage.at<Vec3b>(y, x)[G_CHANNEL];
            uchar pixel_R = sourceImage.at<Vec3b>(y, x)[R_CHANNEL];

            outputImage.at<Vec3b>(y, x)[B_CHANNEL] = saturate_cast<uchar>(255 - pixel_B);
            outputImage.at<Vec3b>(y, x)[G_CHANNEL] = saturate_cast<uchar>(255 - pixel_G);
            outputImage.at<Vec3b>(y, x)[R_CHANNEL] = saturate_cast<uchar>(255 - pixel_R);
        }
    }

} //end of function toNegativeImage

void MonitorClient::toGrayImage(const Mat &sourceImage, Mat &outputImage, const int imageHeight, const int imageWidth)
{
    for(int y = 0; y < imageHeight; y++)
    {
        for(int x = 0; x < imageWidth; x++)
        {
            uchar pixel_B  = sourceImage.at<Vec3b>(y, x)[B_CHANNEL];
            uchar pixel_G  = sourceImage.at<Vec3b>(y, x)[G_CHANNEL];
            uchar pixel_R  = sourceImage.at<Vec3b>(y, x)[R_CHANNEL];
            float avgPixel = saturate_cast<float>((pixel_B + pixel_G + pixel_R) / 3.0);

            outputImage.at<Vec3b>(y, x)[B_CHANNEL] = saturate_cast<uchar>(avgPixel);
            outputImage.at<Vec3b>(y, x)[G_CHANNEL] = saturate_cast<uchar>(avgPixel);
            outputImage.at<Vec3b>(y, x)[R_CHANNEL] = saturate_cast<uchar>(avgPixel);
        }
    }

} //end of function toGrayImage

void MonitorClient::setTimeStr(QString timeStr)
{
	ui->timeLineEdit->setText(timeStr);

} //end of function setTimeStr

void *MonitorClient::showCurrentTime(void *argument)
{
	QDateTime dateTimeObj;
	QString   currentTimeStr;

	MonitorClient *monitorClientObj = static_cast<MonitorClient *>(argument);

	while(1)
	{
		dateTimeObj    = QDateTime::currentDateTime();
		currentTimeStr = dateTimeObj.toString("yyyy-MM-dd   hh:mm:ss");

		monitorClientObj->sendTimeSignal(currentTimeStr);

		sleep(1);
	}

	pthread_exit(NULL);

} //end of function showCurrentTime

void MonitorClient::createUserPackage()
{
	QDateTime  dateTimeObj;
	QByteArray timeDataArray;

	dateTimeObj   = QDateTime::currentDateTime();
	timeRecordStr = dateTimeObj.toString("yyyy-MM-dd hh:mm:ss");
	timeDataArray = timeRecordStr.toLatin1();

	strcpy(userPackage.userName, "Stanley-Chang");
	userPackage.userAge = 41;
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
