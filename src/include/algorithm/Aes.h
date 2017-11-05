/* Aes.h -- AES encryption / decryption
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __AES_H
#define __AES_H

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE 16

typedef struct
{
  unsigned numRounds2; /* = numRounds / 2 */
  uint32_t rkey[(14 + 1) * 4];
} CAes;

/* Call AesGenTables one time before other AES functions */
void AesGenTables(void);

/* keySize = 16 or 24 or 32 (bytes) */
void Aes_SetKeyEncode(CAes *p, const byte_t *key, unsigned keySize);
void Aes_SetKeyDecode(CAes *p, const byte_t *key, unsigned keySize);

/* Aes_Encode32 and Aes_Decode32 functions work with little-endian words.
  src and dest are pointers to 4 uint32_t words.
  arc and dest can point to same block */
void Aes_Encode32(const CAes *p, uint32_t *dest, const uint32_t *src);
void Aes_Decode32(const CAes *p, uint32_t *dest, const uint32_t *src);

typedef struct
{
  uint32_t prev[4];
  CAes aes;
} CAesCbc;

void AesCbc_Init(CAesCbc *p, const byte_t *iv); /* iv size is AES_BLOCK_SIZE */

/* AesCbc_Encode and AesCbc_Decode:
  if (res <= size): Filter have converted res bytes
  if (res > size):  Filter have not converted anything. And it needs at
                    least res = AES_BLOCK_SIZE bytes to convert one block */

size_t AesCbc_Encode(CAesCbc *p, byte_t *data, size_t size);
size_t AesCbc_Decode(CAesCbc *p, byte_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
