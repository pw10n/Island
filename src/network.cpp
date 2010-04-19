
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <cstdio>

#include "network.h"
#include "types.h"

using namespace std;

client::client(): _state(CLIENT_DISCONNECT){}

client::~client(){
	if( _state != CLIENT_DISCONNECT){
		closesocket(_cSocket);
		WSACleanup();
		_state = CLIENT_DISCONNECT;
	}
}

void client::connectTo(char* addr, char* port){
	
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	if( _state != CLIENT_DISCONNECT){
		closesocket(_cSocket);
		WSACleanup();
		_state = CLIENT_DISCONNECT;
	}

	if( WSAStartup(MAKEWORD(2,2), &_wsaData) ){
		throw gException("WSAStartup failed.");
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if( getaddrinfo(addr, port, &hints, &result) ){
		throw gException("getaddrinfo failed.");
	}
	
	for(ptr=result; ptr != NULL; ptr=ptr->ai_next){
		if( (_cSocket = socket(ptr->ai_family, ptr->ai_socktype, 
			ptr->ai_protocol)) == INVALID_SOCKET ){
				gException("Error at socket().");
		}

		if( connect( _cSocket, ptr->ai_addr, 
			(int)ptr->ai_addrlen) == SOCKET_ERROR){
				closesocket(_cSocket);
				_cSocket = INVALID_SOCKET;
				continue;
		}
		break;
	}

	freeaddrinfo(result);

	_state = CLIENT_ACTIVE;
}
