#!/bin/sh

g++ -fPIC -c ImageProcessTool.cpp -Iinclude
sleep 0.1

g++ -o libImageProcessTool.so -shared ImageProcessTool.o
sleep 0.1

if [ -d lib ] && [ -f libImageProcessTool.so ]
then
	echo "Put .so files to lib/"
	mv *.so lib/
fi

if [ -f ImageProcessTool.o ]
then
	echo "Delete ImageProcessTool.o"
	rm -f *.o
else
	echo "ImageProcessTool.o does not exist."
fi

exit 0
