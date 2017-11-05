/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3VmMemBitDecoder_h_
#define __RCRar3VmMemBitDecoder_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCRar3VmMemBitDecoder 
{
public:

    /** 默认构造函数
    */
    RCRar3VmMemBitDecoder() ;
    
    /** 默认析构函数
    */
    ~RCRar3VmMemBitDecoder() ;
    
public:
    void Init(const byte_t *data, uint32_t byteSize) ;
    uint32_t ReadBits(int32_t numBits) ;
    uint32_t ReadBit() ;
    bool Avail() const ;
    
private:
    const byte_t* _data ;
    uint32_t _bitSize ;
    uint32_t _bitPos ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar3VmMemBitDecoder_h_
