#ifndef OPENCV_SOLUTIONS
#define OPENCV_SOLUTIONS
#include <string>

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

public:
	static void help();

	static void onMouse(int event, int x, int y, int, void* me);

	void initOpenCVImage(char* filename);

	void initOpenCVWindow();

	int floodFillFromPoint(int x, int y);

	std::string getContoursFromPoint(int x, int y);

};



#endif // !OPENCV_SOLUTIONS
