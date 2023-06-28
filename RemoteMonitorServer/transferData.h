#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	uchar bValue;
	uchar gValue;
	uchar rValue;

} UserChannel;

typedef struct
{
	char userName[128];
	int  userAge;
	char timeRecord[128];

	UserChannel *imageBuffer;

} UserPackage;

#endif
