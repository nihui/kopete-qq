#include "evaweather.h"

#include <QByteArray>
#include <QDebug>
#include <QString>

#include <stdlib.h>

extern unsigned int localIp;

WeatherOpPacket::WeatherOpPacket() : SessionOutPacket()
{
    m_command = 0x00a6;
}

WeatherOpPacket::~WeatherOpPacket()
{
}

void WeatherOpPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// sub cmd type
    /// 0x03 get weather
    fill1( buf, pos, 0x03 );

    /// ip
    fillN( buf, pos, &localIp, 4 );

    /// no longer port
    zeroN( buf, pos, 2 );
}

WeatherOpReplyPacket::WeatherOpReplyPacket() : SessionInPacket()
{
    m_command = 0x00a6;
}

WeatherOpReplyPacket::~WeatherOpReplyPacket()
{
}

void WeatherOpReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// sub cmd type
    extract1( buf, pos, &b1 );

    /// result
    extract1( buf, pos, &b1 );

    /// time
    extract4( buf, pos, &b4 );

    /// size of text below
    extract1( buf, pos, &b1 );

    unsigned char* weatherStr = (unsigned char*)malloc( b1 );
    extractN( buf, pos, weatherStr, b1 );
    qWarning() << QString::fromLocal8Bit( (char*)weatherStr, b1 );
    free( weatherStr );

    /// size of text below
    extract1( buf, pos, &b1 );

    unsigned char* windStr = (unsigned char*)malloc( b1 );
    extractN( buf, pos, windStr, b1 );
    qWarning() << QString::fromLocal8Bit( (char*)windStr, b1 );
    free( windStr );

    /// 00 09 00 09 06 FIXME unknown, temperature here ?
    skipN( buf, pos, 5 );

    /// size of text below
    extract1( buf, pos, &b1 );

    unsigned char* hintStr = (unsigned char*)malloc( b1 );
    extractN( buf, pos, hintStr, b1 );
    qWarning() << QString::fromLocal8Bit( (char*)hintStr, b1 );
    free( hintStr );

    /// size of city name
    extract2( buf, pos, &b2 );

    unsigned char* cityStr = (unsigned char*)malloc( b2 );
    extractN( buf, pos, cityStr, b2 );
    qWarning() << QString::fromLocal8Bit( (char*)cityStr, b2 );
    free( cityStr );

    /// 00 00 00 00
    skipN( buf, pos, 4 );

    /// day count
    unsigned char dayCount;
    extract1( buf, pos, &dayCount );

    for ( int i = 0; i < dayCount; ++i ) {
        /// date time
        extract4( buf, pos, &b4 );

        /// size of text below
        extract1( buf, pos, &b1 );

        weatherStr = (unsigned char*)malloc( b1 );
        extractN( buf, pos, weatherStr, b1 );
        qWarning() << QString::fromLocal8Bit( (char*)weatherStr, b1 );
        free( weatherStr );

        /// size of text below
        extract1( buf, pos, &b1 );

        windStr = (unsigned char*)malloc( b1 );
        extractN( buf, pos, windStr, b1 );
        qWarning() << QString::fromLocal8Bit( (char*)windStr, b1 );
        free( windStr );

        /// 00 03 00 09 06 FIXME unknown, temperature here ?
        skipN( buf, pos, 5 );

        /// size of text below
        extract1( buf, pos, &b1 );

        hintStr = (unsigned char*)malloc( b1 );
        extractN( buf, pos, hintStr, b1 );
        qWarning() << QString::fromLocal8Bit( (char*)hintStr, b1 );
        free( hintStr );
    }

    /// unknown 00 00
    skipN( buf, pos, 2 );
}
