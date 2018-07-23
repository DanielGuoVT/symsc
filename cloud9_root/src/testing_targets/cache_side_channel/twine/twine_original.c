/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Dmitry Khovratovich <dmitry.khovratovich@uni.lu>
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

/*
 *
 * Cipher characteristics:
 *  BLOCK_SIZE - the cipher block size in bytes
 *  KEY_SIZE - the cipher key size in bytes
 *  ROUND_KEY_SIZE - the cipher round keys size in bytes
 *  NUMBER_OF_ROUNDS - the cipher number of rounds
 *
 */
#define BLOCK_SIZE 8

#define KEY_SIZE 10
#define ROUND_KEYS_SIZE 288

#define NUMBER_OF_ROUNDS 35

#define READ_SBOX_BYTE(x) x
#define READ_ROUND_KEY_BYTE(x) x
#define READ_KS_BYTE(x) x

uint8_t Sbox_byte[16] = 
  { 
    0x0c, 0x00, 0x0f, 0x0a,
    0x02, 0x0b, 0x09, 0x05,
    0x08, 0x03, 0x0d, 0x07,
    0x01, 0x0e, 0x06, 0x04 
  };
  
uint8_t Pi_byte[16] = 
  { 
    0x05, 0x00, 0x01, 0x04,
    0x07, 0x0c, 0x03, 0x08,
    0x0d, 0x06, 0x09, 0x02,
    0x0f, 0x0a, 0x0b, 0x0e 
  };
  
uint8_t RCON[35] = 
  { 
    0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x03, 0x06,
    0x0c, 0x18, 0x30, 0x23,
    0x05, 0x0a, 0x14, 0x28,
    0x13, 0x26, 0x0f, 0x1e,
    0x3c, 0x3b, 0x35, 0x29,
    0x11, 0x22, 0x07, 0x0e,
    0x1c, 0x38, 0x33, 0x25,
    0x09, 0x12, 0x24
  };

uint8_t key[KEY_SIZE];
uint8_t roundKeys[ROUND_KEYS_SIZE];
uint8_t block[BLOCK_SIZE] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe};

void Encrypt()
{
  /* set of nibbles */
  // static uint8_t state[16];

  // static uint8_t t[16];
  static uint8_t i;
  static uint8_t j;
  static uint8_t r;
  static uint8_t tmp;

  for (r = 0; r < NUMBER_OF_ROUNDS; r++)
  {
    for (i = 0; i < 8; ++i)
    {
      block[i] ^= 
              Sbox_byte[block[i] & 0x0F ^ 0x0F & (roundKeys[r * 4 + i / 2]) >> (4 * (i % 2))] << 4;
    }
    /* Output */
    tmp = block[0];
    
    /*0 <-1 */
    block[0] &= 0xF0;
    block[0] ^= block[0] >> 4;

    /*1 <-2 */
    block[0] &= 0x0F;
    block[0] ^= block[1] << 4;

    /* 2 <-11 */
    block[1] &= 0xF0;
    block[1] ^= block[5] >>4;

    /* 11 <-14 */
    block[5] &= 0x0F;
    block[5] ^= block[7] << 4;

    /* 14 <-15 */
    block[7] &= 0xF0;
    block[7] ^= block[7] >> 4;

    /* 15 <-12 */
    block[7] &= 0x0F;
    block[7] ^= block[6] << 4;

    /* 12 <-5 */
    block[6] &= 0xF0;
    block[6] ^= block[2] >> 4;

    /* 5 <-0 */
    block[2] &= 0x0F;
    block[2] ^=tmp << 4;

    tmp = block[1];

    /*3 <-6 */
    block[1] &= 0x0F;
    block[1] ^= block[3] << 4;

    /*6 <-9 */
    block[3] &= 0xF0; 
    block[3] ^= block[4] >> 4;

    /*9 <-10 */
    block[4] &= 0x0F;
    block[4] ^= block[5] << 4;

    /* 10 <-13 */
    block[5] &= 0xF0;
    block[5] ^= block[6] >> 4; 

    /* 13 <-8 */
    block[6] &= 0x0F;
    block[6] ^= block[4] << 4;

    /* 8 <-7 */
    block[4] &= 0xF0;
    block[4] ^= block[3] >> 4;

    /* 7 <-4 */
    block[3] &= 0x0F;
    block[3] ^= block[2] << 4;

    /* 4 <-3 */
    block[2] &= 0xF0;
    block[2] ^= tmp >> 4;
  }
}

void Schedule()
{
  static uint8_t KeyR[20];
  static uint8_t temp;
  static uint8_t temp1;
  static uint8_t temp2;
  static uint8_t temp3;
  static uint8_t i;
  
  static uint16_t *master_key = (uint16_t*)key;

  
  for (i = 0; i < 20; i++)
  {
    KeyR[i] = (master_key[(i / 4)] >> (4 * (i & 0x03))) & 0x0F;
  }

  for (i = 0; i < NUMBER_OF_ROUNDS; i++)
  {
    roundKeys[8 * i + 0] = KeyR[1];
    roundKeys[8 * i + 1] = KeyR[3];
    roundKeys[8 * i + 2] = KeyR[4];
    roundKeys[8 * i + 3] = KeyR[6];
    roundKeys[8 * i + 4] = KeyR[13];
    roundKeys[8 * i + 5] = KeyR[14];
    roundKeys[8 * i + 6] = KeyR[15];
    roundKeys[8 * i + 7] = KeyR[16];

    KeyR[1] = KeyR[1] ^ READ_SBOX_BYTE(Sbox_byte[KeyR[0]]);
    KeyR[4] = KeyR[4] ^ READ_SBOX_BYTE(Sbox_byte[KeyR[16]]);
    KeyR[7] = KeyR[7] ^ (READ_KS_BYTE(RCON[i]) >> 3);
    KeyR[19] = KeyR[19] ^ (READ_KS_BYTE(RCON[i]) & 0x07);

    temp = KeyR[0];
    KeyR[0] = KeyR[1];
    KeyR[1] = KeyR[2];
    KeyR[2] = KeyR[3];
    KeyR[3] = temp;

    temp = KeyR[0];
    temp1 = KeyR[1];
    temp2 = KeyR[2];
    temp3 = KeyR[3];

    KeyR[0] = KeyR[4];
    KeyR[1] = KeyR[5];
    KeyR[2] = KeyR[6];
    KeyR[3] = KeyR[7];

    KeyR[4] = KeyR[8];
    KeyR[5] = KeyR[9];
    KeyR[6] = KeyR[10];
    KeyR[7] = KeyR[11];

    KeyR[8] = KeyR[12];
    KeyR[9] = KeyR[13];
    KeyR[10] = KeyR[14];
    KeyR[11] = KeyR[15];

    KeyR[12] = KeyR[16];
    KeyR[13] = KeyR[17];
    KeyR[14] = KeyR[18];
    KeyR[15] = KeyR[19];

    KeyR[16] = temp;
    KeyR[17] = temp1;
    KeyR[18] = temp2;
    KeyR[19] = temp3;
  }

  roundKeys[280] = KeyR[1];
  roundKeys[281] = KeyR[3];
  roundKeys[282] = KeyR[4];
  roundKeys[283] = KeyR[6];
  roundKeys[284] = KeyR[13];
  roundKeys[285] = KeyR[14];
  roundKeys[286] = KeyR[15];
  roundKeys[287] = KeyR[16];
}

int main()
{
  Schedule();
  Encrypt();
}
