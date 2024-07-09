#include "../include/CleanupShadows.h"

CleanupShadows::CleanupShadows()
{
	//default constructor
}

CleanupShadows::~CleanupShadows()
{
	//no need to deallocate as cv::Mat will deallocate itself
	//http://docs.opencv.org/2.4/modules/core/doc/intro.html#automatic-memory-management
}

//method removes false shadows by subtracting object boundries
//Many times false shadows are detected around objcet boundries
Status CleanupShadows::RemoveObjectBoundries(Mat curFrame, Mat frgMask, Mat& detectedShadowsMask)
{
	try
	{
		if ((curFrame.channels() != 3) || (frgMask.channels() != 1))
		{
			return Status::FAILURE;
		}
		/***subtrack dialated canny from shadow to remove detected shadows around object***/
		int morph_size = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		Mat edge, grayFrame, roiMask;
		cvtColor(curFrame, grayFrame, COLOR_BGR2GRAY);

		//get canny edge detection to get object boundries
		Canny(grayFrame, edge, 50, 150);
		threshold(edge, edge, 100, 255, THRESH_BINARY);

		//dilate frg mask
		morphologyEx(frgMask, roiMask, MORPH_DILATE, element);
		morphologyEx(roiMask, roiMask, MORPH_DILATE, element);

		//keep only edges around detected objects
		bitwise_and(roiMask, edge, roiMask);

		//now only foreground edges remain; dialate those edges
		morphologyEx(roiMask, roiMask, MORPH_DILATE, element);

		//subtrack the dialated edge from shadows to remove false shadows
		detectedShadowsMask = detectedShadowsMask - roiMask;
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

//method rejects small shadow blobs and fills holes in larger blobs
Status CleanupShadows::RemoveNoisyObjects(Mat& detectedShadowsMask)
{
	try
	{
		Rect bounding_rect;
		vector<vector<Point>> contours;
		vector<int> bigContoursIndex;
		vector<Vec4i> hierarchy;
		//get contours
		//两个参数都改名了
		findContours(detectedShadowsMask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		Mat cleanedShadowMask = Mat::zeros(detectedShadowsMask.size(), detectedShadowsMask.type());

		if (contours.size() > 0)
		{
			// iterate through each contour.
			for (int i = 0; i < contours.size(); i++)
			{
				double area = contourArea(contours[i]);
				//is contour big enough
				if (area > config::MIN_CONTOUR_AREA)
				{
					bigContoursIndex.push_back(i);
				}
			}

			//draw big contors and fill holes
			Scalar shadowColor(255);
			for (int i = 0; i < bigContoursIndex.size(); i++)
			{
				drawContours(cleanedShadowMask, contours, bigContoursIndex[i], shadowColor, FILLED, 8, hierarchy, 0);
			}
		}

		detectedShadowsMask = cleanedShadowMask.clone();
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}