#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#pragma once
class Talk
{
private:
	SOCKET ClientSocket;
public:
	Talk();
	void Comm(Talk t, SOCKET listening);
	~Talk();
};

