#ifndef _CONFIGPARAMS_H_
#define _CONFIGPARAMS_H_

enum Status
{
	SUCCESS,
	FAILURE
};

namespace config
{
	extern float BKG_LEARNING_RATE;
	extern int BKG_LEARNING_HISTORY_LENGTH;
	extern double BKG_LEARNING_THRESHOLD;

	extern bool USE_LOWER_VTHRESH;
	extern bool USE_LOWER_STHRESH;

	extern int FRAME_RESIZE_WIDTH;
	extern int FRAME_RESIZE_HEIGHT;

	extern double MIN_CONTOUR_AREA;
}
#endif // !_CONFIGPARAMS_H_
