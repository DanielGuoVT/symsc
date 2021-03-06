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
 #include <stdio.h>
 #include <klee/klee.h>
 #include <pthread.h>

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
#define ROUND_KEYS_SIZE 256

#define NUMBER_OF_ROUNDS 10 

/*
 *
 * Cipher constants
 *
 */
uint8_t sBox4[] = {0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd, 0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2};
uint8_t invsBox4[] = {0x5, 0xe, 0xf, 0x8, 0xC, 0x1, 0x2, 0xD, 0xB, 0x4, 0x6, 0x3, 0x0, 0x7, 0x9, 0xA};

uint8_t block[BLOCK_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t key[KEY_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t roundKeys[ROUND_KEYS_SIZE];

uint8_t key_shadow[KEY_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* input rotated left (4x) */
#define rotate4l_64(r4lin) ( high4_64(r4lin) | ( r4lin << 4 ) )

/* 4 msb as lsb */
#define high4_64(h4in) ( (uint64_t)h4in >> 60 )

#define READ_SBOX_BYTE(x) x
#define READ_ROUND_KEY_DOUBLE_WORD(x) x

void Schedule()
{
  /* 
   *  The following instructions are failing on ARMv7-M using the 
   * arm-none-eabi-g++ (Sourcery CodeBench Lite 2014.05-28) 4.8.3 20140320 
   * (prerelease) compiler because of the optimizer is grouping the 2 memory 
   * accesses in one LDRD instruction. However the 2 memory addresses are not 
   * aligned on 64-bit boundaries and the instruction causes an UNALIGN_TRAP 
   * (which can not be disabled for LDRD instruction):
   *    uint64_t keylow = *(uint64_t *)key;
   *    uint64_t keyhigh = *(uint64_t*)(key + 2);
   *  
   *  The next 3 lines replace the wrong instruction sequence:
   *    uint64_t keylow = *(uint64_t *)key;
   *    uint16_t highBytes = *(uint16_t *)(key + 8);
   *    uint64_t keyhigh = ((uint64_t)(highBytes) << 48) | (keylow >> 16);
   *
   */
  static uint64_t keylow;
  static uint16_t highBytes;
  static uint64_t keyhigh;

  static uint64_t temp;
  static uint8_t round;
  
  keylow = *(uint64_t *)key;
  highBytes = *(uint16_t *)(key + 8);
  keyhigh = ((uint64_t)(highBytes) << 48) | (keylow >> 16);

  for (round = 0; round < NUMBER_OF_ROUNDS; round++)
  {
    /* 61-bit left shift */
    ((uint64_t*)roundKeys)[round] = keyhigh;
    temp = keyhigh;
    keyhigh <<= 61;
    keyhigh |= (keylow << 45);
    keyhigh |= (temp >> 19);
    keylow = (temp >> 3) & 0xFFFF;

    /* S-Box application */
    temp = keyhigh >> 60;
    keyhigh &= 0x0FFFFFFFFFFFFFFF;
    temp = READ_SBOX_BYTE(sBox4[temp]);
    keyhigh |= temp << 60;

    /* round counter addition */
    keylow ^= (((uint64_t)(round + 1) & 0x01) << 15);
    keyhigh ^= ((uint64_t)(round + 1) >> 1);
  }
}

void Encrypt()
{
  static uint64_t state;
  static uint64_t temp;
  static uint8_t round;
  static uint8_t k;

  state = *(uint64_t*)block;

  for (round = 0; round < NUMBER_OF_ROUNDS; round++)
  {
    /* addRoundkey */
    static uint32_t subkey_lo;
    static uint32_t subkey_hi;
  
    subkey_lo = READ_ROUND_KEY_DOUBLE_WORD(((uint32_t*)roundKeys)[2 * round]);
    subkey_hi = READ_ROUND_KEY_DOUBLE_WORD(((uint32_t*)roundKeys)[2 * round + 1]);
    
    state ^= (uint64_t)subkey_lo ^ (((uint64_t)subkey_hi) << 32);
    
    /* sBoxLayer */
    for (k = 0; k < 16; k++)
    {
      /* get lowest nibble */
      // uint16_t sBoxValue = state & 0xF;

      /* kill lowest nibble */
      state &= 0xFFFFFFFFFFFFFFF0; 

      /* put new value to lowest nibble (sBox) */
      state |= READ_SBOX_BYTE(sBox4[state & 0xF]);

      /* next(rotate by one nibble) */
      state = rotate4l_64(state); 
    }
    

    /* pLayer */
    temp = 0;
    for (k = 0; k < 64; k++)
    {
      /* arithmentic calculation of the p-Layer */
      // uint16_t position = (16 * k) % 63;

      //  exception for bit 63 
      // if (k == 63)
      // {
      //   position = 63;
      // }

      /* result writing */
      // temp |= ((state >> k) & 0x1) << position; 
      temp |= ((state >> k) & 0x1) << (k == 63 ? 63 : ((16*k)%63));
    }
    state = temp;
  }


  /* addRoundkey (Round 31) */
  static uint32_t subkey_lo1;
  static uint32_t subkey_hi1;
  
  subkey_lo1 = READ_ROUND_KEY_DOUBLE_WORD(((uint32_t*)roundKeys)[62]);
  subkey_hi1 = READ_ROUND_KEY_DOUBLE_WORD(((uint32_t*)roundKeys)[63]);

  state ^= (uint64_t)subkey_lo1 ^ (((uint64_t)subkey_hi1) << 32);

  
  *(uint64_t*)block = state;
}

void thread1(){
  Schedule();
  Encrypt();
}

int64_t dummy;
void thread2(){
  dummy = 0x1a71ca45c0d01d01;
}

void klee_make_symbolic(void *addr, size_t nbytes, const char *name) __attribute__((weak));
void klee_assume(uintptr_t condition) __attribute__((weak));
void klee_task_boundary() __attribute__((weak));

int main()
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
  Schedule();
  Encrypt();
  #endif 

  return 0;
}
