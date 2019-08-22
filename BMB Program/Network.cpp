#include "Network.h"
#include "mydrawengine.h"

Network* Network::myNetwork = nullptr;


Network* Network::GetInstance()
{
	if (myNetwork == nullptr)
	{
		myNetwork = new Network;
	}
	return myNetwork;
} // GetInstance()



void Network::StartWinsock()
{
	int wsOk = WSAStartup(m_wsaVersion, &wsaData);
	if (wsOk != 0)
	{
  m_winsockStatus = L"::  Winsock Unsuccessful!";
	}
 m_winsockStatus = L"::  Winsock Successful!";
} // Start()



void Network::StartServer(int port)
{
	m_sockIn = socket(AF_INET, SOCK_DGRAM, 0);
	s_sockAddrServerHint.sin_addr.S_un.S_addr = ADDR_ANY;
	s_sockAddrServerHint.sin_family = AF_INET;
	s_sockAddrServerHint.sin_port = htons(port);
	if (bind(m_sockIn, (sockaddr*)&s_sockAddrServerHint, sizeof(s_sockAddrServerHint)) == SOCKET_ERROR)
	{
		return;
	}
 m_networkStatus = L"Server";
} // StartServer()



void Network::StartClient(char* ipaddress, int port)
{
	s_serverAddr.sin_family = AF_INET;
	s_serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ipaddress, &s_serverAddr.sin_addr);
	m_sockOut = socket(AF_INET, SOCK_DGRAM, 0);
 m_networkStatus = L"Client";
} // StartClient()



void Network::UpdateNetwork(void* object, const int& size)
{
	if (MAKE_SERVER == true)
	{
  // Recieves the DynamicObjects array from the client
		char* Arr = new char[size];
		if (recv(m_sockIn, Arr, size, 0) > 0)
		{
   // Copies the data into memory
			memcpy(object, Arr, size);
		}
		delete[] Arr;
	}
	else
	{
  // Sends the entirety of the DynamicObjects instance to the server
		sendto(m_sockOut, (char*)object, size, 0, (sockaddr*)&s_serverAddr, sizeof(s_serverAddr));
	}
} // UpdateNetwork()



void Network::CloseWinsock()
{
 closesocket(m_sockOut);
	closesocket(m_sockIn);
	WSACleanup();
} // CloseWinsock()



void Network::NetworkInfo()
{
 MyDrawEngine::GetInstance()->DrawAt(Vector2D(13.0f, 22.0f), MyDrawEngine::GetInstance()->LoadPicture(L"server.bmp"), 0.6f, 0.0f, 0.0f);
 MyDrawEngine::GetInstance()->WriteText(30.0f, 10.0f, m_networkStatus.c_str(), _XRGB(128, 77, 77));
 MyDrawEngine::GetInstance()->WriteText(100.0f, 10.0f, m_winsockStatus.c_str(), _XRGB(184, 137, 137));
} // NetworkInfo()
