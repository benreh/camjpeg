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
#ifndef SERVER_H
#define SERVER_H
#include "shm.h"
class Server {
public:
Server(Shm* shm);
~Server();
bool run();

private:
bool prepare_socket(int& origsock, int port);
bool select_call(int sock, int time);
int accept_call(int sock_listen);
Shm* shm;
};
#endif //SERVER_H