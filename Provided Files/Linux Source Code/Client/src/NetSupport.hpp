#ifndef __NetSupport
#define __NetSupport

#include "GameControl.hpp"

const int  REQ_WINSOCK_VER   = 2;	// Minimum winsock version required

const int  TEMP_BUFFER_SIZE  = 128;
typedef unsigned long IPNumber;

class NetSupport
{
public:
  NetSupport();
  void NetworkInit();
  void NetworkConnect(const char *pServername, const int HostPort, const char *pTeamname);
  void NetworkConversation();
  void NetworkCloseAndCleanUp();

private:
  IPNumber FindHostIP(const char *pServerName);
  void FillSockAddr(sockaddr_in *pSockAddr, const char *pServerName, int portNumber);

  int hSocket;
  char Team[128];
  GameControl *g;
};

#endif // __NetSupport
