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
#include "handler.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <sstream>

extern bool global_quit;

Handler::Handler() : lastcounter(-1), splitter("splitter"){

}

Handler::~Handler() {

}

bool Handler::send(string str) {
	write(sock,str.c_str(),strlen(str.c_str()));
	return true;
}
bool Handler::sendjpg(int nr) {
	bool ret=true;
	bool wait=false;
	CaptureData& cd=shm->captureData[nr];
	cd.mutex.lock();
	if (lastcounter == -1 ||  lastcounter < cd.picCounter) {
		if (write(sock,cd.jpgdata,cd.jpglen)==-1)
			ret=false;
		lastcounter=cd.picCounter;
		cout << cd.picCounter << endl;
	} else {
		wait=true;
	}
	cd.mutex.unlock();
	if (wait) {
		usleep(100000);
		cout << " wait" << endl;
	}
	return ret;
}



bool Handler::answer(string request) {
	string out;
	int nr=extractNr(request);
	if (!(nr < shm->settings->nocams))
		nr=-1;
	if (request.find(".jpg")!=std::string::npos &&  nr >-1){
		cout << "requested jpg from " << nr << endl;
		out  ="HTTP/1.1 200 OK\r\n";
		out +="Content-Type: image/jpeg\r\n\r\n";
		send(out);
		sendjpg(nr);
	} else if (request.find(".mjpg")!=std::string::npos &&  nr >-1){
		cout << "requested mjpg from " << nr << endl;
		out  ="HTTP/1.1 200 OK\r\n";
		out +="Cache-Control: no-cache\r\n";
		out +="Cache-Control: private\r\n";
		out +="Pragma: no-cache\r\n";
		out +="Content-type: multipart/x-mixed-replace; boundary="+splitter+"\r\n\r\n";
		send(out);
		while (	sendjpg(nr)) {
			send(splitter);
		}
	} else {
		cout << "sending 404" << endl;
		out ="HTTP/1.0 404 Not Found\r\n";
		send(out);
	}

	return true;
}

int Handler::extractNr(string str){
	unsigned int start=str.find("cam");
	if (start==std::string::npos) {
		return -1;
	}
	start+=3;
	unsigned int end=str.find(".",start);
	if (end==std::string::npos) {
		return -1;
	}
	stringstream ss;
	int nr=-1;
	ss << str.substr(start,end-start);
	ss >> nr;
	return nr;
}
bool Handler::read_in(string& str) {
	//~ const int BUFFER_LENGTH=1024;
	const int BUFFER_LENGTH=2;
	char buffer[BUFFER_LENGTH];
	memset (buffer,'\0',BUFFER_LENGTH);
	int nBytes = read (sock, buffer,BUFFER_LENGTH-1);
	if (nBytes < 0)
		return false;
	str=str+string(buffer);
	return true;
}


bool Handler::get_request(string& request) {
	string inbuffer;
	while (request=="") {
		if (read_in(inbuffer)) {
			int l = inbuffer.size();
			if (inbuffer.find("\r\n\r\n")!=std::string::npos) {
				cout << "request finished" << endl;
				unsigned int getstart=inbuffer.find("GET");
				if (getstart==std::string::npos) {
					cout << "did not find GET in request" << endl;
					return false;
				}
				getstart+=4;
				unsigned int getend=inbuffer.find(" ",getstart);
				request=inbuffer.substr(getstart,getend-getstart);
			}
		} else {
			return false;
		}

	}
	return true;
}

void Handler::run(int sock, Shm* shm) {
	this->sock=sock;
	this->shm=shm;
	cout << "Handling" << endl;
	string request("");
	if (!get_request(request))
		return;
	cout << "request is '" << request << "'" << endl;
	if (!answer(request))
		return;
	cout << "finished answering" << endl;
	close(sock);
}