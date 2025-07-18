#include <iostream>
#include "TcpListener.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void Listener_MessageReceived(TcpListener* listener, int client, string msg);

int main(){

	TcpListener server("127.0.0.1", 5400, Listener_MessageReceived);

	if (server.Init()) {
		server.Run();
	}

	return 0;
}

void Listener_MessageReceived(TcpListener* listener, int client, string msg)
{
	listener->Send(client, msg);
}