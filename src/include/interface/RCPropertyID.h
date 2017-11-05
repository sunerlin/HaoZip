/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCPropertyID_h_
#define __RCPropertyID_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Archive 属性
*/
class RCPropID
{
public:
    
    /** 属性ID定义
    */
    enum EnumType
    {
        kpidNoProperty = 0,
        kpidMainSubfile = 1,
        kpidHandlerItemIndex = 2,
        kpidPath,
        kpidName,
        kpidExtension,
        kpidIsDir,
        kpidSize,
        kpidPackSize,
        kpidAttrib,
        kpidCTime,
        kpidATime,
        kpidMTime,
        kpidSolid,
        kpidCommented,
        kpidEncrypted,
        kpidSplitBefore,
        kpidSplitAfter,
        kpidDictionarySize,
        kpidCRC,
        kpidType,
        kpidIsAnti,
        kpidMethod,
        kpidHostOS,
        kpidFileSystem,
        kpidUser,
        kpidGroup,
        kpidBlock,
        kpidComment,
        kpidPosition,
        kpidPrefix,
        kpidNumSubDirs,
        kpidNumSubFiles,
        kpidUnpackVer,
        kpidVolume,
        kpidIsVolume,
        kpidOffset,
        kpidLinks,
        kpidNumBlocks,
        kpidNumVolumes,
        kpidTimeType,
        kpidBit64,
        kpidBigEndian,
        kpidCpu,
        kpidPhySize,
        kpidHeadersSize,
        kpidChecksum,
        kpidCharacts,
        kpidVa,
        kpidId,
        kpidShortName,
        kpidCreatorApp,
        kpidSectorSize,
        kpidPosixAttrib,
        kpidLink,
        
        kpidTotalSize = 0x1100,
        kpidFreeSpace,
        kpidClusterSize,
        kpidVolumeName,
        
        kpidLocalName = 0x1200,
        kpidProvider,
        
        kpidUserDefined = 0x10000,
        kpidCommentSizePosition,
        
        /** 压缩文件类型，属性为64位无符号类型
        */
        kpidArchiveType = 0x20000,

        /** 文件的绝对路径，供文件过滤器查询使用
        */
        kpidAbsolutePath = 0x30000,

        /** 文件的相对路径，供文件过滤器查询使用
        */
        kpidRelativePath
    } ;
};

typedef RCPropID::EnumType RCPropIDEnumType ;

/** 压缩解压编码属性
*/
class RCCoderPropID
{
public:
    
    enum EnumType
    {
        kDefaultProp = 0,
        kDictionarySize,
        kUsedMemorySize,
        kOrder,
        kBlockSize,
        kPosStateBits,
        kLitContextBits,
        kLitPosBits,
        kNumFastBytes,
        kMatchFinder,
        kMatchFinderCycles,
        kNumPasses,
        kAlgorithm,
        kNumThreads,
        kEndMarker
    };
};

typedef RCCoderPropID::EnumType RCCoderPropIDEnumType ;

/** 属性ID类型
*/
typedef uint32_t RCPropertyID ;

END_NAMESPACE_RCZIP

#endif //__RCPropertyID_h_
