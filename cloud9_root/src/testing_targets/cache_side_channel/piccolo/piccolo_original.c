/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Yann Le Corre <yann.lecorre@uni.lu>
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

/****************************************************************************** 
 *
 * Piccolo common functions
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
// #include <klee/klee.h>

#define NUMBER_OF_ROUNDS 25 
/* SBOX */
uint8_t SBOX[] =
{
    0x0e, 0x04, 0x0b, 0x02,
    0x03, 0x08, 0x00, 0x09,
    0x01, 0x0a, 0x07, 0x0f,
    0x06, 0x0c, 0x05, 0x0d
};

/* GF[2^4] multiplication by 2 */
uint8_t GF16_MUL2[] =
{
	0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e,
	0x03, 0x01, 0x07, 0x05, 0x0b, 0x09, 0x0f, 0x0d
};

/* GF[2^4] multiplication by 3 */
uint8_t GF16_MUL3[] =
{
	0x00, 0x03, 0x06, 0x05, 0x0c, 0x0f, 0x0a, 0x09,
	0x0b, 0x08, 0x0d, 0x0e, 0x07, 0x04, 0x01, 0x02
};

uint32_t CON80[] =
{
    0x293d071c,
    0x253e1f1a,
    0x213f1718,
    0x3d382f16,
    0x39392714,
    0x353a3f12,
    0x313b3710,
    0x0d344f0e,
    0x0935470c,
    0x05365f0a,
    0x01375708,
    0x1d306f06,
    0x19316704,
    0x15327f02,
    0x11337700,
    0x6d2c8f3e,
    0x692d873c,
    0x652e9f3a,
    0x612f9738,
    0x7d28af36,
    0x7929a734,
    0x752abf32,
    0x712bb730,
    0x4d24cf2e,
    0x4925c72c
};

uint8_t block[16] = {0};
uint8_t in_key[12] = {0};
uint8_t roundKeys[108];

/* calculate p0 + p1 + 2*p2 + 3*p3 in GF[2^4] with caract. poly = x^4 + x + 1 */
inline uint8_t polyEval(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3) __attribute__((always_inline))
{
	/* uint8_t y = p0 ^ p1 ^ gf16_mul2(p2) ^ gf16_mul3(p3); */
	uint8_t y = p0 ^ p1 ^ (GF16_MUL2[p2]) ^ (GF16_MUL3[p3]);
	
	return y;
}

uint8_t x00 = 0;
uint8_t x01 = 0;
uint8_t x02 = 0;
uint8_t x03 = 0;

uint8_t y00 = 0;
uint8_t y01 = 0;
uint8_t y02 = 0;
uint8_t y03 = 0;

uint16_t *x3 = 0;
uint16_t *x2 = 0;
uint16_t *x1 = 0;
uint16_t *x0 = 0;

inline uint16_t F0() __attribute__((always_inline))
{
    x03 = (*x0 >>  0) & 0x0f;
    x02 = (*x0 >>  4) & 0x0f;
    x01 = (*x0 >>  8) & 0x0f;
    x00 = (*x0 >> 12) & 0x0f;

    x03 = (SBOX[x03]);
    x02 = (SBOX[x02]);
    x01 = (SBOX[x01]);
    x00 = (SBOX[x00]);

	y00 = x02 ^ x03 ^ (GF16_MUL2[x00]) ^ (GF16_MUL3[x01]); 
	y01 = x03 ^ x00 ^ (GF16_MUL2[x01]) ^ (GF16_MUL3[x02]); 
	y02 = x00 ^ x01 ^ (GF16_MUL2[x02]) ^ (GF16_MUL3[x03]); 
	y03 = x01 ^ x02 ^ (GF16_MUL2[x03]) ^ (GF16_MUL3[x00]); 

    // y00 = polyEval(x02, x03, x00, x01);
    // y01 = polyEval(x03, x00, x01, x02);
    // y02 = polyEval(x00, x01, x02, x03);
    // y03 = polyEval(x01, x02, x03, x00);
    y00 = (SBOX[y00]);
    y01 = (SBOX[y01]);
    y02 = (SBOX[y02]);
    y03 = (SBOX[y03]);

	return (y00 << 12) | (y01 << 8) | (y02 << 4) | y03;
}

inline uint16_t F2() __attribute__((always_inline))
{

    x03 = (*x2 >>  0) & 0x0f;
    x02 = (*x2 >>  4) & 0x0f;
    x01 = (*x2 >>  8) & 0x0f;
    x00 = (*x2 >> 12) & 0x0f;

    x03 = (SBOX[x03]);
    x02 = (SBOX[x02]);
    x01 = (SBOX[x01]);
    x00 = (SBOX[x00]);

	y00 = x02 ^ x03 ^ (GF16_MUL2[x00]) ^ (GF16_MUL3[x01]); 
	y01 = x03 ^ x00 ^ (GF16_MUL2[x01]) ^ (GF16_MUL3[x02]); 
	y02 = x00 ^ x01 ^ (GF16_MUL2[x02]) ^ (GF16_MUL3[x03]); 
	y03 = x01 ^ x02 ^ (GF16_MUL2[x03]) ^ (GF16_MUL3[x00]); 

    // y00 = polyEval(x02, x03, x00, x01);
    // y01 = polyEval(x03, x00, x01, x02);
    // y02 = polyEval(x00, x01, x02, x03);
    // y03 = polyEval(x01, x02, x03, x00);
    y00 = (SBOX[y00]);
    y01 = (SBOX[y01]);
    y02 = (SBOX[y02]);
    y03 = (SBOX[y03]);

	return (y00 << 12) | (y01 << 8) | (y02 << 4) | y03;
}

uint16_t y10 = 0;
uint16_t y11 = 0;
uint16_t y12 = 0;
uint16_t y13 = 0;

inline void RP() __attribute__((always_inline))
{
    y10 = (*x1 & 0xff00) | (*x3 & 0x00ff);
    y11 = (*x2 & 0xff00) | (*x0 & 0x00ff);
    y12 = (*x3 & 0xff00) | (*x1 & 0x00ff);
    y13 = (*x0 & 0xff00) | (*x2 & 0x00ff);
	
    *x0 = y10;
    *x1 = y11;
    *x2 = y12;
    *x3 = y13;
}

uint8_t i1 = 0;
uint16_t *rk = 0;

void Piccolo_encrypt()
{
    // uint8_t i;
    // uint16_t *x3 = (uint16_t *)block;
    // uint16_t *x2 = x3 + 1;
    // uint16_t *x1 = x3 + 2;
    // uint16_t *x0 = x3 + 3;
    // uint16_t *rk = (uint16_t *)roundKeys;

	x3 = (uint16_t *)block;
	x2 = x3 + 1;
	x1 = x3 + 2;
	x0 = x3 + 3;
	rk = (uint16_t *)roundKeys;

    *x2 ^= (rk[51]);
    *x0 ^= (rk[50]);

    #pragma unroll
    for (i1 = 0; i1 < NUMBER_OF_ROUNDS - 1; ++i1)
    {
        *x1 = *x1 ^ F0() ^ (rk[2 * i1]);
        *x3 = *x3 ^ F2() ^ (rk[2 * i1 + 1]);
        RP();
    }

    *x1 = *x1 ^ F0() ^ (rk[2*NUMBER_OF_ROUNDS - 2]);
    *x3 = *x3 ^ F2() ^ (rk[2*NUMBER_OF_ROUNDS - 1]);
    *x0 ^= (rk[52]);
    *x2 ^= (rk[53]);
}


uint8_t i = 0;
uint8_t m = 0;
uint16_t *mk = 0;
uint32_t _rk = 0;
uint16_t *wk = 0;

void RunEncryptionKeySchedule()
{
    // uint8_t i;
    // uint8_t m;
    // uint16_t *mk = (uint16_t *)key;
    // uint32_t _rk;
    // uint16_t *rk = (uint16_t *)roundKeys;
    // uint16_t *wk = (uint16_t *)(&roundKeys[100]);

    mk = (uint16_t *)in_key;
    rk = (uint16_t *)roundKeys;
    wk = (uint16_t *)(&roundKeys[100]);

    wk[0] = (mk[0] & 0xff00) | (mk[1] & 0x00ff);
    wk[1] = (mk[1] & 0xff00) | (mk[0] & 0x00ff);
    wk[2] = (mk[4] & 0xff00) | (mk[3] & 0x00ff);
    wk[3] = (mk[3] & 0xff00) | (mk[4] & 0x00ff);

    m = 0;
    for (i = 0; i < NUMBER_OF_ROUNDS; ++i)
    {
        _rk = CON80[i];
        switch (m)
        {
            case 0:
            case 2:
                _rk ^= *(uint32_t *)(&mk[2]);
                break;
            case 3:
                _rk ^= ((uint32_t)(mk[4]) << 16) | (uint32_t)(mk[4]);
                break;
            case 1:
            case 4:
                _rk ^= *(uint32_t *)(&mk[0]);
                break;
        }
        *(uint32_t *)&rk[2*i] = _rk;
        if (m == 4)
        {
            m = 0;
        }
        else
        {
            m++;
        }
    }
}

// void klee_make_symbolic(void *addr, size_t nbytes, const char *name) __attribute__((weak));
// void klee_assume(uintptr_t condition) __attribute__((weak));

int main(int argc, char *argv[])
{
  	// klee_make_symbolic(&in, sizeof(in), "in");
  	// klee_make_symbolic(&in_key, sizeof(in_key), "in_key");
  	// RunEncryptionKeySchedule(in_key, roundKeys);

// uint16_t *mk = 0;
// uint32_t _rk = 0;
// uint16_t *wk = 0;
  	RunEncryptionKeySchedule();
	Piccolo_encrypt();
}

// int main(int argc, char *argv[])
// {
//     int i=0;
//     FILE *keyFile;
//     FILE *inFile;

//     keyFile = fopen("key.txt", "r");
//     if(!keyFile)
//     {
//         printf("Key file not found!\n");
//         return -1;
//     }

//     int key_len =0;
//     fscanf(keyFile, "%d", &key_len);
//     if(key_len > 64 | key_len <=0)
//     {
//         printf("Key len Invalid!\n");
//         return -1;
//     }
//     if(key_len > 8)
//     {
//         key_len = 8;
//         printf("Warning: Only the first 8 bytes key is used!\n");
//     }

//     while (fscanf(keyFile, "%x", &in_key[i]) != EOF && i<key_len)
// 	{
//             ++i;
//     }
//     fclose(keyFile);


//     inFile = fopen("in.txt", "r");
//     if(!inFile)
//     {
//         printf("Input file not found!\n");
//         return -1;
//     }

//     int in_len =0;
//     fscanf(inFile, "%d", &in_len);
//     if(in_len > 8)
//     {
//         in_len = 8;
//         printf("Warning: Only the first 8 bytes input is used!\n");
//     }

//     i =0;
//     while (fscanf(inFile, "%x", &in[i]) != EOF && i< in_len)
// 	{
//         ++i;
//     }
//     fclose(inFile);

//     RunEncryptionKeySchedule(in_key, roundKeys);

// #ifdef gem5            
//     m5_reset_stats(0, 0);
// #endif

//     Piccolo_encrypt(roundKeys, in);

// #ifdef gem5
//     m5_dumpreset_stats(0, 0);
// #endif  

//     return 0;
// }
