
#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void parse(cv::Mat&);

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
