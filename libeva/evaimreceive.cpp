#include "evaimreceive.h"

#include <QByteArray>
#include <QDebug>

ReceiveIMPacket::ReceiveIMPacket() : SessionInPacket()
{
    m_command = 0x0017;

    m_sender = 0;
    m_receiver = 0;
    m_msgseq = 0;
    m_serverIp = 0;

    m_messageText = 0;
}

ReceiveIMPacket::~ReceiveIMPacket()
{
    free( m_messageText );
}

void ReceiveIMPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "ReceiveIMPacket::extractSessionBody";

    unsigned char b1;
    unsigned short b2;
    unsigned int b4;

    /// qq id from
    extract4( buf, pos, &m_sender );

    /// qq id to
    extract4( buf, pos, &m_receiver );

    /// sequence
    extract4( buf, pos, &m_msgseq );

    /// server ip
    extract4( buf, pos, &m_serverIp );

    /// server port
    extract2( buf, pos, &b2 );

    /// message type
    extract2( buf, pos, &m_messageType );

    switch ( m_messageType ) {
        case 0x000a:    /// unknown message 0x000a
        case 0x0009:    /// buddy message 0x0009
        case 0x00a6:    /// buddy message 0x00a6
        case 0x00a7:    /// buddy message 0x00a7
        case 0x0078: {  /// buddy message 0x0078
            /// skip 1 bytes
            skipN( buf, pos, 1 );

            /// tmp8
            unsigned char tmp8;
            int ptmp = *pos;
            extract1( buf, &ptmp, &tmp8 );
            while ( tmp8 == 0 ) {
                /// skip 1 bytes
                skipN( buf, pos, 1 );

                /// tmplen
                unsigned short tmplen;
                extract2( buf, pos, &tmplen );

                /// skip tmplen bytes
                skipN( buf, pos, tmplen );

                /// tmp8
                ptmp = *pos;
                extract1( buf, &ptmp, &tmp8 );
            }
            /// NOTE no break here, go through down
        }
        case 0x0084:    /// buddy message 0x0084
        case 0x0085: {  /// buddy message 0x0085
            /// version from
            extract2( buf, pos, &b2 );

            /// qq id from
            extract4( buf, pos, &b4 );

            /// qq id to
            extract4( buf, pos, &b4 );

            /// session md5
            unsigned char sessionMd5[16];
            extractN( buf, pos, sessionMd5, 16 );

            /// im type
            unsigned short imType;
            extract2( buf, pos, &imType );

            switch ( imType ) {
                case 0x00af: {
                    /// vibrate, nudge
                    qWarning() << "!!!!!!!!!!!!!NUDGE!!!!!!!!!!";
                    break;
                }
                case 0x000b: {
                    /// text
                    /// sequence
                    extract2( buf, pos, &b2 );

                    /// send time
                    extract4( buf, pos, &b4 );

                    /// sender icon
                    extract2( buf, pos, &b2 );

                    /// 3 bytes unknown
                    skipN( buf, pos, 3 );

                    /// has font attribute
                    unsigned char hasFontAttribute;
                    extract1( buf, pos, &hasFontAttribute );

                    /// fragment count
                    extract1( buf, pos, &b1 );

                    /// fragment index
                    extract1( buf, pos, &b1 );

                    /// message id
                    extract2( buf, pos, &b2 );

                    /// auto reply
                    extract1( buf, pos, &b1 );

                    switch ( m_messageType ) {
                        case 0x00a7:
                        case 0x00a6:
                        case 0x0078: {
                            /// skip 8 fixed bytes
                            /// 0x4d53470000000000
                            skipN( buf, pos, 8 );

                            /// send time
                            extract4( buf, pos, &b4 );

                            /// unknown random 4 bytes
                            skipN( buf, pos, 4 );

                            /// if has font attribute
                            if ( hasFontAttribute == 0x01 ) {
                                /// unknown 0x00
                                skipN( buf, pos, 1 );

                                /// font color rgb
                                unsigned char rgb[3];
                                extract1( buf, pos, &rgb[2] );
                                extract1( buf, pos, &rgb[1] );
                                extract1( buf, pos, &rgb[0] );

                                /// font size
                                extract1( buf, pos, &b1 );

                                /// bold 0x01   italic 0x02   underline 0x04
                                extract1( buf, pos, &b1 );

                                /// encoding  0x8602 -> GB18030,  0x0000 -> EN
                                extract2( buf, pos, &b2 );

                                /// size of font name
                                unsigned short fontNameSize;
                                extract2( buf, pos, &fontNameSize );

                                /// font name
                                unsigned char* fontName = (unsigned char*)malloc( fontNameSize + 1 );
                                extractN( buf, pos, fontName, fontNameSize );
                                fontName[ fontNameSize ] = '\0';
                                qWarning() << "FontName" << QString::fromUtf8( (char*)fontName, fontNameSize + 1 );
                                free( fontName );

                                /// 2 bytes 0x00 0x00
                                skipN( buf, pos, 2 );

                                /// content data type
                                unsigned char ctype;
                                extract1( buf, pos, &ctype );

                                /// size of content data
                                unsigned short cSize;
                                extract2( buf, pos, &cSize );

                                switch ( ctype ) {
                                    case 0x01: {    /// text
                                        /// message data segment flag 0x01
                                        extract1( buf, pos, &b1 );

                                        /// size of content text
                                        unsigned short cTextSize;
                                        extract2( buf, pos, &cTextSize );

                                        /// content text
                                        m_messageText = (unsigned char*)malloc( cTextSize + 1 );
                                        extractN( buf, pos, m_messageText, cTextSize );
                                        m_messageText[ cTextSize ] = '\0';
                                        qWarning() << "TEXT" << QString::fromUtf8( (char*)m_messageText );
                                        break;
                                    }
                                    case 0x02: {    /// emoticon
                                        qWarning() << "emoticon content data type not implemented yet";
                                        /// size of index 01 00 01
                                        skipN( buf, pos, 3 );
                                        /// index 6f
                                        skipN( buf, pos, 1 );
                                        /// size of sym ff 00 02
                                        skipN( buf, pos, 3 );
                                        /// sym 14
                                        skipN( buf, pos, 1 );
                                        /// emoticon
                                        unsigned char ecsym;
                                        extract1( buf, pos, &ecsym );
                                        qWarning() << "emoticon" << QByteArray( (char*)&ecsym, 1 ).toHex();
                                        break;
                                    }
                                    case 0x03: {    /// image
                                        qWarning() << "image content data type not implemented yet";
                                        break;
                                    }
                                    default: {
                                        qWarning() << "unhandled content data type" << ctype;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case 0x0084:
                        case 0x0085:
                        case 0x000a:
                        case 0x0009: {
                            /// % % % % % 5
                            ///   ^ 1
                            int r = len - *pos;
                            m_messageText = (unsigned char*)malloc( r + 1 );
                            extractN( buf, pos, m_messageText, r );
                            m_messageText[ r ] = '\0';

                            qWarning() << "r" << r;

                            qWarning() << "TEXT" << QString::fromLocal8Bit( (char*)m_messageText, r + 1 );
                            qWarning() << "TEXT" << QByteArray( (char*)m_messageText, r + 1 );
                            break;
                        }
                    }
                    break;
                }
                default: {
                    qWarning() << "unhandled imType" << QByteArray( (char*)&imType, 2 ).toHex();
                    break;
                }
            }

            break;
        }
        case 0x0079: {  /// typing
            break;
        }
        case 0x0018: {  /// news
            break;
        }
        case 0x0014: {  /// sms NOTE not sure
            break;
        }
        case 0x0020:    /// unknown qun message
        case 0x002a:    /// temp qun message
        case 0x002b:    /// qun message 0x002b
        case 0x0052: {  /// qun message 0x0052
            break;
        }
        case 0x0021: {  /// add to qun
            break;
        }
        case 0x0022: {  /// remove from qun
            break;
        }
        case 0x0023: {  /// apply add to qun
            break;
        }
        case 0x0024: {  /// approve apply add to qun
            break;
        }
        case 0x0025: {  /// reject apply add to qun
            break;
        }
        case 0x0030: {  /// system message
            /// 00 00 00 00
            skipN( buf, pos, 4 );

            /// result
            extract1( buf, pos, &b1 );

            /// size of message text
            unsigned char len = 0;
            extract1( buf, pos, &len );

            /// message text
            m_messageText = (unsigned char*)malloc( len + 1 );
            extractN( buf, pos, m_messageText, len );
            m_messageText[ len ] = '\0';
            qWarning() << QString::fromUtf8( (char*)m_messageText );

            /// unknown 16 bytes 3a3da4dfde45d6fb
            skipN( buf, pos, 16 );
            break;
        }
        default: {
            qWarning() << "unhandled message type" << QByteArray( (char*)&m_messageType, 2 ).toHex();
            break;
        }
    }
}

ReceiveIMAckPacket::ReceiveIMAckPacket() : SessionOutPacket()
{
    m_command = 0x0017;

    m_sender = 0;
    m_receiver = 0;
    m_msgseq = 0;
    m_serverIp = 0;
}

ReceiveIMAckPacket::~ReceiveIMAckPacket()
{
}

void ReceiveIMAckPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// qq id from
    fill4( buf, pos, m_sender );

    /// qq id to
    fill4( buf, pos, m_receiver );

    /// sequence
    fill4( buf, pos, m_msgseq );

    /// server ip
    fill4( buf, pos, m_serverIp );
}
