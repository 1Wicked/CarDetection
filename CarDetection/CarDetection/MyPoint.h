#pragma once
#include <iostream>

class MyPoint {
private:
	int x;
	int y;

public:
	MyPoint(int _x, int _y);

	int getX();
	int getY();
};

std::ostream & operator << (std::ostream &os, MyPoint &point);