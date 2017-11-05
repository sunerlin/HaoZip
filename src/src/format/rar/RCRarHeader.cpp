/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarHeader.h"

/////////////////////////////////////////////////////////////////
//RCRarHeader class implementation

BEGIN_NAMESPACE_RCZIP

namespace RCRarHeader
{
    byte_t kMarker[kMarkerSize] = {0x52 + 1, 0x61, 0x72, 0x21, 0x1a, 0x07, 0x00};

    class RCMarkerInitializer
    {
    public:
        RCMarkerInitializer() { kMarker[0]--; };
    };

    static RCMarkerInitializer g_markerInitializer;
}

END_NAMESPACE_RCZIP
