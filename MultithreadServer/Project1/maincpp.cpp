#include <stdio.h>
#include <iostream>
#include <thread>
#include <WS2tcpip.h>
#include "Talk.h"

#pragma comment(lib, "ws2_32.lib")

void func1() {
	std::cout << "Yo you got somma dat" << std::endl;
	std::cout << "...ASSSympotic behaviour on that graph there" << std::endl;
}

void comm(SOCKET ClientSocket, sockaddr_in client, int clientsize)
{


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

int main() {
	
	//// THREADS
	std::thread t1(func1); // t1 start running
	std::cout << "Hello World!" << std::endl;
	t1.join(); // sync with main
	//t1.detach();
	
	///
	// Initialize Winsock
	WSADATA wsaData;

	int iResult; // startup result
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //startup, request version 2.2
	if (iResult != 0) {
		std::cerr << "Could not initialize Winsock. Error "<< iResult << ". Quitting..." << std::endl;
		return -1;
	}
	// Otherwise, Startup is successful

	// Create a socket


	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Can't create a socket. Error code: " << WSAGetLastError() << std::endl;
		return -1;
	}

	// Bind the socket to an ip address and a port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // or could put in your IP here using inet_pton

	// bind
	iResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
	if (iResult == SOCKET_ERROR) {
		std::cerr << "Trouble binding. Error " << WSAGetLastError() << ". Quitting..." << std::endl;
		closesocket(listening);
		WSACleanup();
		return -1;
	}
	// Tell winsock the socket is for listening
	iResult = listen(listening, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "listening failed :( Error " << WSAGetLastError() << ". Quitting..." << std::endl;
		closesocket(listening);
		WSACleanup();
		return -1;
	}
	

	// listen on the network (wait for a conneciton)

	//Talk talk;
	// Create  a temp socket object for the client
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET ClientSocket;
	// Accept a connection
	ClientSocket = accept(listening, (sockaddr*)&client, &clientsize);
	if (ClientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed. Error " << WSAGetLastError() << std::endl;
		//closesocket(listening);
		//WSACleanup();
		return -1;
	}


	std::thread t2(comm, ClientSocket, client, clientsize);
	t2.join();

	//

	// close the listening socket
	closesocket(listening);

	// Cleanup Winsock
	WSACleanup();


	return 0;
}