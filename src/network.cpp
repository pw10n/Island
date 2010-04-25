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

#include "netutil.h"
#include "network.h"
#include "types.h"

using namespace std;

client::client(): _state(CLIENT_DISCONNECT), _cSocket(INVALID_SOCKET){}

client::~client(){
	if( _state != CLIENT_DISCONNECT){
		shutdown(_cSocket, SD_SEND);
		closesocket(_cSocket);
		WSACleanup();
		_cSocket = INVALID_SOCKET;
		_state = CLIENT_DISCONNECT;
	}
}

void client::connectTo(char* addr, char* port){
	
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	if( _state != CLIENT_DISCONNECT){
		shutdown(_cSocket, SD_SEND);
		closesocket(_cSocket);
		WSACleanup();
		_cSocket = INVALID_SOCKET;
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

int client::sendBuf(char * buf, int len){
	int retval;
	if(SOCKET_ERROR == 
		(retval = send(_cSocket, buf, len, 0))){
		closesocket(_cSocket);
		WSACleanup();
		_state = CLIENT_DISCONNECT;
		throw gException("send failed.");
	}
	return retval;
}

int client::recvBuf(char * buf, int len){
	int retval;
	if(SOCKET_ERROR ==
		(retval = recv(_cSocket, buf, len, 0))){
		closesocket(_cSocket);
		WSACleanup();
		_state = CLIENT_DISCONNECT;
		_cSocket = INVALID_SOCKET;
		throw gException("recv failed.");
	}
	return retval;
}


server::server(): _state(SERVER_DISCONNECT), 
				  _lSocket(INVALID_SOCKET) {}

server::~server(){
	disconnectAll();
}

void server::disconnectAll(){
		if( _state != SERVER_DISCONNECT){
		shutdown(_lSocket, SD_SEND);
		closesocket(_lSocket);
		WSACleanup();
		_lSocket = INVALID_SOCKET;
		_state = SERVER_DISCONNECT;
	}
	
	for(vector<cInfo>::iterator it = _clients.begin();
		it != _clients.end(); ++it){
			if ( (*it).state != CLIENT_DISCONNECT ){
				shutdown((*it).cSocket, SD_SEND);
				closesocket((*it).cSocket);
				(*it).cSocket = INVALID_SOCKET;
				(*it).state = CLIENT_DISCONNECT;
			}
	}

	_clients.erase(_clients.begin(), _clients.end());
}

void server::setup(char* listen_port){

	if( _state != SERVER_DISCONNECT){
		disconnectAll();
	}

	if(0 != WSAStartup(MAKEWORD(2,2), &_wsaData)){
		throw gException("WSAStartup failed.");
	}
	
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if(0 != getaddrinfo(NULL, listen_port, &hints, &result)){
		WSACleanup();
		throw gException("getaddrinfo failed.");
	}

	if( INVALID_SOCKET == 
		(_lSocket = socket(	result->ai_family, 
							result->ai_socktype, 
							result->ai_protocol))){
		freeaddrinfo(result);
		WSACleanup();
		throw gException("Error at socket()");
	}

	if ( SOCKET_ERROR == 
			bind(	_lSocket, 
					result->ai_addr, 
					(int)result->ai_addrlen)) {
		freeaddrinfo(result);
		closesocket(_lSocket);
		WSACleanup();
		throw gException("bind failed");
	}

	if( SOCKET_ERROR ==
			listen(_lSocket, SOMAXCONN) ){
		closesocket(_lSocket);
		WSACleanup();
		throw gException("listen failed.");
	}
}

void server::acceptClient(){

	cInfo client_info;

	client_info.cSocket = INVALID_SOCKET;
	client_info.state = CLIENT_DISCONNECT;

	if( INVALID_SOCKET == 
		( client_info.cSocket = accept(_lSocket, NULL, NULL))) {
		gException("accept failed");
		//TBD: is _lSocket still valid here or do I
		//     have to shutdown the server?
	}

	client_info.state = CLIENT_ACTIVE;
	_clients.push_back(client_info);

}

int server::sendBuf(char * buf, int len){
	// send to all clients, delete from client
	// list if client has disconnected
	for(vector<cInfo>::iterator it = _clients.begin();
		it != _clients.end(); 
		it = ((*it).state==CLIENT_DISCONNECT)?
		_clients.erase(it):it+1){
		if (CLIENT_ACTIVE == (*it).state)
			send((*it).cSocket, buf, len, 0);
	}
	return 1; //TBD
}

void server::tickSnd(){
	char buf[MAX_PKTSZ];
	char* ptr = buf;
	uint32_t size=0;

	int count = 0;

	((pkt_header*)(buf))->start = '#';
	((pkt_header*)(buf))->type = PKT_DELTA_GAMESTATE;
	((pkt_header*)(buf))->clientid = 0;
	((pkt_header*)(buf))->serverid = 0; //TODO
	((pkt_header*)(buf))->checksum = 0;
	((pkt_header*)(buf))->length = 0;
	((pkt_header*)(buf))->seq = 0; //TODO

	size+=sizeof(pkt_header);
	ptr+=size;

	for(vector<gDelta_data>::iterator it = _gObj->_deltas.begin();
		it != _gObj->_deltas.end();
		it = _gObj->_deltas.erase(it)){
			//TBD -- maybe i should copy each field
			memcpy(ptr,&(*it),sizeof(gDelta_data));
			size+=sizeof(gDelta_data);
			ptr+=sizeof(gDelta_data);
			++count;

			if(size > MAX_PKTSZ - sizeof(gDelta_data) - 1)
				break;
	}

	((pkt_header*)(buf))->length = count;

	((pkt_header*)(buf))->checksum = calcAddSum(buf,size);
	
	sendBuf(buf, size);//TODO: send

	if(_gObj->_deltas.size() != 0)
		tickSnd(); // send next packet if more to send.
}

void server::tickRcv(){
	struct timeval timeout;
	fd_set socks;
	int readsocks;
	FD_ZERO(&socks);
	FD_SET(_lSocket, &socks);

	for(vector<cInfo>::iterator it = _clients.begin();
		it != _clients.end();
		++it){
			FD_SET((*it).cSocket, &socks);
	}
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	readsocks = select(0,&socks,NULL,NULL,&timeout);

	if (FD_ISSET(_lSocket, &socks)){
		//listen socket
		acceptClient();
	}
	for(vector<cInfo>::iterator it = _clients.begin();
		it < _clients.end();
		++it){
		if (FD_ISSET((*it).cSocket, &socks)){
			//got data
			int result=0;
			char buf[MAX_PKTSZ];
			
			//read
			if(SOCKET_ERROR ==
				(result = recv((*it).cSocket, buf, MAX_PKTSZ, 0))){
				closesocket((*it).cSocket);
				WSACleanup();
				(*it).state = CLIENT_DISCONNECT;
				(*it).cSocket = INVALID_SOCKET;
				throw gException("recv failed.");
			}
			
			//check then process
			if( ((pkt_header*)(buf))->start != '#' ){
				char ackbuf[1024];
				int acksz = 0;
				acksz = make_ack(ackbuf, 1024, NET_NACK,SEQ_INVALID_PKT);
				send((*it).cSocket, ackbuf, acksz, 0);
			}
			else if( !verify_checksum(buf, result) ){
				char ackbuf[1024];
				int acksz = 0;
				acksz = make_ack(ackbuf, 1024, NET_NACK,((pkt_header*)(buf))->seq);
				send((*it).cSocket, ackbuf, acksz, 0);
			}
			else{
				//TODO process(buf); //process packet
				playerChangeMove_data* mvPtr;
				bool found = false;
				switch(((pkt_header*)(buf))->type){

					case PKT_PLAYER_MOVE:
						mvPtr = (playerChangeMove_data*)(buf+sizeof(pkt_header));
						found = false;
						for(vector<playerstate_t>::iterator it = _gObj->_players.begin();
							it != _gObj->_players.end();
							++it){
								if ((*it)._id == mvPtr->_id){
									found=true;
									(*it).change_velocity(mvPtr->_vel_x, mvPtr->_vel_y);
								}
						}
						break;

					case PKT_PLAYER_ATTACK:
						break; //TODO

					default:
						break;

				}
			}
		}
	}
}
