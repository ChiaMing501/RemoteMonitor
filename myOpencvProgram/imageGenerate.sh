#!/bin/sh

FIRSTPATH="/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include"
SECONDPATH="/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include/opencv4"
THIRDPATH="/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include/opencv4/opencv2"

FIRSTLIBS="/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/lib"

if [ -f AdvancedFaceDetection.cpp ]
then
	echo "Generate file \"libAdvancedFaceDetection.so ...\""
	g++ -fPIC -c AdvancedFaceDetection.cpp -Iinclude -I$FIRSTPATH -I$SECONDPATH -I$THIRDPATH
        sleep 0.1

	g++ -o libAdvancedFaceDetection.so -shared AdvancedFaceDetection.o -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -L$FIRSTLIBS
else
	echo "File \"AdvancedFaceDetection.cpp does not exist !!\""
	exit 1
fi

sleep 0.1

if [ -f libAdvancedFaceDetection.so ] && [ -d lib ]
then
	echo "Put libAdvancedFaceDetection.so to lib/"
	mv libAdvancedFaceDetection.so lib/

	echo "Delete AdvancedFaceDetection.o"
	rm -f AdvancedFaceDetection.o
else
	echo "Can not find file \"libAdvancedFaceDetection.so\" or directory \"lib\""
fi

exit 0
