//==============================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//	Description:
//
//  This code runs the control server for the HERMES Trading Simulator
//  control challenge, typically given to students, interns and graduates.
//  We open a Windows network socket and wait for clients to connect.
//  For each connection, we run the game according to the game input file
//  typically generated from the main Hermes simulation spreadsheet game
//  files.
//
//
//	Copyright Notice:
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	For full licencing terms, please refer to the GNU General Public License
//	(gpl-3_0.txt) distributed with this release, or see
//	http://www.gnu.org/licenses/.
//
//
//	Credits:
//
//	Thomas Bleeker for his Winsock networking tutorial at www.MadWizard.org
//
//	Release History:
//
//	V1.00	Initial Release
//==============================================================

#include "stdafx.h"
#include "Exception.hpp"
#include "NetSupport_Server.hpp"

using namespace std;


int main(int argc, char* argv[]) {
  
  // ARGUNENT LIST:    arg1 = host (def:localhost) arg2 = port (def:18021)  arg3 = filename (def:.\GameFile.txt)

  NetSupport_Server ns;

  const char DEF_SERVER_NAME[] = "localhost";
  const int  DEF_SERVER_PORT   = 28021;
  const char DEF_GAMEFILE[] = "/home/asedighi/Projects/Morgan/hackaton/MSControl_Server/Debug/GameFile.txt";
	try	{

    cout << "Initializing WinSock... ";
    ns.NetworkInit();                                                             // Initialize windows networking stack
	  cout << "initialized.\n";

    const char *pHostname = (argc > 1 ? argv[1] : DEF_SERVER_NAME);               // Collect command params if given, else default them
    int hostPort = DEF_SERVER_PORT;
    if (argc > 2) sscanf(argv[2],"%i",&hostPort);
    const char *GameFile = (argc > 3 ? argv[3] : DEF_GAMEFILE);   
    
    cout << "Initializing listener... ";
    ns.NetworkBindListen( pHostname, hostPort);                                   // Init socket, bind it and set to listen. 
	  cout << "initialized.\n";

    while (true) {
      ns.NetworkAccept();                                                         // Loop listening for connections
      ns.NetworkConversation(GameFile);                                           // Run the game for the connecting team
    }

	  cout << "Closing connection and cleaning up winsock... ";
    ns.NetworkCloseAndCleanUp();                                                  // Close out and shut WinSock

  }	catch (Exception e) {                                                         // If a type Exception is thrown, it's a problem
    int s_err = e.s_err();                                                        // on the server side, so stop. (ConvException is a
		cerr << "\nError: " << e.what();                                              // problem onthe client side, so clean up the connectoon
    if (s_err) {                                                                  // and carry on the the next.)
      cerr << " (Error: " << s_err << ")";                                       // If the WSAerr is non-zero, then show this too. 
    };
    cerr << ".\n";
    ns.NetworkCloseAndCleanUp();
		return -1;
	}

	cout << "Successful completion.\n";
	return 0;
}
