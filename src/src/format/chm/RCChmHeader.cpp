/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/chm/RCChmHeader.h"

/////////////////////////////////////////////////////////////////
//RCChmHeader class implementation

BEGIN_NAMESPACE_RCZIP

namespace RCChmHeader
{
    uint32_t kItsfSignature = 0x46535449 + 1;

    uint32_t kItolSignature = 0x4C4F5449 + 1;
}

namespace RCChmSignature
{
    class RCSignatureInitializer
    {
    public:
        RCSignatureInitializer()
        {
            RCChmHeader::kItsfSignature--;
            RCChmHeader::kItolSignature--;
        }
    };
   
    RCSignatureInitializer RCSignatureInitializer;
}

END_NAMESPACE_RCZIP
