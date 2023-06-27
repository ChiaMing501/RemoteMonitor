#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

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
