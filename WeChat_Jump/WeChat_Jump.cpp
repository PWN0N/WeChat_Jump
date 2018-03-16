// WeChat_Jump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

Mat getProjectiveTransImg(Mat src_img);
Point getJumperPosition(Mat src_img);

int main()
{
	Mat img = imread("7.png");
	
	namedWindow("transed", WINDOW_NORMAL);
	imshow("transed", getProjectiveTransImg(img));

	Point Jumper_pose;
	Jumper_pose = getJumperPosition(img);
	circle(img, Jumper_pose, 10, Scalar(0, 0, 255), 2, 8, 0);
	cout << "棋子坐标为：" << Jumper_pose << endl << endl;
	namedWindow("img", WINDOW_NORMAL);
	imshow("img", img);

	waitKey(0);
	return 0;
}

Mat getProjectiveTransImg(Mat src_img) 
{
	//Point2f src_position[3];
	//Point2f dst_position[3];

	//src_position[0] = Point2f(586, 1102);
	//src_position[1] = Point2f(1056, 1102);
	//src_position[2] = Point2f(821, 967);

	//dst_position[0] = Point2f(586, 1102);
	//dst_position[1] = Point2f(918, 1435);
	//dst_position[2] = Point2f(918, 1102);

	//Mat trans = getAffineTransform(src_position, dst_position);

	Mat transedImg;
	double transtemp[2][3] = { 0.7063829787234042, -1.229629629629629,1527.111426319937, 
		0.7085106382978724, 1.233333333333333, -672.3205673758865 };
	Mat trans = cv::Mat(2, 3, CV_64FC1, transtemp);
	transedImg = Mat::zeros(src_img.rows, src_img.cols, src_img.type());
	warpAffine(src_img, transedImg, trans, transedImg.size());

	return transedImg;
}

Point getJumperPosition(Mat src_img)
{
	Point Jumper_position;
	Mat temp = imread("flag.png");
	Mat result;
	int result_cols = src_img.cols - temp.cols + 1;
	int result_rows = src_img.rows - temp.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);

	matchTemplate(src_img, temp, result, TM_SQDIFF);//标准平方差匹配
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

	//rectangle(src_img, matchLoc, Point(matchLoc.x + temp.cols, matchLoc.y + temp.rows), Scalar(0, 255, 0), 2, 8, 0);
	Jumper_position = minLoc;
	Jumper_position.x = Jumper_position.x + (temp.cols / 2);
	Jumper_position.y = Jumper_position.y + (temp.rows * 0.85);

	return Jumper_position;
}

Point2f getDestPlatform(Mat trans_img)
{
	Point2f Platform_position;

	return Platform_position;
}
