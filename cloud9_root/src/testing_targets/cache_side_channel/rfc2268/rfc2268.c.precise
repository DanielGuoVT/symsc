/* rfc2268.c  - The cipher described in rfc2268; aka Ron's Cipher 2.
 * Copyright (C) 2003 Nikos Mavroyanopoulos
 * Copyright (C) 2004 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser general Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

/* This implementation was written by Nikos Mavroyanopoulos for GNUTLS
 * as a Libgcrypt module (gnutls/lib/x509/rc2.c) and later adapted for
 * direct use by Libgcrypt by Werner Koch.  This implementation is
 * only useful for pkcs#12 decryption.
 *
 * The implementation here is based on Peter Gutmann's RRC.2 paper.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <klee/klee.h>
#include <pthread.h>

/* The AC_CHECK_SIZEOF() in configure fails for some machines.
 * we provide some fallback values here */
#if !SIZEOF_UNSIGNED_SHORT
# undef SIZEOF_UNSIGNED_SHORT
# define SIZEOF_UNSIGNED_SHORT 2
#endif
#if !SIZEOF_UNSIGNED_INT
# undef SIZEOF_UNSIGNED_INT
# define SIZEOF_UNSIGNED_INT 4
#endif
#if !SIZEOF_UNSIGNED_LONG
# undef SIZEOF_UNSIGNED_LONG
# define SIZEOF_UNSIGNED_LONG 4
#endif

/* Provide uintptr_t */
#ifdef HAVE_STDINT_H
# include <stdint.h> /* uintptr_t */
#elif defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#else
/* In this case, uintptr_t is provided by config.h. */
#endif

#ifndef HAVE_BYTE_TYPEDEF
# undef byte  /* In case there is a macro with that name.  */
# if !(defined(_WIN32) && defined(cbNDRContext))
   /* Windows typedefs byte in the rpc headers.  Avoid warning about
      double definition.  */
   typedef unsigned char byte;
# endif
# define HAVE_BYTE_TYPEDEF
#endif

#ifndef HAVE_USHORT_TYPEDEF
# undef ushort  /* In case there is a macro with that name.  */
  typedef unsigned short ushort;
# define HAVE_USHORT_TYPEDEF
#endif

#ifndef HAVE_ULONG_TYPEDEF
# undef ulong   /* In case there is a macro with that name.  */
  typedef unsigned long ulong;
# define HAVE_ULONG_TYPEDEF
#endif

#ifndef HAVE_U16_TYPEDEF
# undef u16 /* In case there is a macro with that name.  */
# if SIZEOF_UNSIGNED_INT == 2
   typedef unsigned int   u16;
# elif SIZEOF_UNSIGNED_SHORT == 2
   typedef unsigned short u16;
# else
#  error no typedef for u16
# endif
# define HAVE_U16_TYPEDEF
#endif

#ifndef HAVE_U32_TYPEDEF
# undef u32 /* In case there is a macro with that name.  */
# if SIZEOF_UNSIGNED_INT == 4
   typedef unsigned int  u32;
# elif SIZEOF_UNSIGNED_LONG == 4
   typedef unsigned long u32;
# else
#  error no typedef for u32
# endif
# define HAVE_U32_TYPEDEF
#endif

/*
 * Warning: Some systems segfault when this u64 typedef and
 * the dummy code in cipher/md.c is not available.  Examples are
 * Solaris and IRIX.
 */
#ifndef HAVE_U64_TYPEDEF
# undef u64 /* In case there is a macro with that name.  */
# if SIZEOF_UNSIGNED_INT == 8
   typedef unsigned int u64;
#  define U64_C(c) (c ## U)
#  define HAVE_U64_TYPEDEF
# elif SIZEOF_UNSIGNED_LONG == 8
   typedef unsigned long u64;
#  define U64_C(c) (c ## UL)
#  define HAVE_U64_TYPEDEF
# elif SIZEOF_UNSIGNED_LONG_LONG == 8
   typedef unsigned long long u64;
#  define U64_C(c) (c ## ULL)
#  define HAVE_U64_TYPEDEF
# elif SIZEOF_UINT64_T == 8
   typedef uint64_t u64;
#  define U64_C(c) (UINT64_C(c))
#  define HAVE_U64_TYPEDEF
# endif
#endif

typedef enum 
  {
    GPG_ERR_NO_ERROR,
    GPG_ERR_BAD_MPI,
    GPG_ERR_BAD_SECKEY,
    GPG_ERR_BAD_SIGNATURE,
    GPG_ERR_CIPHER_ALGO,
    GPG_ERR_CONFLICT,
    GPG_ERR_DECRYPT_FAILED,
    GPG_ERR_DIGEST_ALGO,
    GPG_ERR_GENERAL,
    GPG_ERR_INTERNAL,
    GPG_ERR_INV_ARG,
    GPG_ERR_INV_CIPHER_MODE,
    GPG_ERR_INV_FLAG,
    GPG_ERR_INV_KEYLEN,
    GPG_ERR_INV_OBJ,
    GPG_ERR_INV_OP,
    GPG_ERR_INV_SEXP,
    GPG_ERR_INV_VALUE,
    GPG_ERR_MISSING_VALUE,
    GPG_ERR_NO_ENCRYPTION_SCHEME,
    GPG_ERR_NO_OBJ,
    GPG_ERR_NO_PRIME,
    GPG_ERR_NO_SIGNATURE_SCHEME,
    GPG_ERR_NOT_FOUND,
    GPG_ERR_NOT_IMPLEMENTED,
    GPG_ERR_NOT_SUPPORTED,
    GPG_ERROR_CFLAGS,
    GPG_ERR_PUBKEY_ALGO,
    GPG_ERR_SELFTEST_FAILED,
    GPG_ERR_TOO_SHORT,
    GPG_ERR_UNSUPPORTED,
    GPG_ERR_WEAK_KEY,
    GPG_ERR_WRONG_KEY_USAGE,
    GPG_ERR_WRONG_PUBKEY_ALGO,
    GPG_ERR_OUT_OF_MEMORY,
    GPG_ERR_TOO_LARGE,
    GPG_ERR_ENOMEM
  } gpg_err_code_t;
typedef gpg_err_code_t gpg_error_t;
typedef gpg_error_t gcry_error_t;
typedef gpg_err_code_t gcry_err_code_t;

#define KEY_SIZE 16
#define BLOCK_SIZE 8

typedef struct
{
  u16 S[64];
} RFC2268_context;

RFC2268_context ctx;

/* Test vectors from Peter Gutmann's paper. */
static unsigned char key[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char plaintext[] =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char scratch[16];

static const unsigned char rfc2268_sbox[] = {
  217, 120, 249, 196,  25, 221, 181, 237,
   40, 233, 253, 121,  74, 160, 216, 157,
  198, 126,  55, 131,  43, 118,  83, 142,
   98,  76, 100, 136,  68, 139, 251, 162,
   23, 154,  89, 245, 135, 179,  79,  19,
   97,  69, 109, 141,   9, 129, 125,  50,
  189, 143,  64, 235, 134, 183, 123,  11,
  240, 149,  33,  34,  92, 107,  78, 130,
   84, 214, 101, 147, 206,  96, 178,  28,
  115,  86, 192,  20, 167, 140, 241, 220,
   18, 117, 202,  31,  59, 190, 228, 209,
   66,  61, 212,  48, 163,  60, 182,  38,
  111, 191,  14, 218,  70, 105,   7,  87,
   39, 242,  29, 155, 188, 148,  67,   3,
  248,  17, 199, 246, 144, 239,  62, 231,
    6, 195, 213,  47, 200, 102,  30, 215,
    8, 232, 234, 222, 128,  82, 238, 247,
  132, 170, 114, 172,  53,  77, 106,  42,
  150,  26, 210, 113,  90,  21,  73, 116,
   75, 159, 208,  94,   4,  24, 164, 236,
  194, 224,  65, 110,  15,  81, 203, 204,
   36, 145, 175,  80, 161, 244, 112,  57,
  153, 124,  58, 133,  35, 184, 180, 122,
  252,   2,  54,  91,  37,  85, 151,  49,
   45,  93, 250, 152, 227, 138, 146, 174,
    5, 223,  41,  16, 103, 108, 186, 201,
  211,   0, 230, 207, 225, 158, 168,  44,
   99,  22,   1,  63,  88, 226, 137, 169,
   13,  56,  52,  27, 171,  51, 255, 176,
  187,  72,  12,  95, 185, 177, 205,  46,
  197, 243, 219,  71, 229, 165, 156, 119,
   10, 166,  32, 104, 254, 127, 193, 173
};

#define rotl16(x,n)   (((x) << ((u16)(n))) | ((x) >> (16 - (u16)(n))))
#define rotr16(x,n)   (((x) >> ((u16)(n))) | ((x) << (16 - (u16)(n))))

static void encrypt ()
{
  static int i, j;
  static u16 word0 = 0, word1 = 0, word2 = 0, word3 = 0;

  word0 = (word0 << 8) | plaintext[1];
  word0 = (word0 << 8) | plaintext[0];
  word1 = (word1 << 8) | plaintext[3];
  word1 = (word1 << 8) | plaintext[2];
  word2 = (word2 << 8) | plaintext[5];
  word2 = (word2 << 8) | plaintext[4];
  word3 = (word3 << 8) | plaintext[7];
  word3 = (word3 << 8) | plaintext[6];

  for (i = 0; i < 16; i++)
    {
      j = i * 4;
      /* For some reason I cannot combine those steps. */
      word0 += (word1 & ~word3) + (word2 & word3) + ctx.S[j];
      word0 = rotl16(word0, 1);

      word1 += (word2 & ~word0) + (word3 & word0) + ctx.S[j + 1];
      word1 = rotl16(word1, 2);

      word2 += (word3 & ~word1) + (word0 & word1) + ctx.S[j + 2];
      word2 = rotl16(word2, 3);

      word3 += (word0 & ~word2) + (word1 & word2) + ctx.S[j + 3];
      word3 = rotl16(word3, 5);

      if (i == 4 || i == 10)
        {
          word0 += ctx.S[word3 & 63];
          word1 += ctx.S[word0 & 63];
          word2 += ctx.S[word1 & 63];
          word3 += ctx.S[word2 & 63];
        }

    }

  scratch[0] = word0 & 255;
  scratch[1] = word0 >> 8;
  scratch[2] = word1 & 255;
  scratch[3] = word1 >> 8;
  scratch[4] = word2 & 255;
  scratch[5] = word2 >> 8;
  scratch[6] = word3 & 255;
  scratch[7] = word3 >> 8;
}

static void decrypt ()
{
  static int i, j;
  static u16 word0 = 0, word1 = 0, word2 = 0, word3 = 0;

  word0 = (word0 << 8) | plaintext[1];
  word0 = (word0 << 8) | plaintext[0];
  word1 = (word1 << 8) | plaintext[3];
  word1 = (word1 << 8) | plaintext[2];
  word2 = (word2 << 8) | plaintext[5];
  word2 = (word2 << 8) | plaintext[4];
  word3 = (word3 << 8) | plaintext[7];
  word3 = (word3 << 8) | plaintext[6];

  for (i = 15; i >= 0; i--)
    {
      j = i * 4;

      word3 = rotr16(word3, 5);
      word3 -= (word0 & ~word2) + (word1 & word2) + ctx.S[j + 3];

      word2 = rotr16(word2, 3);
      word2 -= (word3 & ~word1) + (word0 & word1) + ctx.S[j + 2];

      word1 = rotr16(word1, 2);
      word1 -= (word2 & ~word0) + (word3 & word0) + ctx.S[j + 1];

      word0 = rotr16(word0, 1);
      word0 -= (word1 & ~word3) + (word2 & word3) + ctx.S[j];

      if (i == 5 || i == 11)
        {
          word3 = word3 - ctx.S[word2 & 63];
          word2 = word2 - ctx.S[word1 & 63];
          word1 = word1 - ctx.S[word0 & 63];
          word0 = word0 - ctx.S[word3 & 63];
        }

    }

  scratch[0] = word0 & 255;
  scratch[1] = word0 >> 8;
  scratch[2] = word1 & 255;
  scratch[3] = word1 >> 8;
  scratch[4] = word2 & 255;
  scratch[5] = word2 >> 8;
  scratch[6] = word3 & 255;
  scratch[7] = word3 >> 8;
}

#define with_phase2 0 

static gpg_err_code_t
setkey ()
{
  static unsigned int i;
  static unsigned char *S, x;
  static int len;
  static int bits = KEY_SIZE * 8;

  if (KEY_SIZE < 40 / 8)  /* We want at least 40 bits. */
    return GPG_ERR_INV_KEYLEN;

  S = (unsigned char *) ctx.S;

  for (i = 0; i < KEY_SIZE; i++)
    S[i] = key[i];

  for (i = KEY_SIZE; i < 128; i++)
    S[i] = rfc2268_sbox[(S[i - KEY_SIZE] + S[i - 1]) & 255];

  S[0] = rfc2268_sbox[S[0]];

  /* Phase 2 - reduce effective key size to "bits". This was not
   * discussed in Gutmann's paper. I've copied that from the public
   * domain code posted in sci.crypt. */
  if (with_phase2)
    {
      len = (bits + 7) >> 3;
      i = 128 - len;
      x = rfc2268_sbox[S[i] & (255 >> (7 & -bits))];
      S[i] = x;

      while (i--)
        {
          x = rfc2268_sbox[x ^ S[i + len]];
          S[i] = x;
        }
    }

  /* Make the expanded key, endian independent. */
  for (i = 0; i < 64; i++)
    ctx.S[i] = ( (u16) S[i * 2] | (((u16) S[i * 2 + 1]) << 8));

  return 0;
}

////////////////////////////////////////////////////////////////////////////
#define shadow
#ifdef shadow
static unsigned char key_shadow[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static gpg_err_code_t
setkey1 ()
{
  static unsigned int i;
  static unsigned char *S, x;
  static int len;
  static int bits = KEY_SIZE * 8;

  if (KEY_SIZE < 40 / 8)  /* We want at least 40 bits. */
    return GPG_ERR_INV_KEYLEN;

  S = (unsigned char *) ctx.S;

  for (i = 0; i < KEY_SIZE; i++)
    S[i] = key_shadow[i];

  for (i = KEY_SIZE; i < 128; i++)
    S[i] = rfc2268_sbox[(S[i - KEY_SIZE] + S[i - 1]) & 255];

  S[0] = rfc2268_sbox[S[0]];

  /* Phase 2 - reduce effective key size to "bits". This was not
   * discussed in Gutmann's paper. I've copied that from the public
   * domain code posted in sci.crypt. */
  if (with_phase2)
    {
      len = (bits + 7) >> 3;
      i = 128 - len;
      x = rfc2268_sbox[S[i] & (255 >> (7 & -bits))];
      S[i] = x;

      while (i--)
        {
          x = rfc2268_sbox[x ^ S[i + len]];
          S[i] = x;
        }
    }

  /* Make the expanded key, endian independent. */
  for (i = 0; i < 64; i++)
    ctx.S[i] = ( (u16) S[i * 2] | (((u16) S[i * 2 + 1]) << 8));

  return 0;
}

static void encrypt1 ()
{
  static int i, j;
  static u16 word0 = 0, word1 = 0, word2 = 0, word3 = 0;

  word0 = (word0 << 8) | plaintext[1];
  word0 = (word0 << 8) | plaintext[0];
  word1 = (word1 << 8) | plaintext[3];
  word1 = (word1 << 8) | plaintext[2];
  word2 = (word2 << 8) | plaintext[5];
  word2 = (word2 << 8) | plaintext[4];
  word3 = (word3 << 8) | plaintext[7];
  word3 = (word3 << 8) | plaintext[6];

  for (i = 0; i < 16; i++)
    {
      j = i * 4;
      /* For some reason I cannot combine those steps. */
      word0 += (word1 & ~word3) + (word2 & word3) + ctx.S[j];
      word0 = rotl16(word0, 1);

      word1 += (word2 & ~word0) + (word3 & word0) + ctx.S[j + 1];
      word1 = rotl16(word1, 2);

      word2 += (word3 & ~word1) + (word0 & word1) + ctx.S[j + 2];
      word2 = rotl16(word2, 3);

      word3 += (word0 & ~word2) + (word1 & word2) + ctx.S[j + 3];
      word3 = rotl16(word3, 5);

      if (i == 4 || i == 10)
        {
          word0 += ctx.S[word3 & 63];
          word1 += ctx.S[word0 & 63];
          word2 += ctx.S[word1 & 63];
          word3 += ctx.S[word2 & 63];
        }
    }

  scratch[0] = word0 & 255;
  scratch[1] = word0 >> 8;
  scratch[2] = word1 & 255;
  scratch[3] = word1 >> 8;
  scratch[4] = word2 & 255;
  scratch[5] = word2 >> 8;
  scratch[6] = word3 & 255;
}

#endif

////////////////////////////////////////////////////////////////////////////

void klee_make_symbolic(void *addr, size_t nbytes, const char *name) __attribute__((weak));
void klee_assume(uintptr_t condition) __attribute__((weak));
void klee_task_boundary() __attribute__((weak));

void thread1(){
  setkey ();
  encrypt ();
}

int64_t dummy;
void thread2(){
  dummy = 0x1a71ca45c0d01d01;
}

int main(void)
{
  klee_make_symbolic(key, sizeof(key), "key");
  pthread_t t1, t2;
  pthread_create(&t1, 0, (void*)thread1, 0);
  pthread_create(&t2, 0, (void*)thread2, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);

  #ifdef shadow
  klee_make_symbolic(key_shadow, sizeof(key_shadow), "key_shadow");
  klee_task_boundary();
  setkey1();
  encrypt1();
  #endif

  return 0;
}
