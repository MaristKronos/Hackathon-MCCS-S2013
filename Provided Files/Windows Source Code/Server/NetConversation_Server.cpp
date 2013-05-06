//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Runs the network game conversation for MSControl_Server.
//
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================



#include "stdafx.h"
#include "NetSupport_Server.hpp"
#include "Exception.hpp"

using namespace std;


void NetSupport_Server::NetworkConversation(const char *GameFile) {

  char sendBuffer[256];
  char *sb = &sendBuffer[0];
  char recvBuffer[256];
  char *rb=&recvBuffer[0];
  char recvCmd[32];
  int recvBufLen = 256;
  int recvBytes;

  // open sockets. 
  //   INIT {teamname}
  //                      ACCEPT
  //   RECD
  //                      COSTS {profit} {W-cost} {J-cost} {D-Cost}
  //   START
  // while not END..
  //                      CONFIG {NA-W} {NA-J} {NA-D} {EU-W} {EU-J} {EU-D} {AP-W} {AP-J} {AP-D}
  //   RECD
  //                      DEMAND {day} {time} {NA-U} {EU-U} {AP-U}
  //   RECD
  //                      DIST {NA-W} {NA-J} {NA-D} {EU-W} {EU-J} {EU-D} {AP-W} {AP-J} {AP-D}
  //   RECD
  //                      PROFIT {profit_this_period} {cumulative_profit} 
  //
  //   CONTROL {day} {time} {NA-W} {NA-J} {NA-D} {EU-W} {EU-J} {EU-D} {AP-W} {AP-J} {AP-D}
  //
  //  ... and 30secs of game life happens here ... 
  //
  // wend...
  //                      END
  //   STOP
  // close sockets.
  //
  // IF ... any error ... server sends ERROR then a description. (E.g. ERROR Client asked for negative W.NA instances!)
  //
  //
  cout << "\nStarting conversation!\n";

  g = new GameControl(GameFile, ClientDesc);

  try {

    // Expect opening INIT
    if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed", WSAGetLastError());
    recvBuffer[recvBytes] = 0;
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
    if (strcmp(recvCmd,"INIT")) throw ConvException("Expected INIT");
	rb = recvBuffer+5;
    g->recvInit(rb);
    cout << "      <<  " << recvBuffer << "\n";

    // Send ACCEPT
    sprintf_s(sendBuffer, 256, "ACCEPT");
    cout << ">> " << sendBuffer << "\n";
    if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation ACCEPT failed", WSAGetLastError());

    // Expect RECD
    if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed", WSAGetLastError());
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
    if (strcmp(recvCmd,"RECD")) throw ConvException("Expected RECD");

    // Send COSTS
    sendBuffer[0] = 0;
    g->serCosts(sb);
    cout << ">> " << sendBuffer << "\n";
    if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation COSTS failed", WSAGetLastError());

    // Expect START
    if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed", WSAGetLastError());
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
    if (strcmp(recvCmd,"START")) throw ConvException("Expected START");


    while (true) {

      cout << "\n--- G a m e   T u r n ---\n";
      if (!g->gameTurn()) break;

      // Send CONFIG
      g->serConfig(sb);
      cout << ">> " << sendBuffer << "\n";
      if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation CONFIG failed", WSAGetLastError());

      // Expect RECD
      if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed", WSAGetLastError());
      recvBuffer[recvBytes] = 0;
      cout << "      <<  " << recvBuffer << "\n";
      if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
      if (strcmp(recvCmd,"RECD")) throw ConvException("Expected RECD");

      // Send DEMAND
      g->serDemand(sb);
      cout << ">> " << sendBuffer << "\n";
      if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation DEMAND failed");

      // Expect RECD
      if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed");
      recvBuffer[recvBytes] = 0;
      cout << "      <<  " << recvBuffer << "\n";
      if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
      if (strcmp(recvCmd,"RECD")) throw ConvException("Expected RECD");

      // Send DIST
      g->serDist(sb);
      cout << ">> " << sendBuffer << "\n";
      if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation DIST failed", WSAGetLastError());

      // Expect RECD
      if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation INIT failed", WSAGetLastError());
      recvBuffer[recvBytes] = 0;
      cout << "      <<  " << recvBuffer << "\n";
      if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
      if (strcmp(recvCmd,"RECD")) throw ConvException("Expected RECD");

      // Send PROFIT
      g->serProfit(sb);
      cout << ">> " << sendBuffer << "\n";
      if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation PROFIT failed", WSAGetLastError());

      // Expect CONTROL back
      if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation CONTROL failed", WSAGetLastError());
      recvBuffer[recvBytes] = 0;
      cout << "      <<  " << recvBuffer << "\n";
      if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
      if (strcmp(recvCmd,"CONTROL")) throw ConvException("Expected CONTROL"); 
	    rb = recvBuffer+8;
	    g->recvControl(rb);
    }

    // Send END
    sprintf_s(sendBuffer, 256, "END");
    cout << ">> " << sendBuffer << "\n";
    if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw ConvException("Conversation END failed", WSAGetLastError());

    // Expect STOP back
    if ((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw ConvException("Conversation STOP failed", WSAGetLastError());
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw ConvException("Reading command");
    if (strcmp(recvCmd,"STOP")) throw ConvException("Expected STOP"); 
    cout << "\nSuccessful conversation!\n";

  } catch (ConvException e) {
    int s_err = e.s_err();
    if (!s_err) {
      sprintf_s(sendBuffer, "%s", e.what());
    } else {
      sprintf_s(sendBuffer, "%s (Error %d)", e.what(), s_err);
    };
  	cerr << "\nConversation Error: " << sendBuffer << ".\n";
    char sendErr[256];
    sprintf_s(sendErr, 256, "ERROR %s", sendBuffer);
    send(hClientSocket, sendErr, strlen(sendErr), 0);  // No error check ... as we are in an exception already
    g->gameErr(sendBuffer);
  }

  // Clean up at end of conversation. Note - don't want to throw an Exception as we want to pick up the next connect request.
  if (shutdown(hClientSocket,SD_BOTH)!=SOCKET_ERROR) {
    cout << "Shut socket\n";
  } else {
    cout << "ERROR Shutting socket\n";
  }
  if (closesocket(hClientSocket)!=SOCKET_ERROR) {
    cout << "Closed socket\n";
  } else {
    cout << "ERROR Closing socket\n";
  }

  delete g;
  return;
};