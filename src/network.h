#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <cstdio>

#include <iostream>
#include <vector>

#include "gs_types.h"

using namespace std;

class connection{
protected:


public:
	connection();
	
	virtual int sendBuf(char*, int) = 0;

};

#define CLIENT_DISCONNECT 0
#define CLIENT_ACTIVE 1

class client: public connection{
	WSADATA _wsaData;
	SOCKET _cSocket;
	int _state;

public:
	client();
	~client();

	void connectTo(char* addr, char* port);

	int sendBuf(char*, int);
	int recvBuf(char*, int);
};

#define SERVER_DISCONNECT 0
#define SERVER_ACTIVE 1

class server: public connection{
	WSADATA _wsaData;
	SOCKET _lSocket; // listen socket

	struct cInfo{
		SOCKET cSocket;
		int state;
	};
	vector<cInfo> _clients;
	gamestate_t* _gObj;

	int _state;

public:
	server();
	~server();

	void setup(char* listen_port);

	int sendBuf(char*, int);

protected:
	void tickSnd();
	void tickRcv();
	void acceptClient();

	void disconnectAll();

};


