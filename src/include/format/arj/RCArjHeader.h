/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArjHeader_h_
#define __RCArjHeader_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCVariant.h"
#include "interface/IStream.h"
#include "filesystem/RCFileDefs.h"
#ifdef RCZIP_OS_WIN
#include "base/RCWindowsDefs.h"
#endif

BEGIN_NAMESPACE_RCZIP

class RCArjDefs
{
public:

    static const int32_t kBlockSizeMin = 30;
    static const int32_t kBlockSizeMax = 2600;
   
    static const byte_t kSig0 = 0x60;
    static const byte_t kSig1 = 0xEA;

    /** 压缩方法
    */
    enum NCompressionMethod
    {
        kStored = 0,
        kCompressed1a = 1,
        kCompressed1b = 2,
        kCompressed1c = 3,
        kCompressed2 = 4,
        kNoDataNoCRC = 8,
        kNoData = 9
    };

    /** 文件类型
    */
    enum NFileType
    {
        kBinary = 0,
        k7BitText = 1,
        kArchiveHeader = 2,
        kDirectory = 3,
        kVolumeLablel = 4,
        kChapterLabel = 5
    };

    static const byte_t kGarbled = 1;
    static const byte_t kVolume = 4;
    static const byte_t kExtFile = 8;
    static const byte_t kPathSym = 0x10;
    static const byte_t kBackup = 0x20;

    /** 系统平台类型
    */
    enum NHostOS
    {
        kMSDOS = 0,
        kPRIMOS,
        kUnix,
        kAMIGA,
        kMac,
        kOS_2,
        kAPPLE_GS,
        kAtari_ST,
        kNext,
        kVAX_VMS,
        kWIN95
    };

    /** 读字符串
    @param [in] p 内存数据
    @param [in] size 数据大小
    @param [out] res 返回字符串
    @return 成功返回RC_S_OK，否则返回RC_S_FALSE
    */
    static HResult ReadString(const byte_t* p, unsigned& size, RCStringA& res);

    /** 检查标记
    @param [in] p 内存数据
    @param [in] maxSize 检查标记的最大偏移位置
    @return 通过检查返回true，否则返回false
    */
    static inline bool TestMarkerCandidate(const byte_t* p, unsigned maxSize);

    /** 查找并读取标记
    @param [in] stream 输入流
    @param [in] searchHeaderSizeLimit 查找头部的最大偏移位置
    @param [out] position 输出偏移
    @return 成功返回RC_S_OK，否则返回错误号 
    */
    static HResult FindAndReadMarker(ISequentialInStream* stream, 
                                     const uint64_t* searchHeaderSizeLimit, 
                                     uint64_t& position);
    /** 设置时间
    @param [in] dosTime dos时间
    @param [out] prop 时间属性
    */
    static void SetTime(uint32_t dosTime, RCVariant& prop);

    /** 设置系统平台
    @param [in] hostOS 系统平台
    @param [in] prop 平台属性
    */
    static void SetHostOS(byte_t hostOS, RCVariant& prop);

    /** 设置unicode字符串
    @param [in] s ascii字符串
    @param [out] prop unicode字符串属性
    */
    static void SetUnicodeString(const RCStringA& s, RCVariant& prop);
};

struct RCArjArchiveHeader
{
    /** 系统类型
    */
    byte_t m_hostOS;

    /** 创建时间
    */
    uint32_t m_cTime;

    /** 修改时间
    */
    uint32_t m_mTime;

    /** 文档大小
    */
    uint32_t m_archiveSize;

    /** 文件名
    */
    RCStringA m_name;

    /** 注释
    */
    RCStringA m_comment;

    /** 解析
    @param [in] p 内存数据
    @param [in] size 数据大小
    */
    HResult Parse(const byte_t* p, unsigned size);
};

END_NAMESPACE_RCZIP

#endif //__RCArjHeader_h_