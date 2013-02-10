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
#include "shm.h"
#include <iostream>

CaptureData::CaptureData(): jpgdata(0), jpglen(0), picCounter(0), idleCounter(0){
}

CaptureData::~CaptureData() {
}

Shm::Shm(): captureData(0){

}
Shm::~Shm() {
	if (captureData)
		delete[] captureData;

}


void Shm::allocateCapture(int nr) {
	captureData=new CaptureData[nr];
}
