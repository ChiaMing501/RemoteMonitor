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

	private:
    	Ui::MonitorClient *ui;

    	UserPackage userPackage;
    	pthread_t   timeThreadId;

    	QString messageStr;
    	QString userNameStr;
    	QString userAgeStr;
    	QString timeRecordStr;

    	static void *showCurrentTime(void *argument);
};
#endif // MONITORCLIENT_H
