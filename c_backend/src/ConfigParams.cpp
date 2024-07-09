#include "../include/ConfigParams.h"

/*Below are configuration params which needs to fine tuned depending on input video*/
/*Depending on if input video is indoor, outdoor, bright ambiant light etc parameters need to fine tuned*/
namespace config
{
	//decides how fast background shold update
	float BKG_LEARNING_RATE = 0.001;

	//how many past frames are used for learning background
	int BKG_LEARNING_HISTORY_LENGTH = 100;

	//the minimum distance btw foreground and background to classify a pixel as foreground
	double BKG_LEARNING_THRESHOLD = 16.0;

	//Threshold range to be used for cromacity detection
	bool USE_LOWER_VTHRESH = true;
	bool USE_LOWER_STHRESH = true;

	//to speed up processing frame size is reduced
	int FRAME_RESIZE_WIDTH = 400;
	int FRAME_RESIZE_HEIGHT = 400;

	//minimum area of pixels to be rejected in shadow cleanup
	double MIN_CONTOUR_AREA = 500.0;
}