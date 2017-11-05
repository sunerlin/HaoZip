/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCSha1Defs_h_
#define __RCSha1Defs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Sha1 常量定义
*/
class RCSha1Defs
{
public:
    enum
    {
        kBlockSize = 64,
        kDigestSize = 20,
        kBlockSizeInWords = (kBlockSize >> 2),
        kDigestSizeInWords = (kDigestSize >> 2)
    };
};

// define it for speed optimization
// #define _SHA1_UNROLL

static const unsigned int s_kNumW =
#ifdef _SHA1_UNROLL
                                    16;
#else
                                    80;
#endif
  

#define w0(i) (W[(i)] = data[(i)])

#ifdef _SHA1_UNROLL
#define w1(i) (W[(i)&15] = rotlFixed(W[((i)-3)&15] ^ W[((i)-8)&15] ^ W[((i)-14)&15] ^ W[((i)-16)&15], 1))
#else
#define w1(i) (W[(i)] = rotlFixed(W[(i)-3] ^ W[(i)-8] ^ W[(i)-14] ^ W[(i)-16], 1))
#endif

#define f1(x,y,z) (z^(x&(y^z)))
#define f2(x,y,z) (x^y^z)
#define f3(x,y,z) ((x&y)|(z&(x|y)))
#define f4(x,y,z) (x^y^z)

#define RK1(a,b,c,d,e,i, f, w, k) e += f(b,c,d) + w(i) + k + rotlFixed(a,5); b = rotlFixed(b,30);

#define R0(a,b,c,d,e,i) RK1(a,b,c,d,e,i, f1, w0, 0x5A827999)
#define R1(a,b,c,d,e,i) RK1(a,b,c,d,e,i, f1, w1, 0x5A827999)
#define R2(a,b,c,d,e,i) RK1(a,b,c,d,e,i, f2, w1, 0x6ED9EBA1)
#define R3(a,b,c,d,e,i) RK1(a,b,c,d,e,i, f3, w1, 0x8F1BBCDC)
#define R4(a,b,c,d,e,i) RK1(a,b,c,d,e,i, f4, w1, 0xCA62C1D6)

#define RX_1_4(rx1, rx4, i) rx1(a,b,c,d,e,i); rx4(e,a,b,c,d,i+1); rx4(d,e,a,b,c,i+2); rx4(c,d,e,a,b,i+3); rx4(b,c,d,e,a,i+4);
#define RX_5(rx, i) RX_1_4(rx, rx, i);

END_NAMESPACE_RCZIP

#endif //__RCSha1Defs_h_
