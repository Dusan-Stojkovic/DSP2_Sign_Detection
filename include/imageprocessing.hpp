#ifndef IMAGE_PROCESSING_HPP
#define IMAGE_PROCESSING_HPP

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define DISPLAY_ERROSION 0
#define DISPLAY_MASK_R 1
#define DISPLAY_MASK_G 0
#define DISPLAY_MASK_B 0
#define DISPLAY_MASK_Y 0
#define DISPLAY_CONTOURS 1
#define DISPLAY_PT 0

enum shape
{
	CIRCLE,
	TRIANGLE,
	SQUARE
};

typedef enum 
{
	RED = 0,
	GREEN = 1,
	YELLOW = 2,
	BLUE = 3
}color;

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
	SignDetection(cv::Mat);
	void parse();
	void setColorBound(struct color_bound);
	
private:
	cv::Mat m_im;
	std::vector<struct color_bound> m_cb;
	std::vector<cv::Mat1b> m_masks;
	cv::Mat m_contours_im;

	void init_params();
	void mask_im(cv::Mat);
	void errode_masks();
	void find_contour_masked(cv::Mat im);
	void approximate_shape(cv::Mat im, std::vector<std::vector<cv::Point>>);
	void perspective_transform(cv::Mat, std::vector<cv::Point>, std::string);
};

#endif
