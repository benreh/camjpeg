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
#include "main.h"
#include "capture.h"
#include "settings.h"
#include <vector>

using namespace std;

int main() {

	Settings settings;

	vector<Capture> captures;
	for (int i=0; i < settings.cfg.getvalue<int>("nocams",1);i++) {
		Capture capture;
		capture.getSettings(settings);
		captures.push_back(capture);
	}



	Capture C(0,true);
	C.open();

	while(C.loop()) {};

	
	return 0;
}
