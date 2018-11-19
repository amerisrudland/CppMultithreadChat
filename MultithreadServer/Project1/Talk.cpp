#include "Talk.h"
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")



Talk::Talk()
{
}

void Talk::Comm(Talk t, SOCKET listening)
{
	// Create  a temp socket object for the client
	sockaddr_in client;
	int clientsize = sizeof(client);

	//SOCKET ClientSocket;
	// Accept a connection
	t.ClientSocket = accept(listening, (sockaddr*)&client, &clientsize);
	if (ClientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed. Error " << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, clientsize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << "A client connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	// Close the listening socket (not multithreading yet)
	//closesocket(listening);


	// Talk to the client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// wait for client to send data
		int bytesReceived = recv(ClientSocket, buf, 4095, 0);
		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error receiving. Error " << WSAGetLastError() << std::endl;
		}
		if (bytesReceived == 0) {
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		// echo message back to client
		send(ClientSocket, buf, bytesReceived + 1, 0);
	}


	// Close the communication socket
	closesocket(ClientSocket);
}


Talk::~Talk()
{
}
