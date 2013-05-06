#include "stdafx.h"
#include "GameControl.hpp"
#include "Exception.hpp"


using namespace std;

GameControl::GameControl() {

  profit = 0;
  totalProfit = 0;
  profitPct = 0;
  totalProfitPct = 0;
  gameTurnCnt = 0;
  sumDemand = GVec(0,0,0);
  return;
};

GameControl::~GameControl() {
  return;
};

bool GameControl::gameTurn() {

  //
  // Main control logic here ... 
  //
  
  // Simple algo ... every 10 turns, adjust counts of W and J to comfortably handle the previous 10 turns demand
  // plus decide where to run D.

  int sumLoop = 9;
  double hyst = 0.85;
  double hystAP = 0.60;

  sumDemand += demand;
  if (gameTurnCnt == 0) {
	  sumDemand.na *= sumLoop;
	  sumDemand.eu *= sumLoop;
	  sumDemand.ap *= sumLoop;
  }

  if (gameTurnCnt % sumLoop == 0) {
	  cout << "Control active\n";
	  control_out.w.na = (sumDemand.na / (sumLoop * 180)) + 1 - config.w.na;
	  control_out.w.eu = (sumDemand.eu / (sumLoop * 180)) + 1 - config.w.eu;
	  control_out.w.ap = (sumDemand.ap / (sumLoop * 180)) + 1 - config.w.ap;
	  control_out.j.na = (sumDemand.na / (sumLoop * 500)) + 1 - config.j.na;
	  control_out.j.eu = (sumDemand.eu / (sumLoop * 500)) + 1 - config.j.eu;
	  control_out.j.ap = (sumDemand.ap / (sumLoop * 500)) + 1 - config.j.ap;
	  int dControl = (sumDemand.na + sumDemand.eu + sumDemand.ap) / (sumLoop * 1200)+1;
	  if ((sumDemand.na * hyst > sumDemand.eu) && (sumDemand.na * hyst > sumDemand.ap)) {
		  control_out.d.na = dControl - config.d.na;
		  if (config.d.na) {
			  control_out.d.eu = -config.d.eu;
		    control_out.d.ap = -config.d.ap;
		  };
	  } else if ((sumDemand.eu * hyst > sumDemand.na) && (sumDemand.eu * hyst > sumDemand.ap)) {
		  control_out.d.eu = dControl - config.d.eu;
		  if (config.d.eu) {
		    control_out.d.na = -config.d.na;
		    control_out.d.ap = -config.d.ap;
		  };
	  } else if ((sumDemand.ap * hystAP > sumDemand.na) && (sumDemand.ap * hystAP > sumDemand.eu)) {
		  control_out.d.ap = dControl - config.d.ap;
		  if (config.d.ap) {
		    control_out.d.na = -config.d.na;
		    control_out.d.eu = -config.d.eu;
		  };
	  } else {
			  control_out.d.na = (config.d.na == 0? 0 : (dControl - config.d.na));
		    control_out.d.eu = (config.d.eu == 0? 0 : (dControl - config.d.eu));
		    control_out.d.ap = (config.d.ap == 0? 0 : (dControl - config.d.ap));
	  };
	  sumDemand = GVec(0,0,0);
  } else {
	  control_out = GMat(0,0,0,0,0,0,0,0,0);
  }
  gameTurnCnt++;
  return true;
};

void GameControl::recvConfig(char* &buf) {
  if (sscanf_s(buf, "%d %d %d %d %d %d %d %d %d",  &config.w.na, &config.w.eu, &config.w.ap, &config.j.na, &config.j.eu, &config.j.ap, &config.d.na, &config.d.eu, &config.d.ap)
    != 9) throw Exception("Conversation CONFIG parse fail");
  return;
};

void GameControl::recvDemand(char* &buf) {
  if (sscanf_s(buf, "%s %d %d %d %d %d %d", sGameDay, 4, &gameHour, &gameMin, &gameSec, &demand.na, &demand.eu, &demand.ap)
	  != 7) throw Exception("Conversation DEMAND parse fail");
  return;
};

void GameControl::recvDist(char* &buf) {
  if (sscanf_s(buf, "%d %d %d %d %d %d %d %d %d", &dist.w.na, &dist.w.eu, &dist.w.ap, &dist.j.na, &dist.j.eu, &dist.j.ap, &dist.d.na, &dist.d.eu, &dist.d.ap)
	  != 9) throw Exception("Conversation DIST parse fail");
  return;
};

void GameControl::recvProfit(char* &buf) {
  if (sscanf_s(buf, "%d %d %d %d", &profit, &profitPct, &totalProfit, &totalProfitPct)
    != 4) throw Exception("Conversation PROFIT parse fail");
  return;
};

void GameControl::recvCosts(char* &buf) {
  if (sscanf_s(buf, "%d %d %d %d", &transProfit, &transWcost, &transJcost, &transDcost)
    != 4) throw Exception("Conversation COSTS parse fail");
  return;
};

void GameControl::serControl(char* &outBuf) {
  sprintf_s(outBuf, 256, "CONTROL %d %d %d %d %d %d %d %d %d",
    control_out.w.na, control_out.w.eu, control_out.w.ap, control_out.j.na, control_out.j.eu, control_out.j.ap, control_out.d.na, control_out.d.eu, control_out.d.ap);
  return;
};




GVec::GVec() {
  na = 0;
  eu = 0;
  ap = 0;
}

GVec::GVec(int na_in, int eu_in, int ap_in) {
  na = na_in;
  eu = eu_in;
  ap = ap_in;
}

GVec::GVec(const GVec &gv_in) {
  na = gv_in.na;
  eu = gv_in.eu;
  ap = gv_in.ap;
}

GVec& GVec::operator=(const GVec &gv_in) {
  if (this != &gv_in) {
    na = gv_in.na;
    eu = gv_in.eu;
    ap = gv_in.ap;
  }
  return (*this);
}

GVec& GVec::operator+=(const GVec &gv_in) {
  na += gv_in.na;
  eu += gv_in.eu;
  ap += gv_in.ap;
  return (*this);
}



GMat::GMat() {
  w = GVec(0,0,0);
  j = GVec(0,0,0);
  d = GVec(0,0,0);
}

GMat::GMat(int naw, int euw, int apw, int naj, int euj, int apj, int nad, int eud, int apd) {
  w = GVec(naw, euw, apw);
  j = GVec(naj, euj, apj);
  d = GVec(nad, eud, apd);
}


GMat::GMat(const GMat &gm_in) {
  w = gm_in.w;
  j = gm_in.j;
  d = gm_in.d;
}

GMat& GMat::operator=(const GMat &gm_in) {
  if (this != &gm_in) {
    w = gm_in.w;
    j = gm_in.j;
    d = gm_in.d;
  }
  return (*this);
}

