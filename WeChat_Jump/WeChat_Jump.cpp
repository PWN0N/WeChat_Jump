// WeChat_Jump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

static int thresh = 4000; //1000
static int N = 32; //16
static int NUM_SHARPEN = 5;

Mat getProjectiveTransImg(Mat src_img);
Point getJumperPosition(Mat src_img);
Point getDestPlatformPosition(Mat trans_img);
static double angle(Point pt1, Point pt2, Point pt0);
int findSquares(Mat& image, vector<vector<Point> >& squares);
void sharpenImage(const Mat &image, Mat &result);
Rect findRectInfo(vector<Point> rect);
void transformPointsForward(std::vector<cv::Point2i> &points1,
	std::vector<cv::Point2i> &points2,
	cv::Mat affineMat);
Point getCenterPoint(Rect rect);


int main()
{
	RNG rng;
	Mat img = imread("1.png");
	
	//namedWindow("transed", WINDOW_NORMAL);
	Mat imgtransed = getProjectiveTransImg(img);

	
	//imshow("transed", imgtransed);

	Point PlatformPosition = getDestPlatformPosition(getProjectiveTransImg(img));
	circle(img, PlatformPosition, 5, Scalar(0, 0, 255), 5, 8, 0);
	cout << "目标平面中心坐标为：" << PlatformPosition << endl << endl;

	Point Jumper_pose;
	Jumper_pose = getJumperPosition(img);
	circle(img, Jumper_pose, 5, Scalar(0, 0, 255), 5, 8, 0);
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
	Mat trans = Mat(2, 3, CV_64FC1, transtemp);
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
	Jumper_position.y = Jumper_position.y + (temp.rows * 0.86);

	return Jumper_position;
}

Point getDestPlatformPosition(Mat trans_img)
{
	Point Platform_position;

	vector<vector<Point> > squares;
	int RectId = findSquares(trans_img, squares);
	if (RectId == -1)
	{
		cerr << "找不到平面 " << endl;
		Platform_position.x = 0;
		Platform_position.y = 0;
		return Platform_position;
	}
	vector<Point> tempSquares;
	
	//for (size_t i = 0; i<squares.size(); i++)
	//{
	//	Scalar color = Scalar(0, 0, 255);//rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	//	const Point * pc = &(squares[RectId][0]);
	//	int nc = squares[RectId].size();
	//	polylines(trans_img, &pc, &nc, 1, 0, color);
		for (size_t j = 0; j<squares[RectId].size(); j++)
		{
			circle(trans_img, squares[RectId][j], 5, Scalar(0, 0, 255), 5, 8, 0);
			cerr << squares[RectId][j] << " ";
			tempSquares.push_back(squares[RectId][j]);
		}
		cerr << endl;
	//}
	cerr << squares.size() << "square(s) found " << endl;

	Point2f src_position[3];
	Point2f dst_position[3];

	src_position[0] = Point2f(586, 1102);
	src_position[1] = Point2f(1056, 1102);
	src_position[2] = Point2f(821, 967);

	dst_position[0] = Point2f(586, 1102);
	dst_position[1] = Point2f(918, 1435);
	dst_position[2] = Point2f(918, 1102);

	Mat transback = getAffineTransform(dst_position,src_position);
	vector<Point> finalRect;
	transformPointsForward(tempSquares, finalRect,transback);

	Rect findFinalRect = findRectInfo(finalRect);
	Point centerPoint = getCenterPoint(findFinalRect);
	circle(trans_img, centerPoint, 5, Scalar(255, 0, 255), 5, 8, 0);
	for (size_t j = 0; j<finalRect.size(); j++)
	{
		circle(trans_img, finalRect[j], 5, Scalar(255, 0, 255), 5, 8, 0);
		cerr << finalRect[j] << "finalRect coroners " << endl;

	}

	//namedWindow("DestPlatform", WINDOW_NORMAL);
	//imshow("DestPlatform", trans_img);
	//imwrite("DestPlatform.jpg",trans_img);

	Platform_position = centerPoint;

	return Platform_position;
}

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int findSquares(Mat& image, vector<vector<Point> >& squares)
{
	squares.clear();

	Mat pyr, timg, gray0, gray;

	if (image.empty()) return -1;

	gray0 = Mat(image.size(), CV_8U);

	GaussianBlur(image, pyr, Size(0, 0), 3);
	addWeighted(image, 1.25, pyr, -0.25, 0, timg);
	for (int m = 0; m<NUM_SHARPEN; ++m)
		addWeighted(timg, 1.25, pyr, -0.25, 0, timg);

	vector<vector<Point> > contours;

	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);
		for (int l = 1; l < N; l++)
		{
			gray = gray0 >= (l + 1) * 255 / N;
			findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
			vector<Point> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), approx,
				arcLength(Mat(contours[i]), true)*0.02, true);
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 1000 &&
					isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;
					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					if (maxCosine < 0.05)
						squares.push_back(approx);
				}
			}
		}
	}

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	int maxRectArea = 0;
	int maxRectAreaId = -1;
	for (int i = 0; i<squares.size(); i++)
	{	
		cout << i << endl;
		Rect roi = findRectInfo(squares[i]);
		cout << "width:" << roi.width << endl;
		cout << "height:" << roi.height << endl;
		cout << "<<<<<<<<<<<<<" << endl;
		
		
		
		int currentRectArea = roi.width*roi.height;
		if (currentRectArea > maxRectArea)
		{
			maxRectArea = currentRectArea;
			maxRectAreaId = i;
            
		}
	}

	
	return maxRectAreaId;
}

Point getCenterPoint(Rect rect)
{
	Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0);
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}

void sharpenImage(const Mat &image, Mat &result)
{
	//为输出图像分配内存
	result.create(image.size(), image.type());


	for (int j = 1; j<image.rows - 1; ++j)
	{
		const uchar *previous = image.ptr<const uchar>(j - 1);
		const uchar *current = image.ptr<const uchar>(j);
		const uchar *next = image.ptr<const uchar>(j + 1);
		uchar *output = result.ptr<uchar>(j);
		for (int i = 1; i<image.cols - 1; ++i)
		{
			*output++ = saturate_cast<uchar>(5 * current[i] - previous[i] - next[i] - current[i - 1] - current[i + 1]);  //saturate_cast<uchar>()保证结果在uchar范围内
		}
	}
	result.row(0).setTo(Scalar(0));
	result.row(result.rows - 1).setTo(Scalar(0));
	result.col(0).setTo(Scalar(0));
	result.col(result.cols - 1).setTo(Scalar(0));
}
Rect findRectInfo(vector<Point> rect)
{
	int rectInfo[6] = { 0 };
	int x[4] = { 0 }, y[4] = { 0 };
	int maxX = 0, maxY = 0, minX = 2000, minY = 2000;
	//get the rect points
	for (int i = 0; i<4; i++)
	{
		x[i] = rect[i].x;
		y[i] = rect[i].y;

		if (maxX<x[i])
			maxX = x[i];
		if (maxY<y[i])
			maxY = y[i];
		if (minX>x[i])
			minX = x[i];
		if (minY>y[i])
			minY = y[i];
	}
	rectInfo[0] = minY;
	rectInfo[1] = minX;
	rectInfo[2] = maxY - minY;
	rectInfo[3] = maxX - minX;
	cout << "minY=" << minY << endl;
	cout << "minX=" << minX << endl;
	cout << "maxY - minY=" << maxY - minY << endl;
	cout << "maxX - minX=" << maxX - minX << endl;

	Rect roi(rectInfo[1], rectInfo[0], rectInfo[3], rectInfo[2]);
	return roi;// 得到矩形的左上角的坐标和矩形的边长
}
void transformPointsForward(std::vector<cv::Point2i> &points1,
	std::vector<cv::Point2i> &points2,
	cv::Mat affineMat)
{
	int len = points1.size();
	std::vector<cv::Point2i> _points1;
	if (points1 != points2)
	{
		_points1 = points1;
	}
	else
	{
		_points1 = std::vector<cv::Point2i>(len);
		for (int i = 0; i<len; i++)
			_points1[i] = points1[i];
	}
	points2.clear();

	if (affineMat.type() == 5)
	{
		for (int i = 0; i<_points1.size(); i++)
		{
			cv::Point2i out = cv::Point2i();
			out.x = (int)(affineMat.at<float>(0, 0) * (float)_points1[i].x
				+ affineMat.at<float>(0, 1) * (float)_points1[i].y
				+ affineMat.at<float>(0, 2) + 0.5);
			out.y = (int)(affineMat.at<float>(1, 0) * (float)_points1[i].x
				+ affineMat.at<float>(1, 1) * (float)_points1[i].y
				+ affineMat.at<float>(1, 2) + 0.5);

			points2.push_back(out);
		}
	}
	else if (affineMat.type() == 6)
	{
		for (int i = 0; i<_points1.size(); i++)
		{
			cv::Point2i out = cv::Point2i();
			out.x = (int)(affineMat.at<double>(0, 0) * (double)_points1[i].x
				+ affineMat.at<double>(0, 1) * (double)_points1[i].y
				+ affineMat.at<double>(0, 2));
			out.y = (int)(affineMat.at<double>(1, 0) * (double)_points1[i].x
				+ affineMat.at<double>(1, 1) * (double)_points1[i].y
				+ affineMat.at<double>(1, 2));

			points2.push_back(out);
		}
	}

	return;
}