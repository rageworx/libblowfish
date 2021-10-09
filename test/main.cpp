#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <blowfish.h>

using namespace std;

void prtHex( const uint8_t* p, size_t len )
{
    if ( p == NULL )
        return;

    uint8_t* pp = (uint8_t*)p;
    for( size_t cnt=0; cnt<len; cnt++ )
    {
        printf( "%02X", *pp );
        pp++;
    }
    printf( "\n" );
}

int main( int argc, char* argv[] )
{
    BlowFish bf;

    char testString[] = "This word will be encrypted and not be seen.";
    char encKey[]     = "This is a key word.";
    
    uint8_t* testOutEnc = NULL;
    uint8_t* testOutDec = NULL;

    bf.Initialize( (uint8_t*)encKey, strlen(encKey) );
    size_t outsz = bf.GetEncodeLength( strlen(testString) );
    testOutEnc = new uint8_t[ outsz + 1 ];
    if ( testOutEnc ==  NULL )
    {
        printf( "memory should not be allocated.\n" );
        return -1;
    }

    printf( "source : %s\n", testString );

    size_t encsz = bf.Encode( (uint8_t*)testString, 
                              testOutEnc, 
                              strlen(testString) );
    if ( encsz == 0 )
    {
        printf( "encode failure, size 0.\n" );
        delete[] testOutEnc;
        return -1;
    }

    printf( "source %lu bytes  : ", strlen(testString) );
    prtHex( (const uint8_t*)testString, strlen(testString) );
    printf( "encoded %lu bytes : ", encsz );
    prtHex( testOutEnc, encsz );

    testOutDec = new uint8_t[ strlen( testString ) + 1 ];

    if ( testOutDec == NULL )
    {
        printf( "memory should not be allocated.\n" );
        delete[] testOutEnc;
        return -1;
    }

    if ( bf.Decode( testOutEnc, testOutDec, encsz ) == true )
    {
        printf( "deocded : %s\n", (const char*)testOutDec );
    }
    
    delete[] testOutEnc;
    delete[] testOutDec;

    return 0;
}
