#ifndef __GameControl
#define __GameControl

#include <stdio.h>


class GVec {
  public:
    GVec();
    GVec(int na_in, int eu_in, int ap_in);
    GVec(const GVec &gv_in);
    GVec& operator+=(const GVec &gv_in);
    GVec& operator=(const GVec &gv_in);
    int na, eu, ap;
};

class GMat {
  public:
    GMat();
    GMat(int naw, int euw, int apw, int naj, int euj, int apj, int nad, int eud, int apd);
    GMat(const GMat &gm_in);
    GMat& operator=(const GMat &gm_in);
    GVec w, j, d;
};

class GameControl
{
  public:
    GameControl();
    ~GameControl();
    bool gameTurn();
    void recvCosts(char* &buf);
    void recvConfig(char* &buf);
    void recvDemand(char* &buf);
    void recvDist(char* &buf);
    void recvProfit(char* &buf);
    void serControl(char* &outBuf);
  private:
    char buf[256];
    char *vp, *bp;

    GMat control_out;
    GVec demand_in;

    GMat config;
    GVec demand;
    GMat dist;
    GMat pending[10];

    int gameDay, gameHour, gameMin, gameSec;
    char sGameDay[4];
    int transProfit, transWcost, transJcost, transDcost;
    int profit, totalProfit; 
    int profitPct, totalProfitPct; 
	int gameTurnCnt;
	GVec sumDemand;


    void distDemand(GVec &dmd, GVec &out, GVec &cfg, int thresh2, int lossTA, int lossTP);
    void __distL(int &in, int &out, int cfg, int thresh);
    void __distR(int &in, int &out, int cfg, int thresh, int loss);

    void procDemand(GVec &proc, GVec &cfg, int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 );
    void __proc(int &proc, int cfg, int thresh1, int loss1, int thresh2, int loss2, int thresh3, int loss3 );

    bool readLine();

};

#endif // __GameControl