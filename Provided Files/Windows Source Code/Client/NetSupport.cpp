#include "stdafx.h"
#include "NetSupport.hpp"
#include "Exception.hpp"

using namespace std;

NetSupport::NetSupport() {};

IPNumber NetSupport::FindHostIP(const char *pServerName)
{
	HOSTENT *pHostent;

	// Get hostent structure for hostname:
	if (!(pHostent = gethostbyname(pServerName)))
			throw Exception("could not resolve hostname.");
	
	// Extract primary IP address from hostent structure:
	if (pHostent->h_addr_list && pHostent->h_addr_list[0])
		return *reinterpret_cast<IPNumber*>(pHostent->h_addr_list[0]);
	
	return 0;
}

void NetSupport::FillSockAddr(sockaddr_in *pSockAddr, const char *pServerName, int portNumber)
{
	// Set family, port and find IP
	pSockAddr->sin_family = AF_INET;
	pSockAddr->sin_port = htons(portNumber);
	pSockAddr->sin_addr.S_un.S_addr = FindHostIP(pServerName);
}

void NetSupport::NetworkInit() {
	WSADATA wsaData;

	if (!WSAStartup(MAKEWORD(REQ_WINSOCK_VER,0), &wsaData)) {               // Winsock startup
		if (LOBYTE(wsaData.wVersion) < REQ_WINSOCK_VER) {                    // Check it supports Winsock 2
			throw Exception("Winsock required version not supported");
		}
  } else {
		throw Exception("Winsock startup failed");
  }
  return;
}

void NetSupport::NetworkConnect(const char *pServername, const int HostPort, const char *pTeamname) {

	sockaddr_in	sockAddr = {0};
  char tempbuf[128];
	
  strcpy_s(Team, 128, pTeamname);

	// Lookup hostname and fill sockaddr_in structure:
	cout << "Looking up hostname " << pServername << "... ";
	FillSockAddr(&sockAddr, pServername, HostPort);
	cout << "found.\n";
	
	cout << "Creating socket... ";
	if ((hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) throw Exception("Could not create socket");
	cout << "created.\n";
		
	sprintf_s(tempbuf, 128,"%s:%d", inet_ntoa(sockAddr.sin_addr),HostPort); 
  cout << "Attempting to connect to " << tempbuf <<  "... ";
	if (connect(hSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr))!=0) {
  	sprintf_s(tempbuf, 128,"Could not connect to %s:%d", inet_ntoa(sockAddr.sin_addr),HostPort); 
    throw Exception(tempbuf);
  }
	cout << "connected.\n";
	return;
};



void NetSupport::NetworkCloseAndCleanUp() {
  if (hSocket!=INVALID_SOCKET)
	{
		closesocket(hSocket);
	}
  WSACleanup();
  return;
};

