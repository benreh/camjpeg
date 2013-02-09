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
#include <assert.h>
#include "capture.h"
#include <sstream>
#include <iostream>
#include "ipl2jpeg.h"
#include <fstream>
extern bool global_quit;

//If not return false
#define IFNF(X) if (!X) return false

using namespace std;
Capture::Capture(int captureNumber, bool gui) :capture(0),frame(0) {
	this->captureNumber=captureNumber;
	this->gui=gui;
	std::stringstream ss;
	ss << "camjpeg" << captureNumber;
	windowname = ss.str();
}
Capture::~Capture() {
	if (capture)
		cvReleaseCapture( &capture );
}

void Capture::getSettings(Settings &settings) {
	w=settings.cfg.getvalueidx<int>("width",captureNumber,320);
	h=settings.cfg.getvalueidx<int>("height",captureNumber,240);
	BGR2RGB=settings.cfg.getvalueidx<bool>("bgr2rgb",captureNumber,true);
	cout << w << "x" << h << endl;
}


bool Capture::open() {
	capture = cvCaptureFromCAM( captureNumber );
	if (!capture)
		return false;
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,h);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,w);
	if (gui) {
		cvNamedWindow( windowname.c_str(), CV_WINDOW_AUTOSIZE );
	}
	return true;
}
bool Capture::query() {
	frame = cvQueryFrame( capture );
	if (!frame)
		return false;
	cout << ".";
	cout.flush();
	if(gui) {
		cvShowImage( windowname.c_str(), frame );
		int key = cvWaitKey(10);
		if (key=='q')
			global_quit=true;
	}
	return true;
}

bool Capture::save(std::string filename,unsigned char *outbuffer, long unsigned int outlen) {

	ofstream f;
	f.open(filename.c_str(),ofstream::binary);
	f.write((const char*)outbuffer,outlen);
	f.close();
	cout << outlen << endl;
}


bool Capture::convert() {
	unsigned char *outbuffer;
	long unsigned int outlen;

	if (BGR2RGB)
		cvCvtColor ( frame, frame, CV_BGR2RGB );

	ipl2jpeg(frame, &outbuffer, &outlen);


	save("test.jpg",outbuffer,outlen);

	return true;
}


bool Capture::loop() {
	IFNF(query());
	IFNF(convert());

	return true;
}

void Capture::run(Shm* shm) {
	cout << "Capture thread started #" << captureNumber << endl;
	getSettings(*(shm->settings));
	open();
	while(loop() && !global_quit) {};


}
