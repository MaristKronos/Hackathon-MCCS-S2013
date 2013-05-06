#include "stdafx.h"
#include "NetSupport.hpp"
#include "Exception.hpp"

using namespace std;


void NetSupport::NetworkConversation() {

  char sendBuffer[256];
  char recvBuffer[256];
  char recvCmd[32];
  int recvBufLen = 256;
  int recvBytes;
  char *sb = &sendBuffer[0];

  // open sockets. 
  //   INIT {teamname}
  //                      ACCEPT
  //   RECD
  //                      COSTS {profit} {W-cost} {J-cost} {D-Cost}
  //
  //   START
  // while not END..
  //                      CONFIG {NA-W} {NA-J} {NA-D} {EU-W} {EU-J} {EU-D} {AP-W} {AP-J} {AP-D}
  //   RECD
  //                      DEMAND {day} {time} {NA-U} {EU-U} {AP-U}
  //   RECD
  //                      DIST {NA-W} {NA-J} {NA-D} {EU-W} {EU-J} {EU-D} {AP-W} {AP-J} {AP-D}
  //   RECD
  //                      PROFIT {day} {time} {p} 
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

  cout << "\nStarting conversation!\n";

  g = new GameControl();
  char *bp;

  // Open Conversation with INIT
  sprintf_s(sendBuffer, 256, "INIT %s", Team);
  cout << ">> " << sendBuffer << "\n";
  if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation INIT failed");

  // Expect ACCEPT back
  if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation ACCEPT failed");
  recvBuffer[recvBytes] = 0;
  cout << "      <<  " << recvBuffer << "\n";
  if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
  if (strcmp(recvCmd,"ACCEPT")) throw Exception("Expected ACCEPT");

  // Send RECD
  sprintf_s(sendBuffer, 256, "RECD");
//  cout << ">> " << sendBuffer << "\n";
  if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation RECD failed");

  // Expect COSTS back
  if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation COSTS failed");
  recvBuffer[recvBytes] = 0;
  cout << "      <<  " << recvBuffer << "\n";
  if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
  if (strcmp(recvCmd,"COSTS")) throw Exception("Expected COSTS");
  bp = recvBuffer + 6;
  g->recvCosts(bp);

  // Send START
  sprintf_s(sendBuffer, 256, "START");
  cout << ">> " << sendBuffer << "\n";
  if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation START failed");


  while (true) {

    cout << "\n--- G a m e   T u r n ---\n";

    // Expect CONFIG or END back
    if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation CONFIG or END failed");
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
    if (strcmp(recvCmd,"END")==0) break;
    if (strcmp(recvCmd,"CONFIG")) throw Exception("Expected CONFIG or END");      
	bp = recvBuffer + 7;
	g->recvConfig(bp);

    // Send RECD
    sprintf_s(sendBuffer, 256, "RECD");
//    cout << ">> " << sendBuffer << "\n";
    if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation RECD failed");

    // Expect DEMAND back
    if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation DEMAND failed");
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
    if (strcmp(recvCmd,"DEMAND")) throw Exception("Expected DEMAND"); 
	bp = recvBuffer + 7;
	g->recvDemand(bp);

    // Send RECD
    sprintf_s(sendBuffer, 256, "RECD");
//    cout << ">> " << sendBuffer << "\n";
    if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation RECD failed");

    // Expect DIST back
    if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation DIST failed");
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
    if (strcmp(recvCmd,"DIST")) throw Exception("Expected DIST"); 
	bp = recvBuffer + 5;
	g->recvDist(bp);

    // Send RECD
    sprintf_s(sendBuffer, 256, "RECD");
//    cout << ">> " << sendBuffer << "\n";
    if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation RECD failed");

    // Expect PROFIT back
    if ((recvBytes=recv(hSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR)	throw Exception("Conversation PROFIT failed");
    recvBuffer[recvBytes] = 0;
    cout << "      <<  " << recvBuffer << "\n";
    if (sscanf_s(recvBuffer, "%s", recvCmd, 32)!=1) throw Exception("Reading command");
    if (strcmp(recvCmd,"PROFIT")) throw Exception("Expected PROFIT"); // Don't we all?
	bp = recvBuffer + 7;
	g->recvProfit(bp);    

	g->gameTurn();

    // Send CONTROL
	g->serControl(sb);
    cout << ">> " << sendBuffer << "\n";
    if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation CONTROL failed");

  }

  sprintf_s(sendBuffer, 256, "STOP", Team);
  cout << ">> " << sendBuffer << "\n";
  if (send(hSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	throw Exception("Conversation STOP failed");

  cout << "\nSuccessful conversation!\n";

  return;
};