/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "RCRar3VmUtils.h"
#include "RCRar3VmMemBitDecoder.h"
#include "algorithm/CpuArch.h"

/////////////////////////////////////////////////////////////////
//RCRar3VmUtils class implementation

BEGIN_NAMESPACE_RCZIP

uint32_t RCRar3VmUtils::GetValue32(const void* addr)
{
    return GetUi32(addr) ;
}

void RCRar3VmUtils::SetValue32(void* addr, uint32_t value)
{
    SetUi32(addr, value);
}

uint32_t RCRar3VmUtils::ReadEncodedUInt32(RCRar3VmMemBitDecoder& inp)
{
    switch(inp.ReadBits(2))
    {
        case 0:
            return inp.ReadBits(4);
        case 1:
        {
            uint32_t v = inp.ReadBits(4);
            if (v == 0)
            {
                return 0xFFFFFF00 | inp.ReadBits(8);
            }
            else
            {
                return (v << 4) | inp.ReadBits(4);
            }
        }
        case 2:
            return inp.ReadBits(16);
        default:
            return inp.ReadBits(32);
    }
}

END_NAMESPACE_RCZIP
