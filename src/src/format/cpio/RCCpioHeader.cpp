/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/cpio/RCCpioHeader.h"

/////////////////////////////////////////////////////////////////
//RCCpioHeader class implementation

BEGIN_NAMESPACE_RCZIP

namespace RCCpioHeader 
{
    namespace NMagic
    {
        const char* kMagic1 = "070701";
        const char* kMagic2 = "070702";
        const char* kMagic3 = "070707";
        const char* kEndName = "TRAILER!!!";
        const byte_t kMagicForRecord2[2] = { 0xC7, 0x71 };
    }

}

END_NAMESPACE_RCZIP
