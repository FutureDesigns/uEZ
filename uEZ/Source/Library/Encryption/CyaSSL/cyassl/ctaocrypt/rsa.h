/* rsa.h
 *
 * Copyright (C) 2006-2014 wolfSSL Inc.
 *
 * This file is part of CyaSSL.
 *
 * CyaSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CyaSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef NO_RSA

#ifndef CTAO_CRYPT_RSA_H
#define CTAO_CRYPT_RSA_H

#include <cyassl/ctaocrypt/types.h>
#include <cyassl/ctaocrypt/integer.h>
#include <cyassl/ctaocrypt/random.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define CYASSL_RSA_CAVIUM_MAGIC 0xBEEF0006

enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1
};

/* RSA */
typedef struct RsaKey {
    mp_int n, e, d, p, q, dP, dQ, u;
    int32_t   type;                               /* public or private */
    void* heap;                               /* for user memory overrides */
#ifdef HAVE_CAVIUM
    int32_t    devId;           /* nitrox device id */
    word32 magic;           /* using cavium magic */
    word64 contextHandle;   /* nitrox context memory handle */
    byte*  c_n;             /* cavium byte buffers for key parts */
    byte*  c_e;
    byte*  c_d;
    byte*  c_p;
    byte*  c_q;
    byte*  c_dP;
    byte*  c_dQ;
    byte*  c_u;             /* sizes in bytes */
    word16 c_nSz, c_eSz, c_dSz, c_pSz, c_qSz, c_dP_Sz, c_dQ_Sz, c_uSz;
#endif
} RsaKey;


CYASSL_API int32_t  InitRsaKey(RsaKey* key, void*);
CYASSL_API int32_t  FreeRsaKey(RsaKey* key);

CYASSL_API int32_t  RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, RNG* rng);
CYASSL_API int32_t  RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);
CYASSL_API int32_t  RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);
CYASSL_API int32_t  RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, RNG* rng);
CYASSL_API int32_t  RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
CYASSL_API int32_t  RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
CYASSL_API int32_t  RsaEncryptSize(RsaKey* key);

CYASSL_API int32_t RsaPrivateKeyDecode(const byte* input, word32* inOutIdx, RsaKey*,
                                   word32);
CYASSL_API int32_t RsaPublicKeyDecode(const byte* input, word32* inOutIdx, RsaKey*,
                                  word32);
CYASSL_API int32_t RsaPublicKeyDecodeRaw(const byte* n, word32 nSz, const byte* e,
                                     word32 eSz, RsaKey* key);
CYASSL_API int32_t RsaFlattenPublicKey(RsaKey*, byte*, word32*, byte*, word32*);

#ifdef CYASSL_KEY_GEN
    CYASSL_API int32_t MakeRsaKey(RsaKey* key, int32_t size, long e, RNG* rng);
    CYASSL_API int32_t RsaKeyToDer(RsaKey*, byte* output, word32 inLen);
#endif

#ifdef HAVE_CAVIUM
    CYASSL_API int32_t  RsaInitCavium(RsaKey*, int32_t);
    CYASSL_API void RsaFreeCavium(RsaKey*);
#endif


#ifdef HAVE_FIPS
    /* fips wrapper calls, user can call direct */
    CYASSL_API int32_t  InitRsaKey_fips(RsaKey* key, void*);
    CYASSL_API int32_t  FreeRsaKey_fips(RsaKey* key);

    CYASSL_API int32_t  RsaPublicEncrypt_fips(const byte* in,word32 inLen,byte* out,
                                 word32 outLen, RsaKey* key, RNG* rng);
    CYASSL_API int32_t  RsaPrivateDecryptInline_fips(byte* in, word32 inLen,
                                                 byte** out, RsaKey* key);
    CYASSL_API int32_t  RsaPrivateDecrypt_fips(const byte* in, word32 inLen,
                                           byte* out,word32 outLen,RsaKey* key);
    CYASSL_API int32_t  RsaSSL_Sign_fips(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, RNG* rng);
    CYASSL_API int32_t  RsaSSL_VerifyInline_fips(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
    CYASSL_API int32_t  RsaSSL_Verify_fips(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
    CYASSL_API int32_t  RsaEncryptSize_fips(RsaKey* key);

    CYASSL_API int32_t RsaPrivateKeyDecode_fips(const byte* input, word32* inOutIdx,
                                            RsaKey*, word32);
    CYASSL_API int32_t RsaPublicKeyDecode_fips(const byte* input, word32* inOutIdx,
                                           RsaKey*, word32);
    #ifndef FIPS_NO_WRAPPERS
        /* if not impl or fips.c impl wrapper force fips calls if fips build */
        #define InitRsaKey              InitRsaKey_fips 
        #define FreeRsaKey              FreeRsaKey_fips 
        #define RsaPublicEncrypt        RsaPublicEncrypt_fips 
        #define RsaPrivateDecryptInline RsaPrivateDecryptInline_fips 
        #define RsaPrivateDecrypt       RsaPrivateDecrypt_fips 
        #define RsaSSL_Sign             RsaSSL_Sign_fips
        #define RsaSSL_VerifyInline     RsaSSL_VerifyInline_fips
        #define RsaSSL_Verify           RsaSSL_Verify_fips
        #define RsaEncryptSize          RsaEncryptSize_fips
        /* no implicit KeyDecodes since in asn.c (not rsa.c) */
    #endif /* FIPS_NO_WRAPPERS */

#endif /* HAVE_FIPS */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* CTAO_CRYPT_RSA_H */

#endif /* NO_RSA */
