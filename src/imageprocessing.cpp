#include "imageprocessing.hpp"

void SignDetection::init_params()
{
	//define default color bounds
	m_cb.push_back((struct color_bound){
		RED,
		cv::Scalar(0, 110, 120),
		cv::Scalar(180, 255, 255)});
	m_cb.push_back((struct color_bound){
		GREEN,
		cv::Scalar(73, 75, 165),
		cv::Scalar(88, 255, 255)});
	m_cb.push_back((struct color_bound){
		BLUE,
		cv::Scalar(84, 100, 100),
		//cv::Scalar(130, 255, 255)});
		//testing
		cv::Scalar(130, 255, 255)});
	m_cb.push_back((struct color_bound){
		YELLOW,
		cv::Scalar(16, 170, 220),
		cv::Scalar(33, 255, 255)});
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

void SignDetection::setImParse(cv::Mat& im)
{
	m_im = im;
	parse();
}

void SignDetection::setColorBound(struct color_bound cb)
{
	m_cb[cb.c] = cb;
}

cv::Mat SignDetection::getCroppedPT()
{
	std::cout << m_cropped_pt_im.size() <<std::endl;
	return m_cropped_pt_im;
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
	//dilate_masks();

#if DISPLAY_ERROSION
	cv::imshow("erroded_r", m_masks[0]);
#endif

	//approximate shape of contour
	find_contour_masked(m_im);

	cv::imshow("contours", m_contours_im);

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

void SignDetection::dilate_masks()
{
	//apply dilation
	int n = 2;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
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
  	colors[2] = cv::Scalar(255, 0, 0);
	colors[3] = cv::Scalar(120, 120, 0);
	
	std::vector<cv::Mat1b>::iterator mask_it;
    cv::Mat contourImage(im.size(), CV_8UC3, cv::Scalar(0,0,0));
	int i = 0;
	for(mask_it = m_masks.begin(); mask_it != m_masks.end(); ++mask_it)
	{
		std::vector<std::vector<cv::Point>> contours;
		cv::Mat threshold = (*mask_it).clone();
		cv::threshold(threshold, threshold, 128, 255, cv::THRESH_BINARY);
		cv::Mat contourOutput = threshold.clone();
		cv::findContours(contourOutput, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

		//Draw the contours
    	for (size_t idx = 0; idx < contours.size(); idx++) {
    	    cv::drawContours(contourImage, contours, idx, colors[m_cb[i].c]);
    	}

		approximate_shape(im, contours, i);
		i++;
	}
	m_contours_im = contourImage;
}

//TODO improve this function for better curve analysis
void SignDetection::approximate_shape(cv::Mat im, std::vector<std::vector<cv::Point>> contours, int c)
{
	//approximate shape of contour
	double area = 0;
	int order = 0;
	double arclen = 0;
	double epsilon = 0;
	std::vector<std::vector<cv::Point>>::iterator contour;

	int i = 0;
	std::vector<cv::Point> approx_curve;
	for(contour = contours.begin(); contour != contours.end(); ++contour)
	{ 
		arclen = cv::arcLength((*contour), true);
		epsilon = 0.01*arclen;
		cv::approxPolyDP(*contour, approx_curve, epsilon, true);
		order = approx_curve.size();
		area = cv::contourArea(approx_curve);

		//TODO better polynome order test!
		if(order > 12 && order < 17 && c == RED && area > 1000 )
		{
			std::cout << "Red circle!\n"<<std::endl;
			//TODO too many curves to try and warp
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);
#endif

		}
		else if(order == 7 && c == BLUE && area > 1000){
			std::cout << "Forbidden parking!\n";
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);
#endif

		}
		else if (order == 8 && area > 1000 && c==RED)
		{
			std::cout<<"Red octagon\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		else if (order == 3 && area > 1000 && c==RED)
		{
			std::cout<<"Red triangle\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		else if (order == 4 && area > 1000 && c==YELLOW){
			std::cout<<"Yellow rectangle\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		else if(order == 4 && area >1000 && c == BLUE){
			std::cout<<"Blue rectangle\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		else if(order > 12 && order <15 && area >1000 && c == BLUE){
			std::cout<<"Blue Circle\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		else if(order == 4 && area >1000 && c == GREEN){
			std::cout<<"Green rectangle\n"<<std::endl;
#if DISPLAY_PT
			perspective_transform(im, *contour, c, i++);	
#endif
		}
		
		
	}
}

void SignDetection::perspective_transform(cv::Mat im, std::vector<cv::Point> approx_curve, int c, int tag)
{
	//get corners of contours and perform perspective transformation
	//TODO crop Rect box to save img
	std::string stamp = std::to_string(c) + std::to_string(tag);
	cv::Mat boxed_im = im.clone();

	cv::Rect crop_box = cv::boundingRect(approx_curve);
	cv::RotatedRect box = cv::minAreaRect(approx_curve);
	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; i++)
	{
        line(boxed_im, vertices[i], vertices[(i+1)%4], cv::Scalar(0,255,0), 2);
	}

	line(boxed_im, cv::Point(crop_box.x, crop_box.y), cv::Point(crop_box.x + crop_box.width, crop_box.y), cv::Scalar(0,255,255), 2);
	line(boxed_im, cv::Point(crop_box.x + crop_box.width, crop_box.y), cv::Point(crop_box.x + crop_box.width, crop_box.y + crop_box.height), cv::Scalar(0,255,255), 2);
	line(boxed_im, cv::Point(crop_box.x + crop_box.width, crop_box.y + crop_box.height), cv::Point(crop_box.x, crop_box.y + crop_box.height), cv::Scalar(0,255,255), 2);
	line(boxed_im, cv::Point(crop_box.x, crop_box.y + crop_box.height), cv::Point(crop_box.x, crop_box.y), cv::Scalar(0,255,255), 2);

	std::vector<std::vector<cv::Point>> curves;
	curves.push_back(approx_curve);
	cv::Mat filled_contour = cv::Mat::zeros(im.size(), CV_8UC1);
	cv::fillPoly(filled_contour, curves, cv::Scalar(255));

	cv::Mat cropped_im = im.clone(); 
	cv::cvtColor(filled_contour, filled_contour, cv::COLOR_GRAY2BGR);
	cv::bitwise_and(im, filled_contour, cropped_im);  // Update foreground with bitwise_and to extract real foreground
	
	cropped_im = cropped_im(crop_box);

	cv::imshow("boxed" + stamp, boxed_im);
	cv::imshow("cropped" + stamp, cropped_im);

	std::vector<cv::Point2f> src;
	src.push_back(vertices[0]);
	src.push_back(vertices[1]);
	src.push_back(vertices[2]);
	src.push_back(vertices[3]);

	float widthLT = sqrt((src[0].x - src[3].x)*(src[0].x - src[3].x) + (src[0].y - src[3].y)*(src[0].y - src[3].y));
	float widthBR = sqrt((src[1].x - src[2].x)*(src[1].x - src[2].x) + (src[1].y - src[2].y)*(src[1].y - src[2].y));
	int maxWidth = widthLT > widthBR ? (int)widthLT : (int)widthBR;

	float heightLB = sqrt((src[0].x - src[1].x)*(src[0].x - src[1].x) + (src[0].y - src[1].y)*(src[0].y - src[1].y));
	float heightRT = sqrt((src[2].x - src[3].x)*(src[2].x - src[3].x) + (src[2].y - src[3].y)*(src[2].y - src[3].y));
	int maxHeight = heightLB > heightRT ? (int)heightLB : (int)heightRT;

	std::vector<cv::Point2f> dst;
	dst.push_back(cv::Point2f(0, 0));
	dst.push_back(cv::Point2f(0, maxHeight - 1));
	dst.push_back(cv::Point2f(maxWidth - 1, maxHeight - 1));
	dst.push_back(cv::Point2f(maxWidth - 1, 0));

	cv::Mat M = cv::getPerspectiveTransform(src,dst);

	cv::Size warped_image_size = cv::Size(maxWidth, maxHeight); // +15 is for padding of the sign

	cv::Mat warpedImg;
	cv::Mat cropped_pt;
	cv::warpPerspective(im,warpedImg,M,warped_image_size,cv::INTER_LINEAR);

	m_cropped_pt_im = cropped_im;
	cv::imshow("Warped" + stamp, warpedImg);
}


