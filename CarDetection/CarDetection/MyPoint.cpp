#include "MyPoint.h"
#include <iostream>

MyPoint::MyPoint(int _x, int _y) {
	x = _x;
	y = _y;
}

int MyPoint::getX() {
	return x;
}

int MyPoint::getY() {
	return y;
}

std::ostream & operator << (std::ostream &os, MyPoint &point) {
	return os << "[" << point.getX() << ", " << point.getY() << "]" << std::endl;
}