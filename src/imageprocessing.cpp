#include "imageprocessing.hpp"

void SignDetection::init_params()
{
	//define default color bounds
	m_cb.push_back((struct color_bound){
		RED,
		cv::Scalar(165, 50, 129),
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

SignDetection::SignDetection()
{
	init_params();
}

SignDetection::SignDetection(cv::Mat im)
{
	init_params();
	m_im = im;
}

void SignDetection::setColorBound(struct color_bound cb)
{
	std::vector<struct color_bound>::iterator it;
	for(it = m_cb.begin(); it != m_cb.end(); ++it)
	{
		if(cb.c == (*it).c)
		{
			*it = cb;
			break;
		}
	}
}

void SignDetection::parse()
{
	//hsv transform
	cv::Mat hsv_im;
	cv::cvtColor(m_im, hsv_im, cv::COLOR_BGR2HSV);
	//cv::imshow("hsv test", hsv_im);
	//cv::moveWindow("hsv test", 0, 0);

	mask_im(hsv_im);
	
#if DISPLAY_MASK_R
	cv::imshow("hsv_red", m_masks[RED]);
#endif
#if DISPLAY_MASK_G
	cv::imshow("hsv_green", m_masks[GREEN]);
#endif
#if DISPLAY_MASK_B
	cv::imshow("hsv_blue", m_masks[BLUE]);
#endif
#if DISPLAY_MASK_Y
	cv::imshow("hsv_yellow", m_masks[YELLOW]);
#endif

	errode_masks();

#if DISPLAY_ERROSION
	cv::imshow("erroded_r", m_masks[0]);
#endif

	//approximate shape of contour
	find_contour_masked(hsv_im);

	cv::imshow("contours", m_contours_im);

	//clean up retard!
	m_masks.clear();
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
	int n = 3;
	cv::Mat kernel(n, n, CV_8UC1, 1);
	std::vector<cv::Mat1b>::iterator mask_it;
	for(mask_it = m_masks.begin(); mask_it != m_masks.end(); ++mask_it)
	{
		cv::Mat1b erroded;
		cv::erode(*mask_it, *mask_it, kernel);
	}
}

void SignDetection::find_contour_masked(cv::Mat im)
{
	//find contours
  	cv::Scalar colors[4];
  	colors[0] = cv::Scalar(0, 0, 255);
  	colors[1] = cv::Scalar(0, 255, 0);
	colors[2] = cv::Scalar(255, 255, 0);
  	colors[3] = cv::Scalar(255, 0, 0);
	std::vector<cv::Mat1b>::iterator mask_it;
    cv::Mat contourImage(im.size(), CV_8UC3, cv::Scalar(0,0,0));
	int i = 0;
	for(mask_it = m_masks.begin(); mask_it != m_masks.end(); ++mask_it)
	{
		std::vector<std::vector<cv::Point>> contours;
		cv::Mat threshold = (*mask_it).clone();
		cv::threshold(threshold, threshold, 128, 255, cv::THRESH_BINARY);
		cv::Mat contourOutput = threshold.clone();
		cv::findContours(contourOutput, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

		//Draw the contours
    	for (size_t idx = 0; idx < contours.size(); idx++) {
			//TODO draw contours on one image!
    	    cv::drawContours(contourImage, contours, idx, colors[m_cb[i].c]);
    	}
		i++;

		approximate_shape(im, contours);
	}
	m_contours_im = contourImage;
}

//TODO improve this function for better curve analysis
void SignDetection::approximate_shape(cv::Mat im, std::vector<std::vector<cv::Point>> contours)
{
	//approximate shape of contour
	int order = 0;
	double arclen = 0;
	double epsilon = 0;
	std::vector<std::vector<cv::Point>>::iterator contour;
	std::vector<cv::Point> approx_curve;
	int i = 0;
	for(contour = contours.begin(); contour != contours.end(); ++contour)
	{ 
		arclen = cv::arcLength((*contour), true);
		epsilon = 0.01*arclen;
		cv::approxPolyDP(*contour, approx_curve, epsilon, true);
		order = approx_curve.size();

		//TODO better polynome order test!
		if(order > 12 && order < 17)
		{
			std::cout << "Red circle!\n";
			//TODO too many curves to try and warp
			//perspective_transform(im, *contour, std::to_string(i++));	
		}
	}
}

void SignDetection::perspective_transform(cv::Mat im, std::vector<cv::Point> approx_curve, std::string tag)
{
	//get corners of contours and perform perspective transformation
	std::vector<cv::Point> exLeft;
	std::vector<cv::Point> exRight;
	std::vector<cv::Point> exTop;
	std::vector<cv::Point> exBott;

	cv::Point tmpLeft;
	cv::Point tmpRight;
	cv::Point tmpTop;
	cv::Point tmpBott;
	tmpLeft.x = 5000;
	tmpRight.x = -5000;
	tmpBott.y = -5000;
	tmpTop.y = 5000;

	std::vector<cv::Point> cnt;
	cnt = approx_curve;
	for (size_t j=0; j < cnt.size();j++){
		cv::Point current = cnt[j];
		if (current.x < tmpLeft.x){
			tmpLeft = current;
		}
		if (current.x > tmpRight.x){
			tmpRight = current;
		}
		if (current.y > tmpBott.y){
			tmpBott = current;
		}
		if (current.y < tmpTop.y){
			tmpTop = current;
		}
	}
	exLeft.push_back(tmpLeft);
	exRight.push_back(tmpRight);
	exTop.push_back(tmpTop);
	exBott.push_back(tmpBott);


	int farL = exLeft[0].x;
	int farR = exRight[0].x;
	int farT = exTop[0].y;
	int farB = exBott[0].y;
	
	for(int i = 0; i < exLeft.size();i++){
		if(farL > exLeft[i].x){
			farL = exLeft[i].x;
		}
	}
	for(int i = 0; i < exRight.size();i++){
		if(farR < exRight[i].x){
			farR = exRight[i].x;
		}
	}
	for(int i = 0; i < exTop.size();i++){
		if(farL > exTop[i].y){
			farL = exTop[i].y;
		}
	}
	for(int i = 0; i < exBott.size();i++){
		if(farB < exBott[i].y){
			farB = exBott[i].y;
		}
	}
	
	std::vector<cv::Point2f> src;
	src.push_back(cv::Point2f( (float)(farL),(float)(farT)));
	src.push_back(cv::Point2f( (float)(farR),(float)(farT)));
	src.push_back(cv::Point2f( (float)(farL),(float)(farB)));
	src.push_back(cv::Point2f( (float)(farR),(float)(farB)));
	
	
	std::vector<cv::Point2f> dst;
	dst.push_back(cv::Point2f( (float)(0),(float)(0)));
	dst.push_back(cv::Point2f( (float)(farR),(float)(0)));
	dst.push_back(cv::Point2f( (float)(0),(float)(farB)));
	dst.push_back(cv::Point2f( (float)(farR),(float)(farB)));
	

	cv::Mat M = cv::getPerspectiveTransform(src,dst);

	cv::Size warped_image_size = cv::Size(farR+15,farB+15); // +15 is for padding of the sign

	cv::Mat warpedImg;
	cv::warpPerspective(im,warpedImg,M,warped_image_size);

	//cv::imshow("Warped" + tag, warpedImg);
}


