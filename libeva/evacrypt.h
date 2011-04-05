#ifndef EVACRYPT_H
#define EVACRYPT_H

namespace EvaCrypt
{
    void encrypt( unsigned char* in, int inlen, const unsigned char* key,
                  unsigned char* out, int* outlen_ptr );
    int decrypt( unsigned char* in, int inlen, const unsigned char* key,
                 unsigned char* out, int* outlen_ptr );
}

#endif // EVACRYPT_H
