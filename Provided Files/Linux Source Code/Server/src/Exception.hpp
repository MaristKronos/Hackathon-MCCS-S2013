//======================================================================
//
//	Morgan Stanley Control Server for HERMES Trading App Simulator
//	==============================================================
//
//  Header and implementation for the Exception and ConvException classes
//  used to throw errors in the code. Throwing an Exception indicates
//  a problem on the server side, so we will gracefully exit. Throwing a
//  ConvException indicates we didn't like something from the client, so
//  we gracefully close the conversation with the client, clean up and
//  reset for the next client. 
//
//	Copyright (C) 2013	Morgan Stanley and Andrew Stokes
//                   All rights reserved
//
//  For full details of this copyright, please see MSControl_Server.cpp
//======================================================================

#ifndef __Exception
#define __Exception


class Exception
{
public:
    Exception() :
         m_pMessage(""), m_WSAerr(0) {}                               // Default no message, no WSA error
    virtual ~Exception() {}
    Exception(const char *pMessage) :
         m_pMessage(pMessage), m_WSAerr(0) {}                         // For throwing error unrelated to sockets
    Exception(const char *pMessage,const int WSAerr) :
         m_pMessage(pMessage), m_WSAerr(WSAerr) {}                    // For throwing sockets error
    const char * what() { return m_pMessage; }
    const int s_err() { return m_WSAerr; }
private:
    const char *m_pMessage;
    const int m_WSAerr;
};

class ConvException                                                   // Same implementation as Exception
{                                                                     // Used to provide a way to catch a conversation error
public:                                                               // i.e. drop the conversation and move to the next client
    ConvException() :                                                 // versus dropping the whole control server.
         m_pMessage(""), m_WSAerr(0) {}
    virtual ~ConvException() {}
    ConvException(const char *pMessage) :
         m_pMessage(pMessage), m_WSAerr(0) {}
    ConvException(const char *pMessage, const int WSAerr) :
         m_pMessage(pMessage), m_WSAerr(WSAerr) {}

    const char * what() { return m_pMessage; }
    const int s_err() { return m_WSAerr; }
private:
    const char *m_pMessage;
    const int m_WSAerr;
};

#endif // __Exception
