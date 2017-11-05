/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCDebHeader_h_
#define __RCDebHeader_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "interface/IStream.h"

BEGIN_NAMESPACE_RCZIP

class RCDebDefs
{
public:
    static const int32_t kSignatureLen = 8;

    //static const char *kSignature  = "!<arch>\n";
    static const char *kSignature;

    static const int32_t kNameSize = 16;
    static const int32_t kTimeSize = 12;
    static const int32_t kModeSize = 8;
    static const int32_t kSizeSize = 10;
    static const int32_t kHeaderSize = kNameSize + kTimeSize + 6 + 6 + kModeSize + kSizeSize + 1 + 1;

    static void MyStrNCpy(char *dest, const char *src, int32_t size);
    static bool OctalToNumber(const char *s, int32_t size, uint64_t &res);
    static bool OctalToNumber32(const char *s, int32_t size, uint32_t &res);
    static bool DecimalToNumber(const char *s, int32_t size, uint64_t &res);
    static bool DecimalToNumber32(const char *s, int32_t size, uint32_t &res);
};

class RCDebItem
{
public:

    /** 文件名 
    */
    RCStringA m_name;

    /** 大小
    */
    uint64_t m_size;

    /** 修改时间
    */
    uint32_t m_mTime;

    /** 模式
    */
    uint32_t m_mode;

    /** 头部偏移位置
    */
    uint64_t m_headerPos;

    /** 数据偏移位置
    @return 返回数据偏移位置
    */
    uint64_t GetDataPos() const
    {
        return m_headerPos + RCDebDefs::kHeaderSize; 
    };
};

class RCDebInArchive
{
public:

    /** 打开文档
    @param [in] inStream 输入流
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult Open(IInStream* inStream);

    /** 获取下一项
    @param [out] filled 是否填充
    @param [out] itemInfo deb项信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetNextItem(bool& filled, RCDebItem& itemInfo);

    /** 调过数据
    @param [in] 调过大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SkipData(uint64_t dataSize);

private:
    
    /** 获取下一项
    @param [out] filled 是否填充
    @param [out] itemInfo deb项信息
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult GetNextItemReal(bool &filled, RCDebItem &itemInfo);

private:

    /** 输入流
    */
    IInStreamPtr m_stream;

    /** 位置
    */
    uint64_t m_position;
};

END_NAMESPACE_RCZIP

#endif //__RCDebHeader_h_