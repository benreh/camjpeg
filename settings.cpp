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
#include "settings.h"
#include <sstream>
using namespace std;
Settings::Settings(std::string filename) {
	cfg.load(filename);
	cfg.dump();
	nocams=cfg.getvalue<int>("nocams",1);
	requestLength=cfg.getvalue<int>("requestlength",10*1024);
	maxConnections=cfg.getvalue<int>("maxvonnections",20);
	idleTimeout=cfg.getvalue<int>("idletimeout",100);
	useGui=cfg.getvalue<int>("usegui",false);
}
Settings::~Settings() {

}
