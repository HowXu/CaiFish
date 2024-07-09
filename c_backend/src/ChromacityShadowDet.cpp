#include "../include/ChromacityShadowDet.h"

ChromacityShadowDet::ChromacityShadowDet()
{
	m_vThreshLower = (config::USE_LOWER_VTHRESH ? m_vThreshLowerLowAtten : m_vThreshLowerHighAtten);
	m_vThreshUpper = (config::USE_LOWER_VTHRESH ? m_vThreshUpperLowAtten : m_vThreshUpperHighAtten);
	m_hThresh = (config::USE_LOWER_STHRESH ? m_hThreshLowSat : m_hThreshHighSat);
	m_sThresh = (config::USE_LOWER_STHRESH ? m_sThreshLowSat : m_sThreshHighSat);
}

ChromacityShadowDet::~ChromacityShadowDet()
{
	//no need to deallocate as cv::Mat will deallocate itself
	//http://docs.opencv.org/2.4/modules/core/doc/intro.html#automatic-memory-management
}

//Detects shadows using cromacity values
//The thresholds are derieved experimentally by using statistical data
Status ChromacityShadowDet::GetShadowMask(Mat rgbCurFrame, Mat rgbBkgFrame, Mat frgMask, Mat& shadowMask)
{
	try
	{
		if ((rgbCurFrame.channels() != 3) || (rgbBkgFrame.channels() != 3) || (frgMask.channels() != 1))
		{
			cout << "channel number mismatch!!";
			return Status::FAILURE;
		}

		//convert to hsv
		//COLOR_BGR改名字了
		cvtColor(rgbCurFrame, m_hsvCurFrame, COLOR_BGR2HSV);
		cvtColor(rgbBkgFrame, m_hsvBkgFrame, COLOR_BGR2HSV);
		shadowMask = Mat::zeros(frgMask.size(), frgMask.type());


		for (int i = 0; i < rgbCurFrame.rows; i++)
		{
			for (int j = 0; j < rgbCurFrame.cols; j++)
			{
				if (frgMask.at<uchar>(i, j) == 255)
				{
					//find darker pixels
					float vRatio = (float)m_hsvCurFrame.at<Vec3b>(i, j).val[2] / (float)m_hsvBkgFrame.at<Vec3b>(i, j).val[2];
					bool isVShadow = (vRatio > m_vThreshLower && vRatio < m_vThreshUpper);

					if (isVShadow)
					{
						int hDiffSum = 0;
						int sDiffSum = 0;
						int winArea = 0;
						int minY = std::max(i - m_winSize, 0);
						int maxY = std::min(i + m_winSize, rgbCurFrame.rows - 1);
						int minX = std::max(j - m_winSize, 0);
						int maxX = std::min(j + m_winSize, rgbCurFrame.cols - 1);

						//loop over roi to reduce noise
						for (int dy = minY; dy <= maxY; dy++)
						{
							for (int dx = minX; dx <= maxX; dx++)
							{
								int hDiff = abs(m_hsvCurFrame.at<Vec3b>(dy, dx).val[0] - m_hsvBkgFrame.at<Vec3b>(dy, dx).val[0]); 
								if (hDiff > 90)
								{
									hDiff = 180 - hDiff;
								}

								hDiffSum += hDiff;
								//bool isHShadow = (hDiff < hThresh);

								int sDiff = m_hsvCurFrame.at<Vec3b>(dy, dx).val[1] - m_hsvBkgFrame.at<Vec3b>(dy, dx).val[1];
								//bool isSShadow = (sDiff < sThresh);
								sDiffSum += sDiff;
								++winArea;
							}
						}

						bool isHShadow = (hDiffSum / winArea < m_hThresh);
						bool isSShadow = (sDiffSum / winArea < m_sThresh);

						if (isSShadow && isHShadow)
						{
							//shadow pixel found
							shadowMask.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}