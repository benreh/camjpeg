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

#include <vector>
#include <boost/thread.hpp>
#include <iostream>
#include <signal.h>

#include "main.h"
#include "capture.h"
#include "settings.h"
#include "shm.h"
#include "server.h"

bool global_quit=false;

using namespace std;

void  SIGhandler(int sig) {
	cout << "Received SIGINT" << endl;
	global_quit=true;
	signal(SIGINT, 0);
}



int main() {
	signal(SIGINT, SIGhandler);
	Shm shm;
	Settings settings;
	shm.settings=&settings;
	vector<boost::thread*> threads;
	vector<Capture*> captures;
	int nocams=settings.nocams;
	shm.allocateCapture(nocams);
	for (int i=0; i < nocams ;i++) {
		Capture* capture=new Capture(i,settings.useGui);
		captures.push_back(capture);
		boost::thread* thr = new boost::thread ( boost::bind( &Capture::run, capture,&shm ) );
		threads.push_back(thr);
	}

	Server server(&shm);
	
	server.run();

	cout << "\nwaiting for threads to quit..." << endl;
	for (vector<boost::thread*>::iterator th=threads.begin();th!=threads.end(); th++) {
		(*th)->join();
		delete *th;
	}

	cout << "\nquitting" << endl;
	return 0;
}
