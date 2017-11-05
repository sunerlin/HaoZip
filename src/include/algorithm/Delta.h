/* Delta.h -- Delta converter
2009-04-15 : Igor Pavlov : Public domain */

#ifndef __DELTA_H
#define __DELTA_H

#include "algorithm/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DELTA_STATE_SIZE 256

void Delta_Init(byte_t *state);
void Delta_Encode(byte_t *state, unsigned delta, byte_t *data, size_t size);
void Delta_Decode(byte_t *state, unsigned delta, byte_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif
