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

using namespace std;

class connection{
protected:


public:
	connection();
	
	virtual int send(char*, int) = 0;
	virtual int recv(char*, int) =0;

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

	int send(char*, int);
	int recv(char*, int);
};

class server: public connection{
	WSADATA _wsaData;
	SOCKET _lSocket; // listen socket
	vector<SOCKET> _clientSocket;

public:
	server();
	~server();

	void setup(char* listen_port);
	void listen();

	int send(char*, int);
	int recv(char*, int);

};
