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
#include <string.h>


#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

using namespace std;


void senderror(int cSocket)
{
	char sendBuffer[256];
	snprintf(sendBuffer, 256, "WRONG_HANDSHAKE_CLOSING_SOCKET");
	cout << ">> " << sendBuffer << "\n";
	send(cSocket, sendBuffer, strlen(sendBuffer), 0);
	close(cSocket);
}


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
		if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation INIT failed");
		{
			recvBuffer[recvBytes] = 0;
			cout << "      <<  " << recvBuffer << "\n";
			if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
			{
				senderror(hClientSocket);
				return;
			}
			if (strcmp(recvCmd,"INIT")) //throw ConvException("Expected INIT");
			{
				senderror(hClientSocket);
				return;
			}
			rb = recvBuffer+5;
			g->recvInit(rb);
		}
		else
		{
			senderror(hClientSocket);
			return;
		}

		// Send ACCEPT
		snprintf(sendBuffer, 256, "ACCEPT");
		cout << ">> " << sendBuffer << "\n";
		if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)
		{
			//throw ConvException("Conversation ACCEPT failed");
			senderror(hClientSocket);
			return;
		}
		// Expect RECD
		if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation INIT failed");
		{
			recvBuffer[recvBytes] = 0;
			cout << "      <<  " << recvBuffer << "\n";
			if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
			{
				senderror(hClientSocket);
				return;
			}
			if (strcmp(recvCmd,"RECD")) //throw ConvException("Expected RECD");
			{
				senderror(hClientSocket);
				return;
			}
		}
		else
		{
			senderror(hClientSocket);
			return;
		}

		// Send COSTS
		sendBuffer[0] = 0;
		g->serCosts(sb);
		cout << ">> " << sendBuffer << "\n";
		if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	//throw ConvException("Conversation COSTS failed");
		{

			senderror(hClientSocket);
			return;
		}
		// Expect START
		if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation INIT failed");
		{
			recvBuffer[recvBytes] = 0;

			cout << "      <<  " << recvBuffer << "\n";
			if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
			{
				senderror(hClientSocket);
				return;
			}
			if (strcmp(recvCmd,"START")) //throw ConvException("Expected START");
			{

				senderror(hClientSocket);
				return;
			}
		}
		else
		{

			senderror(hClientSocket);
			return;
		}

		while (true) {

			cout << "\n--- G a m e   T u r n ---\n";
			if (!g->gameTurn()) break;

			// Send CONFIG
			g->serConfig(sb);
			cout << ">> " << sendBuffer << "\n";
			if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	//throw ConvException("Conversation CONFIG failed");
			{
				senderror(hClientSocket);
				return;
			}
			// Expect RECD
			if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation INIT failed");
			{
				recvBuffer[recvBytes] = 0;
				cout << "      <<  " << recvBuffer << "\n";
				if (sscanf(recvBuffer, "%s", recvCmd)!=1) throw ConvException("Reading command");
				if (strcmp(recvCmd,"RECD"))

				{

					senderror(hClientSocket);
					return;
				}

			}
			else
			{
				senderror(hClientSocket);
				return;
			}
			// Send DEMAND
			g->serDemand(sb);
			cout << ">> " << sendBuffer << "\n";
			if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)//	throw ConvException("Conversation DEMAND failed");
			{

				senderror(hClientSocket);
				return;
			}

			// Expect RECD
			if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation INIT failed");
			{
				recvBuffer[recvBytes] = 0;
				cout << "      <<  " << recvBuffer << "\n";
				if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
				{
					senderror(hClientSocket);
					return;
				}

				if (strcmp(recvCmd,"RECD")) //throw ConvException("Expected RECD");
				{

					senderror(hClientSocket);
					return;
				}
			}
			else
			{
				senderror(hClientSocket);
				return;
			}

			// Send DIST
			g->serDist(sb);
			cout << ">> " << sendBuffer << "\n";
			if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	//throw ConvException("Conversation DIST failed");
			{

				senderror(hClientSocket);
				return;
			}
			// Expect RECD
			if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))//	throw ConvException("Conversation INIT failed");
			{recvBuffer[recvBytes] = 0;
			cout << "      <<  " << recvBuffer << "\n";
			if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
			{
				senderror(hClientSocket);
				return;
			}

			if (strcmp(recvCmd,"RECD")) //throw ConvException("Expected RECD");
			{

				senderror(hClientSocket);
				return;
			}
			}
			else
			{
				senderror(hClientSocket);
				return;
			}
			// Send PROFIT
			g->serProfit(sb);
			cout << ">> " << sendBuffer << "\n";
			if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	//throw ConvException("Conversation PROFIT failed");
			{
				senderror(hClientSocket);
				return;
			}
			// Expect CONTROL back
			if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation CONTROL failed");
			{
				recvBuffer[recvBytes] = 0;
				cout << "      <<  " << recvBuffer << "\n";
				if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
				{
					senderror(hClientSocket);
					return;
				}

				if (strcmp(recvCmd,"CONTROL")) //throw ConvException("Expected CONTROL");
				{

					senderror(hClientSocket);
					return;
				}
				rb = recvBuffer+8;
				g->recvControl(rb);
			}
			else
			{
				senderror(hClientSocket);
				return;
			}
		}

		// Send END
		snprintf(sendBuffer, 256, "END");
		cout << ">> " << sendBuffer << "\n";
		if (send(hClientSocket, sendBuffer, strlen(sendBuffer), 0)==SOCKET_ERROR)	//throw ConvException("Conversation END failed");
		{

			senderror(hClientSocket);
			return;
		}

		// Expect STOP back
		if (!((recvBytes=recv(hClientSocket, recvBuffer, recvBufLen, 0))==SOCKET_ERROR))	//throw ConvException("Conversation STOP failed");
		{
			recvBuffer[recvBytes] = 0;
			cout << "      <<  " << recvBuffer << "\n";
			if (sscanf(recvBuffer, "%s", recvCmd)!=1) //throw ConvException("Reading command");
			{
				senderror(hClientSocket);
				return;
			}

			if (strcmp(recvCmd,"STOP"))	//throw ConvException("Expected STOP");
			{

				senderror(hClientSocket);
				return;
			}
			cout << "\nSuccessful conversation!\n";
		}
		else
		{

			senderror(hClientSocket);
			return;
		}

	} catch (ConvException e) {
		int s_err = e.s_err();
		if (!s_err) {
			sprintf(sendBuffer, "%s", e.what());
		} else {
			sprintf(sendBuffer, "%s (Error %d)", e.what(), s_err);
		};
		cerr << "\nConversation Error: " << sendBuffer << ".\n";
		char sendErr[256];
		snprintf(sendErr, 256, "ERROR %s", sendBuffer);
		send(hClientSocket, sendErr, strlen(sendErr), 0);  // No error check ... as we are in an exception already
		g->gameErr(sendBuffer);
		close(hClientSocket);
		return;

	}

	// Clean up at end of conversation. Note - don't want to throw an Exception as we want to pick up the next connect request.
	if (close(hClientSocket)!=SOCKET_ERROR) {
		cout << "Closing Socket\n";
	} else {
		cout << "ERROR Closing socket\n";
	}


	delete g;
	return;
};


