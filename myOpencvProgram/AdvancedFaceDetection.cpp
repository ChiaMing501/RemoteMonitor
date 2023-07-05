#include "AdvancedFaceDetection.h"

//Obtain the result of skin collecting
//Parameters --> sourceImage: original color image, processedImage: an image which is converted into skin range
void skinCollector(const Mat &sourceImage, Mat &processedImage)
{
	// "Model-1.png"

	// "Model-2.png"

	// "Yuju4-z.jpg"

	//String directoryPath = "D:/OpenCV/Video/Static-Image/";

	int width  = sourceImage.cols;
	int height = sourceImage.rows;

	MatConstIterator_<Vec3b> sourceIterator;
	MatIterator_<Vec3b>      processedIterator;

	processedImage.create(height, width, CV_8UC3);

	//cout << "Image Size: " << width << " x " << height << "; Image Channel: " << sourceImage.channels() << endl;

	sourceIterator    = sourceImage.begin<Vec3b>();
	processedIterator = processedImage.begin<Vec3b>();

	//Find the skin region
	for(; sourceIterator != sourceImage.end<Vec3b>(); sourceIterator++, processedIterator++)
	{
		uchar bValue = 0;
		uchar gValue = 0;
		uchar rValue = 0;

		double yValue  = 0.0;
		double cbValue = 0.0;
		double crValue = 0.0;

		bValue = (*sourceIterator)[B_CHL];
		gValue = (*sourceIterator)[G_CHL];
		rValue = (*sourceIterator)[R_CHL];

		yValue  = (0.299 * rValue) + (0.587 * gValue) + (0.114 * bValue);
		cbValue = ((-0.1687) * rValue) - (0.3313 * gValue) + (0.5 * bValue) + 128.0;
		crValue = (0.5 * rValue) - (0.4187 * gValue) - (0.0813 * bValue) + 128.0;

		// Original paper range: Cb[96, 131], Cr[131, 173]

		// Cb[100, 120], Cr[140, 160] ==> "Model-1.png"

		// Cb[100, 120], Cr[145, 170] ==> "Model-2.png"

		// Cb[100, 120], Cr[140, 160] ==> "Yuju4-z.jpg"

		if((cbValue >= 100) && (cbValue <= 120) && (crValue >= 140) && (crValue <= 160))
		{
			(*processedIterator)[B_CHL] = 255;
			(*processedIterator)[G_CHL] = 255;
			(*processedIterator)[R_CHL] = 255;
		}
		else
		{
			(*processedIterator)[B_CHL] = 0;
			(*processedIterator)[G_CHL] = 0;
			(*processedIterator)[R_CHL] = 0;
		}
	}

	//imwrite(directoryPath + "Skin-found-Model.jpg", processedImage);

	// (erosion times: 5 --> dilation times: 12) ==> "Model-1.png"

	// (erosion times: 8 --> dilation times: 23) ==> "Model-2.png"

	// (erosion times: 4 --> dilation times: 12) ==> "Yuju4-z.jpg"

	erosionProcess(processedImage, 5); // 4
	//imwrite(directoryPath + "erosion-Model.jpg", processedImage);

	dilationProcess(processedImage, 12); // 12
	//imwrite(directoryPath + "dilation-Model.jpg", processedImage);

} //end of function skinCollector

//Enlarge the found skin region (white region)
void dilationProcess(Mat &outputImage, int times)
{
	Mat originalImage;

	int width  = outputImage.cols;
	int height = outputImage.rows;

	if(times < 1)
	{
		cout << endl << "times format error !!" << endl << endl;

		return;
	}

	do
	{
		originalImage = outputImage.clone();

		for (int y = 1; y < height - 1; y++)
		{
			for (int x = 1; x < width - 1; x++)
			{
				uchar bValue = originalImage.at<Vec3b>(y, x)[B_CHL];
				uchar gValue = originalImage.at<Vec3b>(y, x)[G_CHL];
				uchar rValue = originalImage.at<Vec3b>(y, x)[R_CHL];

				if ((bValue == 255) && (gValue == 255) && (rValue == 255))
				{
					outputImage.at<Vec3b>(y - 1, x)[B_CHL] = 255;
					outputImage.at<Vec3b>(y - 1, x)[G_CHL] = 255;
					outputImage.at<Vec3b>(y - 1, x)[R_CHL] = 255;

					outputImage.at<Vec3b>(y + 1, x)[B_CHL] = 255;
					outputImage.at<Vec3b>(y + 1, x)[G_CHL] = 255;
					outputImage.at<Vec3b>(y + 1, x)[R_CHL] = 255;

					outputImage.at<Vec3b>(y, x - 1)[B_CHL] = 255;
					outputImage.at<Vec3b>(y, x - 1)[G_CHL] = 255;
					outputImage.at<Vec3b>(y, x - 1)[R_CHL] = 255;

					outputImage.at<Vec3b>(y, x + 1)[B_CHL] = 255;
					outputImage.at<Vec3b>(y, x + 1)[G_CHL] = 255;
					outputImage.at<Vec3b>(y, x + 1)[R_CHL] = 255;
				}
			}
		}

		times--;

	} while(times != 0);

	//cout << endl << "Dilation Process is complete" << endl << endl;

} //end of function dilationProcess

//Narrow the found skin region (white region), and the non-skin region (black region) is enlarged
void erosionProcess(Mat &outputImage, int times)
{
	Mat originalImage;

	int width = outputImage.cols;
	int height = outputImage.rows;

	if (times < 1)
	{
		cout << endl << "times format error !!" << endl << endl;

		return;
	}

	do
	{
		originalImage = outputImage.clone();

		for (int y = 1; y < height - 1; y++)
		{
			for (int x = 1; x < width - 1; x++)
			{
				uchar bValue = originalImage.at<Vec3b>(y, x)[B_CHL];
				uchar gValue = originalImage.at<Vec3b>(y, x)[G_CHL];
				uchar rValue = originalImage.at<Vec3b>(y, x)[R_CHL];

				if ((bValue == 0) && (gValue == 0) && (rValue == 0))
				{
					outputImage.at<Vec3b>(y - 1, x)[B_CHL] = 0;
					outputImage.at<Vec3b>(y - 1, x)[G_CHL] = 0;
					outputImage.at<Vec3b>(y - 1, x)[R_CHL] = 0;

					outputImage.at<Vec3b>(y + 1, x)[B_CHL] = 0;
					outputImage.at<Vec3b>(y + 1, x)[G_CHL] = 0;
					outputImage.at<Vec3b>(y + 1, x)[R_CHL] = 0;

					outputImage.at<Vec3b>(y, x - 1)[B_CHL] = 0;
					outputImage.at<Vec3b>(y, x - 1)[G_CHL] = 0;
					outputImage.at<Vec3b>(y, x - 1)[R_CHL] = 0;

					outputImage.at<Vec3b>(y, x + 1)[B_CHL] = 0;
					outputImage.at<Vec3b>(y, x + 1)[G_CHL] = 0;
					outputImage.at<Vec3b>(y, x + 1)[R_CHL] = 0;
				}
			}
		}

		times--;

	} while (times != 0);

	//cout << endl << "Erosion Process is complete" << endl << endl;

} //end of function erosionProcess

//Obtain the result of face detection
//Parameters: originalImage: original color image, skinRangeImage: an image which is converted into skin range
//            userFaceWidth: face rectangle width, userFaceHeight: face rectangle height
void faceDetection(Mat &originalImage, const Mat &skinRangeImage, int *userFaceWidth, int *userFaceHeight)
{
	// "Model-1.png"

	// "Model-2.png"

	// "Yuju4-z.jpg"

	int width       = originalImage.cols;
	int height      = originalImage.rows;
	int channel     = originalImage.channels();
	int *yHistogram = NULL;
	int *xHistogram = NULL;
	int xStart      = 0;
	int xEnd        = 0;
	int yStart      = 0;
	int yEnd        = 0;

	if((width != skinRangeImage.cols) && (height != skinRangeImage.rows) && (channel != skinRangeImage.channels()))
	{
		cout << "Image size does not match !!" << endl;

		return;
	}

	yHistogram = new int[height];
	xHistogram = new int[width];

	memset(yHistogram, 0, sizeof(int) * height);
	memset(xHistogram, 0, sizeof(int) * width);

	//Calculate the histogram of Y
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			//Refer to only one channel because the values of three channels are the same (0 or 255)
			uchar targetPixel = skinRangeImage.at<Vec3b>(y, x)[B_CHL];

			if(targetPixel == 255)
			{
				yHistogram[y]++;
			}
		}
	}

	//Calculate the histogram of X
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			//Refer to only one channel because the values of three channels are the same (0 or 255)
			uchar targetPixel = skinRangeImage.at<Vec3b>(y, x)[B_CHL];

			if(targetPixel == 255)
			{
				xHistogram[x]++;
			}
		}
	}

	//Find X start
	for(int i = 0; i < width; i++)
	{
		if(xHistogram[i] >= X_THRESHOLD)
		{
			xStart = i;

			break;
		}
	}

	//Find X end
	for(int i = width - 1; i >= 0; i--)
	{
		if(xHistogram[i] >= X_THRESHOLD)
		{
			xEnd = i;

			break;
		}
	}

	//Find Y start
	for(int i = 0; i < height; i++)
	{
		if(yHistogram[i] >= Y_THRESHOLD)
		{
			yStart = i;

			break;
		}
	}

	//Find Y end
	for(int i = height - 1; i >= 0; i--)
	{
		if(yHistogram[i] >= Y_THRESHOLD)
		{
			yEnd = i;

			break;
		}
	}

	Rect rect(xStart, yStart, xEnd - xStart, yEnd - yStart);
	rectangle(originalImage, rect, Scalar(0, 0, 255), 2, LINE_8, 0);

	*userFaceWidth  = xEnd - xStart;
	*userFaceHeight = yEnd - yStart;

	if(yHistogram != NULL)
	{
		delete [] yHistogram;

		yHistogram = NULL;
	}

	if(xHistogram != NULL)
	{
		delete [] xHistogram;

		xHistogram = NULL;
	}

} //end of function faceDetection
