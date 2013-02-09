// Copyright 2013 Benjamin Reh <ich@benjaminreh.de>
 // 
 // This program is free software; you can redistribute it and/or modify
 // it under the terms of the GNU General Public License as published by
 // the Free Software Foundation; either version 2 of the License, or
 // (at your option) any later version.
 // 
 // This program is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 // GNU General Public License for more details.
 // 
 // You should have received a copy of the GNU General Public License
 // along with this program; if not, write to the Free Software
 // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 // MA 02110-1301, USA.
#ifndef CAPTURE_H
#define CAPTURE_H
#include <stddef.h>
#include <cv.h>
#include <highgui.h>
#include <string>

class Capture {
public:
Capture(int captureNumber=0, bool gui=false);
~Capture();
bool open();
bool loop();

	int h,w;

private:

	bool query();
	bool convert();

	int captureNumber;
	bool gui;
	CvCapture *capture;
	IplImage  *frame ;
	std::string windowname;
};
#endif //CAPTURE_H
