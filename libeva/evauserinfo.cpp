#include "evauserinfo.h"

#include <QByteArray>
#include <QDebug>

#include <string.h>
#include <time.h>

GetUserInfoPacket::GetUserInfoPacket() : SessionOutPacket()
{
    m_command = 0x003c;

    m_QQId = 0;
}

GetUserInfoPacket::~GetUserInfoPacket()
{
}

void GetUserInfoPacket::fillSessionBody( unsigned char* buf, int* pos )
{
    /// fixed 0x00 0x01
    fill2( buf, pos, 0x0001 );

    /// qq id
    fill4( buf, pos, m_QQId );

    /// 22 bytes zero filling
    zeroN( buf, pos, 22 );

    /// fixed info string
    const unsigned char info[54] = {
        0x00, 0x1a, 0x4e, 0x22, 0x4e, 0x25, 0x4e, 0x26, 0x4e, 0x27, 0x4e, 0x29, 0x4e, 0x2a, 0x4e, 0x2b,
        0x4e, 0x2c, 0x4e, 0x2d, 0x4e, 0x2e, 0x4e, 0x2f, 0x4e, 0x30, 0x4e, 0x31, 0x4e, 0x33, 0x4e, 0x35,
        0x4e, 0x36, 0x4e, 0x37, 0x4e, 0x38, 0x4e, 0x3f, 0x4e, 0x40, 0x4e, 0x41, 0x4e, 0x42, 0x4e, 0x43,
        0x4e, 0x45, 0x52, 0x0b, 0x52, 0x0f
    };
    fillN( buf, pos, info, 54 );
}

GetUserInfoReplyPacket::GetUserInfoReplyPacket() : SessionInPacket()
{
    m_command = 0x003c;

    m_QQId = 0;
    m_nickname = 0;
    m_name = 0;
    m_email = 0;
    m_occupation = 0;
    m_homepage = 0;
    m_mobile = 0;
    m_introduction = 0;
}

GetUserInfoReplyPacket::~GetUserInfoReplyPacket()
{
    free( m_nickname );
    free( m_name );
    free( m_email );
    free( m_occupation );
    free( m_homepage );
    free( m_mobile );
    free( m_introduction );
}

void GetUserInfoReplyPacket::extractSessionBody( const unsigned char* buf, int* pos, int len )
{
    qWarning() << "GetUserInfoReplyPacket::extractSessionBody";

    qWarning() << QByteArray( (char*)buf, len ).toHex();

    /// FIXME skip 2 bytes
    skipN( buf, pos, 2 );

    /// get buddy info flag, 0x00 -> successful
    unsigned char flag;
    extract1( buf, pos, &flag );
    if ( flag != 0x00 ) {
        qWarning() << "GetUserInfo failed";
        /// through all the remaining bytes
        skipN( buf, pos, len - 2 - 1 );
        return;
    }

    /// qq id
    extract4( buf, pos, &m_QQId );

    /// FIXME skip 4 bytes
    skipN( buf, pos, 4 );

    /// count
    unsigned short count;
    extract2( buf, pos, &count );

    for ( unsigned short i = 0; i < count; ++i ) {
        /// info type flag
        unsigned short infoType;
        extract2( buf, pos, &infoType );

        /// info entry size
        unsigned short infoSize;
        extract2( buf, pos, &infoSize );

        /// info content
        unsigned char* infoContent = (unsigned char*)malloc( infoSize );
        extractN( buf, pos, infoContent, infoSize );

        switch ( infoType ) {
            case 0x4e22: {  /// nick
                m_nickname = strndup( (char*)infoContent, infoSize );
                qWarning() << "nick" << QString::fromUtf8( m_nickname );
                break;
            }
            case 0x4e25: {  /// zipcode or address
                qWarning() << "zipcode/address" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e27: {  /// telephone
                qWarning() << "telephone" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e29: {  /// gender
                qWarning() << "gender" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e2a: {  /// name
                m_name = strndup( (char*)infoContent, infoSize );
                qWarning() << "nick" << QString::fromUtf8( m_name );
                break;
            }
            case 0x4e2b: {  /// email
                m_email = strndup( (char*)infoContent, infoSize );
                qWarning() << "email" << QString::fromUtf8( m_email );
                break;
            }
            case 0x4e2c: {  /// occupation
                m_occupation = strndup( (char*)infoContent, infoSize );
                qWarning() << "occupation" << QString::fromUtf8( m_occupation );
                break;
            }
            case 0x4e2d: {  /// homepage
                m_homepage = strndup( (char*)infoContent, infoSize );
                qWarning() << "homepage" << QString::fromUtf8( m_homepage );
                break;
            }
            case 0x4e2e: {  /// country
                qWarning() << "country" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e2f: {  /// face
                qWarning() << "face" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e30: {  /// mobile
                m_mobile = strndup( (char*)infoContent, infoSize );
                qWarning() << "mobile" << QString::fromUtf8( m_mobile );
                break;
            }
            case 0x4e31: {  /// privacy
                qWarning() << "privacy" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e33: {  /// introduction
                m_introduction = strndup( (char*)infoContent, infoSize );
                qWarning() << "introduction" << QString::fromUtf8( m_introduction );
                break;
            }
            case 0x4e35: {  /// school
                m_school = strndup( (char*)infoContent, infoSize );
                qWarning() << "school" << QString::fromUtf8( m_school );
                break;
            }
            case 0x4e36: {  /// horoscope
                qWarning() << "horoscope" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e37: {  /// zodiac
                qWarning() << "zodiac" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e38: {  /// blood
                qWarning() << "blood" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e3f: {  /// birthday
                qWarning() << "birthday" << QByteArray( (char*)infoContent, infoSize ).toHex();
//                 time_t time;
//                 int p = 0;
//                 extractN( infoContent, &p, &time, 4 );
//                 qWarning() << "birthday" << ctime( &time );
                break;
            }
            case 0x4e40: {  /// country province city
                qWarning() << "country province city" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e41: {  /// 1st language
                qWarning() << "1st language" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e42: {  /// 2nd language
                qWarning() << "2nd language" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e43: {  /// 3rd language
                qWarning() << "3rd language" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x4e45: {  /// age
                qWarning() << "age" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x520b: {  /// vip
                qWarning() << "vip" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            case 0x520f: {  /// client
                qWarning() << "client" << QByteArray( (char*)infoContent, infoSize ).toHex();
                break;
            }
            default:
                break;
        }

        free( infoContent );
    }
}
