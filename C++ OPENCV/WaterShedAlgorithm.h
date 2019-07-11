
/*
*  Vincent and Soille(1991)
*/

#ifndef WATERSHEDALGORITHM_H
#define WATERSHEDALGORITHM_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <queue>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <chrono> 
#include "WatershedStructure.h"

using namespace std::chrono;
using namespace std; 

class WatershedAlgorithm {
    static const int HMIN = 0;	
    static const int HMAX = 256;
    

public:
    void run(IplImage* pSrc, const std::string& imgName) { 
        std::string inTmp;
        double aux1=0,aux2=0,aux3 = 0;
		
        IplImage* pGray = cvCreateImage(cvGetSize(pSrc), IPL_DEPTH_8U, 1);
        if (pSrc->nChannels == 3) {
            cvCvtColor(pSrc, pGray, CV_BGR2GRAY);
            inTmp = imgName + "_Gray.bmp"; 
            cv::Mat m1 = cv::cvarrToMat(pGray);
            cv::imwrite(inTmp.c_str(), m1);
        }
        else if (pSrc->nChannels == 1)
            cvCopy(pSrc, pGray);


        IplImage* pBW = cvCreateImage(cvGetSize(pGray), IPL_DEPTH_8U, 1);
        cvAdaptiveThreshold(pGray, pBW, 255, 0, 0, 31); 
        inTmp = imgName + "_BW.bmp"; 
        cv::Mat m2 = cv::cvarrToMat(pBW);
        cv::imwrite(inTmp.c_str(), m2);

        char* pixels = pBW->imageData;
        int width = pBW->width;
        int height = pBW->height;

        
        WatershedStructure  watershedStructure(pixels, width, height);

		
        std::queue<WatershedPixel*> pque;	
        int curlab = 0;
        int heightIndex1 = 0;
        int heightIndex2 = 0;
        
        
        // aqui função 01
        auto start = high_resolution_clock::now();
        for (int h = HMIN; h < HMAX; ++h) { 

            
            for (int pixelIndex = heightIndex1 ; pixelIndex < watershedStructure.size() ; ++pixelIndex) {
                WatershedPixel* p = watershedStructure.at(pixelIndex);

				
                if (p->getIntHeight() != h) { heightIndex1 = pixelIndex; break; }

                p->setLabelToMASK(); 

                std::vector<WatershedPixel*> neighbours = p->getNeighbours();
                for (unsigned i = 0 ; i < neighbours.size() ; ++i) {
                    WatershedPixel* q =  neighbours.at(i);

					 
                    if (q->getLabel() >= 0) { p->setDistance(1); pque.push(p); break; }
                }
            }
  

            int curdist = 1;
            pque.push(new WatershedPixel());
            //printf("Fim função 01 ");
             
            
            //fim função 01

            //printf("Inicio função 02 ");
           
            
            while (true) { 
                WatershedPixel* p = pque.front(); pque.pop();

                if (p->isFICTITIOUS())
                    if (pque.empty()) { delete p; p = NULL; break; }
                    else {
                        pque.push(new WatershedPixel());
                        curdist++;
                        delete p; p = pque.front(); pque.pop();
                    }

                std::vector<WatershedPixel*> neighbours = p->getNeighbours();
                for (unsigned i = 0 ; i < neighbours.size() ; ++i) { 
                    WatershedPixel* q =  neighbours.at(i);

					
                    if ( (q->getDistance() <= curdist) &&  (q->getLabel() >= 0) ) {             

                        if (q->getLabel() > 0) {
                            if ( p->isLabelMASK() )
                                p->setLabel(q->getLabel());
                            else if (p->getLabel() != q->getLabel())
                                p->setLabelToWSHED();
                        } else if (p->isLabelMASK()) 
							p->setLabelToWSHED();
                    } else if ( q->isLabelMASK() && (q->getDistance() == 0) ) {
                        q->setDistance( curdist + 1 );
                        pque.push(q);
                    }
                } 
            } 
            //printf("Fim função 02 ");

            
            //printf("Inicio função 03 ");
            for (int pixelIndex = heightIndex2 ; pixelIndex < watershedStructure.size() ; pixelIndex++) {
                WatershedPixel* p = watershedStructure.at(pixelIndex);

				
                if (p->getIntHeight() != h) { heightIndex2 = pixelIndex; break; }

                p->setDistance(0); 

                if (p->isLabelMASK()) { 
                    curlab++;
                    p->setLabel(curlab);
                    pque.push(p);

                    while (!pque.empty()) {
                        WatershedPixel* q = pque.front();
                        pque.pop();

                        std::vector<WatershedPixel*> neighbours = q->getNeighbours();

                        for (unsigned i = 0 ; i < neighbours.size() ; i++) { 
                            WatershedPixel* r =  neighbours.at(i);

                            if ( r->isLabelMASK() ) { r->setLabel(curlab); pque.push(r); }
                        }
                    } // end while
                } // end if
            } // end for
            //printf("Fim função 03 ");
            

        }


        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() << endl; 		

		
        IplImage* pWS = cvCreateImage(cvGetSize(pBW), IPL_DEPTH_8U, 1);
        //cvCopyImage(pBW, pWS);
		cvZero(pBW);
        char* wsPixels = pWS->imageData;
        char* grayPixels = pGray->imageData;


        for (int pixelIndex = 0 ; pixelIndex < watershedStructure.size() ; pixelIndex++) {
            WatershedPixel* p = watershedStructure.at(pixelIndex);

            if (p->isLabelWSHED() && !p->allNeighboursAreWSHED()) {
                wsPixels[p->getX() + p->getY()*width] = (char)255; 
                grayPixels[p->getX() + p->getY()*width] = (char)255;	
            }
        }


        inTmp = imgName + "_WS.bmp"; 
        cv::Mat m3 = cv::cvarrToMat(pWS);
        cv::imwrite(inTmp.c_str(), m3);
        //cv::imwrite(inTmp.c_str(), pWS);
        inTmp = imgName + "_Gray_WS.bmp";
        cv::Mat m4 = cv::cvarrToMat(pGray);
        cv::imwrite(inTmp.c_str(), m4);

		cvReleaseImage(&pGray);
		cvReleaseImage(&pBW);
    }
};

#endif
