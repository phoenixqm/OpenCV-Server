

#include"opencv_solutions.hpp"

#include<fstream>
#include<iostream>

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
	image0 = imread(filename, 1);

	if (image0.empty())
	{
		cout << "Image empty. Usage: ffilldemo <image_name>\n";
		return;
	}

	image0.copyTo(image);
	cvtColor(image0, gray, COLOR_BGR2GRAY);
	mask.create(image0.rows + 2, image0.cols + 2, CV_8UC1);


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
			cout << "Gradient (floating range) floodfill mode is set\n";
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


	cout << area << " pixels were repainted\n";
	return std::to_string(area) + " pixels were repainted\n";
}