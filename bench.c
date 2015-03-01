#include "api.h"
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>

int crypto_onetimeauth_poly1305_mips32r2donna(
    unsigned char*,const unsigned char*,unsigned long long,const unsigned char*);
int crypto_onetimeauth_poly1305_mips32r2donna_verify(
    const unsigned char*,const unsigned char*,unsigned long long,const unsigned char*);

static uint32_t seed[32] = { 3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5 } ;
static uint32_t in[12];
static uint32_t out[8];
static int outleft = 0;

#define ROTATE(x,b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i,b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x,b));

static void surf(void)
{
    uint32_t t[12]; uint32_t x; uint32_t sum = 0;
    int r; int i; int loop;

    for (i = 0;i < 12;++i) { t[i] = in[i] ^ seed[12 + i]; }
    for (i = 0;i < 8;++i) { out[i] = seed[24 + i]; }
    x = t[11];
    for (loop = 0;loop < 2;++loop) {
        for (r = 0;r < 16;++r) {
            sum += 0x9e3779b9;
            MUSH(0,5) MUSH(1,7) MUSH(2,9) MUSH(3,13)
            MUSH(4,5) MUSH(5,7) MUSH(6,9) MUSH(7,13)
            MUSH(8,5) MUSH(9,7) MUSH(10,9) MUSH(11,13)
        }
        for (i = 0;i < 8;++i) { out[i] ^= t[i + 4]; }
    }
}

static void randombytes(unsigned char *x,unsigned long long xlen)
{
    while (xlen > 0) {
        if (!outleft) {
            if (!++in[0]) {
                if (!++in[1]) {
                    if (!++in[2]) { ++in[3]; }
                }
            }
            surf();
            outleft = 8;
        }
        *x = out[--outleft];
        ++x;
        --xlen;
    }
}


#define MAXTEST_BYTES 4096
#define MGAP 8192

static void fail(const char *why)
{
    fprintf(stderr,"measure: fatal: %s\n",why);
    exit(111);
}

static unsigned char *alignedcalloc(unsigned long long len)
{
    unsigned char *x = (unsigned char *) calloc(1,len + 128);
    if (!x) fail("out of memory");
    /* will never deallocate so shifting is ok */
    x += 63 & (-(unsigned long) x);
    return x;
}

int main(int argc, char** argv)
{
    char checksum[CRYPTO_BYTES * 2 + 1];
    unsigned char* k;
    unsigned char* m;
    unsigned char* h;
    int i;
    int loop;
    int loops;
    int mlen;

    loops = 1000;
    if (argc > 1 && atoi(argv[1])) { loops = atoi(argv[1]); }

    k = alignedcalloc(CRYPTO_KEYBYTES);
    m = alignedcalloc(MAXTEST_BYTES);
    h = alignedcalloc(CRYPTO_BYTES);
    randombytes(k, CRYPTO_KEYBYTES);
    randombytes(h, CRYPTO_BYTES);

    for (loop = 0; loop < loops; ++loop) {
        for (mlen = 0; mlen <= MAXTEST_BYTES; mlen += 1 + mlen / MGAP) {
            crypto_onetimeauth_poly1305_mips32r2donna(h,m,mlen,k);
        }
        for (i = 0; i < CRYPTO_KEYBYTES/4; i++) { k[i] ^= h[i%(CRYPTO_BYTES/4)]; }
        for (i = 0; i < MAXTEST_BYTES/4; i++) { m[i] ^= h[i%(CRYPTO_BYTES/4)]; }
    }
    for (i = 0;i < CRYPTO_BYTES;++i) {
        checksum[2 * i] = "0123456789abcdef"[15 & (h[i] >> 4)];
        checksum[2 * i + 1] = "0123456789abcdef"[15 & h[i]];
    }
    printf("%s\n",checksum);
    return 0;
}
