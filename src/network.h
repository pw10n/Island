#pragma once

class connection{

public:
	connection();
	
	virtual int send(char*, int) = 0;
	virtual int recv(char*, int) =0;

};