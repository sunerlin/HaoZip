/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/nsis/RCNsisHeader.h"

/////////////////////////////////////////////////////////////////
//RCNsisHeader class implementation

BEGIN_NAMESPACE_RCZIP

namespace RCNsisSignature
{
    byte_t kSignature[kSignatureSize] = 
    { 
        0xEF + 1, 0xBE, 0xAD, 0xDE, 0x4E, 0x75, 0x6C, 0x6C, 
        0x73, 0x6F, 0x66, 0x74, 0x49, 0x6E, 0x73, 0x74
    };

    class RCSignatureInitializer 
    {
    public:

        RCSignatureInitializer() 
        { 
            kSignature[0]--; 
        } 
    };

    static RCSignatureInitializer g_signatureInitializer;
}

END_NAMESPACE_RCZIP
