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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <boost/thread.hpp>
#include <signal.h>


#include "server.h"
#include "handler.h"

using namespace std;

extern bool global_quit;

//If not return false
#define IFNF(X) if (!X) return false

Server::Server(Shm* shm) {
	this->shm=shm;
}
Server::~Server() {

}

bool Server::prepare_socket(int& origsock, int port) {
	struct sockaddr_in name;
    origsock = socket (PF_INET, SOCK_STREAM, 0);
    name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	IFNF(bind (origsock, (struct sockaddr *) &name, sizeof (name)) == 0);
	IFNF(listen (origsock, 1) == 0);
	cout << "Starting to serve at port " << port << endl;
	return true;
}

bool Server::select_call(int sock, int time) {
	fd_set fd_set;
	FD_ZERO (&fd_set);
	FD_SET(sock, &fd_set);
	// Set a timout for the select call
	//  this allows to catch the quit-command
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = time;
	//Select call
	int ret=select (FD_SETSIZE, &fd_set, NULL, NULL, &tv);

	if (ret > 0 && FD_ISSET(sock, &fd_set))
		return true;
	else
		return false;
}

int Server::accept_call(int sock_listen) {
        struct sockaddr_in clientname;
        unsigned int size = sizeof (clientname);
        int sock_new = accept (sock_listen, (struct sockaddr *) &clientname,  &size);
        assert(sock_new >=0);

        cout << "Connection from " <<  inet_ntoa (clientname.sin_addr) << ":" <<   ntohs (clientname.sin_port) << endl;
        //~ int flag = 1;
        //~ setsockopt(sock_new,            /* socket affected */
                         //~ IPPROTO_TCP,     /* set option at TCP level */
                         //~ TCP_NODELAY,     /* name of option */
                         //~ (char *) &flag,  /* the cast is historical cruft */
                         //~ sizeof(int));   /* length of option value */

        return sock_new;
}


bool Server::run() {
	int sock;
	while (! prepare_socket(sock, shm->settings->cfg.getvalue<int>("port",8888)))
	{
		 sleep(1);
	}

	vector<boost::thread*> threads;
	vector<Handler*> handler;

	while(!global_quit) {
		bool ret=select_call(sock,100000);
		if (ret) {
			cout << "CONNETION" << endl;
			int sock_new = accept_call(sock);
			Handler* h=new Handler();
			handler.push_back(h);
			boost::thread* thr = new boost::thread ( boost::bind( &Handler::run, h,sock_new,shm ) );
			threads.push_back(thr);
		}
		
	

	}
	cout << "stopping server..." << endl;
	for (vector<boost::thread*>::iterator th=threads.begin();th!=threads.end(); th++) {
		 int n = (int)boost::posix_time::time_duration::ticks_per_second() / 10; 
        boost::posix_time::time_duration delay(0,0,0,n); 
        (*th)->timed_join(delay);
		delete *th;
	}
	return true;
}
