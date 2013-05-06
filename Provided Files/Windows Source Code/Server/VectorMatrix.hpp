//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  GVec and GMat class definition for MSControl_Server.cpp.
//  These two classes help store a 3-value vector and a 3x3 matrix for
//  the game dynamics. E.g. Web Server Config for {NA, EU, AP} or
//  full configuration for Web, Java, Database across NA, EU, AP
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#ifndef __VectorMatrix
#define __VectorMatrix

class GVec {
  public:
    GVec();
    GVec(int na_in, int eu_in, int ap_in);
    GVec(const GVec &gv_in);
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
#endif // __VectorMatrix