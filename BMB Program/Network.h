// Title  :  Network.h
// Purpose:  Networking Functionality
// Author :  Jonathon H Tyson
// Date   :  09/05/2018

#include <iostream>
#include <WS2tcpip.h>
#include <string.h>

// Used for server choice
#define IPADDRESS   "172.16.1.30"
#define PORT        4000
#define MAKE_SERVER false

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

class Network
{
private:

	WSADATA         wsaData;
	WORD            m_wsaVersion = MAKEWORD(2, 2);
	SOCKET          m_sockIn;
	SOCKET          m_sockOut;
	sockaddr_in     s_sockAddrServerHint;
	sockaddr_in     s_clientAddr;
	sockaddr_in     s_serverAddr;
	int             m_clientLength = sizeof(s_clientAddr);
	static Network* myNetwork;
 std::wstring    m_networkStatus;
 std::wstring    m_winsockStatus;

public:
	static Network* GetInstance();
	void StartWinsock();
	void StartServer(int port);
	void StartClient(char* ipaddress, int port);
	void UpdateNetwork(void* object, const int& size);
	void CloseWinsock();
 void NetworkInfo();
};