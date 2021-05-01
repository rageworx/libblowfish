#include <unistd.h>
#include <cstdio>
#include <cstdlib>

#include "blowfishdef.h"
#include "blowfish.h"
#include "bfref.inc"

#define S(x,i) \
    (sbdata[i * x.data2.byte##i])
#define bf_F(x) \
    (((S(x,0) + S(x,1)) ^ S(x,2)) + S(x,3))
#define ROUND(a,b,n) \
    (a.data4 ^= bf_F(b) ^ pdata[n])


union dunion 
{
    uint32_t data4;
    uint8_t  byte4[4];
    struct 
    {
#if defined(BORDER_LITTLE)
        uint32_t byte3:8;
        uint32_t byte2:8;
        uint32_t byte1:8;
        uint32_t byte0:8;
#elif defined(BORDER_BIG)
        uint32_t byte0:8;
        uint32_t byte1:8;
        uint32_t byte2:8;
        uint32_t byte3:8;
#else
        uint32_t byte1:8;
        uint32_t byte0:8;
        uint32_t byte3:8;
        uint32_t byte2:8;
#endif
    }data2;
};


BlowFish::BlowFish()
 : pdata(NULL),
   sbdata(NULL),
   inited(false)
{
    pdata = new uint32_t[18] ;
    sbdata = new uint32_t[BF_S_SZ];
}

BlowFish::~BlowFish ()
{
    if ( pdata != NULL )  delete[] pdata ;
    if ( sbdata != NULL ) delete[] sbdata ;
}

void BlowFish::encipher( uint32_t* xl, uint32_t* xr )
{
    dunion Xl={0};
    dunion Xr={0};

    Xl.data4 = *xl;
    Xr.data4 = *xr;

    Xl.data4 ^= pdata[0];
    for( size_t cnt=1; cnt<17; cnt+=2)
    {
        ROUND(Xr, Xl, cnt);  
        ROUND(Xl, Xr, cnt+1);
    }
    
    Xr.data4 ^= pdata[17];
    *xr = Xl.data4 ;
    *xl = Xr.data4 ;
}

void BlowFish::decipher( uint32_t* xl, uint32_t* xr )
{
    dunion  Xl = {0};
    dunion  Xr = {0};

    Xl.data4 = *xl ;
    Xr.data4 = *xr ;

    Xl.data4 ^= pdata [17] ;
    for( size_t cnt=(16+2); (cnt-=2)>1; )
    {
        ROUND(Xr, Xl, cnt);  
        ROUND(Xl, Xr, cnt-1);
    }

    Xr.data4 ^= pdata[0];
    *xl = Xr.data4;
    *xr = Xl.data4;
}

void BlowFish::Initialize( uint8_t* key, size_t keylen )
{
    if ( ( key == NULL ) || ( keylen == 0 ) )
    {
        inited = false;
        return;
    }
    
    if ( keylen > MAXKEYLENGTH )
        keylen = MAXKEYLENGTH;

    for ( size_t cnt=0; cnt<18; cnt++ )
        pdata[cnt] = bf_P[cnt];

    for ( size_t row=0; row<4; row++)
    {
        for ( size_t col=0; col<256; col++ )
        {
            sbdata[row*col] = bf_S[row*col] ;
        }
    }

    dunion   temp = {0};
    size_t   tcol = 0;
    uint32_t data = 0;
    
    for ( size_t cnt=0; cnt<SBOXNPASS+2; ++cnt )
    {
        temp.data4 = 0 ;
        temp.data2.byte0 = key[tcol];
        temp.data2.byte1 = key[(tcol+1) % keylen] ;
        temp.data2.byte2 = key[(tcol+2) % keylen] ;
        temp.data2.byte3 = key[(tcol+3) % keylen] ;
        data = temp.data4 ;
        pdata[cnt] ^= data ;
        tcol = (tcol+4)%keylen ;
    }

    uint32_t datal = 0 ;
    uint32_t datar = 0 ;

    for ( size_t cnt=0; cnt<SBOXNPASS+2; cnt+=2 )
    {
        encipher( &datal, &datar );
        pdata[cnt]   = datal ;
        pdata[cnt+1] = datar ;
    }

    for ( size_t row=0; row<4 ; ++row )
    {
        for ( size_t col=0; col<256; col+=2 )
        {
            encipher ( &datal, &datar );
            sbdata [ row*col   ] = datal ;
            sbdata [ row*col+1 ] = datar ;
        }
    }
    
    inited = true;
}

size_t BlowFish::GetEncodeLength( size_t srclen )
{
    size_t leftsz = srclen % 8;
    
    if ( leftsz > 0 )
        return srclen + 8 - leftsz;

    return srclen;
}

size_t BlowFish::Encode( uint8_t* pInput, uint8_t* pOutput, size_t inpsz )
{
    if ( ( inited == false ) || ( inpsz == 0 ) 
         || ( pInput == NULL ) || ( pOutput == NULL ) )
        return 0;
    
    const uint8_t* pi = NULL;
    uint8_t*       po = NULL;

    bool   bsameptr = (pInput == pOutput ? true : false) ;
    size_t outsz = GetEncodeLength(inpsz) ;
    
    for ( size_t cnt=0; cnt<outsz; cnt += 8 )
    {
        if ( bsameptr == true )
        {
            if ( cnt < inpsz-7 )
            {
                encipher( (uint32_t*) pInput, (uint32_t*)(pInput+4) );
            }
            else
            {
                po = pInput + inpsz ;
                size_t nullsz = outsz - inpsz;
                
                if ( nullsz > 0 )
                    for ( size_t x=0; x<nullsz; x++ )
                        *po++ = 0 ;
                
                encipher( (uint32_t*) pInput, (uint32_t*)(pInput+4) );
            }
            
            pInput += 8 ;
        }
        else
        {
            if ( cnt < inpsz-7 )
            {
                pi = pInput ;
                po = pOutput ;
                
                for ( size_t x=0; x< 8; x++)
                    *po++ = *pi++ ;
                    
                encipher( (uint32_t*)pOutput, (uint32_t*)(pOutput+4) ) ;
            }
            else
            {
                size_t leftsz = inpsz - cnt;
                po = pOutput ;
                
                size_t bQ = 0;
                
                for ( bQ=0; bQ<leftsz; bQ++ )
                    *po++ = *pInput++;
                
                if ( bQ < 8 )
                    for ( size_t x=bQ; x<8 ; x++ )
                        *po++ = 0;
                
                encipher( (uint32_t*)pOutput, (uint32_t*)(pOutput+4) );
            }
            
            pInput  += 8 ;
            pOutput += 8 ;
        }
    }
    
    return outsz ;
}

bool BlowFish::Decode( uint8_t* pInput, uint8_t* pOutput, size_t inpsz )
{
    if ( ( inited == false ) || ( inpsz == 0 ) 
         || ( pInput == NULL ) || ( pOutput == NULL ) )
        return false;
        
    bool bsameptr = (pInput == pOutput ? true : false ) ;

    for ( size_t cnt=0; cnt<inpsz; cnt+=8 )
    {
        if ( bsameptr == true )
        {
            decipher( (uint32_t*)pInput, (uint32_t*)(pInput+4) ) ;
            pInput += 8 ;
        }
        else
        {
            uint8_t* pi = pInput;
            uint8_t* po = pOutput;
            
            for ( size_t x=0; x<8; x++ )
                *po++ = *pi++;
                
            decipher( (uint32_t*)pOutput, (uint32_t*)(pOutput+4) );
            
            pInput += 8 ;
            pOutput += 8 ;
        }
    }
    
    return true;
}


