#ifndef EVAIMSEND_H
#define EVAIMSEND_H

#include "evapacket.h"

#include <stdlib.h>
#include <string.h>

class SendImPacket : public SessionOutPacket
{
    public:
        explicit SendImPacket();
        virtual ~SendImPacket();
        void setReceiver( unsigned int receiver ) { m_receiver = receiver; }
        void setFontColor( unsigned int rgb ) { m_fontColor = rgb; }
        void setFontSize( unsigned char fontSize ) { m_fontSize = fontSize; }
        void setFontName( const unsigned char* fontName, int len )
            { free( m_fontName ); m_fontName = (unsigned char*)malloc( len ); memcpy( m_fontName, fontName, len ); }
        void setMsgText( const unsigned char* msgText, int len )
            { free( m_msgText ); m_msgText = (unsigned char*)malloc( len ); memcpy( m_msgText, msgText, len ); }
    protected:
        virtual void fillSessionBody( unsigned char* buf, int* pos );
    private:
        unsigned int m_receiver;
        unsigned int m_fontColor;
        unsigned char m_fontSize;
        unsigned char* m_fontName;
        unsigned char* m_msgText;
};

class SendImReplyPacket : public SessionInPacket
{
    public:
        explicit SendImReplyPacket();
        virtual ~SendImReplyPacket();
    protected:
        virtual void extractSessionBody( const unsigned char* buf, int* pos, int len );
};

#endif // EVAIMSEND_H
