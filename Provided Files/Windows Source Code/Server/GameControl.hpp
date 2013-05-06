//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Game Control class definition for MSControl_Server.cpp.
//  This class controls all the game parameters. It is created and
//  destroyed once per game (i.e. once per client connection).
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#ifndef __GameControl
#define __GameControl

#include <stdio.h>
#include "VectorMatrix.hpp"

class GameControl
{
  public:
    GameControl(const char *GameFile, const char *Client);                          
    ~GameControl();                                             
    bool gameTurn();                                                  // Called each virtual 30 secs of game time
    void gameErr(char * errText);                                     // Write a game error into the team file
    void serCosts(char* &outBuf);                                     // 'ser' functions serialize data for sending on the connection
    void serConfig(char* &outBuf);
    void serDemand(char* &outBuf);
    void serDist(char* &outBuf);
    void serProfit(char* &outBuf);
    void recvInit(char* &ctrl_in);                                    // 'recv' functions receives and deserializes off the connection
    void recvControl(char* &ctrl_in);
private:
	bool teamFileOpen;												  // Don't try using a closed team file
	bool gameFileOpen;												  // Don't try using a closed team file
    FILE *hTeamFile;                                                  // handle to the results for for client's team
    FILE *hGameFile;                                                  // handle to the controlling game file
    char buf[256];                                                    // parsing buffer for the game file and input from client
    char *vp, *bp;                                                    // verb pointer (e.g. INIT), and buffer pointer

    GMat control_in;                                                  // holds the CONTROL input from client                                                 

    GMat config;                                                      // holds the game's current configuration (counts of Web/Jave/DB by region)
    GVec demand;                                                      // holds the current demand by region
    GMat dist;                                                        // holds the distribution matrix for this game turn
    GMat pending[10];                                                 // holds the pending actions. Things move towards pending[0] each turn.
    GVec pendingDBstart;                                              // used to note DB in progress of starting (penalty immediately)

    char teamName[64];                                                // Used for the output file. 

    int gameDay, gameHour, gameMin, gameSec;                          // Game time. Done in integer dd hh mm ss to avoid any floating point slippage
    char sGameDay[4];                                                 // Converts gameDay into MON, TUE, etc.
    int transProfit, transWcost, transJcost, transDcost;              // Transaction revenue (cents) and costs for infra (dollars)
    int profit, totalProfit;                                          // Tracking profit on game tuen and for the whole game
    int maxProfit, maxTotalProfit;                                    // Tracking theoretical max profit (no infra costs or losses)

    void distDemand(GVec &dmd, GVec &out, GVec &cfg, int thresh2, int lossTA, int lossTP);
                                                                      // Distributes demand onto next tier in Hermes
    void __distL(int &in, int &out, int cfg, int thresh);             // Dist helper ... for local distribution.
    void __distR(int &in, int &out, int cfg, int thresh, int loss);   // Dist helper ... for cross-region remote distribution.

    void procDemand(GVec &proc, GVec &cfg, int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 );
                                                                      // Processes demand on this tier. (Applying losses per loading.)
    void __proc(int &proc, int cfg, int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 );
                                                                      // Process helper for specific location and tier processing.

    int parseInt(char **bp, const char* desc);                        // Safer the sscanf! Manual handling of integer sent from client
                                                                      // gracefully handling any crud thrown at us (e.g. text in int which would
                                                                      // access violate the sscanf).
    char* parseStr(char **bp, int maxlen, const char* desc);          // Manual string handling, stripping double quotes if used
                                                                      // and forbidding anything but A-Z a-z 0-9.
                                                                      // Note - no spaces in team name please, as it's in a file name too.
    bool readLine();                                                  // Helper for reading the game file
    const char *cliDesc;                                              // Pointer to the Client's host description
    int turnCount;                                                    // turn counter - used for flushing team file
};

#endif // __GameControl