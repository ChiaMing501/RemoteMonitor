#ifndef IMAGE_PROCESS_TOOL_H
#define IMAGE_PROCESS_TOOL_H

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

class ImageProcessTool
{
	public:
		ImageProcessTool();
		~ImageProcessTool();

		void getUserMessage(char *message, const int messageSize);

	private:
		char userMessage[256];

}; //end of class ImageProcessTool

#endif
