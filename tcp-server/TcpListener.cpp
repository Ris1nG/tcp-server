#include "TcpListener.h"
#include <iostream>

TcpListener::TcpListener(std::string ipAddress, int port, MessageReceivedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler) {

}

TcpListener::~TcpListener() {
	Cleanup();
}

bool TcpListener::Init() {
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	if (WSAStartup(ver, &wsData) != 0) {
		std::cerr << "Couldnt create a socket." << std::endl;
		return false;
	}

	return true;
}

void TcpListener::Run() {
	SOCKET listening = CreateSocket();
	if (listening == NULL)
		return;

	SOCKET client = WaitForConnection(listening);

	if (client == NULL)
		return;

	char buf[MAX_BUFF_SIZE];

	while (true) {
		closesocket(listening);

		int bytesReceived = 0;
		do {
			memset(buf, 0, MAX_BUFF_SIZE);
			bytesReceived = recv(client, buf, MAX_BUFF_SIZE, 0);
			if (bytesReceived > 0) {
				if (MessageReceived != NULL) {
					MessageReceived(this, client, std::string(buf, 0, bytesReceived));
				}
			}

		} while (bytesReceived > 0);
	}

	closesocket(client);
}

void TcpListener::Send(int clientSocket, std::string msg) {
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

void TcpListener::Cleanup() {
	WSACleanup();
}

SOCKET TcpListener::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET) {
		std::cerr << "Invalid socket." << std::endl;
		return NULL;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(TcpListener::m_port); //htons = h.ost t.o n.etwork s.hort
	inet_pton(AF_INET, TcpListener::m_ipAddress.c_str(), &hint.sin_addr);

	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		std::cerr << "Bind failed." << std::endl;
		closesocket(listening);
		WSACleanup();
		return NULL;
	}

	if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed." << std::endl;
		closesocket(listening);
		WSACleanup();
		return NULL;
	}

	return listening;
}

SOCKET TcpListener::WaitForConnection(SOCKET listening) {
	sockaddr_in client;

	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		closesocket(clientSocket);
		WSACleanup();
		return NULL;
	}

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXHOST]; //Service the client is connected on

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " is connected at port: " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " is connect at port: " << ntohs(client.sin_port) << std::endl;
	}

	return clientSocket;
}