#include <opencv2/opencv.hpp>
#include "iostream"
#include "../include/ChromacityShadowDet.h"
#include "../include/ShadowRemover.h"
#include "../include/CleanupShadows.h"
#include "../include/ConfigParams.h"
using namespace std;
using namespace cv;

// to enable debugging mode
// #define DEBUG 1

// 写的啥我也看不懂

void Split(const std::string &src, const std::string &separator, std::vector<std::string> &dest) // 字符串分割到数组
{

	// 参数1：要分割的字符串；参�?2：作为分隔符的字符；参数3：存放分割后的字符串的vector向量

	string str = src;
	string substring;
	string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do
	{
		if (index != string::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == string::npos)
				break;
		}
	} while (index != string::npos);

	// the last part
	substring = str.substr(start);
	dest.push_back(substring);
}


//这个是核心函数，要改返回值
void VideoShadowRemove()
{

	// cout << "Hello  OpenCV" << endl;
	string path;
	cout << "Enter input video filepath with extension:";
	cin >> path;
	// 根据path创建视频输出对象
	cv::VideoWriter writer;

	// string outputdir;
	// cout << "Enter input video filepath with extension:";
	// cin >> outputdir;
	// cout << "Press ESC to exit..." << endl;

	// CSDN上找的字符串分割函数
	std::vector<string> tmp_file_name;
	Split(path,"\\",tmp_file_name);

	//char *path_name = new char[strlen(path.c_str() + 1)];
	//strcpy(path_name, path.c_str());
	//char *tmp_file_name = strtok(path_name, "\\");
	//cout << "Step 1 error check" << tmp_file_name.size() << endl;
	VideoCapture cap(path);

	// Check if camera opened successfully
	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		//return -1;
	}
	//cout << "Step 1A0 error check" << endl;
	// 使用VideoCapture打开视频后为writer创建对象
	string tmp = tmp_file_name.at(tmp_file_name.size() - 1);
	string outputname = "output_";
	outputname.append(tmp);
	//cout << "Step 1A error check" << outputname << endl;
	writer = VideoWriter(outputname, cv::VideoWriter::fourcc('m','p','4','v'), cap.get(CAP_PROP_FPS), Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)), 1);
	//cout << "Step 2 error check" << endl;
	Mat bkgMat, frgMat, frgMask, accBkgMat, shadowMask, gradShadowMask;

	Ptr<BackgroundSubtractor> pBackSub;
	pBackSub = createBackgroundSubtractorMOG2(config::BKG_LEARNING_HISTORY_LENGTH, config::BKG_LEARNING_THRESHOLD, true);
	ChromacityShadowDet chromacityShadowDet;
	ShadowRemover shadowRemover;
	CleanupShadows cleanupShadow;
	Status returnVal = Status::SUCCESS;
	//cout << "Step 3 error check" << endl;


	int num = 1;
	while (1)
	{

		Mat frame;
		// Capture frame-by-frame
		cap >> frame;
		// If the frame is empty, break immediately
		if (frame.empty())
			break;
		// to speed up RESIZE会毁掉的
		//resize(frame, frame, Size(config::FRAME_RESIZE_WIDTH, config::FRAME_RESIZE_HEIGHT));
		// remove noise
		GaussianBlur(frame, frame, Size(5, 5), 0.0);

		frgMat = Mat::zeros(frame.size(), frame.type());
		frgMask = Mat::zeros(frame.size(), frame.type());
		/*Learn backgroud using mixture of gaussian*/
		// update the background model
		pBackSub->apply(frame, frgMat, config::BKG_LEARNING_RATE);
		// get latest background image
		pBackSub->getBackgroundImage(bkgMat);

		// THRESH_BINARY改名字了

		/*cleanup foreground object*/
		// binarize detected foreground object
		threshold(frgMat, frgMat, 100, 255, THRESH_BINARY);
		// do opening and closing on detected foreground
		int morph_size = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		morphologyEx(frgMat, frgMat, MORPH_CLOSE, element);
		morphologyEx(frgMat, frgMat, MORPH_OPEN, element);
		threshold(frgMat, frgMask, 200, 255, THRESH_BINARY);

		/*get shadow based on chromaticity*/
		returnVal = chromacityShadowDet.GetShadowMask(frame, bkgMat, frgMask, shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Chromacity Shadow detection Failed!";
			break;
		}

		/*cleanup shadows*/
		returnVal = cleanupShadow.RemoveObjectBoundries(frame, frgMask, shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Cleanup Shadow by object boundries Failed!";
			break;
		}

		returnVal = cleanupShadow.RemoveNoisyObjects(shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Cleanup boundries by noisy objects Failed!";
			break;
		}

		/*Remove shadows*/
		Mat shadowRemFrame = frame.clone();
		returnVal = shadowRemover.RemoveShadow(bkgMat, shadowMask, shadowRemFrame);
		if (returnVal == Status::FAILURE)
		{
			cout << "Shadow removal Failed!";
			break;
		}

		/*add metadata to output*/
		Mat opFrame = frame.clone();
		Scalar shadowColor(160, 0, 0);
		opFrame.setTo(shadowColor, shadowMask);
		// get the frame number and write it on the current frame
		stringstream ss;
		ss << cap.get(CAP_PROP_POS_FRAMES);
		// 这一段会给视频左上角加上帧数
		string frameNumberString = "frame:" + ss.str();
		string shadowLegend = "Detected Shadow";
		putText(frame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		putText(opFrame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		putText(shadowRemFrame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

		putText(opFrame, shadowLegend.c_str(), cv::Point(28, 30), FONT_HERSHEY_SIMPLEX, 0.5, shadowColor);
		circle(opFrame, Point(20, 25), 5, shadowColor, FILLED);
		/*Display output*/

		//cout << "Step A error check" << endl;
		// imshow("Input Frame", frame);
		// imshow("Shadow Detection", opFrame);
		//imshow("Shadow Removal", shadowRemFrame); 相当于预览窗口
		char img_name[20];
		sprintf(img_name, "%s%d%s", "img", num, ".png");
		num++;
		//imwrite(img_name, shadowRemFrame);
		//cout << "Step B error check" << endl;
		//这里是核心，会输出产品视频
		writer.write(shadowRemFrame);
		//cout << "Step C error check" << endl;
#ifdef DEBUG
		imshow("frgMask", frgMask);
		imshow("bkgMat", bkgMat);
		imshow("shadowMask", shadowMask);
#endif // DEBUG

		// Press  ESC to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	// When everything done, release the video capture object
	cap.release();
	// Closes all the frames
	//jusr for debug it's useless
	//destroyAllWindows();
}
