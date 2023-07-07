#ifndef ADVANCED_FACE_DETECTION_H
#define ADVANCED_FACE_DETECTION_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/legacy/constants_c.h>

using namespace std;
using namespace cv;

// X_THRESHOLD: 40, Y_THRESHOLD: 40 ==> "Model-1.png"

// X_THRESHOLD: 15, Y_THRESHOLD: 20 ==> "Model-2.png"

// X_THRESHOLD: 70, Y_THRESHOLD: 70 ==> "Yuju4-z.jpg"

typedef enum
{
	X_THRESHOLD = 20,
	Y_THRESHOLD = 20

} ThresholdDefine;

typedef enum
{
	B_CHL = 0,
	G_CHL,
	R_CHL

} ColorChannel;

typedef struct
{
	int xStart;
	int xEnd;
	int yStart;
	int yEnd;

} FaceRect;

void skinCollector(const Mat &sourceImage, Mat &processedImage);
void dilationProcess(Mat &outputImage, int times);
void erosionProcess(Mat &outputImage, int times);
void faceDetection(Mat &originalImage, const Mat &skinRangeImage, int *userFaceWidth, int *userFaceHeight);
void faceDetection2(Mat &originalImage, const Mat &skinRangeImage, int *userFaceWidth, int *userFaceHeight, FaceRect &faceRectObj);

#endif
