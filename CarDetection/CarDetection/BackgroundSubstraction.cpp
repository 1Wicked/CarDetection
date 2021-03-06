#include "BackgroundSubstraction.h"

using namespace cv;

BackgroundSubstraction::BackgroundSubstraction() {
}

BackgroundSubstraction::~BackgroundSubstraction() {
}

Mat BackgroundSubstraction::substract(VideoCapture video) {
	Mat frame;
	video.read(frame);
	video.set(CV_CAP_PROP_POS_FRAMES, 0);

	Mat background;
	frame.copyTo(background);
	//background = Mat::zeros(background.size(), CV_32FC3);
	background.convertTo(background, CV_32FC3);

	double frame_count = video.get(CV_CAP_PROP_FRAME_COUNT);
	double alpha = 1.0 / frame_count;

	std::cout << "Frame count: " << frame_count << std::endl;
	std::cout << "alpha: " << alpha << std::endl;

	while(true) {
		bool read_success = video.read(frame);
		if (!read_success)
			break;
		accumulateWeighted(frame, background, 0.01);
	}
	video.set(CV_CAP_PROP_POS_FRAMES, 0);

	convertScaleAbs(background, background);
	return background;
}