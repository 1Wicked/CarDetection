#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;

class BackgroundSubstraction {
public:
	BackgroundSubstraction();
	~BackgroundSubstraction();
	Mat substract(VideoCapture video);
};