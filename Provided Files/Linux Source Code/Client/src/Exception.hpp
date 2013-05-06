#ifndef __Exception
#define __Exception


class Exception
{
public:
    Exception() :
         m_pMessage("") {}
    virtual ~Exception() {}
    Exception(const char *pMessage) :
         m_pMessage(pMessage) {}
    const char * what() { return m_pMessage; }
private:
    const char *m_pMessage;
};

class ConvException
{
public:
    ConvException() :
         m_pMessage("") {}
    virtual ~ConvException() {}
    ConvException(const char *pMessage) :
         m_pMessage(pMessage) {}
    const char * what() { return m_pMessage; }
private:
    const char *m_pMessage;
};

#endif // __Exception