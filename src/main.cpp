
#include <iostream>
#include <vector>
#include <thread>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void parse(cv::Mat&);
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
	
	parse(resized_image);
	

	return 0;
}

void parse(cv::Mat& im)
{
	//hsv transform
	cv::Mat hsv_im;
	cv::cvtColor(im, hsv_im, cv::COLOR_BGR2HSV);
	cv::imshow("hsv test", hsv_im);
	cv::moveWindow("hsv test", 0, 0);

	//create upper and lower limits of masks
	cv::Scalar masks[5][2] = 
	{
		//red mask
		//TODO recalculate this
		cv::Scalar(165, 50, 50),
		cv::Scalar(180, 255, 255),
		//cv::Scalar(0, 50, 5),
		//cv::Scalar(10, 255, 255),

		//green mask
		cv::Scalar(35, 50, 50),
		cv::Scalar(75, 255, 255),
		
		//blue mask
		cv::Scalar(84, 50, 50),
		cv::Scalar(130, 255, 255),

		//yellow mask
		cv::Scalar(27, 75, 75),
		cv::Scalar(33, 50, 50)
	};

	//apply masks
	cv::Mat1b hsv_r, hsv_g, hsv_b, hsv_y;
	cv::inRange(hsv_im, masks[0][0], masks[0][1], hsv_r);
	//TODO transform other colors
	//cv::inRange()
	//cv::inRange()
	
	//apply errosion
	int n = 5;
	cv::Mat kernel(n, n, CV_8UC1, 1);
	cv::Mat1b erroded_r, erroded_g, erroded_b, erroded_y;
	cv::erode(hsv_r, erroded_r, kernel);

	cv::imshow("hsv_r", hsv_r);

	cv::imshow("erroded_r", erroded_r);

	//find contours
	cv::Mat threshold_r = erroded_r.clone();
	std::vector<std::vector<cv::Point> > contours;
	cv::threshold(threshold_r, threshold_r, 128, 255, cv::THRESH_BINARY);
	cv::Mat contourOutput = threshold_r.clone();
	cv::findContours(contourOutput, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	//Draw the contours
    cv::Mat contourImage(hsv_im.size(), CV_8UC3, cv::Scalar(0,0,0));

    cv::Scalar colors[3];
    colors[0] = cv::Scalar(0, 0, 255);
    colors[1] = cv::Scalar(0, 255, 0);
    colors[2] = cv::Scalar(255, 0, 0);
    for (size_t idx = 0; idx < contours.size(); idx++) {
        cv::drawContours(contourImage, contours, idx, colors[idx % 3]);
    }

	cv::imshow("contours", contourImage);

	//approximate shape of contour
	int order = 0;
	double arclen = 0;
	double epsilon = 0;
	std::vector<std::vector<cv::Point>>::iterator contour;
	std::vector<cv::Point> approx_curve;
	for(contour = contours.begin(); contour != contours.end(); ++contour)
	{ 
		arclen = cv::arcLength((*contour), true);
		epsilon = 0.01*arclen;
		cv::approxPolyDP(*contour, approx_curve, epsilon, true);
		order = approx_curve.size();
		if(order > 15)
		{
			std::cout << "Red circle!\n";
		}
	}

	cv::waitKey();
}
