// MSControl_Client.cpp : Defines the entry point for the console application.
//
// Credit: Winsock networking tutorial by Thomas Bleeker at www.MadWizard.org
//

#include "stdafx.h"
#include "Exception.hpp"
#include "NetSupport.hpp"

using namespace std;




const char HEAD_REQUEST_PART1[] =
{
	"HEAD / HTTP/1.1\r\n" 			// Get root index from server
	"Host: "						// Specify host name used
};

const char HEAD_REQUEST_PART2[] =
{
	"\r\n"							// End hostname header from part1
	"User-agent: HeadReqSample\r\n" // Specify user agent
	"Connection: close\r\n" 		// Close connection after response
	"\r\n"							// Empty line indicating end of request
};



int main(int argc, char* argv[]) {                                      // arg1 = host (def:localhost) arg2 = port (def:18021)

  NetSupport ns;

  const char DEF_SERVER_NAME[] = "localhost";
  const char DEF_TEAM_NAME[] = "Rattlesnake";
  const int  DEF_SERVER_PORT   = 18021;	

	try	{

    // Initialize Winsock
    cout << "Initializing winsock... ";
    ns.NetworkInit();
	  cout << "initialized.\n";

	  // Collect command line params
    const char *pHostname = (argc > 1 ? argv[1] : DEF_SERVER_NAME);       
    int hostPort = DEF_SERVER_PORT;
    if (argc > 2) sscanf_s(argv[2],"%i",&hostPort);
    const char *pTeamname = (argc > 3 ? argv[3] : DEF_TEAM_NAME);       



	  // Resolve names, init socekt and open connection
    cout << "Initializing connection... ";
    ns.NetworkConnect( pHostname, hostPort, pTeamname);
	  cout << "initialized.\n";

    // Main network conversation
	  cout << "Starting conversation.\n";
    ns.NetworkConversation();
	  cout << "Conversation completed successfully.\n";

    // Wrap up
	  cout << "Closing connection and cleaning up winsock... ";
    ns.NetworkCloseAndCleanUp();

  }	catch (Exception e) {
		cerr << "\nError: " << e.what() << ".\n";
    ns.NetworkCloseAndCleanUp();
		return -1;
	}

	cout << "Successful completion.\n";
	return 0;
}
