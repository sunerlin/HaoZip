/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCGuidDefs_h_
#define __RCGuidDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** GUID
*/
typedef struct tagRCGuid
{
    uint32_t Data1;

    uint16_t Data2;

    uint16_t Data3;

    unsigned char Data4[8];

}RC_GUID;

#ifdef __cplusplus
#define REFGUID const RC_GUID &
#else
#define REFGUID const RC_GUID *
#endif

#define REFCLSID REFGUID
#define REFIID REFGUID

#ifdef __cplusplus
inline int operator==(REFGUID g1, REFGUID g2)
{
  for (int i = 0; i < (int)sizeof(g1); i++)
    if (((unsigned char *)&g1)[i] != ((unsigned char *)&g2)[i])
      return 0;
  return 1;
}
inline int operator!=(REFGUID g1, REFGUID g2) { return !(g1 == g2); }
#endif

#ifdef __cplusplus
  #define MY_EXTERN_C extern "C"
#else
  #define MY_EXTERN_C extern
#endif

#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif

#ifdef  INITGUID
  #define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    MY_EXTERN_C const RC_GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
  #define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    MY_EXTERN_C const RC_GUID name
#endif

END_NAMESPACE_RCZIP

#endif //__RCGuidDefs_h_
