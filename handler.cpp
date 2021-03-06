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

Handler::Handler() :  splitter("splitter"){

}

Handler::~Handler() {

}

bool Handler::send_str(string str) {
	if (send(sock,str.c_str(),strlen(str.c_str()),MSG_NOSIGNAL)==-1)
		return false;
	return true;
}
bool Handler::sendjpg(int nr) {
	bool ret=true;
	int picCounter=0;
	CaptureData& cd=shm->captureData[nr];
	cd.mutex.lock();
	cd.idleCounter=0;
	picCounter=cd.picCounter;
	cd.mutex.unlock();

	bool repeat=true;
	unsigned char* outbuffer=0;
	long unsigned int outlen=0;
	while (repeat) {
		cd.mutex.lock();
		if (picCounter!=cd.picCounter) {
			outlen=cd.jpglen;
			outbuffer = new unsigned char[outlen];
			memcpy(outbuffer,cd.jpgdata,outlen);
			repeat=false;
		} 
		cd.mutex.unlock();
		if (repeat)
			usleep(10000);
	}
	if (outbuffer) {
		if (send(sock,outbuffer,outlen,MSG_NOSIGNAL)==-1) {
			ret=false;
		}
		delete[] outbuffer;
	}
	return ret;
}



bool Handler::answer(string request) {
	string out;
	int nr=extractNr(request);
	if (!(nr < shm->settings->nocams))
		nr=-1;
	if ((request.find(".jpg")!=std::string::npos || request.find(".jpeg")!=std::string::npos) &&  nr >-1){
		cout << "requested jpg from " << nr << endl;
		out  ="HTTP/1.1 200 OK\r\n";
		out +="Content-Type: image/jpeg\r\n\r\n";
		send_str(out);
		sendjpg(nr);
	} else if ((request.find(".mjpg")!=std::string::npos || request.find(".mjpeg")!=std::string::npos) &&  nr >-1){
		cout << "requested mjpg from " << nr << endl;
		out  ="HTTP/1.1 200 OK\r\n";
		out +="Cache-Control: no-cache\r\n";
		out +="Cache-Control: private\r\n";
		out +="Pragma: no-cache\r\n";
		out +="Content-type: multipart/x-mixed-replace; boundary="+splitter+"\r\n\r\n";
		send_str(out);
		bool wait=false;
		bool running=true;
		int lastcounter(-1);
		while (running && !global_quit) {
			unsigned char* outbuffer=0;
			long unsigned int outlen=0;
			CaptureData& cd=shm->captureData[nr];
			cd.mutex.lock();
			if (lastcounter == -1 ||  lastcounter < cd.picCounter) {
				running=false;
				outlen=cd.jpglen;
				outbuffer = new unsigned char[outlen];
				memcpy(outbuffer,cd.jpgdata,outlen);
				lastcounter=cd.picCounter;
				cd.idleCounter=0;
			} else {
				wait=true;
			}
			cd.mutex.unlock();
			if (wait) {
				usleep(10000);
			}
			if (outbuffer){
				if (send_str(string("--")+splitter)) {
					running=true;
				} 
				if (send_str("Content-type: image/jpeg\n\n")) {
					running=true;
				} 
				if (send(sock,cd.jpgdata,cd.jpglen,MSG_NOSIGNAL)!=-1) {
					running=true;
				}
				delete[] outbuffer;
			}
		}
	} else {
		cout << "sending 404" << endl;
		out ="HTTP/1.0 404 Not Found\r\n";
		send_str(out);
	}

	return true;
}

int Handler::extractNr(string str){
	unsigned int start=str.find("cam");
	if (start==std::string::npos) {
		return -1;
	}
	start+=3;
	if (str.length()<start)
		return -1;
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
	if (nBytes > 0) {
		str=str+string(buffer);
		return true;
	}
	return false;
}


bool Handler::get_request(string& request) {
	string inbuffer;
	while (request=="") {
		if (read_in(inbuffer)) {
			int l = inbuffer.size();
			if (l> shm->settings->requestLength) {
				cout << "Request header is too big, someone is messing around?" << endl;
				return false;
			}
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
			cout << "error while reading:" << endl;
			cout << inbuffer << endl;
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
	if (!get_request(request)) {
		cout << "Error in request" << endl;
		close(sock);
		return;
	}
	cout << "request is '" << request << "'" << endl;
	if (!answer(request)) {
		close(sock);
		return;
	}
	cout << "finished answering" << endl;
	close(sock);
}
