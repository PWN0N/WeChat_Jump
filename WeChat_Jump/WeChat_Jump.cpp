// WeChat_Jump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

int main()
{

	Mat temp = imread("flag.png");
	Mat img = imread("5.png");
	Mat result;
	int result_cols = img.cols - temp.cols + 1;
	int result_rows = img.rows - temp.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);

	matchTemplate(img, temp, result, CV_TM_SQDIFF_NORMED);//标准平方差匹配
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = minLoc;
	rectangle(img, matchLoc, Point(matchLoc.x + temp.cols, matchLoc.y + temp.rows), Scalar(0, 255, 0), 2, 8, 0);
	namedWindow("img", WINDOW_NORMAL);
	imshow("img", img);
	waitKey(0);
	return 0;
}
