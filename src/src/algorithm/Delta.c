/* Delta.c -- Delta converter
2009-05-26 : Igor Pavlov : Public domain */

#include "algorithm/Delta.h"

void Delta_Init(byte_t *state)
{
    unsigned i;
    for (i = 0; i < DELTA_STATE_SIZE; i++)
        state[i] = 0;
}

static void MyMemCpy(byte_t *dest, const byte_t *src, unsigned size)
{
    unsigned i;
    for (i = 0; i < size; i++)
        dest[i] = src[i];
}

void Delta_Encode(byte_t *state, unsigned delta, byte_t *data, size_t size)
{
    byte_t buf[DELTA_STATE_SIZE];
    unsigned j = 0;
    MyMemCpy(buf, state, delta);
    {
        size_t i;
        for (i = 0; i < size;)
        {
            for (j = 0; j < delta && i < size; i++, j++)
            {
                byte_t b = data[i];
                data[i] = (byte_t)(b - buf[j]);
                buf[j] = b;
            }
        }
    }
    if (j == delta)
        j = 0;
    MyMemCpy(state, buf + j, delta - j);
    MyMemCpy(state + delta - j, buf, j);
}

void Delta_Decode(byte_t *state, unsigned delta, byte_t *data, size_t size)
{
    byte_t buf[DELTA_STATE_SIZE];
    unsigned j = 0;
    MyMemCpy(buf, state, delta);
    {
        size_t i;
        for (i = 0; i < size;)
        {
            for (j = 0; j < delta && i < size; i++, j++)
            {
                buf[j] = data[i] = (byte_t)(buf[j] + data[i]);
            }
        }
    }
    if (j == delta)
        j = 0;
    MyMemCpy(state, buf + j, delta - j);
    MyMemCpy(state + delta - j, buf, j);
}
