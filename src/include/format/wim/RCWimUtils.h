/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWimUtils_h_
#define __RCWimUtils_h_ 1

#include "format/wim/RCWimHeader.h"
#include "format/wim/RCWimDatabase.h"
#include "format/wim/RCWimResource.h"
#include "format/wim/RCWimItem.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "common/RCBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** Wim Utils
*/
class RCWimUtils
{
public:
    
    /** 默认构造函数
    */
    RCWimUtils(ICompressCodecsInfo* compressCodecsInfo) ;

    /** 读头标记
    @param [in] inStream 输入流
    @param [out] header 头结构
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadHeader(IInStream* inStream, RCWimHeader& header) ;

    /** 打开文档
    @param [in] inStream 输入流
    @param [in] header 头结构
    @param [in] xml
    @param [out] database
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult OpenArchive(IInStream* inStream,
                               const RCWimHeader& header, 
                               RCByteBuffer& xml,
                               RCWimDatabase& database);

    /** 排序
    @param [in,out] database
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SortDatabase(RCWimDatabase& database) ;

private:

    /** 读取流
    @param [in] inStream 输入流
    @param [in] h 头结构
    @param [out] db 
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ReadStreams(IInStream* inStream, const RCWimHeader& h, RCWimDatabase& db) ;

    /** 解压数据
    @param [in] inStream 输入流
    @param [in] resource
    @param [in] lzxMode lzx模式
    @param [out] buf 输出
    @param [out] digest 摘要
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult UnpackData(IInStream* inStream, const RCWimResource& resource, bool lzxMode, RCByteBuffer& buf, byte_t* digest) ;

    /** 取得流
    @param [in] p 缓存
    @param [in] s 流结构
    */
    void GetStream(const byte_t* p, RCWimStreamInfo& s) ;

    /** 取得文件时间
    @param [in] p 缓存
    @param [out] ft 文件时间
    */
    void GetFileTimeFromMem(const byte_t* p, RC_FILE_TIME* ft) ;

    /** 解析目录
    @param [in] base
    @param [in] pos 位置
    @param [in] size 大小
    @param [in] prefix 前缀
    @param [out] items 项列表
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ParseDirItem(const byte_t* base, 
                         size_t pos, 
                         size_t size,
                         const RCString& prefix, 
                         RCVector<RCWimItem>& items) ;

    /** 解析目录
    @param [in] base
    @param [in] size 大小
    @param [in] prefix 前缀
    @param [in] items 项列表
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult ParseDir(const byte_t* base, 
                     size_t size,
                     const RCString& prefix, 
                     RCVector<RCWimItem>& items) ;

    /** 查找hash
    @param [in] streams 流列表
    @param [in] sortedByHash 排序hash
    @param [in] hash hash值
    @return 返回结果编号
    */
    int32_t FindHash(const RCVector<RCWimStreamInfo>& streams,
                     const RCVector<int32_t>& sortedByHash, 
                     const byte_t* hash) ;

    /** 编码解码管理器
    */
    ICompressCodecsInfoPtr m_compressCodecsInfo ;
};

END_NAMESPACE_RCZIP

#endif //__RCWimUtils_h_
