#include "stdafx.h"
#include "VectorMatrix.hpp"
#include "GameControl.hpp"
#include "Exception.hpp"


using namespace std;

GameControl::GameControl(const char *GameFile, const char *Client) {

  errno_t err;
  err= fopen_s(&hGameFile, GameFile, "r");
  if (err) throw Exception("Could not open GameFile");

  gameFileOpen = true;
  cout << "Opened Game File\n";

  bool needCosts = true;
  bool needConfig = true;

  teamFileOpen = false;

  while (needCosts || needConfig) {
    if (!readLine()) throw Exception("Game File read error before COSTS or CONFIG found");
    if (!strcmp(vp,"COSTS")) {
      if (sscanf_s(bp,"%d %d %d %d", &transProfit, &transWcost, &transJcost, &transDcost) !=4) Exception("Game file read error parsing COSTS");
      cout << "Parsed COSTS\n";
      needCosts = false;
    } else if (!strcmp(vp,"CONFIG")) {
      if (sscanf_s(bp,"%d %d %d %d %d %d %d %d %d", 
        &config.w.na, &config.w.eu, &config.w.ap, &config.j.na, &config.j.eu, &config.j.ap, &config.d.na, &config.d.eu, &config.d.ap)       
        !=9) Exception("Game file read error parsing CONFIG");
      cout << "Parsed CONFIG\n";
      needConfig = false;
    };
  };
  profit = 0;
  totalProfit = 0;
  maxProfit = 0;
  maxTotalProfit = 0;
  turnCount = 0;
  cliDesc = Client;
  return;
};



GameControl::~GameControl() {
  if (hGameFile && gameFileOpen) {
    fclose(hGameFile);
  }
  if (hTeamFile && teamFileOpen) {
    fflush(hTeamFile);
    fclose(hTeamFile);
  }
  return;
};


bool GameControl::gameTurn() {

  bool needDemand = true;
  const char sDays[] = "MON\0TUE\0WED\0THU\0FRI\0SAT\0SUN\0";
  char dayTime[32]; 

  while (needDemand) {
    if (!readLine()) {                                                              // End of file, so game over

      char teamBuf[512];
      int teamBufLen;
      int w_cnt;
      SYSTEMTIME T;
      GetLocalTime(&T);

      sprintf_s(teamBuf, 512, "\n\nSuccessful end at %d-%02d-%02d %02d:%02d:%02d.%03d\n",
        T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond, T.wMilliseconds);
      teamBufLen = strlen(teamBuf);
      w_cnt = fwrite(teamBuf,1,teamBufLen,hTeamFile);
      if (w_cnt != teamBufLen) throw Exception("Error writing teamfile");
      return false; // end of file = end of game
    }
    if (strcmp(vp,"DEMAND")) throw Exception("Expected DEMAND");
    if (sscanf_s(bp,"%d %d %d %d %d %d %d", 
      &gameDay, &gameHour, &gameMin, &gameSec, &demand.na, &demand.eu, &demand.ap) !=7) throw Exception("Game file read error parsing DEMAND");
    if (gameDay<0 || gameDay>6) throw Exception("Bad day in gamefile");
    strcpy_s(sGameDay,4,sDays+(gameDay*4));
    sprintf_s(dayTime, "%s %02d:%02d:%02d", sGameDay, gameHour, gameMin, gameSec); 

    cout << "Game Turn at " << dayTime << "\n";
    cout << "Demand NA:" << demand.na << " EU:" << demand.eu << " AP:" << demand.ap << "\n";
    needDemand = false;
  };

  //
  // Main game logic here ... distributing client demand over web, web over java, java over db, db split loss.
  //

  turnCount++;
  if (turnCount % 100 == 0) {
        fflush(hTeamFile);
  }
  
  // Step 1 ... Apply pending actions

  if (pending[0].w.na) {config.w.na += pending[0].w.na;};
  if (pending[0].w.eu) {config.w.eu += pending[0].w.eu;};
  if (pending[0].w.ap) {config.w.ap += pending[0].w.ap;};
  if (pending[0].j.na) {config.j.na += pending[0].j.na;};
  if (pending[0].j.eu) {config.j.eu += pending[0].j.eu;};
  if (pending[0].j.ap) {config.j.ap += pending[0].j.ap;};
  if (pending[0].d.na) {
    config.d.na += pending[0].d.na;
    if (pending[0].d.na>0) {pendingDBstart.na -= pending[0].d.na;};
  };
  if (pending[0].d.eu) {
    config.d.eu += pending[0].d.eu;
    if (pending[0].d.eu>0) {pendingDBstart.eu -= pending[0].d.eu;};
  };
  if (pending[0].d.ap) {
    config.d.ap += pending[0].d.ap;
    if (pending[0].d.ap>0) {pendingDBstart.ap -= pending[0].d.ap;};
  };

  if (config.w.na<0) {throw ConvException("Client asked for negative W.NA instances!");};
  if (config.w.eu<0) {throw ConvException("Client asked for negative W.EU instances!");};
  if (config.w.ap<0) {throw ConvException("Client asked for negative W.AP instances!");};
  if (config.j.na<0) {throw ConvException("Client asked for negative J.NA instances!");};
  if (config.j.eu<0) {throw ConvException("Client asked for negative J.EU instances!");};
  if (config.j.ap<0) {throw ConvException("Client asked for negative J.AP instances!");};
  if (config.d.na<0) {throw ConvException("Client asked for negative D.NA instances!");};
  if (config.d.eu<0) {throw ConvException("Client asked for negative D.EU instances!");};
  if (config.d.ap<0) {throw ConvException("Client asked for negative D.AP instances!");};

  for (int i=0;i<9;i++) {
    pending[i] = pending[i+1];
  }
  pending[9] = GMat(0,0,0,0,0,0,0,0,0);

  // Step 2 .. Distribute demand

  GMat wkgDist = GMat(0,0,0,0,0,0,0,0,0);

  distDemand(demand, wkgDist.w, config.w, 200, -30, -50);  // Client to web ... -30% EU<>NA, -50% to/from AP
  dist = wkgDist;
  procDemand(wkgDist.w, config.w, 180, -10, 200, -80, 250, -100); // Web: 0-180 good, 181-200 = -10%, 201-250 = -80%, 251+ = -100%

  distDemand(wkgDist.w, wkgDist.j, config.j, 500, -10, -20);  // Web to Java ... -10% EU<>NA, -20% to/from AP
  dist.j = wkgDist.j;
  procDemand(wkgDist.j, config.j, 400, -10, 500, -80, 600, -100); // Java: 0-400 good, 401-500 = -10%, 501-600 = -80%, 601+ = -100%

  distDemand(wkgDist.j, wkgDist.d, config.d, 1200, -10, -10);  // Java to DB ... -10% EU<>NA, -10% to/from AP
  dist.d = wkgDist.d;
  procDemand(wkgDist.d, config.d, 1000, -10, 1200, -80, 1400, -100); // DB: 0-1000 good, 1001-1200 = -10%, 1201-1400 = -80%, 1401+ = -100%

  int remainingTransactions = (wkgDist.d.na + wkgDist.d.eu + wkgDist.d.ap);

  // Step 3 ... Database split losses
  if ((config.d.na>0 || pendingDBstart.na>0) && (config.d.eu>0 || pendingDBstart.eu>0)) remainingTransactions = int (remainingTransactions * 0.8);
  if ((config.d.ap>0 || pendingDBstart.ap>0) && (config.d.eu>0 || pendingDBstart.eu>0)) remainingTransactions = int (remainingTransactions * 0.7);
  if ((config.d.ap>0 || pendingDBstart.ap>0) && (config.d.na>0 || pendingDBstart.na>0)) remainingTransactions = int (remainingTransactions * 0.7);


  // Step 4 ... Pay for Infrastructure
  int infraCost;
  infraCost = (config.w.na + config.w.eu + config.w.ap) * transWcost;
  infraCost += (config.j.na + config.j.eu + config.j.ap) * transJcost;
  infraCost += (config.d.na + config.d.eu + config.d.ap) * transDcost;

  // Step 5 ... Calculate Profit
  int revenue = (transProfit * remainingTransactions / 100);
  profit = revenue - infraCost;
  int totDemand = (demand.na + demand.eu + demand.ap);
  maxProfit = (transProfit * totDemand / 100);
  totalProfit += profit;
  maxTotalProfit += maxProfit;

  // Write the team file
  char teamBuf[512];
  int teamBufLen;
  int w_cnt;
  sprintf_s(teamBuf, 512, "%s,%02d:%02d:%02d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%d,%.2f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
    sGameDay, gameHour, gameMin, gameSec, demand.na, demand.eu, demand.ap, totDemand,
    config.w.na, config.w.eu, config.w.ap, config.j.na, config.j.eu, config.j.ap, config.d.na, config.d.eu, config.d.ap,
    maxProfit, revenue, infraCost, profit,  (100.0 * profit / maxProfit), totalProfit, (100.0 * totalProfit / maxTotalProfit),
    dist.w.na, dist.w.eu, dist.w.ap, dist.j.na, dist.j.eu, dist.j.ap, dist.d.na, dist.d.eu, dist.d.ap, remainingTransactions,
    control_in.w.na, control_in.w.eu, control_in.w.ap, control_in.j.na, control_in.j.eu, control_in.j.ap, control_in.d.na, control_in.d.eu, control_in.d.ap );
  teamBufLen = strlen(teamBuf);
  w_cnt = fwrite(teamBuf,1,teamBufLen,hTeamFile);
  if (w_cnt != teamBufLen) throw Exception("Error writing teamfile");
  return true;
};


bool GameControl::readLine() {

  bool needVerb = true;
  char *tp;

  while (needVerb) {
    if (!fgets(buf, 256, hGameFile)) return false; // End of File
    vp = buf;
    while (*vp=='\t' || *vp==' ') vp++;
    if (*vp== ';' || *vp == '\n' || *vp == '\0') continue;  // Comment or blank line
    
    bp=vp;
    while (*bp!='\t' && *bp!=' ') bp++;
    *bp = '\0';
    bp++;

    tp=bp;
    while(*tp!='\0') {
      if (*tp=='\t' || *tp=='\n') *tp = ' ';
      tp++;
    };
    needVerb = false;
  }

  return true;
};

void GameControl::serCosts(char* &outBuf) {
  sprintf_s(outBuf, 256, "COSTS %d %d %d %d", transProfit, transWcost, transJcost, transDcost);
  return;
};

void GameControl::serConfig(char* &outBuf) {
  sprintf_s(outBuf, 256, "CONFIG %d %d %d %d %d %d %d %d %d", config.w.na, config.w.eu, config.w.ap, config.j.na, config.j.eu, config.j.ap, config.d.na, config.d.eu, config.d.ap);
  return;
};

void GameControl::serDemand(char* &outBuf) {
  sprintf_s(outBuf, 256, "DEMAND %s %02d %02d %02d %d %d %d", sGameDay, gameHour, gameMin, gameSec, demand.na, demand.eu, demand.ap);
  return;
};

void GameControl::serDist(char* &outBuf) {
  sprintf_s(outBuf, 256, "DIST %d %d %d %d %d %d %d %d %d", dist.w.na, dist.w.eu, dist.w.ap, dist.j.na, dist.j.eu, dist.j.ap, dist.d.na, dist.d.eu, dist.d.ap);
  return;
};

void GameControl::serProfit(char* &outBuf) {
  sprintf_s(outBuf, 256, "PROFIT %d %d  %d %d", profit, int (100 * profit / maxProfit), totalProfit, int (100 * totalProfit / maxTotalProfit));
  return;
};

void GameControl::recvInit(char* &ctrl_in) {

  char **sb = &ctrl_in;
  char ctrlErr[42] = "Conversation INIT parse fail on team name";
  char *tbuf = parseStr(sb, 64,ctrlErr);
  strcpy_s(teamName, 64, tbuf);

  errno_t err;
  char fname[256];
  sprintf_s(fname, 256, ".\\Team_%s.csv", teamName);
  err= fopen_s(&hTeamFile, fname, "w");
  if (err) throw ConvException("Could not open TeamFile for write", err);
  teamFileOpen = true;

  char teamBuf[512];
  int teamBufLen;
  int w_cnt;
  SYSTEMTIME T;
  GetLocalTime(&T);

  sprintf_s(teamBuf, 512, "Session for team %s from client %s at %d-%02d-%02d %02d:%02d:%02d.%03d\n\n",
    teamName, cliDesc, T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond, T.wMilliseconds);
  teamBufLen = strlen(teamBuf);
  w_cnt = fwrite(teamBuf,1,teamBufLen,hTeamFile);
  if (w_cnt != teamBufLen) throw Exception("Error writing teamfile");

  sprintf_s(teamBuf, 512,"Day,Time, dmd.NA, dmd.EU, dmd.AP, dmd.tot, w.NA, w.EU, w.AP, j.NA, j.EU, j.AP, d.NA, d.EU, d.AP, maxrev, rev, costs, profit, %cprofit, totpft, %ctotpft, dist.w.NA, dist.w.EU, dist.w.AP, dist.j.NA, dist.j.EU, dist.j.AP, dist.d.NA, dist.d.EU, dist.d.AP, dist.tot, con.w.NA, con.w.EU, con.w.AP, con.j.NA, con.j.EU, con.j.AP, con.d.NA, con.d.EU, con.d.AP \n",'%','%');
  teamBufLen = strlen(teamBuf);
  w_cnt = fwrite(teamBuf,1,teamBufLen,hTeamFile);
  if (w_cnt != teamBufLen) throw Exception("Error writing teamfile");

  cout << "Created team file\n";

};

void GameControl::recvControl(char* &ctrl_in) {

  char **sb = &ctrl_in;
  char ctrlErr[40] = "Conversation CONTROL parse fail on w.na";

  control_in.w.na = parseInt(sb,ctrlErr);  // All control ints have to be below 10,000,000 !!
  control_in.w.eu = parseInt(sb,ctrlErr);
  control_in.w.ap = parseInt(sb,ctrlErr);
  control_in.j.na = parseInt(sb,ctrlErr);
  control_in.j.eu = parseInt(sb,ctrlErr);
  control_in.j.ap = parseInt(sb,ctrlErr);
  control_in.d.na = parseInt(sb,ctrlErr);
  control_in.d.eu = parseInt(sb,ctrlErr);
  control_in.d.ap = parseInt(sb,ctrlErr);
  
  if (control_in.w.na>0) {pending[2].w.na += control_in.w.na;}; // Web launches in 60 sec
  if (control_in.w.eu>0) {pending[2].w.eu += control_in.w.eu;};
  if (control_in.w.ap>0) {pending[2].w.ap += control_in.w.ap;};
  if (control_in.j.na>0) {pending[4].j.na += control_in.j.na;}; // Java launches in 120 sec
  if (control_in.j.eu>0) {pending[4].j.eu += control_in.j.eu;};
  if (control_in.j.ap>0) {pending[4].j.ap += control_in.j.ap;};
  if (control_in.d.na>0) {
    pending[8].d.na += control_in.d.na;                        // Database launches in 240 sec
    pendingDBstart.na += control_in.d.na;
  };
  if (control_in.d.eu>0) {
    pending[8].d.eu += control_in.d.eu;
    pendingDBstart.eu += control_in.d.eu;
  };
  if (control_in.d.ap>0) {
    pending[8].d.ap += control_in.d.ap;
    pendingDBstart.ap += control_in.d.ap;
  };

  if (control_in.w.na<0) {pending[0].w.na += control_in.w.na;}; // Web stops immediately
  if (control_in.w.eu<0) {pending[0].w.eu += control_in.w.eu;};
  if (control_in.w.ap<0) {pending[0].w.ap += control_in.w.ap;};
  if (control_in.j.na<0) {pending[1].j.na += control_in.j.na;}; // Java stops in 30 sec
  if (control_in.j.eu<0) {pending[1].j.eu += control_in.j.eu;};
  if (control_in.j.ap<0) {pending[1].j.ap += control_in.j.ap;};
  if (control_in.d.na<0) {pending[2].d.na += control_in.d.na;}; // Database stops in 60 sec
  if (control_in.d.eu<0) {pending[2].d.eu += control_in.d.eu;};
  if (control_in.d.ap<0) {pending[2].d.ap += control_in.d.ap;};
  return;
};



void GameControl::distDemand(GVec &dmd, GVec &out, GVec &cfg, int thresh2, int lossTA, int lossTP) {

  GVec in = dmd;

  // Step 1 ... distribute locally to the thresh2 limit

  __distL(in.na, out.na, cfg.na, thresh2);
  __distL(in.eu, out.eu, cfg.eu, thresh2);
  __distL(in.ap, out.ap, cfg.ap, thresh2);

  // Step 2 ... distribute overspill

  __distR(in.na, out.eu, cfg.eu, thresh2, lossTA);
  __distR(in.na, out.ap, cfg.ap, thresh2, lossTP);
  __distR(in.eu, out.na, cfg.na, thresh2, lossTA);
  __distR(in.eu, out.ap, cfg.ap, thresh2, lossTP);
  __distR(in.ap, out.eu, cfg.eu, thresh2, lossTA);
  __distR(in.ap, out.na, cfg.na, thresh2, lossTP);

  // Step 3 ... force remainder local

  __distL(in.na, out.na, cfg.na, -1);
  __distL(in.eu, out.eu, cfg.eu, -1);
  __distL(in.ap, out.ap, cfg.ap, -1);

  return;
};

void GameControl::__distL(int &in, int &out, int cfg, int thresh) {
  if (thresh==-1) {
    out += in;
    in = 0;
  } else if ((cfg*thresh) >= in) {
    out += in;
    in = 0;
  } else {
    out = cfg*thresh;
    in -= cfg*thresh;
  };
  return;
};


void GameControl::__distR(int &in, int &out, int cfg, int thresh, int loss) {
  if ((in>0) && (out<cfg*thresh)) {
    int maxTake = (int) ((cfg*thresh - out) * 100 / (100 + loss));
    if (in <= maxTake) {
      out += (int) (in * (100 + loss) / 100);  // applying loss here
      in = 0;
    } else {
      out = cfg*thresh;
      in -= maxTake;
    };
  };
  return;
}

void GameControl::procDemand(GVec &proc, GVec &cfg, int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 ) {

  // Process demand on this layer (apply losses as the load grows)

  __proc(proc.na, cfg.na, thresh1, loss1, thresh2, loss2, thresh3, loss3);
  __proc(proc.eu, cfg.eu, thresh1, loss1, thresh2, loss2, thresh3, loss3);
  __proc(proc.ap, cfg.ap, thresh1, loss1, thresh2, loss2, thresh3, loss3);
  return;
};

void GameControl::__proc(int &proc,  int cfg,  int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 ) {
  if (proc>(thresh3*cfg)) {
    proc = (int) ( proc * (100 + loss3) / 100);
  } else if (proc>(thresh2*cfg)) {
    proc = (int) ( proc * (100 + loss2) / 100);
  } else if (proc>(thresh1*cfg)) {
    proc = (int) ( proc * (100 + loss1) / 100);
  };
  return;
}

int GameControl::parseInt(char **bp, const char *desc) {

  char* curc = *bp;
  int sign = 1;
  int ret = 0;
  bool goodParse = false;

  while ((*curc == ' ') || (*curc == '\t')) {
    curc++; 
  };
  if ((*curc == '\n') || (*curc == '\0')) throw ConvException(desc);

  if (*curc == '-') {
    sign = -1;
    curc++;
  };

  while ((*curc >= '0')&&(*curc <= '9')) {
    goodParse = true;
    ret = (ret * 10) + (int) *curc - (int) '0';
    if (ret>10000000) throw ConvException(desc);
    curc++; 
  };

  if ((*curc != '\n') && (*curc != '\0') && (*curc != ' ') && (*curc != '\t')) throw ConvException(desc);
  if (!goodParse) throw ConvException(desc);
  *bp = curc+1;
  return ret * sign;
}; 

char* GameControl::parseStr(char **bp, int maxlen, const char* desc) {

  char* curc = *bp;
  bool quoted = false;
  char* startc;
  int i = maxlen-1;


  while ((*curc == ' ') || (*curc == '\t')) {
    curc++; 
  };
  if ((*curc == '\n') || (*curc == '\0')) throw ConvException(desc);

  if ((*curc == '\"')) {
    quoted = true;
    curc++;
  }
  startc = curc;

  while ((*curc != ' ') && (*curc != '\t') && (*curc != '\"') && (*curc != '\n') && (*curc != '\0') && (i) ) {
    i--;
    curc++; 
  };

  if ((i==0)||(i==(maxlen-1))) throw ConvException(desc);
  if ((*curc == '\"') && (!quoted)) throw ConvException(desc);

  *curc++ = '\0';
  *bp = curc;
  return startc;
}; 


void GameControl::gameErr(char * errText) {
  char teamBuf[512];
  int teamBufLen;
  int w_cnt;
  SYSTEMTIME T;

  GetLocalTime(&T);

  sprintf_s(teamBuf, 512, "\n\nERROR in conversation at %d-%02d-%02d %02d:%02d:%02d.%03d: %s\n",
    T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond, T.wMilliseconds, errText);
  teamBufLen = strlen(teamBuf);
  if (teamFileOpen) {
	  w_cnt = fwrite(teamBuf,1,teamBufLen,hTeamFile);
      if (w_cnt != teamBufLen) throw Exception("Error writing teamfile");
  };
  return;

}
