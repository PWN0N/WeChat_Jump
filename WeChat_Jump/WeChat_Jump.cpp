// WeChat_Jump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <opencv2/opencv.hpp>  
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace std;

int main()
{
	//Create SIFT class pointer
	Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
	Mat img_1 = imread("flag.png");
	Mat img_2 = imread("5.png");
	vector<KeyPoint> keypoints_1, keypoints_2;
	f2d->detect(img_1, keypoints_1);
	f2d->detect(img_2, keypoints_2);
	//Calculate descriptors (feature vectors)
	Mat descriptors_1, descriptors_2;
	f2d->compute(img_1, keypoints_1, descriptors_1);
	f2d->compute(img_2, keypoints_2, descriptors_2);
	//Matching descriptor vector using BFMatcher
	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_1, descriptors_2, matches);
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	namedWindow("result", WINDOW_NORMAL);
	imshow("result", img_matches);
	waitKey(0);
	return 0;
}
