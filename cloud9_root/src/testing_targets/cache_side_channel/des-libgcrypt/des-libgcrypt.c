/* des.c - DES and Triple-DES encryption/decryption Algorithm
 * Copyright (C) 1998, 1999, 2001, 2002, 2003,
 *               2008  Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
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
 *
 * For a description of triple encryption, see:
 *   Bruce Schneier: Applied Cryptography. Second Edition.
 *   John Wiley & Sons, 1996. ISBN 0-471-12845-7. Pages 358 ff.
 * This implementation is according to the definition of DES in FIPS
 * PUB 46-2 from December 1993. 
 */


/*
 * Written by Michael Roth <mroth@nessie.de>, September 1998
 */


/*
 *  U S A G E
 * ===========
 *
 * For DES or Triple-DES encryption/decryption you must initialize a proper
 * encryption context with a key.
 *
 * A DES key is 64bit wide but only 56bits of the key are used. The remaining
 * bits are parity bits and they will _not_ checked in this implementation, but
 * simply ignored.
 *
 * For Triple-DES you could use either two 64bit keys or three 64bit keys.
 * The parity bits will _not_ checked, too.
 *
 * After initializing a context with a key you could use this context to
 * encrypt or decrypt data in 64bit blocks in Electronic Codebook Mode.
 *
 * (In the examples below the slashes at the beginning and ending of comments
 * are omited.)
 *
 * DES Example
 * -----------
 *     unsigned char key[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     des_ctx context;
 *
 *     * Fill 'key' and 'plaintext' with some data *
 *     ....
 *
 *     * Set up the DES encryption context *
 *     des_setkey(context, key);
 *
 *     * Encrypt the plaintext *
 *     des_ecb_encrypt(context, plaintext, ciphertext);
 *
 *     * To recover the orginal plaintext from ciphertext use: *
 *     des_ecb_decrypt(context, ciphertext, recoverd);
 *
 *
 * Triple-DES Example
 * ------------------
 *     unsigned char key1[8];
 *     unsigned char key2[8];
 *     unsigned char key3[8];
 *     unsigned char plaintext[8];
 *     unsigned char ciphertext[8];
 *     unsigned char recoverd[8];
 *     tripledes_ctx context;
 *
 *     * If you would like to use two 64bit keys, fill 'key1' and'key2'
 *   then setup the encryption context: *
 *     tripledes_set2keys(context, key1, key2);
 *
 *     * To use three 64bit keys with Triple-DES use: *
 *     tripledes_set3keys(context, key1, key2, key3);
 *
 *     * Encrypting plaintext with Triple-DES *
 *     tripledes_ecb_encrypt(context, plaintext, ciphertext);
 *
 *     * Decrypting ciphertext to recover the plaintext with Triple-DES *
 *     tripledes_ecb_decrypt(context, ciphertext, recoverd);
 *
 *
 * Selftest
 * --------
 *     char *error_msg;
 *
 *     * To perform a selftest of this DES/Triple-DES implementation use the
 *   function selftest(). It will return an error string if there are
 *   some problems with this library. *
 *
 *     if ( (error_msg = selftest()) )
 *     {
 *     fprintf(stderr, "An error in the DES/Tripple-DES implementation occured: %s\n", error_msg);
 *     abort();
 *     }
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <klee/klee.h>
#include <pthread.h>

#define mode 0
#define KEY_SIZE 8
#define BLOCK_SIZE 8

typedef uint32_t u32;
typedef uint8_t byte;

byte key[KEY_SIZE] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
byte input[BLOCK_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
byte result[BLOCK_SIZE] = {};

byte key_shadow[KEY_SIZE] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

/*
 * These two tables are part of the 'permuted choice 1' function.
 * In this implementation several speed improvements are done.
 */
u32 leftkey_swap[16] =
{
  0x00000000, 0x00000001, 0x00000100, 0x00000101,
  0x00010000, 0x00010001, 0x00010100, 0x00010101,
  0x01000000, 0x01000001, 0x01000100, 0x01000101,
  0x01010000, 0x01010001, 0x01010100, 0x01010101
};

u32 rightkey_swap[16] =
{
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Encryption/Decryption context of DES
 */
typedef struct _des_ctx
{
    u32 encrypt_subkeys[32];
    u32 decrypt_subkeys[32];
}des_ctx;

des_ctx ctx;
/*
 * The s-box values are permuted according to the 'primitive function P'
 * and are rotated one bit to the left.
 */
static u32 sbox1[64] =
{
  0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404, 0x00000004, 0x00010000,
  0x00000400, 0x01010400, 0x01010404, 0x00000400, 0x01000404, 0x01010004, 0x01000000, 0x00000004,
  0x00000404, 0x01000400, 0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
  0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404, 0x00010404, 0x01000000,
  0x00010000, 0x01010404, 0x00000004, 0x01010000, 0x01010400, 0x01000000, 0x01000000, 0x00000400,
  0x01010004, 0x00010000, 0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
  0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404, 0x00010404, 0x01010400,
  0x00000404, 0x01000400, 0x01000400, 0x00000000, 0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static u32 sbox2[64] =
{
  0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020, 0x80100020, 0x80008020,
  0x80000020, 0x80108020, 0x80108000, 0x80000000, 0x80008000, 0x00100000, 0x00000020, 0x80100020,
  0x00108000, 0x00100020, 0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
  0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000, 0x80100000, 0x00008020,
  0x00000000, 0x00108020, 0x80100020, 0x00100000, 0x80008020, 0x80100000, 0x80108000, 0x00008000,
  0x80100000, 0x80008000, 0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
  0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020, 0x80000020, 0x00100020,
  0x00108000, 0x00000000, 0x80008000, 0x00008020, 0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static u32 sbox3[64] =
{
  0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000, 0x00020208, 0x08000200,
  0x00020008, 0x08000008, 0x08000008, 0x00020000, 0x08020208, 0x00020008, 0x08020000, 0x00000208,
  0x08000000, 0x00000008, 0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
  0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208, 0x00000200, 0x08000000,
  0x08020200, 0x08000000, 0x00020008, 0x00000208, 0x00020000, 0x08020200, 0x08000200, 0x00000000,
  0x00000200, 0x00020008, 0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
  0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208, 0x00020200, 0x08000008,
  0x08020000, 0x08000208, 0x00000208, 0x08020000, 0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static u32 sbox4[64] =
{
  0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081, 0x00800001, 0x00002001,
  0x00000000, 0x00802000, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00800080, 0x00800001,
  0x00000001, 0x00002000, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
  0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080, 0x00802081, 0x00000081,
  0x00800080, 0x00800001, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00000000, 0x00802000,
  0x00002080, 0x00800080, 0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
  0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001, 0x00802080, 0x00800081,
  0x00002001, 0x00002080, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static u32 sbox5[64] =
{
  0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100, 0x40000000, 0x02080000,
  0x40080100, 0x00080000, 0x02000100, 0x40080100, 0x42000100, 0x42080000, 0x00080100, 0x40000000,
  0x02000000, 0x40080000, 0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
  0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000, 0x42000000, 0x00080100,
  0x00080000, 0x42000100, 0x00000100, 0x02000000, 0x40000000, 0x02080000, 0x42000100, 0x40080100,
  0x02000100, 0x40000000, 0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
  0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000, 0x40080000, 0x42000000,
  0x00080100, 0x02000100, 0x40000100, 0x00080000, 0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static u32 sbox6[64] =
{
  0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010, 0x20404010, 0x00400000,
  0x20004000, 0x00404010, 0x00400000, 0x20000010, 0x00400010, 0x20004000, 0x20000000, 0x00004010,
  0x00000000, 0x00400010, 0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
  0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000, 0x20404000, 0x20000000,
  0x20004000, 0x00000010, 0x20400010, 0x00404000, 0x20404010, 0x00400000, 0x00004010, 0x20000010,
  0x00400000, 0x20004000, 0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
  0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000, 0x20400000, 0x00404010,
  0x00004000, 0x00400010, 0x20004010, 0x00000000, 0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static u32 sbox7[64] =
{
  0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802, 0x00200802, 0x04200800,
  0x04200802, 0x00200000, 0x00000000, 0x04000002, 0x00000002, 0x04000000, 0x04200002, 0x00000802,
  0x04000800, 0x00200802, 0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
  0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002, 0x04000000, 0x00200800,
  0x04000000, 0x00200800, 0x00200000, 0x04000802, 0x04000802, 0x04200002, 0x04200002, 0x00000002,
  0x00200002, 0x04000000, 0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
  0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000, 0x00000002, 0x04200802,
  0x00000000, 0x00200802, 0x04200000, 0x00000800, 0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static u32 sbox8[64] =
{
  0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040, 0x00000040, 0x10000000,
  0x00040040, 0x10040000, 0x10041040, 0x00041000, 0x10041000, 0x00041040, 0x00001000, 0x00000040,
  0x10040000, 0x10000040, 0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
  0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000, 0x00041040, 0x00040000,
  0x00041040, 0x00040000, 0x10041000, 0x00001000, 0x00000040, 0x10040040, 0x00001000, 0x00041040,
  0x10001000, 0x00000040, 0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
  0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000, 0x10001040, 0x00000000,
  0x10041040, 0x00041000, 0x00041000, 0x00001040, 0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * Numbers of left shifts per round for encryption subkeys.
 * To calculate the decryption subkeys we just reverse the
 * ordering of the calculated encryption subkeys. So their
 * is no need for a decryption rotate tab.
 */
static byte encrypt_rotate_tab[16] =
{
  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * Table with weak DES keys sorted in ascending order.
 * In DES their are 64 known keys which are weak. They are weak
 * because they produce only one, two or four different
 * subkeys in the subkey scheduling process.
 * The keys in this table have all their parity bits cleared.
 */
static byte weak_keys[64][8] =
{
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /*w*/
  { 0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e },
  { 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0 },
  { 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe },
  { 0x00, 0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e }, /*sw*/
  { 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00 },
  { 0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe },
  { 0x00, 0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0 },
  { 0x00, 0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0 }, /*sw*/
  { 0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe },
  { 0x00, 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00 },
  { 0x00, 0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e },
  { 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe }, /*sw*/
  { 0x00, 0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0 },
  { 0x00, 0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e },
  { 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00 },
  { 0x1e, 0x00, 0x00, 0x1e, 0x0e, 0x00, 0x00, 0x0e },
  { 0x1e, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x0e, 0x00 }, /*sw*/
  { 0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0, 0xfe },
  { 0x1e, 0x00, 0xfe, 0xe0, 0x0e, 0x00, 0xfe, 0xf0 },
  { 0x1e, 0x1e, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x00 },
  { 0x1e, 0x1e, 0x1e, 0x1e, 0x0e, 0x0e, 0x0e, 0x0e }, /*w*/
  { 0x1e, 0x1e, 0xe0, 0xe0, 0x0e, 0x0e, 0xf0, 0xf0 },
  { 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe, 0xfe },
  { 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00, 0xfe },
  { 0x1e, 0xe0, 0x1e, 0xe0, 0x0e, 0xf0, 0x0e, 0xf0 }, /*sw*/
  { 0x1e, 0xe0, 0xe0, 0x1e, 0x0e, 0xf0, 0xf0, 0x0e },
  { 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0, 0xfe, 0x00 },
  { 0x1e, 0xfe, 0x00, 0xe0, 0x0e, 0xfe, 0x00, 0xf0 },
  { 0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e, 0xfe }, /*sw*/
  { 0x1e, 0xfe, 0xe0, 0x00, 0x0e, 0xfe, 0xf0, 0x00 },
  { 0x1e, 0xfe, 0xfe, 0x1e, 0x0e, 0xfe, 0xfe, 0x0e },
  { 0xe0, 0x00, 0x00, 0xe0, 0xf0, 0x00, 0x00, 0xf0 },
  { 0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e, 0xfe },
  { 0xe0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xf0, 0x00 }, /*sw*/
  { 0xe0, 0x00, 0xfe, 0x1e, 0xf0, 0x00, 0xfe, 0x0e },
  { 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00, 0xfe },
  { 0xe0, 0x1e, 0x1e, 0xe0, 0xf0, 0x0e, 0x0e, 0xf0 },
  { 0xe0, 0x1e, 0xe0, 0x1e, 0xf0, 0x0e, 0xf0, 0x0e }, /*sw*/
  { 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e, 0xfe, 0x00 },
  { 0xe0, 0xe0, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00 },
  { 0xe0, 0xe0, 0x1e, 0x1e, 0xf0, 0xf0, 0x0e, 0x0e },
  { 0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0 }, /*w*/
  { 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe, 0xfe },
  { 0xe0, 0xfe, 0x00, 0x1e, 0xf0, 0xfe, 0x00, 0x0e },
  { 0xe0, 0xfe, 0x1e, 0x00, 0xf0, 0xfe, 0x0e, 0x00 },
  { 0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0, 0xfe }, /*sw*/
  { 0xe0, 0xfe, 0xfe, 0xe0, 0xf0, 0xfe, 0xfe, 0xf0 },
  { 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0xfe },
  { 0xfe, 0x00, 0x1e, 0xe0, 0xfe, 0x00, 0x0e, 0xf0 },
  { 0xfe, 0x00, 0xe0, 0x1e, 0xfe, 0x00, 0xf0, 0x0e },
  { 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00, 0xfe, 0x00 }, /*sw*/
  { 0xfe, 0x1e, 0x00, 0xe0, 0xfe, 0x0e, 0x00, 0xf0 },
  { 0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e, 0xfe },
  { 0xfe, 0x1e, 0xe0, 0x00, 0xfe, 0x0e, 0xf0, 0x00 },
  { 0xfe, 0x1e, 0xfe, 0x1e, 0xfe, 0x0e, 0xfe, 0x0e }, /*sw*/
  { 0xfe, 0xe0, 0x00, 0x1e, 0xfe, 0xf0, 0x00, 0x0e },
  { 0xfe, 0xe0, 0x1e, 0x00, 0xfe, 0xf0, 0x0e, 0x00 },
  { 0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0, 0xfe },
  { 0xfe, 0xe0, 0xfe, 0xe0, 0xfe, 0xf0, 0xfe, 0xf0 }, /*sw*/
  { 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00 },
  { 0xfe, 0xfe, 0x1e, 0x1e, 0xfe, 0xfe, 0x0e, 0x0e },
  { 0xfe, 0xfe, 0xe0, 0xe0, 0xfe, 0xfe, 0xf0, 0xf0 },
  { 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe }  /*w*/
};
static unsigned char weak_keys_chksum[20] = {
  0xD0, 0xCF, 0x07, 0x38, 0x93, 0x70, 0x8A, 0x83, 0x7D, 0xD7,
  0x8A, 0x36, 0x65, 0x29, 0x6C, 0x1F, 0x7C, 0x3F, 0xD3, 0x41 
};

/*
 * Macro to swap bits across two words.
 */
#define DO_PERMUTATION(a, temp, b, offset, mask)  \
    temp = ((a>>offset) ^ b) & mask;      \
    b ^= temp;            \
    a ^= temp<<offset;

/*
 * This performs the 'initial permutation' of the data to be encrypted
 * or decrypted. Additionally the resulting two words are rotated one bit
 * to the left.
 */
#define INITIAL_PERMUTATION(left, temp, right)    \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)  \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff) \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)  \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)  \
    right =  (right << 1) | (right >> 31);    \
    temp  =  (left ^ right) & 0xaaaaaaaa;   \
    right ^= temp;          \
    left  ^= temp;          \
    left  =  (left << 1) | (left >> 31);

/*
 * The 'inverse initial permutation'.
 */
#define FINAL_PERMUTATION(left, temp, right)    \
    left  =  (left << 31) | (left >> 1);    \
    temp  =  (left ^ right) & 0xaaaaaaaa;   \
    left  ^= temp;          \
    right ^= temp;          \
    right  =  (right << 31) | (right >> 1);   \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff)  \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333)  \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff) \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)

/*
 * A full DES round including 'expansion function', 'sbox substitution'
 * and 'primitive function P' but without swapping the left and right word.
 * Please note: The data in 'from' and 'to' is already rotated one bit to
 * the left, done in the initial permutation.
 */
#define DES_ROUND(from, to, work, subkey)   \
    work = from ^ *subkey++;        \
    to ^= sbox8[  work      & 0x3f ];     \
    to ^= sbox6[ (work>>8)  & 0x3f ];     \
    to ^= sbox4[ (work>>16) & 0x3f ];     \
    to ^= sbox2[ (work>>24) & 0x3f ];     \
    work = ((from << 28) | (from >> 4)) ^ *subkey++;  \
    to ^= sbox7[  work      & 0x3f ];     \
    to ^= sbox5[ (work>>8)  & 0x3f ];     \
    to ^= sbox3[ (work>>16) & 0x3f ];     \
    to ^= sbox1[ (work>>24) & 0x3f ];

/*
 * Macros to convert 8 bytes from/to 32bit words.
 */
#define READ_64BIT_DATA(data, left, right)           \
    left  = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];  \
    right = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

#define WRITE_64BIT_DATA(data, left, right)          \
    data[0] = (left >> 24) &0xff; data[1] = (left >> 16) &0xff;      \
    data[2] = (left >> 8) &0xff; data[3] = left &0xff;         \
    data[4] = (right >> 24) &0xff; data[5] = (right >> 16) &0xff;    \
    data[6] = (right >> 8) &0xff; data[7] = right &0xff;

/*
 * des_sched():    Calculate 16 subkeys pairs (even/odd) for
 *        16 encryption rounds.
 *        To calculate subkeys for decryption the caller
 *        have to reorder the generated subkeys.
 *
 *    rawkey:     8 Bytes of key data
 *    subkey:     Array of at least 32 u32s. Will be filled
 *        with calculated subkeys.
 *
 */
static void des_sched ()
{
  static u32 left, right, work;
  static int round;
  static u32 *subkey = ctx.encrypt_subkeys;

  READ_64BIT_DATA (key, left, right)

  DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
  DO_PERMUTATION (right, work, left, 0, 0x10101010)

  left = ((leftkey_swap[(left >> 0) & 0xf] << 3)
          | (leftkey_swap[(left >> 8) & 0xf] << 2)
          | (leftkey_swap[(left >> 16) & 0xf] << 1)
          | (leftkey_swap[(left >> 24) & 0xf])
          | (leftkey_swap[(left >> 5) & 0xf] << 7)
          | (leftkey_swap[(left >> 13) & 0xf] << 6)
          | (leftkey_swap[(left >> 21) & 0xf] << 5)
          | (leftkey_swap[(left >> 29) & 0xf] << 4));

  left &= 0x0fffffff;

  right = ((rightkey_swap[(right >> 1) & 0xf] << 3)
           | (rightkey_swap[(right >> 9) & 0xf] << 2)
           | (rightkey_swap[(right >> 17) & 0xf] << 1)
           | (rightkey_swap[(right >> 25) & 0xf])
           | (rightkey_swap[(right >> 4) & 0xf] << 7)
           | (rightkey_swap[(right >> 12) & 0xf] << 6)
           | (rightkey_swap[(right >> 20) & 0xf] << 5)
           | (rightkey_swap[(right >> 28) & 0xf] << 4));

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)
    {
      left = ((left << encrypt_rotate_tab[round])
              | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
      right = ((right << encrypt_rotate_tab[round])
               | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

      *subkey++ = (((left << 4) & 0x24000000)
                   | ((left << 28) & 0x10000000)
                   | ((left << 14) & 0x08000000)
                   | ((left << 18) & 0x02080000)
                   | ((left << 6) & 0x01000000)
                   | ((left << 9) & 0x00200000)
                   | ((left >> 1) & 0x00100000)
                   | ((left << 10) & 0x00040000)
                   | ((left << 2) & 0x00020000)
                   | ((left >> 10) & 0x00010000)
                   | ((right >> 13) & 0x00002000)
                   | ((right >> 4) & 0x00001000)
                   | ((right << 6) & 0x00000800)
                   | ((right >> 1) & 0x00000400)
                   | ((right >> 14) & 0x00000200)
                   | (right & 0x00000100)
                   | ((right >> 5) & 0x00000020)
                   | ((right >> 10) & 0x00000010)
                   | ((right >> 3) & 0x00000008)
                   | ((right >> 18) & 0x00000004)
                   | ((right >> 26) & 0x00000002)
                   | ((right >> 24) & 0x00000001));

      *subkey++ = (((left << 15) & 0x20000000)
                   | ((left << 17) & 0x10000000)
                   | ((left << 10) & 0x08000000)
                   | ((left << 22) & 0x04000000)
                   | ((left >> 2) & 0x02000000)
                   | ((left << 1) & 0x01000000)
                   | ((left << 16) & 0x00200000)
                   | ((left << 11) & 0x00100000)
                   | ((left << 3) & 0x00080000)
                   | ((left >> 6) & 0x00040000)
                   | ((left << 15) & 0x00020000)
                   | ((left >> 4) & 0x00010000)
                   | ((right >> 2) & 0x00002000)
                   | ((right << 8) & 0x00001000)
                   | ((right >> 14) & 0x00000808)
                   | ((right >> 9) & 0x00000400)
                   | ((right) & 0x00000200)
                   | ((right << 7) & 0x00000100)
                   | ((right >> 7) & 0x00000020)
                   | ((right >> 3) & 0x00000011)
                   | ((right << 2) & 0x00000004)
                   | ((right >> 21) & 0x00000002));
    }
}


/*
 * Fill a DES context with subkeys calculated from a 64bit key.
 * Does not check parity bits, but simply ignore them.
 * Does not check for weak keys.
 */
static int des_setkey ()
{
  static int i;

  des_sched ();

  for(i=0; i<32; i+=2)
  {
    ctx.decrypt_subkeys[i] = ctx.encrypt_subkeys[30-i];
    ctx.decrypt_subkeys[i+1] = ctx.encrypt_subkeys[31-i];
  }

  return 0;
}

/*
 * Electronic Codebook Mode DES encryption/decryption of data according
 * to 'mode'.
 */
static int des_crypt ()
{
  static u32 left, right, work;
  static u32 *keys;

  keys = mode ? ctx.decrypt_subkeys : ctx.encrypt_subkeys;

  READ_64BIT_DATA (input, left, right)
  INITIAL_PERMUTATION (left, work, right)

  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)

  FINAL_PERMUTATION (right, work, left)
  WRITE_64BIT_DATA (result, right, left)

  return 0;
}

////////////////////////////////////////////////////////////
static void des_sched1 ()
{
  static u32 left, right, work;
  static int round;
  static u32 *subkey = ctx.encrypt_subkeys;

  READ_64BIT_DATA (key_shadow, left, right)

  DO_PERMUTATION (right, work, left, 4, 0x0f0f0f0f)
  DO_PERMUTATION (right, work, left, 0, 0x10101010)

  left = ((leftkey_swap[(left >> 0) & 0xf] << 3)
          | (leftkey_swap[(left >> 8) & 0xf] << 2)
          | (leftkey_swap[(left >> 16) & 0xf] << 1)
          | (leftkey_swap[(left >> 24) & 0xf])
          | (leftkey_swap[(left >> 5) & 0xf] << 7)
          | (leftkey_swap[(left >> 13) & 0xf] << 6)
          | (leftkey_swap[(left >> 21) & 0xf] << 5)
          | (leftkey_swap[(left >> 29) & 0xf] << 4));

  left &= 0x0fffffff;

  right = ((rightkey_swap[(right >> 1) & 0xf] << 3)
           | (rightkey_swap[(right >> 9) & 0xf] << 2)
           | (rightkey_swap[(right >> 17) & 0xf] << 1)
           | (rightkey_swap[(right >> 25) & 0xf])
           | (rightkey_swap[(right >> 4) & 0xf] << 7)
           | (rightkey_swap[(right >> 12) & 0xf] << 6)
           | (rightkey_swap[(right >> 20) & 0xf] << 5)
           | (rightkey_swap[(right >> 28) & 0xf] << 4));

  right &= 0x0fffffff;

  for (round = 0; round < 16; ++round)
    {
      left = ((left << encrypt_rotate_tab[round])
              | (left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
      right = ((right << encrypt_rotate_tab[round])
               | (right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;

      *subkey++ = (((left << 4) & 0x24000000)
                   | ((left << 28) & 0x10000000)
                   | ((left << 14) & 0x08000000)
                   | ((left << 18) & 0x02080000)
                   | ((left << 6) & 0x01000000)
                   | ((left << 9) & 0x00200000)
                   | ((left >> 1) & 0x00100000)
                   | ((left << 10) & 0x00040000)
                   | ((left << 2) & 0x00020000)
                   | ((left >> 10) & 0x00010000)
                   | ((right >> 13) & 0x00002000)
                   | ((right >> 4) & 0x00001000)
                   | ((right << 6) & 0x00000800)
                   | ((right >> 1) & 0x00000400)
                   | ((right >> 14) & 0x00000200)
                   | (right & 0x00000100)
                   | ((right >> 5) & 0x00000020)
                   | ((right >> 10) & 0x00000010)
                   | ((right >> 3) & 0x00000008)
                   | ((right >> 18) & 0x00000004)
                   | ((right >> 26) & 0x00000002)
                   | ((right >> 24) & 0x00000001));

      *subkey++ = (((left << 15) & 0x20000000)
                   | ((left << 17) & 0x10000000)
                   | ((left << 10) & 0x08000000)
                   | ((left << 22) & 0x04000000)
                   | ((left >> 2) & 0x02000000)
                   | ((left << 1) & 0x01000000)
                   | ((left << 16) & 0x00200000)
                   | ((left << 11) & 0x00100000)
                   | ((left << 3) & 0x00080000)
                   | ((left >> 6) & 0x00040000)
                   | ((left << 15) & 0x00020000)
                   | ((left >> 4) & 0x00010000)
                   | ((right >> 2) & 0x00002000)
                   | ((right << 8) & 0x00001000)
                   | ((right >> 14) & 0x00000808)
                   | ((right >> 9) & 0x00000400)
                   | ((right) & 0x00000200)
                   | ((right << 7) & 0x00000100)
                   | ((right >> 7) & 0x00000020)
                   | ((right >> 3) & 0x00000011)
                   | ((right << 2) & 0x00000004)
                   | ((right >> 21) & 0x00000002));
    }
}

static int des_crypt1 ()
{
  static u32 left, right, work;
  static u32 *keys;

  keys = mode ? ctx.decrypt_subkeys : ctx.encrypt_subkeys;

  READ_64BIT_DATA (input, left, right)
  INITIAL_PERMUTATION (left, work, right)

  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)
  DES_ROUND (right, left, work, keys) 
  DES_ROUND (left, right, work, keys)

  FINAL_PERMUTATION (right, work, left)
  WRITE_64BIT_DATA (result, right, left)

  return 0;
}

static int des_setkey1 ()
{
  static int i;

  des_sched1 ();

  for(i=0; i<32; i+=2)
  {
    ctx.decrypt_subkeys[i] = ctx.encrypt_subkeys[30-i];
    ctx.decrypt_subkeys[i+1] = ctx.encrypt_subkeys[31-i];
  }

  return 0;
}

////////////////////////////////////////////////////////////


void thread1(){
  /*
    * DES Test
    */
  des_setkey ();
  des_crypt ();
}

int64_t dummy;
void thread2(){
    dummy = 0x1111dddfffb1353c;
}

void klee_make_symbolic(void *addr, size_t nbytes, const char *name) __attribute__((weak));
void klee_assume(uintptr_t condition) __attribute__((weak));
void klee_task_boundary() __attribute__((weak));


/*
 * Performs a selftest of this DES implementation.
 * Returns NULL if all is ok.
 */
int main(void)
{
  klee_make_symbolic(key, sizeof(key), "key");
  pthread_t t1, t2;
  pthread_create(&t1, 0, (void*)thread1, 0);
  pthread_create(&t2, 0, (void*)thread2, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);

  #define shadow
  #ifdef shadow
  klee_make_symbolic(key_shadow, sizeof(key_shadow), "key_shadow");
  klee_task_boundary();
  des_setkey1();
  des_crypt1 ();
  #endif

  return 0;
}
