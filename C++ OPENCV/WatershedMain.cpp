
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "WaterShedAlgorithm.h"
#include <time.h>
#include <math.h>
#include <sys/time.h>
using namespace std;

void hora();

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

void hora(){
	char buffer[26];
  	int millisec;
  	struct tm* tm_info;
  	struct timeval tv;

  	gettimeofday(&tv, NULL);

  	millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  	if (millisec>=1000) { // Allow for rounding up to nearest second
    	millisec -=1000;
    	tv.tv_sec++;
  	}

  	tm_info = localtime(&tv.tv_sec);

  	strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
  	printf("%s.%03d\n", buffer, millisec);
}



