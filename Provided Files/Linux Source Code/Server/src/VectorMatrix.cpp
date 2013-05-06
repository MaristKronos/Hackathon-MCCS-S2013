//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Provides supporting Vector and Matrix utility calls for the game
//  controller in MSControl_Server.cpp.
//
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#include "stdafx.h"
#include "VectorMatrix.hpp"


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
