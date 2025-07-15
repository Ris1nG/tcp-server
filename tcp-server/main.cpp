#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main(){
	//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	if (WSAStartup(ver, &wsData) != 0) {
		cerr << "Couldnt create a socket." << endl;
		return 1;
	}
	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Invalid socket." << endl;
		return 2;
	}

	//Bind the ip address and port to an socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(5400); //htons = h.ost t.o n.etwork s.hort
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		cerr << "Bind failed."<< endl;
		closesocket(listening);
		WSACleanup();
		return 3;
	}

	//Tell winsock that the socket in listening
	if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Listen failed." << endl;
		closesocket(listening);
		WSACleanup();
		return 4;
	}

	//Wait for a connection
	sockaddr_in client;
	
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*) & client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Client socket is invalid." << endl;
		closesocket(listening);
		WSACleanup();
		return 5;
	}

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXHOST]; //Service the client is connected on

	memset(host, 0, NI_MAXHOST); //Could use ZeroMemory from minwin
	memset(service, 0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " is connected at port: " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " is connect at port: " << ntohs(client.sin_port) << endl;
	}


	//Close listening socket
	closesocket(listening);

	//While loop: accept and message back the client
	const int MSG_BYTES = 4096;
	char buf[MSG_BYTES]; //TODO: accept more than 4k

	while (true) {
		memset(buf, 0, MSG_BYTES);
		//Wait for client send data
		const int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error receiving message from client." << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client disconnected." << endl;
			break;
		}

		//Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0); // bytesReceived + 1 -> when we receive a msg there is not a terminating 0 byte, 
													   // as we initialized buf with 0, with + 1 sends the 0 byte termination.
	}

	//Close the stock
	closesocket(clientSocket);

	//Shutdown winsock
	WSACleanup();
	return 0;
}