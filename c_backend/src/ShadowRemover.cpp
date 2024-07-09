#include "../include/ShadowRemover.h"

ShadowRemover::ShadowRemover()
{
	//default constructor
}

ShadowRemover::~ShadowRemover()
{
	//no need to deallocate as cv::Mat will deallocate itself
	//http://docs.opencv.org/2.4/modules/core/doc/intro.html#automatic-memory-management
}

//method removes shadow by getting average V value in the background and adds the difference to current pixel
Status ShadowRemover::RemoveShadow(Mat rgbBkgFrame, Mat detectedShadows, Mat& rgbCurFrame)
{
	try
	{
		if ((rgbBkgFrame.channels() != 3) || (rgbCurFrame.channels() != 3) || (detectedShadows.channels() != 1))
		{
			return Status::FAILURE;
		}

		//convert to HSV space to isolate luminocity
		cvtColor(rgbCurFrame, m_hsvCurFrame, COLOR_BGR2HSV);
		cvtColor(rgbBkgFrame, m_hsvBkgFrame, COLOR_BGR2HSV);
		vector<Mat> hsvCurChannels(3), hsvBkgChannels(3);

		//split channels
		split(m_hsvCurFrame, hsvCurChannels);
		split(m_hsvBkgFrame, hsvBkgChannels);

		for (int i = 1; i < rgbCurFrame.rows - 1; i++)
		{
			for (int j = 1; j < rgbCurFrame.cols - 1; j++)
			{
				if (detectedShadows.at<uchar>(i, j) == 255)
				{
					//get avg background intensity around the pixel
					float avgIntensity = 0.0;
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i - 1, j - 1);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i - 1, j);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i - 1, j + 1);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i, j - 1);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i, j);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i, j + 1);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i + 1, j - 1);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i + 1, j);
					avgIntensity += (float)hsvBkgChannels[2].at<uchar>(i + 1, j + 1);
					avgIntensity = avgIntensity / 9;

					float intensityDiff = avgIntensity - hsvCurChannels[2].at<uchar>(i, j);

					//add diff to foreground to remove shadow
					hsvCurChannels[2].at<uchar>(i, j) += intensityDiff;
				}
			}
		}

		Mat tmpImg = Mat::zeros(rgbCurFrame.size(), rgbCurFrame.type());
		merge(hsvCurChannels, tmpImg);
		cvtColor(tmpImg, rgbCurFrame, COLOR_HSV2BGR);
		//cleanup
		hsvCurChannels.clear();
		hsvBkgChannels.clear();
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}