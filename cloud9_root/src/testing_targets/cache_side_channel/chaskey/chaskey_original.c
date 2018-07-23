/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Daniel Dinu <dumitru-daniel.dinu@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/*
 *
 * Cipher characteristics:
 *      BLOCK_SIZE - the cipher block size in bytes
 *      KEY_SIZE - the cipher key size in bytes
 *      ROUND_KEY_SIZE - the cipher round keys size in bytes
 *      NUMBER_OF_ROUNDS - the cipher number of rounds
 *
 */
#define BLOCK_SIZE 16
#define KEY_SIZE 16
#define ROUND_KEYS_SIZE 16
#define NUMBER_OF_ROUNDS 8

uint8_t key[KEY_SIZE];
uint8_t roundKeys[ROUND_KEYS_SIZE];
uint8_t block[BLOCK_SIZE];

/*
 *
 * Run the encryption key schedule
 * ... key - the cipher key
 * ... roundKeys - the encryption round keys
 *
 */
void RunEncryptionKeySchedule();

/*
 *
 * Run the decryption key schedule
 * ... key - the cipher key
 * ... roundKeys - the decryption round keys
 *
 */
void RunDecryptionKeySchedule(uint8_t *key, uint8_t *roundKeys);

/*
 *
 * Encrypt the given block using the given round keys
 * ... block - the block to encrypt
 * ... roundKeys - the round keys to be used during encryption
 *
 */
void Encrypt();

/*
 *
 * Decrypt the given block using the given round keys
 * ... block - the block to decrypt
 * ... roundKeys - the round keys to be used during decryption
 *
 */
void Decrypt();

/****************************************************************************** 
 *
 * common macro and inline functions for simon cipher
 *
 ******************************************************************************/

// static inline uint32_t rol(uint32_t x, const uint8_t n)
// {
//     return (x << n) | (x >> (32 - n));
// }

#define rol(x, n) (x << n) | (x >> (32 - n))

// static inline uint32_t ror(uint32_t x, const uint8_t n)
// {
//     return (x >> n) | (x << (32 - n));
// }
#define ror(x, n) (x >> n) | (x << (32 - n))

void chaskey_encrypt()
{
  static uint32_t *v = (uint32_t *)block;
  static uint32_t *k = (uint32_t *)roundKeys;
  static uint8_t i;
  
  /* Whitening */
  v[0] ^= (k[0]); 
  v[1] ^= (k[1]); 
  v[2] ^= (k[2]); 
  v[3] ^= (k[3]);
  
  /* Chaskey permutation */
  for (i = 0; i < NUMBER_OF_ROUNDS; ++i)
  {
    v[0] += v[1]; 
    v[1] = rol(v[1], 5); 
    // v[1] = (v1 << 5)|(v[1] >> (32 - 5))); 
    v[1] ^= v[0]; 
    v[0] = rol(v[0],16);
    v[2] += v[3]; 
    v[3] = rol(v[3], 8); 
    v[3] ^= v[2];
    v[0] += v[3]; 
    v[3] = rol(v[3],13); 
    v[3] ^= v[0];
    v[2] += v[1]; 
    v[1] = rol(v[1], 7); 
    v[1] ^= v[2]; 
    v[2] = rol(v[2],16);
  }
  
  /* Whitening */
  v[0] ^= (k[0]); 
  v[1] ^= (k[1]); 
  v[2] ^= (k[2]); 
  v[3] ^= (k[3]);
}

void chaskey_decrypt()
{
  static uint32_t *v = (uint32_t *)block;
  static uint32_t *k = (uint32_t *)roundKeys;
  static uint8_t i;

  /* Whitening */
  v[0] ^= (k[0]); 
  v[1] ^= (k[1]); 
  v[2] ^= (k[2]); 
  v[3] ^= (k[3]);
  
  /* Chaskey permutation */
  for (i = 0; i < NUMBER_OF_ROUNDS; ++i)
  {
    v[2] = ror(v[2],16); 
    v[1] ^= v[2]; 
    v[1] = ror(v[1], 7); 
    v[2] -= v[1];
    v[3] ^= v[0]; 
    v[3] = ror(v[3],13); 
    v[0] -= v[3];
    v[3] ^= v[2]; 
    v[3] = ror(v[3], 8); 
    v[2] -= v[3];
    v[0] = ror(v[0],16); 
    v[1] ^= v[0]; 
    v[1] = ror(v[1], 5); 
    v[0] -= v[1];
  }
  
  /* Whitening */
  v[0] ^= (k[0]); 
  v[1] ^= (k[1]); 
  v[2] ^= (k[2]); 
  v[3] ^= (k[3]);
}

void RunEncryptionKeySchedule()
{
  memcpy(roundKeys, key, KEY_SIZE);
}

int main()
{
    int i=0;
    FILE *keyFile;
    FILE *inFile;

    keyFile = fopen("key.txt", "r");
    if(!keyFile)
    {
        printf("Key file not found!\n");
        return -1;
    }

    int key_len =0;
    fscanf(keyFile, "%d", &key_len);
    if(key_len > 64 | key_len <=0)
    {
        printf("Key len Invalid!\n");
        return -1;
    }
    if(key_len > 16)
    {
        key_len = 16;
        printf("Warning: Only the first 16 bytes key is used!\n");
    }

    while (fscanf(keyFile, "%s", &key[i]) != EOF && i<key_len)
    {
            ++i;
    }
    fclose(keyFile);


    inFile = fopen("in.txt", "r");
    if(!inFile)
    {
        printf("Input file not found!\n");
        return -1;
    }

    int in_len =0;
    fscanf(inFile, "%d", &in_len);
    if(in_len > 16)
    {
        in_len = 16;
        printf("Warning: Only the first 8 bytes input is used!\n");
    }

    i =0;
    while (fscanf(inFile, "%s", &block[i]) != EOF && i< in_len)
    {
        ++i;
    }
    fclose(inFile);

    RunEncryptionKeySchedule();
    chaskey_encrypt();
    chaskey_decrypt();

    return 0;
}
