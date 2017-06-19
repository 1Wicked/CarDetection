#pragma warning(disable:4996)
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "BackgroundSubstraction.h"

using namespace std;
using namespace cv;

void Morphology_Operations(int, void*);

int threshold_value = 30;
int dilate_shape = 0;
int dilate_size = 1;
Mat dilate_element = getStructuringElement(dilate_shape, Size(2 * dilate_size + 1, 2 * dilate_size + 1), Point(dilate_size, dilate_size));
int erode_shape = 0;
int erode_size = 0;
Mat erode_element = getStructuringElement(erode_shape, Size(2*erode_size+1, 2*erode_size+1), Point(erode_size, erode_size));
int close_shape = 0;
int close_size = 2;
Mat close_element = getStructuringElement(close_shape, Size(2*close_size+1, 2*close_size+1), Point(close_size, close_size));

int main() {
	/// Loading the video
	VideoCapture video("../../video.avi");
	if (!video.isOpened()) {
		cout << "ERROR! Cannot load video.";
		getchar();
		return 0;
	}
	
	/// Background Substraction from the whole video
	BackgroundSubstraction backgroundSubstraction;
	Mat background = backgroundSubstraction.substract(video);
	imshow("Background", background);
	
	/// Creating trackbars for morphological operations and writing out instructions
	cout << "OPTIONS" << endl;
	cout << "	Threshold" << endl;
	cout << "		1. Threshold value:" << endl;
	cout << "	Dilate operation" << endl;
	cout << "		1. Element:" << endl;
	cout << "			0: Rect" << endl;
	cout << "			1: Cross" << endl;
	cout << "			2: Ellipse" << endl;
	cout << "		2. Kernel size (2n +1):" << endl;
	cout << "	Erode operation" << endl;
	cout << "		1. Element:" << endl;
	cout << "			0: Rect" << endl;
	cout << "			1: Cross" << endl;
	cout << "			2: Ellipse" << endl;
	cout << "		2. Kernel size (2n +1):" << endl;
	cout << "	Closing operation" << endl;
	cout << "		1. Element:" << endl;
	cout << "			0: Rect" << endl;
	cout << "			1: Cross" << endl;
	cout << "			2: Ellipse" << endl;
	cout << "		2. Kernel size (2n +1):" << endl;
	cout << "Press ESC to exit..." << endl;

	namedWindow("Options", CV_WINDOW_AUTOSIZE);
	createTrackbar("Threshold 1", "Options", &threshold_value, 255, Morphology_Operations);
	createTrackbar("Dilate 1", "Options", &dilate_shape, 2, Morphology_Operations);
	createTrackbar("Dilate 2", "Options", &dilate_size, 30, Morphology_Operations);
	createTrackbar("Erode 1", "Options", &erode_shape, 2, Morphology_Operations);
	createTrackbar("Erode 2", "Options", &erode_size, 30, Morphology_Operations);
	createTrackbar("Close 1", "Options", &close_shape, 2, Morphology_Operations);
	createTrackbar("Close 2", "Options", &close_size, 30, Morphology_Operations);
	Morphology_Operations(0, 0);

	int key_pressed = 0;
	Mat frame;
	while(key_pressed != 27) {
		/// Loading next frame if possible, taking first frame if not
		if (!video.read(frame)) {
			video.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}

		/// Displaying original video
		imshow("Video", frame);

		/// Calculating and displaying difference between current frame and background
		Mat frame_diff;
		frame.copyTo(frame_diff);
		absdiff(background, frame_diff, frame_diff);
		imshow("Difference between video and background", frame_diff);

		/// Thresholding difference between current frame and background and displaying the result
		Mat frame_binary;
		frame_diff.copyTo(frame_binary);
		cvtColor(frame_binary, frame_binary, CV_BGR2GRAY);
		frame_binary.convertTo(frame_binary, CV_8UC1);
		threshold(frame_binary, frame_binary, threshold_value, 255, CV_THRESH_BINARY);
		imshow("Difference binarized", frame_binary);

		/// Using morphological operations on binarized image
		Mat frame_morph;
		frame_binary.copyTo(frame_morph);
		dilate(frame_morph, frame_morph, dilate_element);
		erode(frame_morph, frame_morph, erode_element);
		morphologyEx(frame_morph, frame_morph, MORPH_CLOSE, close_element);
		imshow("Binarized difference after morphological operations", frame_morph);

		/// Infinite loop until ESC is pressed
		key_pressed = waitKey(1);
	}

	video.release();
	return 0;
}

void Morphology_Operations(int, void*) {
	dilate_element = getStructuringElement(dilate_shape, Size(2 * dilate_size + 1, 2 * dilate_size + 1), Point(dilate_size, dilate_size));
	erode_element = getStructuringElement(erode_shape, Size(2 * erode_size + 1, 2 * erode_size + 1), Point(erode_size, erode_size));
	close_element = getStructuringElement(close_shape, Size(2*close_size+1, 2*close_size+1), Point(close_size, close_size));
}