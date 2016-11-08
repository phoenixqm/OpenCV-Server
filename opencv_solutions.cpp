

#include"opencv_solutions.hpp"

#include<fstream>
#include<iostream>
#include<vector>
#include<cmath>

using namespace cv;
using namespace std;



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

// ref http://stackoverflow.com/questions/1716274/fill-the-holes-in-opencv

void fillEdgeImage(cv::Mat edgesIn, cv::Mat& filledEdgesOut)
{
	cv::Mat edgesNeg = edgesIn.clone();

	// fill point can not from (0,0), its buggy
	cv::floodFill(edgesNeg, cv::Point(1, 100), CV_RGB(255, 255, 255));
	//imwrite("web/neg.jpg", edgesNeg);
	bitwise_not(edgesNeg, edgesNeg);
	//imwrite("web/not.jpg", edgesNeg);
	filledEdgesOut = (edgesNeg | edgesIn);
	//imwrite("web/or.jpg", filledEdgesOut);

	return;
}

string FloodFillSolution::getContoursFromPoint(int x, int y) {

	useMask = true;
	mask = Scalar::all(0);

	if (showWindow) {
		namedWindow("mask", 0);
		imshow("mask", mask);
	}
	int area = floodFillFromPoint(x, y);
	if (area > 60000) {
		return "none";
	}
	fillEdgeImage(mask, mask);


	//int dilation_type = MORPH_RECT;
	//// int dilation_type = MORPH_CROSS;
	//// int dilation_type = MORPH_ELLIPSE;

	//int dilation_size = 2;

	//Mat element = getStructuringElement(dilation_type,
	//	Size(2 * dilation_size + 1, 2 * dilation_size + 1),
	//	Point(dilation_size, dilation_size));
	///// Apply the dilation operation
	//dilate(mask, mask, element);
	////dilate(mask, mask, element);
	//medianBlur(mask, mask, 5);
	////erode(mask, mask, element);

	imwrite("web/maskout.jpg", mask);


	imencode(".jpg", mask, buf, params);

	//Mat img = imread("web/maskout.jpg", 1);

	Mat img = imdecode(buf, 1);

	Mat gray_image;
	cvtColor(img, gray_image, CV_BGR2GRAY);
	
	vector<vector<Point> > contours0;
	findContours(gray_image, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	//findContours(gray_image, contours0, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	contours.resize(contours0.size());
	for (size_t k = 0; k < contours0.size(); k++)
		approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

	int largestIndex = 0;
	int largestLength = 0;
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() <= 4) {
			continue;
		}
		if (contours[i][0].x < 50 || contours[i][0].y < 50) {
			continue;
		}
		if (contours[i].size() > largestLength) {
			largestLength = contours[i].size();
			largestIndex = i;
		}
	}
	vector<vector<Point> > thecontour;
	thecontour.push_back(contours[largestIndex]);

	Mat cnt_img = Mat::zeros(height + 2, width + 2, CV_8UC3);


	polylines(cnt_img, contours[largestIndex], true, Scalar(128, 255, 255), 3);



	imwrite("web/contourout.jpg", cnt_img);

	//// stringfy all contours
	//string data = "[";

	//for (int i = 0; i < contours.size(); i++) {
	//	data += "[";
	//	for (int j = 0; j < contours[i].size(); j++) {
	//		data += to_string(contours[i][j].x);
	//		data += ", ";
	//		data += to_string(contours[i][j].y);
	//		if (j < contours[i].size() - 1) {
	//			data += ", ";
	//		}

	//	}
	//	data += "]";
	//	if (i < contours.size() - 1) {
	//		data += ", \n";
	//	}
	//}
	//data += "]";


	// stringfy only the largest contour
	string data = "[";	
	for (int j = 0; j < contours[largestIndex].size(); j++) {
		data += to_string(contours[largestIndex][j].x);
		data += ", ";
		data += to_string(contours[largestIndex][j].y);
		if (j < contours[largestIndex].size() - 1) {
			data += ", ";
		}
	}
	data += "]";

	cout << area << " pixels were repainted\n";
	cout << contours.size() << " contours were detected\n";
	cout << "largest contour is " << data << endl;
	return data;

}