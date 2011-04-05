#ifndef EVAWEATHER_H
#define EVAWEATHER_H

#include "evapacket.h"

class WeatherOpPacket : public SessionOutPacket
{
    public:
        explicit WeatherOpPacket();
        virtual ~WeatherOpPacket();
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
};

class WeatherOpReplyPacket : public SessionInPacket
{
    public:
        explicit WeatherOpReplyPacket();
        virtual ~WeatherOpReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAWEATHER_H
