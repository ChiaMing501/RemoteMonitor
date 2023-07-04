#include "ImageProcessTool.h"

ImageProcessTool::ImageProcessTool()
{
	memset(userMessage, 0, sizeof(userMessage));
	sprintf(userMessage, "Dynamic library used for QT.");

} //end of constructor

ImageProcessTool::~ImageProcessTool()
{
	//Empty

} //end of destructor

void ImageProcessTool::getUserMessage(char *message, const int messageSize)
{
	if(messageSize < sizeof(userMessage))
	{
		return;
	}

	strncpy(message, userMessage, sizeof(userMessage));

} //end of function getUserMessage
