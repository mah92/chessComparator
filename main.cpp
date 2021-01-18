//ALLAH
//compares images in two folders with same image sequences,...
//to find rms of grid point locations in them
//

#define ADDR1 "../patternLogs/pure-rot-NoDelay/screenshot00001.bmp" //non delayed
#define ADDR2 "../patternLogs/pure-rot-1sDelay-RK1-1step/screenshot00000.bmp" //delayed

#define MAX_FILES 1

//#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "opencv2/opencv.hpp"
//#include "cv.h"
#include <iostream>
#include <fstream>
#include "MyUtility-Win.hpp"

#define TAG "Chessboard:"
using namespace cv;
using namespace std;
//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char** argv)
{
    Mat frame, frame2;
    Point QRpt;
    VideoCapture QRCap;
    Scalar color;
	vector<Point2f> image_points1, image_points2;
	char addr_str[100];
	int i, j;
	bool found;
	int board_w, board_h, board_n, board_type;
	double diffx, diffy, im_rms, total_rms;
	int num_images;


	enum {BOARD_UNKNOWN, BOARD_CHESSBOARD, BOARD_CIRCLES_GRID, BOARD_ASYMMETRIC_CIRCLES_GRID};
	board_type = BOARD_ASYMMETRIC_CIRCLES_GRID;

	board_w = 4; // Board width
	board_h = 11; // Board height
	//converting to real trackable points
	if(board_type == BOARD_CHESSBOARD) {
		board_w--; board_h--;
	}

	board_n = board_w * board_h;
	CvSize board_sz = cvSize( board_w, board_h);

    LOGOPEN(".");

    LOGI(TAG, "In the name of ALLAH\n");

	namedWindow("im1",1);
	namedWindow("im2",1);

	num_images = 0;
	total_rms = 0;
	for(i=0; ;i++ ) {
		//im1//////////////////////////////////////////////
		image_points1.clear();
		sprintf(addr_str, ADDR1, i);
		frame = imread(addr_str);
		if (!frame.data) break;
		//cvtColor(frame, frame2, CV_RGB2GRAY);

		switch(board_type) {
		case BOARD_CHESSBOARD:
		  //found = findChessboardCorners(QRColorFrame, board_sz, QRMatchLoc, CALIB_CB_FAST_CHECK);
		  found = findChessboardCorners( frame, board_sz, image_points1,
			 CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		  break;
		case BOARD_CIRCLES_GRID:
		  found = findCirclesGrid( frame, board_sz, image_points1 );
		  break;
		case BOARD_ASYMMETRIC_CIRCLES_GRID:
		  found = findCirclesGrid( frame, board_sz, image_points1, CALIB_CB_ASYMMETRIC_GRID );
		  break;
		}

		if(!found || image_points1.size()!=board_n) //not found correct number of points, really needed
			continue;

		for(j = 0; j < board_n; j++){
			color=Scalar(255-255*j/board_n, 255*j/board_n, 0);
			QRpt = Point(image_points1[j].x, image_points1[j].y);
			circle(frame, QRpt, 3, color, CV_FILLED, 8, 0);
		}
		imshow("im1",frame);

		//im2//////////////////////////////////////////////
		image_points2.clear();
		sprintf(addr_str, ADDR2, i);
		frame = imread(addr_str);
		if (!frame.data) break;
		//cvtColor(frame, frame2, CV_RGB2GRAY);

		switch(board_type) {
		case BOARD_CHESSBOARD:
		  //found = findChessboardCorners(QRColorFrame, board_sz, QRMatchLoc, CALIB_CB_FAST_CHECK);
		  found = findChessboardCorners( frame, board_sz, image_points2,
			 CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		  break;
		case BOARD_CIRCLES_GRID:
		  found = findCirclesGrid( frame, board_sz, image_points2 );
		  break;
		case BOARD_ASYMMETRIC_CIRCLES_GRID:
		  found = findCirclesGrid( frame, board_sz, image_points2, CALIB_CB_ASYMMETRIC_GRID );
		  break;
		}

		if(!found || image_points2.size()!=board_n) //not found correct number of points, really needed
			continue;

		for(j = 0; j < board_n; j++){
			color=Scalar(255-255*j/board_n, 255*j/board_n, 0);
			QRpt = Point(image_points2[j].x, image_points2[j].y);
			circle(frame, QRpt, 3, color, CV_FILLED, 8, 0);
		}
		imshow("im2",frame);

        //sub pixel is not robust, some times causes big differences
        //subpixel can work with just grayscale image
        //cornerSubPix(frame2, image_points, cvSize (11,11), cvSize (-1,-1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 0.1));

		//compare//////////////////////////////////////////
		im_rms = 0;
		for(j = 0; j < board_n; j++){
			diffx = image_points1[j].x - image_points2[j].x;
			diffy = image_points1[j].y - image_points2[j].y;
			im_rms += sqrt(diffx*diffx + diffy*diffy);
		}
		im_rms/=board_n;

		/*if(im_rms > 10.) {
			for(j = 0; j < board_n; j++){
				LOGI(TAG, "%i, ");
			}
		}*/

		LOGI(TAG, "%i->%f\n", num_images, im_rms);
		if(im_rms < 50) {//non-outlier
			total_rms+=im_rms;
			num_images++;
		}
		
		if(num_images >= MAX_FILES) break;

		if(waitKey(33) >= 0) break;
    }
	total_rms/=num_images;
	LOGI(TAG, "num_images:%i -> total_rms: %f\n", num_images, total_rms);

    LOGDUMP();
    LOGCLOSE();
    return 0;
}

