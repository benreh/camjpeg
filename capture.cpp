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
#include <ctime>
#include <iomanip>

extern bool global_quit;

//If not return false
#define IFNF(X) if (!X) return false

using namespace std;
Capture::Capture(int captureNumber, bool gui) :capture(0),frame(0),shm(0){
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
	timestamp=settings.cfg.getvalueidx<bool>("timestamp",captureNumber,true);
	flip=settings.cfg.getvalueidx<int>("flip",captureNumber,false);
	timestampPos=cvPoint(settings.cfg.getvalueidx<int>("timestampposx",captureNumber,1),
		settings.cfg.getvalueidx<int>("timestampposy",captureNumber,20));
	//in BGR
	fontcolor = cvScalar(settings.cfg.getvalueidx<int>("fontcolorb",captureNumber,128),
			settings.cfg.getvalueidx<int>("fontcolorg",captureNumber,128),
			settings.cfg.getvalueidx<int>("fontcolorr",captureNumber,128));
	float fontsize=settings.cfg.getvalueidx<float>("fontsize",captureNumber,.5);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, fontsize,fontsize,0,1);
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
	captureframe = cvQueryFrame( capture );
	frame=captureframe;
	
	if (!frame)
		return false;
	//~ cout << ".";
	//~ cout.flush();
	return true;
}

bool Capture::show() {
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
}

bool Capture::annotate() {
// current date/time based on current system
	time_t now = time(0);
	tm *ltm = localtime(&now);
	stringstream ss;
	ss << 1900 + ltm->tm_year ;
	ss << "-" <<  setw(2) << setfill('0')  <<  1 + ltm->tm_mon; 
	ss << "-" <<  setw(2) << setfill('0')  << ltm->tm_mday << " "; 
	ss << setw(2) << setfill('0')  << ltm->tm_hour << ":";
	ss << setw(2) << setfill('0')  << ltm->tm_min << ":";
	ss << setw(2) << setfill('0')  << ltm->tm_sec ;
	cvPutText(frame,ss.str().c_str(), timestampPos,&font, fontcolor);
	return true;

}


bool Capture::convert() {
	CaptureData& cd=shm->captureData[captureNumber];
	unsigned char* outbuffer=0;
	long unsigned int outlen=0;

	bool notIdle=(cd.idleCounter < shm->settings->idleTimeout);

	
	if (notIdle) {
		if (flip==2) {
			cvFlip(frame, frame, -1);
		} else if (flip==1) {
			IplImage *rotated;
			rotated = cvCreateImage(cvSize(frame->height, frame->width), frame->depth, frame->nChannels);
			cvTranspose(frame, rotated);
			frame=rotated;
			cvFlip(frame, frame, 1);
		} else if (flip==3) {
			cvFlip(frame, frame, 1);
			IplImage *rotated;
			rotated = cvCreateImage(cvSize(frame->height, frame->width), frame->depth, frame->nChannels);
			cvTranspose(frame, rotated);
			frame=rotated;
			
		}
		IFNF(annotate());
	}
	if (notIdle) {
		IFNF(show());
	}
	if (notIdle) {
		if (BGR2RGB)
			cvCvtColor ( frame, frame, CV_BGR2RGB );

		ipl2jpeg(frame, &outbuffer, &outlen);
		cd.mutex.lock();
		if (cd.jpgdata!=0)
			free(cd.jpgdata);
		cd.jpgdata=outbuffer;
		cd.jpglen=outlen;
		cd.picCounter++;
		cd.idleCounter++;
		cd.mutex.unlock();
	}	
	if (captureframe!=frame) {
		cvReleaseImage(&frame);
	}
	//~ save("test.jpg",outbuffer,outlen);

	


	return true;
}


bool Capture::loop() {
	IFNF(query());
	IFNF(convert());
	return true;
}

void Capture::run(Shm* shm) {
	cout << "Capture thread started #" << captureNumber << endl;
	this->shm=shm;
	getSettings(*(shm->settings));
	while(!open()&& ! global_quit) {
		cout << "could not open capture device #" << captureNumber << endl;
		sleep(2);
	};
	while(loop() && !global_quit) {};


}
