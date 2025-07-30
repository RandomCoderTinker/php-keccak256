/**
 * Filename: Keccak256.c
 *
 * Hashes the input using the Keccak-256 algorithm.
 *
 * @param string $data  The input data to hash.
 * @param bool $raw_output (optional) If set to `true`, returns the raw binary output;
 *                          if set to `false` (default), returns a 64-character hexadecimal string.
 *
 * @return string The Keccak-256 hash of the input data.
 */

#include "php.h"
#include <string.h>

// Keccak reference code (same as yours, condensed here)
#define FOR(i,n) for(i=0; i<n; ++i)
typedef unsigned char u8;
typedef unsigned long long int u64;
typedef unsigned int ui;

void Keccak(ui r, ui c, const u8 *in, u64 inLen, u8 sfx, u8 *out, u64 outLen);
void KECCAK_256(const u8 *in, u64 inLen, u8 *out) {
    Keccak(1088, 512, in, inLen, 0x01, out, 32);
}

int LFSR86540(u8 *R) { (*R)=((*R)<<1)^(((*R)&0x80)?0x71:0); return ((*R)&2)>>1; }
#define ROL(a,o) ((((u64)a)<<o)^(((u64)a)>>(64-o)))
static u64 load64(const u8 *x) { ui i; u64 u=0; FOR(i,8) { u<<=8; u|=x[7-i]; } return u; }
static void store64(u8 *x, u64 u) { ui i; FOR(i,8) { x[i]=u; u>>=8; } }
static void xor64(u8 *x, u64 u) { ui i; FOR(i,8) { x[i]^=u; u>>=8; } }
#define rL(x,y) load64((u8*)s+8*(x+5*y))
#define wL(x,y,l) store64((u8*)s+8*(x+5*y),l)
#define XL(x,y,l) xor64((u8*)s+8*(x+5*y),l)

void KeccakF1600(void *s) {
    ui r,x,y,i,j,Y; u8 R=0x01; u64 C[5],D;
    for(i=0; i<24; i++) {
        FOR(x,5) C[x]=rL(x,0)^rL(x,1)^rL(x,2)^rL(x,3)^rL(x,4);
        FOR(x,5) { D=C[(x+4)%5]^ROL(C[(x+1)%5],1); FOR(y,5) XL(x,y,D); }
        x=1; y=r=0; D=rL(x,y);
        FOR(j,24) { r+=j+1; Y=(2*x+3*y)%5; x=y; y=Y; C[0]=rL(x,y); wL(x,y,ROL(D,r%64)); D=C[0]; }
        FOR(y,5) { FOR(x,5) C[x]=rL(x,y); FOR(x,5) wL(x,y,C[x]^((~C[(x+1)%5])&C[(x+2)%5])); }
        FOR(j,7) if (LFSR86540(&R)) XL(0,0,(u64)1<<((1<<j)-1));
    }
}

void Keccak(ui r, ui c, const u8 *in, u64 inLen, u8 sfx, u8 *out, u64 outLen) {
    u8 s[200]; ui R=r/8; ui i,b=0; FOR(i,200) s[i]=0;
    while(inLen>0) { b=(inLen<R)?inLen:R; FOR(i,b) s[i]^=in[i]; in+=b; inLen-=b; if (b==R) { KeccakF1600(s); b=0; } }
    s[b]^=sfx; if((sfx&0x80)&&(b==(R-1))) KeccakF1600(s); s[R-1]^=0x80; KeccakF1600(s);
    while(outLen>0) { b=(outLen<R)?outLen:R; FOR(i,b) out[i]=s[i]; out+=b; outLen-=b; if(outLen>0) KeccakF1600(s); }
}

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_keccak_hash, 0, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, raw_output, _IS_BOOL,  0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(keccak_hash) {
    zend_string *in_str;
    zend_bool    raw_output = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(in_str)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(raw_output)
    ZEND_PARSE_PARAMETERS_END();

    unsigned char hash[32];
    KECCAK_256((u8*)ZSTR_VAL(in_str), (u64)ZSTR_LEN(in_str), hash);

    if (raw_output) {
        /* return the raw 32-byte binary blob */
        RETURN_STRINGL((char*)hash, 32);
    } else {
        /* return the 64-char hex string */
        char hex[65];
        for (int i = 0; i < 32; i++) {
            snprintf(hex + (i * 2), 3, "%02x", hash[i]); // 3 for 2 digits + null terminator
        }
        hex[64] = '\0';
        RETURN_STRING(hex);
    }
}

// Function registration
static const zend_function_entry keccak_functions[] = {
    PHP_FE(keccak_hash, arginfo_keccak_hash)
    PHP_FE_END
};

zend_module_entry keccak_module_entry = {
    STANDARD_MODULE_HEADER,
    "keccak",              // Extension name
    keccak_functions,      // Functions
    NULL, NULL, NULL, NULL, NULL,
    "0.1",                 // Version
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(keccak)
