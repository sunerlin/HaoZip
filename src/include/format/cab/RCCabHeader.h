/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCabHeader_h_
#define __RCCabHeader_h_ 1

BEGIN_NAMESPACE_RCZIP

class RCCabHeaderDefs
{
public:
    enum
    {
        kMarkerSize = 8
    };

    /** 标志
    */
    enum nFlags
    {
        kPrevCabinet = 0x0001,
        kNextCabinet = 0x0002,
        kReservePresent = 0x0004
    };

    /** 压缩模式
    */
    enum nCompressionMethodMajor
    {
        kNone = 0,
        kMSZip = 1,
        kQuantum = 2,
        kLZX = 3
    };

    /** 文件名UTF属性掩码
    */
    enum 
    {
        kFileNameIsUTFAttributeMask = 0x80
    };

    /** 文件夹索引
    */
    enum nFolderIndex
    {
        kContinuedFromPrev    = 0xFFFD,
        kContinuedToNext      = 0xFFFE,
        kContinuedPrevAndNext = 0xFFFF
    };
};

END_NAMESPACE_RCZIP

#endif //__RCCabHeader_h_