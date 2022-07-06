
#include <iostream>
#include "imageprocessing.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

int hue_r, value_r, saturation_r,
	hue_g, value_g, saturation_g,
	hue_b, value_b, saturation_b,
	hue_y, value_y, saturation_y;

static void onChangeRed(int,void*){
	hue_r = cv::getTrackbarPos("H_RED","Trackbars");
	value_r = cv::getTrackbarPos("V_RED","Trackbars");
	saturation_r = cv::getTrackbarPos("S_RED","Trackbars");
	
	std::cout<<hue_r<<value_r<<saturation_r<<std::endl;
}

static void onChangeGreen(int,void*){
	hue_g = cv::getTrackbarPos("H_GREEN","Trackbars");
	value_g = cv::getTrackbarPos("V_GREEN","Trackbars");
	saturation_g = cv::getTrackbarPos("S_GREEN","Trackbars");

	std::cout<<hue_g<<value_g<<saturation_g<<std::endl;
}

static void onChangeBlue(int,void*){
	hue_b = cv::getTrackbarPos("H_BLUE","Trackbars");
	value_b = cv::getTrackbarPos("V_BLUE","Trackbars");
	saturation_b = cv::getTrackbarPos("S_BLUE","Trackbars");

	std::cout<<hue_b<<value_b<<saturation_b<<std::endl;
}

static void onChangeYellow(int,void*){
	hue_y = cv::getTrackbarPos("H_YELLOW","Trackbars");
	value_y = cv::getTrackbarPos("V_YELLOW","Trackbars");
	saturation_y = cv::getTrackbarPos("S_YELLOW","Trackbars");
	
	std::cout<<hue_y<<value_y<<saturation_y<<std::endl;
}
	
int main()
{
	int h_min = 50, s_min =100, v_min=100;
	cv::namedWindow("Trackbars",cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H_RED","Trackbars", &h_min, 180, onChangeRed);
	cv::createTrackbar("S_RED","Trackbars", &s_min, 255, onChangeRed);
	cv::createTrackbar("V_RED","Trackbars", &v_min, 255, onChangeRed);
	
	cv::createTrackbar("H_GREEN","Trackbars", &h_min, 180, onChangeGreen);
	cv::createTrackbar("S_GREEN","Trackbars", &s_min, 255, onChangeGreen);
	cv::createTrackbar("V_GREEN","Trackbars", &v_min, 255, onChangeGreen);

	cv::createTrackbar("H_BLUE","Trackbars", &h_min, 180, onChangeBlue);
	cv::createTrackbar("S_BLUE","Trackbars", &s_min, 255, onChangeBlue);
	cv::createTrackbar("V_BLUE","Trackbars", &v_min, 255, onChangeBlue);

	cv::createTrackbar("H_YELLOW","Trackbars", &h_min, 180, onChangeYellow);
	cv::createTrackbar("S_YELLOW","Trackbars", &s_min, 255, onChangeYellow);
	cv::createTrackbar("V_YELLOW","Trackbars", &v_min, 255, onChangeYellow);

	//Variable setup
	std::cout << "Sign image processing..\n";
	cv::Mat image, resized_image;

	//Load image
	image = cv::imread("data/jednosmerna.png");

	//Resize for better performance
	int scale_per = 15;
	int w = image.size().width * scale_per / 100;
	int h = image.size().height * scale_per / 100;
	cv::resize(image, resized_image, cv::Size((int)w,(int)h), cv::INTER_CUBIC);

	//Display result
	cv::imshow("resized test", resized_image);

	//Do some image processing
	SignDetection det;
	det.parse(resized_image);

	return 0;
}


