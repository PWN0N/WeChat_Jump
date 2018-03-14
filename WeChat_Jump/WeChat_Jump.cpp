// WeChat_Jump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

int main()
{
	Mat dst;
	Point2f src_position[3];
	Point2f dst_position[3];


	src_position[0] = Point2f(586, 1102);
	src_position[1] = Point2f(1057, 1102);
	src_position[2] = Point2f(821, 966);

	dst_position[0] = Point2f(586, 1102);
	dst_position[1] = Point2f(919, 1102);
	dst_position[2] = Point2f(586, 769);
	Mat trans = getAffineTransform(src_position, dst_position);


	Mat temp = imread("flag.png");
	Mat img = imread("5.png");
	dst = Mat::zeros(img.rows, img.cols, img.type());
	warpAffine(img, dst, trans, img.size());
	Mat result;
	int result_cols = img.cols - temp.cols + 1;
	int result_rows = img.rows - temp.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);
	
	matchTemplate(img, temp, result, TM_CCORR);//标准平方差匹配
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	cout << "匹配度：" << minVal << endl;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	cout << "匹配度：" << minVal << endl;
	matchLoc = minLoc;

	rectangle(img, matchLoc, Point(matchLoc.x + temp.cols, matchLoc.y + temp.rows), Scalar(0, 255, 0), 2, 8, 0);

	namedWindow("img", WINDOW_NORMAL);
	imshow("img", img);
	namedWindow("dst", WINDOW_NORMAL);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}
