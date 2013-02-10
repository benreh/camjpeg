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
#ifndef SHM_H
#define SHM_H
#include "settings.h"
#include <boost/thread.hpp>

class CaptureData {
public:
CaptureData();
~CaptureData();
unsigned char* jpgdata;
long unsigned int jpglen;
long unsigned int picCounter;
boost::mutex mutex;
unsigned int idleCounter;


};

class Shm {
public:
Shm();
~Shm();
void allocateCapture(int nr);

CaptureData* captureData;

Settings* settings;
};
#endif //SHM_H
