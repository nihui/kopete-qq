#ifndef EVAUTIL_H
#define EVAUTIL_H

namespace EvaUtil {
    /// utility functions for out packet
    void fill1( unsigned char* buf, int* pos, unsigned char n );
    void fill2( unsigned char* buf, int* pos, unsigned short n );
    void fill4( unsigned char* buf, int* pos, unsigned int n );
    void fillN( unsigned char* buf, int* pos, const void* array, int len );
    void zeroN( unsigned char* buf, int* pos, int len );
    void randN( unsigned char* buf, int* pos, int len );

    /// utility functions for in packet
    void extract1( const unsigned char* buf, int* pos, unsigned char* n );
    void extract2( const unsigned char* buf, int* pos, unsigned short* n );
    void extract4( const unsigned char* buf, int* pos, unsigned int* n );
    void extractN( const unsigned char* buf, int* pos, void* array, int len );
    void skipN( const unsigned char* buf, int* pos, int len );
}

#endif // EVAUTIL_H
