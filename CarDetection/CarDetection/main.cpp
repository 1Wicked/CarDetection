#pragma warning(disable:4996)
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "BackgroundSubstraction.h"
#include "MyPoint.h"

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
int dist = 25;
int max_idle_time = 10;

int main() {
	/// Loading the video
	VideoCapture video("../../FroggerHighway_001.avi");
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
	cout << "OPTIONS MORPHOLOGY" << endl;
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
	cout << "OPTIONS" << endl;
	cout << "	Distance" << endl;
	cout << "		Maximal distance between detected cars in between frames to be counted as one car" << endl;
	cout << "	Max idle time" << endl;
	cout << "		Number of frames to wait for new position, otherwise car is assumed to be out of frame" << endl;
	cout << "Press ESC to exit..." << endl;

	namedWindow("Options Morphology", CV_WINDOW_AUTOSIZE);
	createTrackbar("Threshold 1", "Options Morphology", &threshold_value, 255, Morphology_Operations);
	createTrackbar("Dilate 1", "Options Morphology", &dilate_shape, 2, Morphology_Operations);
	createTrackbar("Dilate 2", "Options Morphology", &dilate_size, 30, Morphology_Operations);
	createTrackbar("Erode 1", "Options Morphology", &erode_shape, 2, Morphology_Operations);
	createTrackbar("Erode 2", "Options Morphology", &erode_size, 30, Morphology_Operations);
	createTrackbar("Close 1", "Options Morphology", &close_shape, 2, Morphology_Operations);
	createTrackbar("Close 2", "Options Morphology", &close_size, 30, Morphology_Operations);
	Morphology_Operations(0, 0);
	namedWindow("Options", CV_WINDOW_AUTOSIZE);
	createTrackbar("Distance", "Options", &dist, 100);
	createTrackbar("Max idle time", "Options", &max_idle_time, 100);
	
	SimpleBlobDetector::Params params;
	params.filterByColor = true;
	params.blobColor = 255;
	params.filterByArea = true;
	params.minArea = 1000;
	params.maxArea = 10000000;
	params.filterByInertia = true;
	params.minInertiaRatio = 0.15;
	params.maxInertiaRatio = 1;
	params.filterByCircularity = false;
	params.filterByConvexity = false;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;

	vector<vector<MyPoint>> paths;
	vector<int> paths_frames_idle;
	vector<Scalar> path_color;
	RNG rng(0xFFFFFFFF);

	int key_pressed = 0;
	Mat frame;
	int frame_count = 0;
	while(key_pressed != 27) {
		/// Loading next frame if possible, taking first frame if not, resetting values
		if (!video.read(frame)) {
			paths.clear();
			path_color.clear();
			paths_frames_idle.clear();
			cout << endl << endl << endl;
			video.set(CV_CAP_PROP_POS_FRAMES, 0);
			continue;
		}
		frame_count++;

		/// Displaying original video
		imshow("Video", frame);

		/// Calculating and displaying difference between current frame and background
		Mat frame_diff;
		frame.copyTo(frame_diff);
		absdiff(background, frame_diff, frame_diff);
		// Attempt at other way of differentiating between current frame and background, sadly didnt bring better results
		/*int rows = frame_diff.rows;
		int cols = frame_diff.cols;
		for (int x = 1; x <= rows - 1; x++) {
			for (int y = 1; y <= cols - 1; y++) {
				Vec3b point_color = frame_diff.at<Vec3b>(Point(y, x));
				if (point_color.val[0] > 30 && point_color.val[1] > 30 && point_color.val[2] > 30) {
					frame_diff.at<Vec3b>(Point(y, x))[0] = frame.at<Vec3b>(Point(y, x))[0];
					frame_diff.at<Vec3b>(Point(y, x))[1] = frame.at<Vec3b>(Point(y, x))[1];
					frame_diff.at<Vec3b>(Point(y, x))[2] = frame.at<Vec3b>(Point(y, x))[2];
				}
			}
		}*/
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

		/// Detecting and marking cars on current frame
		Mat frame_blob;
		frame_morph.copyTo(frame_blob);
		detector->detect(frame_blob, keypoints);
		drawKeypoints(frame_blob, keypoints, frame_blob, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("Detected cars", frame_blob);

		Mat frame_blob_video;
		frame.copyTo(frame_blob_video);
		drawKeypoints(frame_blob_video, keypoints, frame_blob_video, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("Detected cars on video", frame_blob_video);
		
		/// Tracking car paths
		for (int i = 0; i<keypoints.size(); i++) {
			float x = keypoints[i].pt.x;
			float y = keypoints[i].pt.y;

			if(paths.size() == 0) {
				// First car detected, creating first path
				vector<MyPoint> new_path;
				new_path.push_back(MyPoint(x, y));
				paths.push_back(new_path);
				Scalar new_path_color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
				path_color.push_back(new_path_color);
				paths_frames_idle.push_back(0);
				cout << "Detected cars: " << paths.size() << endl;
			}
			else {
				double d_min = dist;
				int track = 999;
				for (int i=paths.size()-1; i>=0; i--) {
					int x_last = paths[i].back().getX();
					int y_last = paths[i].back().getY();
					if (x_last == 0 && y_last == 0)
						continue;
					double d = sqrt((x-x_last)*(x-x_last) + (y-y_last)*(y-y_last));
					if (d < d_min) {
						track = i;
						continue;
					}
				}
				if (track == 999) {
					// Detected car is too far away from any existing path, creating new one
					vector<MyPoint> new_path;
					new_path.push_back(MyPoint(x, y));
					paths.push_back(new_path);
					Scalar new_path_color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
					path_color.push_back(new_path_color);
					paths_frames_idle.push_back(0);
					cout << "Detected cars: " << paths.size() << endl;
				}
				else {
					// Adding new detected position to proper exisiting path
					vector<MyPoint> extended_path = paths[track];
					extended_path.push_back(MyPoint(x, y));
					paths[track] = extended_path;
					paths_frames_idle[track] = 0;
				}
			}
		}

		// Incrementing idle time for paths
		if (paths_frames_idle.size() > 0) {
			for (int i = 0; i <= paths_frames_idle.size() - 1; i++) {
				paths_frames_idle[i] = paths_frames_idle[i] + 1;
				if (paths_frames_idle[i] == max_idle_time) {
					vector<MyPoint> extended_path = paths[i];
					extended_path.push_back(MyPoint(0, 0));
					paths[i] = extended_path;
				}
			}
		}

		/// Marking detected cars on current frame
		Mat frame_blob_marked_video;
		frame.copyTo(frame_blob_marked_video);
		if (paths.size() > 0) {
			for (int i = 0; i <= paths.size()-1; i++) {
				if (paths[i].back().getX() != 0 && paths[i].back().getY() != 0) {
					circle(frame_blob_marked_video, Point(paths[i].back().getX(), paths[i].back().getY()), dist, path_color[i]);
				}
			}
		}
		imshow("Marked cars on video", frame_blob_marked_video);

		/// Infinite loop until ESC is pressed
		key_pressed = waitKey(1);
	}

	delete detector;
	video.release();
	return 0;
}

void Morphology_Operations(int, void*) {
	dilate_element = getStructuringElement(dilate_shape, Size(2 * dilate_size + 1, 2 * dilate_size + 1), Point(dilate_size, dilate_size));
	erode_element = getStructuringElement(erode_shape, Size(2 * erode_size + 1, 2 * erode_size + 1), Point(erode_size, erode_size));
	close_element = getStructuringElement(close_shape, Size(2*close_size+1, 2*close_size+1), Point(close_size, close_size));
}