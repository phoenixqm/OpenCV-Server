#include "server_http.hpp"
#include "client_http.hpp"

//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//Added for the default_resource example
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

const int PORT = 8080;

//Added for the json-example:
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

//Added for the default_resource example
void default_resource_send(const HttpServer &server, const shared_ptr<HttpServer::Response> &response,
                           const shared_ptr<ifstream> &ifs);



class FloodFillSolution {
public:
	Mat image0, image, gray, mask;
	int ffillMode = 1;
	int loDiff = 20, upDiff = 20;
	int connectivity = 4;
	int isColor = true;
	bool useMask = false;
	bool showWindow = false;
	int newMaskVal = 255;


	static void help()
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


	static void onMouse(int event, int x, int y, int, void* me)
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


	void initOpenCVImage(char* filename) {
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


	void initOpenCVWindow(){

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

	int floodFillFromPoint(int x, int y) {

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

	string getContoursFromPoint(int x, int y) {

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
};


int main() {


	FloodFillSolution solution;
	solution.initOpenCVImage("web/floorplan2.png");
	//solution.initOpenCVWindow();

    HttpServer server(PORT, 1);
	cout << "listening port " << PORT << endl;

    server.resource["^/string$"]["POST"]=[](shared_ptr<HttpServer::Response> response, 
											shared_ptr<HttpServer::Request> request) {

        auto content=request->content.string();
        //request->content.string() is a convenience function for:
        //stringstream ss;
        //ss << request->content.rdbuf();
        //string content=ss.str();
        
        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    };
    

    server.resource["^/opencv-floodfill/([0-9]+)/([0-9]+)$"]["GET"]
		= [&server, &solution](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        int x = stoi(request->path_match[1]);
		int y = stoi(request->path_match[2]);
		string ret = solution.getContoursFromPoint(x, y);

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret.length() << "\r\n\r\n" << ret;
    };
    
    //Get example simulating heavy work in a separate thread
    server.resource["^/work$"]["GET"]
		= [&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
        thread work_thread([response] {
            this_thread::sleep_for(chrono::seconds(5));
            string message="Work done";
            *response << "HTTP/1.1 200 OK\r\nContent-Length: " << message.length() << "\r\n\r\n" << message;
        });
        work_thread.detach();
    };
    
    server.default_resource["GET"] = [&server](shared_ptr<HttpServer::Response> response, 
												shared_ptr<HttpServer::Request> request) {
        try {
            auto web_root_path = boost::filesystem::canonical("web");
            auto path=boost::filesystem::canonical(web_root_path/request->path);
            //Check if path is within web_root_path
            if(distance(web_root_path.begin(), web_root_path.end())>distance(path.begin(), path.end()) ||
               !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
                throw invalid_argument("path must be within root path");
            if(boost::filesystem::is_directory(path))
                path/="index.html";
            if(!(boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)))
                throw invalid_argument("file does not exist");
            
            auto ifs = make_shared<ifstream>();
            ifs->open(path.string(), ifstream::in | ios::binary);
            
            if (*ifs) {
                ifs->seekg(0, ios::end);
                auto length=ifs->tellg();
                
                ifs->seekg(0, ios::beg);
                
                *response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n";
                default_resource_send(server, response, ifs);
            }
            else
                throw invalid_argument("could not read file");
        }
        catch(const exception &e) {
            string content = "Could not open path "+request->path+": "+e.what();
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " 
						<< content.length() << "\r\n\r\n" << content;
        }
    };
    
    thread server_thread([&server](){
        //Start server
        server.start();
    });
    
          
    server_thread.join();
    
    return 0;
}

void default_resource_send(const HttpServer &server, const shared_ptr<HttpServer::Response> &response,
                           const shared_ptr<ifstream> &ifs) {
    //read and send 128 KB at a time
    static vector<char> buffer(131072); // Safe when server is running on one thread
    streamsize read_length;
    if((read_length=ifs->read(&buffer[0], buffer.size()).gcount())>0) {
        response->write(&buffer[0], read_length);
        if(read_length==static_cast<streamsize>(buffer.size())) {
            server.send(response, [&server, response, ifs](const boost::system::error_code &ec) {
                if(!ec)
                    default_resource_send(server, response, ifs);
                else
                    cerr << "Connection interrupted" << endl;
            });
        }
    }
}
