#ifndef _CLEANUPSHADOWS_H_
#define _CLEANUPSHADOWS_H_

#include<opencv2/opencv.hpp>
#include"iostream"
#include "ConfigParams.h"
using namespace std;
using namespace cv;

/*class to cleanup detected shadows*/
class CleanupShadows
{
private:
	//none
public:
	CleanupShadows();
	virtual ~CleanupShadows();
	Status RemoveObjectBoundries(Mat curFrame, Mat frgMask, Mat& detectedShadowsMask);
	Status RemoveNoisyObjects(Mat& detectedShadowsMask);
};
#endif // !_CLEANUPSHADOWS_H_

