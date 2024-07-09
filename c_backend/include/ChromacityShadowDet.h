#ifndef CHROMATICITYSHADOWDET_H_
#define CHROMATICITYSHADOWDET_H_

#include <opencv2/opencv.hpp>
#include "iostream"
#include "ConfigParams.h"
using namespace std;
using namespace cv;

/*Class to detect shadows using cromacity values*/
class ChromacityShadowDet
{
private:
	Mat m_hsvCurFrame, m_hsvBkgFrame;
	const float m_vThreshUpperLowAtten = 1;
	const float m_vThreshUpperHighAtten = 0.99;
	const float m_vThreshLowerLowAtten = 0.6;
	const float m_vThreshLowerHighAtten = 0.21;
	const int m_hThreshLowSat = 76;
	const int m_hThreshHighSat = 62;
	const int m_sThreshLowSat = 36;
	const int m_sThreshHighSat = 93;

	const int m_winSize = 1;
	float m_vThreshLower;
	float m_vThreshUpper;
	float m_hThresh;
	float m_sThresh;
public:
	ChromacityShadowDet();
	Status GetShadowMask(Mat rgbCurFrame, Mat rgbBkgFrame, Mat frgMask, Mat& shadowMask);
	virtual ~ChromacityShadowDet();
};

#endif