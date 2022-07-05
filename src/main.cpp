
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
	std::cout << "Hello world!\n";
	cv::Mat image = cv::imread("data/jednosmerna.png");
	cv::imshow("test", image);
	cv::waitKey(0);
	return 0;
}
