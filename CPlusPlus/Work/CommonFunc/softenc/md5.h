#ifndef __MD5_H__
#define __MD5_H__

#include "inttypes.h"

/* MD5 context. */
typedef struct {
  uint32 state[4];                                   /* state (ABCD) */
  uint32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);

#endif	//__MD5_H__