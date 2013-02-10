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
#ifndef HANDLER_H
#define HANDLER_H
#include <string>
#include "shm.h"

using namespace std;
class Handler {
public:
	Handler();
	~Handler();
	void run(int sock, Shm* shm);
private:
	bool read_in(string& str);
	bool get_request(string& request);
	bool answer(string request);
	bool send(string str);
	bool sendjpg(int nr);
	int extractNr(string str);
	int sock;
	Shm* shm;
	int lastcounter;
	string splitter;
};
#endif //HANDLER_H
