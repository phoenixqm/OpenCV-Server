

#include"opencv_solutions.hpp"

#include<fstream>
#include<iostream>
#include<vector>
#include<cmath>

using namespace cv;
using namespace std;


int levels = 3;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

static void on_trackbar(int, void*)
{
	Mat cnt_img = Mat::zeros(1082, 1922, CV_8UC3);
	int _levels = levels - 3;
	drawContours(cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128, 255, 255),
		3, CV_AA, hierarchy, std::abs(_levels));


	imwrite("web/contour-img.jpg", cnt_img);

	imshow("contours", cnt_img);
}

int test_findContours(int argc, char**)
{
	Mat img = imread("web/maskout.jpg", 1);

	Mat gray_image;
	cvtColor(img, gray_image, CV_BGR2GRAY);

	//show the faces
	namedWindow("image", 1);
	imshow("image", gray_image);

	//Extract the contours so that
	vector<vector<Point> > contours0;
	findContours(gray_image, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	contours.resize(contours0.size());
	for (size_t k = 0; k < contours0.size(); k++)
		approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

	namedWindow("contours", 1);
	createTrackbar("levels+3", "contours", &levels, 7, on_trackbar);

	on_trackbar(0, 0);
	waitKey();

	return 0;
}



void FloodFillSolution::help()
{
	cout << "\nThis program demonstrated the floodFill() function\n"
		"Call:\n"
		"./ffilldemo [image_name -- Default: fruits.jpg]\n" << endl;

	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tc - switch color/grayscale mode\n"
		"\tm - switch mask mode\n"
		"\tr - restore the original image\n"
		"\ts - use null-range floodfill\n"
		"\tf - use gradient floodfill with fixed(absolute) range\n"
		"\tg - use gradient floodfill with floating(relative) range\n"
		"\t4 - use 4-connectivity mode\n"
		"\t8 - use 8-connectivity mode\n" << endl;
}



void FloodFillSolution::onMouse(int event, int x, int y, int, void* me)
{
	FloodFillSolution* that = (FloodFillSolution*)me;
	if (event != CV_EVENT_LBUTTONDOWN)
		return;

	Point seed = Point(x, y);
	int lo = that->ffillMode == 0 ? 0 : that->loDiff;
	int up = that->ffillMode == 0 ? 0 : that->upDiff;
	int flags = that->connectivity + (that->newMaskVal << 8) +
		(that->ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
	int b = (unsigned)theRNG() & 255;
	int g = (unsigned)theRNG() & 255;
	int r = (unsigned)theRNG() & 255;
	Rect ccomp;

	Scalar newVal = that->isColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);
	Mat dst = that->isColor ? that->image : that->gray;
	int area;

	if (that->useMask)
	{
		threshold(that->mask, that->mask, 1, 128, CV_THRESH_BINARY);
		area = floodFill(dst, that->mask, seed, newVal, &ccomp, Scalar(lo, lo, lo),
			Scalar(up, up, up), flags);
		imshow("mask", that->mask);
	}
	else
	{
		area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo),
			Scalar(up, up, up), flags);
	}

	imshow("image", dst);
	cout << area << " pixels were repainted\n";
}



void FloodFillSolution::initOpenCVImage(char* filename) {

	//test_findContours(0, 0);

	ffillMode = 1;
	loDiff = 20, upDiff = 20;
	connectivity = 4;
	isColor = true;
	useMask = false;
	showWindow = false;
	newMaskVal = 255;

	levels = 3;


	image0 = imread(filename, 1);


	width = image0.size().width;
	height = image0.size().height;


	if (image0.empty())
	{
		cout << "Error: Image empty\n";
		return;
	}

	image0.copyTo(image);
	cvtColor(image0, gray, COLOR_BGR2GRAY);
	mask.create(image0.rows + 2, image0.cols + 2, CV_8UC1);

	//initOpenCVWindow();

	return;
}



void FloodFillSolution::initOpenCVWindow() {

	showWindow = true;

	namedWindow("image", 0);
	createTrackbar("lo_diff", "image", &loDiff, 255, 0);
	createTrackbar("up_diff", "image", &upDiff, 255, 0);


	setMouseCallback("image", onMouse, this);

	for (;;)
	{
		imshow("image", isColor ? image : gray);

		int c = waitKey(0);
		if ((c & 255) == 27)
		{
			cout << "Exiting ...\n";
			break;
		}
		switch ((char)c)
		{
		case 'c':
			if (isColor)
			{
				cout << "Grayscale mode is set\n";
				cvtColor(image0, gray, COLOR_BGR2GRAY);
				mask = Scalar::all(0);
				isColor = false;
			}
			else
			{
				cout << "Color mode is set\n";
				image0.copyTo(image);
				mask = Scalar::all(0);
				isColor = true;
			}
			break;
		case 'm':
			if (useMask)
			{
				destroyWindow("mask");
				useMask = false;
			}
			else
			{
				namedWindow("mask", 0);
				mask = Scalar::all(0);
				imshow("mask", mask);
				useMask = true;
			}
			break;
		case 'r':
			cout << "Original image is restored\n";
			image0.copyTo(image);
			cvtColor(image, gray, COLOR_BGR2GRAY);
			mask = Scalar::all(0);
			break;
		case 's':
			cout << "Simple floodfill mode is set\n";
			ffillMode = 0;
			break;
		case 'f':
			cout << "Fixed Range floodfill mode is set\n";
			ffillMode = 1;
			break;
		case 'g':
			cout << "Floating range mode is set\n";
			ffillMode = 2;
			break;
		case '4':
			cout << "4-connectivity mode is set\n";
			connectivity = 4;
			break;
		case '8':
			cout << "8-connectivity mode is set\n";
			connectivity = 8;
			break;
		}
	}

	return;

}

int FloodFillSolution::floodFillFromPoint(int x, int y) {

	Point seed = Point(x, y);
	int lo = ffillMode == 0 ? 0 : loDiff;
	int up = ffillMode == 0 ? 0 : upDiff;
	int flags = connectivity + (newMaskVal << 8) +
		(ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
	int b = (unsigned)theRNG() & 255;
	int g = (unsigned)theRNG() & 255;
	int r = (unsigned)theRNG() & 255;
	Rect ccomp;

	Scalar newVal = isColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);
	Mat dst = isColor ? image : gray;
	int area;


	threshold(mask, mask, 1, 128, CV_THRESH_BINARY);
	area = floodFill(dst, mask, seed, newVal, &ccomp, Scalar(lo, lo, lo),
		Scalar(up, up, up), flags);


	if (showWindow) {
		imshow("mask", mask);
		imshow("image", dst);
	}

	return area;
}

string FloodFillSolution::getContoursFromPoint(int x, int y) {

	useMask = true;
	mask = Scalar::all(0);

	if (showWindow) {
		namedWindow("mask", 0);
		imshow("mask", mask);
	}
	int area = floodFillFromPoint(x, y);
	
	imwrite("web/maskout.jpg", mask);


	imencode(".jpg", mask, buf, params);

	//Mat img = imread("web/maskout.jpg", 1);

	Mat img = imdecode(buf, 1);

	Mat gray_image;
	cvtColor(img, gray_image, CV_BGR2GRAY);

	vector<vector<Point> > contours0;
	findContours(gray_image, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	contours.resize(contours0.size());
	for (size_t k = 0; k < contours0.size(); k++)
		approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

	Mat cnt_img = Mat::zeros(height+2, width+2, CV_8UC3);


	int _levels = levels - 3;
	drawContours(cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128, 255, 255),
		3, CV_AA, hierarchy, std::abs(_levels));
	
	imwrite("web/contourout.jpg", cnt_img);

	cout << area << " pixels were repainted\n";
	return std::to_string(area) + " pixels were repainted\n";

}