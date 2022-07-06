#include "imageprocessing.hpp"

SignDetection::SignDetection()
{
	//define default color bounds
	m_cb.push_back((struct color_bound){
		RED,
		cv::Scalar(165, 50, 50),
		cv::Scalar(180, 255, 255)});
	m_cb.push_back((struct color_bound){
		GREEN,
		cv::Scalar(35, 50, 50),
		cv::Scalar(75, 255, 255)});
	m_cb.push_back((struct color_bound){
		YELLOW,
		cv::Scalar(27, 75, 75),
		cv::Scalar(33, 50, 50)});
	m_cb.push_back((struct color_bound){
		BLUE,
		cv::Scalar(84, 50, 50),
		cv::Scalar(130, 255, 255)});
}

void SignDetection::parse(cv::Mat& im)
{
	//hsv transform
	cv::Mat hsv_im;
	cv::cvtColor(im, hsv_im, cv::COLOR_BGR2HSV);
	cv::imshow("hsv test", hsv_im);
	cv::moveWindow("hsv test", 0, 0);

	mask_im(hsv_im);
	cv::imshow("hsv_r", m_masks[0]);

	errode_masks();
	cv::imshow("erroded_r", m_masks[0]);

	//find contours
	cv::Mat threshold_r = m_masks[0].clone();
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

void SignDetection::mask_im(cv::Mat im)
{
	//apply masks
	std::vector<struct color_bound>::iterator cb_it;
	for(cb_it = m_cb.begin(); cb_it != m_cb.end(); ++cb_it)
	{
		cv::Mat1b mask;
		cv::inRange(im, (*cb_it).lower_b, (*cb_it).upper_b, mask);
		m_masks.push_back(mask);
	}
}

void SignDetection::errode_masks()
{
	//apply errosion
	int n = 5;
	cv::Mat kernel(n, n, CV_8UC1, 1);
	std::vector<cv::Mat1b>::iterator mask_it;
	for(mask_it = m_masks.begin(); mask_it != m_masks.end(); ++mask_it)
	{
		cv::Mat1b erroded;
		cv::erode(*mask_it, *mask_it, kernel);
	}
}
