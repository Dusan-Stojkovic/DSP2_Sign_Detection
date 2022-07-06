#ifndef IMAGE_PROCESSING_HPP
#define IMAGE_PROCESSING_HPP

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

struct color_bound
{
	std::string color;
	cv::Scalar lower_b;
	cv::Scalar upper_b;
};

class SignDetection
{
public:
	SignDetection();
	SignDetection(cv::Mat);
	void parse();
	void setColorBound(struct color_bound);
	
private:
	cv::Mat m_im;
	std::vector<struct color_bound> m_cb;
	std::vector<cv::Mat1b> m_masks;
	std::vector<cv::Mat> m_contour_im;

	void init_params();
	void mask_im(cv::Mat);
	void errode_masks();
	void find_contour_masked(cv::Mat im);
	void approximate_shape(cv::Mat im, std::vector<std::vector<cv::Point>>);
};

#endif
