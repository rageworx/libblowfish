#ifndef __LIBBLOWFISHDEF_H__
#define __LIBBLOWFISHDEF_H__

// KEY length in byte : 56 ^ 2 = 448 bits
#define MAXKEYLENGTH    56

// SBOX number of passes
#define SBOXNPASS       16

// sens machine byte order
//    DCBA = little endian
//    ABCD = big endian
//    BADC = VAX ( it is used on modern system ?? 
//
#if defined(_WIN32)
    // windows machines are every little-endian
    #define BORDER_LITTLE       1
#elif defined(__linux__)
    #define BORDER_LITTLE       1
#elif defined(__APPLE__)
    #if defined(__BIG_ENDIAN__)
        #define BODER_BIG       1
    #else
        // nodern Apple machines are little-endian
        #define BORDER_LITTLE   1
    #endif 
#endif 

#endif /// of __LIBBLOWFISHDEF_H__
