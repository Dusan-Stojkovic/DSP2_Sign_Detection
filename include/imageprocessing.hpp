#ifndef IMAGE_PROCESSING_HPP
#define IMAGE_PROCESSING_HPP

//#include <vector>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

enum shape
{
	CIRCLE,
	TRIANGLE,
	SQUARE
};

enum color
{
	RED,
	GREEN,
	BLUE,
	YELLOW
};

struct color_bound
{
	color c;
	cv::Scalar lower_b;
	cv::Scalar upper_b;
};

class SignDetection
{
public:
	SignDetection();
	void parse(cv::Mat&);
	
private:
	std::vector<struct color_bound> m_cb;
	std::vector<cv::Mat1b> m_masks;

	void mask_im(cv::Mat);
	void errode_masks();
	void find_contour_masked();
};

#endif
