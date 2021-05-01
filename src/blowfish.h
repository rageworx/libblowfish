#ifndef __LIBBLOWFISH_H__
#define __LIBBLOWFISH_H__

/**
* libblowfish
* ========================================================
* Based on Bruce Schneier's BlowFish.
* Reference on https://www.schneier.com/academic/blowfish/
* Code referenced who Jim Conger.
* 
* (C)2021, Raphael Kim
**/

#include <cstdint>

class BlowFish
{
    public:
        BlowFish();
        ~BlowFish();

    public:
        void   Initialize( uint8_t* key = NULL, size_t keylen = 0 );
        size_t GetEncodeLength( size_t srclen );
        size_t Encode( uint8_t* pInput, uint8_t* pOutput, size_t inpsz );
        bool   Decode( uint8_t* pInput, uint8_t* pOutput, size_t inpsz );
        
    private:
        void encipher( uint32_t* xl, uint32_t* xr );
        void decipher( uint32_t* xl, uint32_t* xr );

    private:
        bool        inited;
        uint32_t*   pdata;
        uint32_t*   sbdata;
};


#endif // of __LIBBLOWFISH_H__

