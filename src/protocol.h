#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

class Protocol
{
public:
    virtual void readRequest(int client_socet, const std::string& str) = 0;
};

#endif // PROTOCOL_H
