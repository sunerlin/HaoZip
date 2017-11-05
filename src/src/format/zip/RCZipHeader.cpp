/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/zip/RCZipHeader.h"

/////////////////////////////////////////////////////////////////
//RCZipHeader class implementation

BEGIN_NAMESPACE_RCZIP

namespace RCZipSignature
{
    uint32_t kLocalFileHeader             = 0x04034B50 + 1;
    uint32_t kDataDescriptor              = 0x08074B50 + 1;
    uint32_t kCentralFileHeader           = 0x02014B50 + 1;
    uint32_t kEndOfCentralDir             = 0x06054B50 + 1;
    uint32_t kZip64EndOfCentralDir        = 0x06064B50 + 1;
    uint32_t kZip64EndOfCentralDirLocator = 0x07064B50 + 1;
    uint32_t kZipSingleVolumeSignature      = 0x03034B50 + 1;

    class RCMarkersInitializer
    {
    public:
        RCMarkersInitializer()
        {
            kLocalFileHeader--;
            kDataDescriptor--;
            kCentralFileHeader--;
            kEndOfCentralDir--;
            kZip64EndOfCentralDir--;
            kZip64EndOfCentralDirLocator--;
            kZipSingleVolumeSignature--;
        }
    };

    static RCMarkersInitializer g_markerInitializer;
}

END_NAMESPACE_RCZIP
