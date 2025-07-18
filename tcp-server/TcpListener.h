#pragma once

#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

#define MAX_BUFF_SIZE 89064

class TcpListener;

typedef void (*MessageReceivedHandler)(TcpListener* listener, int socketId, std::string msg);

class TcpListener {

public:
	TcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);

	~TcpListener();

	bool Init();

	void Run();

	void Send(int clientSocket, std::string msg);

	void Cleanup();


private:
	std::string m_ipAddress;
	int m_port;
	MessageReceivedHandler MessageReceived;

	SOCKET CreateSocket();
	SOCKET WaitForConnection(SOCKET listening);
};