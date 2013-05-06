//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Per Microsoft, this is the standard include file for defining
//  backwards compatibility. These 4 lines have to be in this order,
//  and tell the compiler to process this just once per build,
//  picking up the Windows SDK versioning data, then we get to
//  define what compatibility we want (e.g. back to Win XP), then
//  the SDK Versions are set in the last include,
//
//  ... which is a long way of saying we are just defining that we 
//  want to be able to run this for compatibility back to Win XP. 
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#pragma once
#include <WinSDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WINXP 
#include <SDKDDKVer.h>

