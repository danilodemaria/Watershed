

#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "WaterShedAlgorithm.h"

int main( int argc, char* argv[] )
{
	std::string imgName;
	if (argc > 1) imgName = argv[1];
	else { printf("usage: ws image_name \n"); return -1; }

	WatershedAlgorithm myWatershed;
	IplImage* pSrc = cvLoadImage(imgName.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	if (pSrc == NULL) {
		printf("Can't open image: %s \n", imgName.c_str());
		return -1;
	}
	myWatershed.run(pSrc, imgName);

	cvShowImage(imgName.c_str(), pSrc);
	std::string inTmp;
	if (pSrc->nChannels == 3) {
		inTmp = imgName + "_Gray.bmp";
		IplImage* pGray = cvLoadImage(inTmp.c_str(), CV_LOAD_IMAGE_UNCHANGED);
		cvShowImage(inTmp.c_str(), pGray);
	}
	inTmp = imgName + "_BW.bmp";
	IplImage* pBW = cvLoadImage(inTmp.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	cvShowImage(inTmp.c_str(), pBW);
	inTmp = imgName + "_WS.bmp";
	IplImage* pWS = cvLoadImage(inTmp.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	cvShowImage(inTmp.c_str(), pWS);
	inTmp = imgName + "_Gray_WS.bmp";
	IplImage* pGWS = cvLoadImage(inTmp.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	cvShowImage(inTmp.c_str(), pGWS);
	cvWaitKey();

	cvReleaseImage(&pSrc);
	return 0;
}
