#ifndef OPENCV_SOLUTIONS
#define OPENCV_SOLUTIONS
#include <string>
#include <vector>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


class FloodFillSolution {

public:
	cv::Mat image0, image, gray, mask;
	int ffillMode = 1;
	int loDiff = 20, upDiff = 20;
	int connectivity = 4;
	int isColor = true;
	bool useMask = false;
	bool showWindow = false;
	int newMaskVal = 255;

	int width = 1920;	//
	int height = 1080;	//  these are default values
	int levels = 3;		//

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	std::vector<uchar> buf;
	std::vector<int> params = std::vector<int>();

public:
	static void help();

	static void onMouse(int event, int x, int y, int, void* me);

	void initOpenCVImage(char* filename);

	void initOpenCVWindow();

	int floodFillFromPoint(int x, int y);

	std::string getContoursFromPoint(int x, int y);

};



#endif // !OPENCV_SOLUTIONS
