//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  NetSupport_Server class definition for MSControl_Server.cpp.
//  This class controls all the supporting data to run the sockets
//  interface.
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#ifndef __NetSupport
#define __NetSupport

#include "GameControl.hpp"

const int  REQ_WINSOCK_VER   = 2;	                                        // Minimum winsock version required
typedef unsigned long IPNumber;                                           // IPv4 assumed

class NetSupport_Server
{
public:
  NetSupport_Server();
  void NetworkInit();                                                     // WSA Startup
  void NetworkBindListen(const char *pServername, const int HostPort);    // Socket create, bind, listen
  void NetworkAccept();                                                   // Socket accept
  void NetworkConversation(const char*GameFile);                          // Socket recv, send for the game
  void NetworkCloseAndCleanUp();                                          // Socket close and WSA Cleanup

private:
  IPNumber FindHostIP(const char *pServerName);                           // Helper for IP lookup from host name
  void SetServerSockAddr(sockaddr_in *pSockAddr, int portNumber);         // Helper to set up the bind paranters 
  void GetClientDescription(const sockaddr_in &sockAddr);                 // Helper to look up client's host details

  SOCKET hListenerSocket;                                                 // Listener socket for inbound connections
  SOCKET hClientSocket;                                                   // Client socket for the game conversation (post-accept)
  char ClientDesc[128];                                                   // Client's host details
  sockaddr_in clientSockAddr;                                             // Socket structure for bind
  int clientSockSize;                                                     // Size of client socket!
  GameControl *g;                                                         // The Game !!
};

#endif // __NetSupport