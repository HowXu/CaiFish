#ifndef _SHADOWREMOVER_H_
#define _SHADOWREMOVER_H_
#include <opencv2/opencv.hpp>
#include "iostream"
#include "ConfigParams.h"
using namespace std;
using namespace cv;

/*class to remove detected shadows*/
class ShadowRemover
{
private:
	Mat m_hsvCurFrame, m_hsvBkgFrame;
public:
	ShadowRemover();
	virtual ~ShadowRemover();
	Status RemoveShadow(Mat rgbBkgFrame, Mat detectedShadows, Mat& rgbCurFrame);
};
#endif // !_SHADOWREMOVER_H_

