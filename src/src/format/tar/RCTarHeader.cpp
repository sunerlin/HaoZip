/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarHeader.h"

BEGIN_NAMESPACE_RCZIP

namespace RCTarHeader 
{

    // The checksum field is filled with this while the checksum is computed.
    const char *s_checkSumBlanks = "        ";   // 8 blanks, no null

    const char *s_longLink = "././@LongLink";
    const char *s_longLink2 = "@LongLink";

    // The magic field is filled with this if uname and gname are valid.
    namespace NMagic
    {
        const char *s_usTar  = "ustar"; // 5 chars
        const char *s_gnuTar = "GNUtar "; // 7 chars and a null
        const char *s_empty = "\0\0\0\0\0\0\0\0"; // 7 chars and a null
    }
}

END_NAMESPACE_RCZIP