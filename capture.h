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
#include "settings.h"
#include "shm.h"

class Capture {
public:
Capture(int captureNumber=0, bool gui=false);
~Capture();
bool open();
bool loop();
void getSettings(Settings &settings);
void run(Shm* shm);
	int h,w;

private:

	bool query();
	bool convert();
	bool annotate();
	bool show();
	bool save(std::string filename,unsigned char *outbuffer, long unsigned int outlen);

	Shm* shm;

	int captureNumber;
	bool gui;
	bool timestamp;
	int flip;
	CvCapture *capture;
	IplImage  *frame ;
	IplImage  *captureframe ;
	CvPoint timestampPos;
	CvScalar fontcolor;
	std::string windowname;
	CvFont font;
	bool BGR2RGB;
};
#endif //CAPTURE_H
