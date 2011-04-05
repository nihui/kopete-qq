#ifndef EVALOGOUT_H
#define EVALOGOUT_H

#include "evapacket.h"

class LogoutPacket : public SessionOutPacket
{
    public:
        explicit LogoutPacket();
        virtual ~LogoutPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

#endif // EVALOGOUT_H
