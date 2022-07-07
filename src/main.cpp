
#include <thread>
#include <iostream>
#include "imageprocessing.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//to draw trackbar set to 1
#define DRAW_T_HSV_R 1
#define DRAW_T_HSV_G 0
#define DRAW_T_HSV_B 0
#define DRAW_T_HSV_Y 0

#if DRAW_T_HSV_R
static void onChangeRed(int i,void* sd);
#endif

#if DRAW_T_HSV_G
static void onChangeGreen(int i,void* sd);
#endif

#if DRAW_T_HSV_B
static void onChangeBlue(int i, void* sd);
#endif

#if DRAW_T_HSV_Y
static void onChangeYellow(int i, void* sd);
#endif
	
void imageProcessingThread(cv::Mat im);

int main()
{
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
	//cv::imshow("resized test", resized_image);
	std::thread t(imageProcessingThread, resized_image);

	std::string keyboard_input;
	//TODO trackbars must spawn new thread
	do
	{
		std::cin >> keyboard_input;

		//echo
		std::cout << keyboard_input;

		if(keyboard_input == "save")
		{
			//TODO write to file trackbar values 
		}

	}while(keyboard_input != "quit");

	return 0;
}

void imageProcessingThread(cv::Mat im)
{
	//Do some image processing
	SignDetection det(im);

	det.parse();

#if DRAW_T_HSV_R 
	int r_h_min = 165, r_s_min = 50, r_v_min = 129;
	int r_h_high = 180, r_s_high = 255, r_v_high = 255;
	cv::namedWindow("RED",cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H_RED HIGH","RED", &r_h_high, 180, onChangeRed, (void*)&det);
	cv::createTrackbar("H_RED LOW","RED", &r_h_min, 180, onChangeRed, (void*)&det);
	cv::createTrackbar("S_RED HIGH","RED", &r_s_high, 255, onChangeRed, (void*)&det);
	cv::createTrackbar("S_RED LOW","RED", &r_s_min, 180, onChangeRed, (void*)&det);
	cv::createTrackbar("V_RED HIGH","RED", &r_v_high, 255, onChangeRed, (void*)&det);
	cv::createTrackbar("V_RED LOW","RED", &r_v_min, 255, onChangeRed, (void*)&det);
#endif

#if DRAW_T_HSV_G
	int g_h_min = 35, g_s_min = 50, g_v_min = 50;
	int g_h_high = 75, g_s_high = 255, g_v_high = 255;
	cv::namedWindow("GREEN",cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H_GREEN HIGH","GREEN", &g_h_high, 180, onChangeGreen, (void*)&det);
	cv::createTrackbar("H_GREEN LOW","GREEN", &g_h_min, 180, onChangeGreen, (void*)&det);
	cv::createTrackbar("S_GREEN HIGH","GREEN", &g_s_high, 255, onChangeGreen, (void*)&det);
	cv::createTrackbar("S_GREEN LOW","GREEN", &g_s_min, 255, onChangeGreen, (void*)&det);
	cv::createTrackbar("V_GREEN HIGH","GREEN", &g_v_high, 255, onChangeGreen, (void*)&det);
	cv::createTrackbar("V_GREEN LOW","GREEN", &g_v_min, 255, onChangeGreen, (void*)&det);
#endif

#if DRAW_T_HSV_B
	int b_h_min = 84, b_s_min = 50, b_v_min = 50;
	int b_h_high = 130, b_s_high = 184, b_v_high = 255;
	cv::namedWindow("BLUE",cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H_BLUE HIGH","BLUE", &b_h_high, 180, onChangeBlue, (void*)&det);
	cv::createTrackbar("H_BLUE LOW","BLUE", &b_h_min, 180, onChangeBlue, (void*)&det);
	cv::createTrackbar("S_BLUE HIGH","BLUE", &b_s_high, 255, onChangeBlue, (void*)&det);
	cv::createTrackbar("S_BLUE LOW","BLUE", &b_s_min, 255, onChangeBlue, (void*)&det);
	cv::createTrackbar("V_BLUE HIGH","BLUE", &b_v_high, 255, onChangeBlue, (void*)&det);
	cv::createTrackbar("V_BLUE LOW","BLUE", &b_v_min, 255, onChangeBlue, (void*)&det);
#endif

#if DRAW_T_HSV_Y
	int y_h_min = 27, y_s_min = 75, y_v_min = 75;
	int y_h_high = 33, y_s_high = 50, y_v_high = 50;
	cv::namedWindow("YELLOW",cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("H_YELLOW HIGH","YELLOW", &y_h_high, 180, onChangeYellow, (void*)&det);
	cv::createTrackbar("H_YELLOW LOW","YELLOW", &y_h_min, 180, onChangeYellow, (void*)&det);
	cv::createTrackbar("S_YELLOW HIGH","YELLOW", &y_s_high, 255, onChangeYellow, (void*)&det);
	cv::createTrackbar("S_YELLOW LOW","YELLOW", &y_s_min, 255, onChangeYellow, (void*)&det);
	cv::createTrackbar("V_YELLOW HIGH","YELLOW", &y_v_high, 255, onChangeYellow, (void*)&det);
	cv::createTrackbar("V_YELLOW LOW","YELLOW", &y_v_min, 255, onChangeYellow, (void*)&det);
#endif

	cv::waitKey(0);
} 

static void onChangeRed(int i,void* sd){
	int hue_l, value_l, saturation_l;
	int hue_h, value_h, saturation_h;
	hue_h = cv::getTrackbarPos("H_RED HIGH","RED");
	value_h = cv::getTrackbarPos("V_RED HIGH","RED");
	saturation_h = cv::getTrackbarPos("S_RED HIGH","RED");
	hue_l = cv::getTrackbarPos("H_RED LOW","RED");
	value_l = cv::getTrackbarPos("V_RED LOW","RED");
	saturation_l = cv::getTrackbarPos("S_RED LOW","RED");
	
	SignDetection* det = (SignDetection*)sd;
	cv::Scalar s_lower(hue_l, value_l, saturation_l);
	cv::Scalar s_higher(hue_h, value_h, saturation_h);
	det->setColorBound((struct color_bound){RED, s_lower, s_higher});
	det->parse();
}

static void onChangeGreen(int i,void* sd){
	int hue_l, value_l, saturation_l;
	int hue_h, value_h, saturation_h;
	hue_h = cv::getTrackbarPos("H_GREEN HIGH","GREEN");
	value_h = cv::getTrackbarPos("V_GREEN HIGH","GREEN");
	saturation_h = cv::getTrackbarPos("S_GREEN HIGH","GREEN");
	hue_h = cv::getTrackbarPos("H_GREEN LOW","GREEN");
	value_h = cv::getTrackbarPos("V_GREEN LOW","GREEN");
	saturation_h = cv::getTrackbarPos("S_GREEN LOW","GREEN");
	
	SignDetection* det = (SignDetection*)sd;
	cv::Scalar s_lower(hue_l, value_l, saturation_l);
	cv::Scalar s_higher(hue_h, value_h, saturation_h);
	(*det).setColorBound((struct color_bound){GREEN, s_lower, s_higher});
	(*det).parse();
}

static void onChangeBlue(int i, void* sd){
	int hue_l, value_l, saturation_l;
	int hue_h, value_h, saturation_h;
	hue_h = cv::getTrackbarPos("H_BLUE HIGH","BLUE");
	value_h = cv::getTrackbarPos("V_BLUE HIGH","BLUE");
	saturation_h = cv::getTrackbarPos("S_BLUE HIGH","BLUE");
	hue_h = cv::getTrackbarPos("H_BLUE LOW","BLUE");
	value_h = cv::getTrackbarPos("V_BLUE LOW","BLUE");
	saturation_h = cv::getTrackbarPos("S_BLUE LOW","BLUE");
	
	SignDetection* det = (SignDetection*)sd;
	cv::Scalar s_lower(hue_l, value_l, saturation_l);
	cv::Scalar s_higher(hue_h, value_h, saturation_h);
	(*det).setColorBound((struct color_bound){BLUE, s_lower, s_higher});
	(*det).parse();
}

static void onChangeYellow(int i, void* sd){
	int hue_l, value_l, saturation_l;
	int hue_h, value_h, saturation_h;
	hue_h = cv::getTrackbarPos("H_YELLOW HIGH","YELLOW");
	value_h = cv::getTrackbarPos("V_YELLOW HIGH","YELLOW");
	saturation_h = cv::getTrackbarPos("S_YELLOW HIGH","YELLOW");
	hue_l = cv::getTrackbarPos("H_YELLOW LOW","YELLOW");
	value_l = cv::getTrackbarPos("V_YELLOW LOW","YELLOW");
	saturation_l = cv::getTrackbarPos("S_YELLOW LOW","YELLOW");
	
	SignDetection* det = (SignDetection*)sd;
	cv::Scalar s_lower(hue_l, value_l, saturation_l);
	cv::Scalar s_higher(hue_h, value_h, saturation_h);
	(*det).setColorBound((struct color_bound){YELLOW, s_lower, s_higher});
	(*det).parse();
}

