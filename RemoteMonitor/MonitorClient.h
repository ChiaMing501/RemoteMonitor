#ifndef MONITORCLIENT_H
#define MONITORCLIENT_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QImage>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/legacy/constants_c.h>

#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../RemoteMonitorServer/transferData.h"
#include "ImageProcessTool.h"
#include "AdvancedFaceDetection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MonitorClient; }
QT_END_NAMESPACE

using namespace cv;

typedef struct
{
	bool isDetected;
	bool isTransmitted;
	bool isTextShowed;

	unsigned int detectedCount;

} LiveViewAction;

class MonitorClient : public QMainWindow
{
    Q_OBJECT

	public:
    	MonitorClient(QWidget *parent = nullptr);
    	~MonitorClient();

    signals:
    	void sendTimeSignal(QString timeStr);

    private slots:
    	void sendDataToServer();
    	void createUserPackage();
    	void setTimeStr(QString timeStr);
    	void displayLiveView();
    	void stopLiveView();
    	void nextFrame();

	private:
    	Ui::MonitorClient *ui;

    	UserPackage    userPackage;
    	pthread_t      timeThreadId;
    	LiveViewAction liveViewActionObj;

    	QString messageStr;
    	QString userNameStr;
    	QString userAgeStr;
    	QString timeRecordStr;

    	VideoCapture *capturePtr;
		Mat          *framePtr;
		QTimer       *timerPtr;
		QImage       qImage;
		QPixmap      qPixmap;

		QImage cvMat2QImage(const cv::Mat &mat);
		void toGrayImage(const cv::Mat &sourceImage, cv::Mat &outputImage, const int imageHeight, const int imageWidth);
		void toNegativeImage(const cv::Mat &sourceImage, cv::Mat &outputImage, const int imageHeight, const int imageWidth);

    	static void *showCurrentTime(void *argument);
};
#endif // MONITORCLIENT_H
