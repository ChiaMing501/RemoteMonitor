all: remoteServer

CC := g++

FIRSTPATH   := -I/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include
SECONDPATH  := -I/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include/opencv4
THIRDPATH   := -I/home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/include/opencv4/opencv2
INCLUDEPATH := $(FIRSTPATH) $(SECONDPATH) $(THIRDPATH)

LIBS := /home/stanleychang/stanleychangFiles/opencvProgram/opencv-4.7.0/build/installed/lib/

remoteServer: remoteServer.o mysqlServer.o
	$(CC) -o $@ $^ -lmysqlclient -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -L$(LIBS)

remoteServer.o: remoteServer.c
	$(CC) -c $< $(INCLUDEPATH)

mysqlServer.o: mysqlServer.c
	$(CC) -c $<

clean:
	rm -f *.o
