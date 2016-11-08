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

#include"opencv_solutions.hpp"


using namespace std;

const int PORT = 8080;

//Added for the json-example:
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

//Added for the default_resource example
void default_resource_send(const HttpServer &server, const shared_ptr<HttpServer::Response> &response,
                           const shared_ptr<ifstream> &ifs);



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
