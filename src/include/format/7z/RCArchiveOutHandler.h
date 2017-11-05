/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveOutHandler_h_
#define __RCArchiveOutHandler_h_ 1

#include "format/common/RCMethodProps.h"
#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "common/RCVariant.h"
#include "RC7zExtFilter.h"
#include "RC7zFileFilter.h"

BEGIN_NAMESPACE_RCZIP

struct RCOneMethodInfo
{
    /** 属性
    */
    RCVector<RCArchiveProp> m_properties;

    /** 模式名
    */
    RCString m_methodName;

    /** 是否是LZMA
    */
    bool IsLzma() const ;

    /** 是否是BZIP2
    */
    bool IsBZip2() const ;

    /** 是否是PPMD
    */
    bool IsPPMd() const ;

    /** 是否是Deflate
    */
    bool IsDeflate() const ;
};

class RCArchiveOutHandler:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCArchiveOutHandler() ;
    
    /** 默认析构函数
    */
    ~RCArchiveOutHandler() ;
    
public:

    /** 初始化
    */
    void Init() ;

    /** 初始化固实
    */
    void InitSolid() ;

    /** 初始化固实文件
    */
    void InitSolidFiles() ;

    /** 初始化固实大小
    */
    void InitSolidSize() ;

    /** 在设置属性之前
    */
    void BeforeSetProperty() ;

    /** 设置属性
    @param [in] nameSpec
    @param [in] value
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetProperty(const RCString& nameSpec, const RCVariant& value) ;

    /** 设置固实
    @param [in] s 属性
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetSolidSettings(const RCString& s);

    /** 设置固实
    @param [in] value 数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetSolidSettings(const RCVariant& value) ;

    /** 设置参数
    @param [in] oneMethodInfo 模式信息
    @param [in] name 名字
    @param [in] value 数值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetParam(RCOneMethodInfo& oneMethodInfo, const RCString& name, const RCString& value);

    /** 设置参数
    @param [in] oneMethodInfo 模式信息
    @param [in] srcString 属性
    @return 成功返回RC_S_OK，否则返回错误号
    */
    HResult SetParams(RCOneMethodInfo& oneMethodInfo, const RCString& srcString);

    /** 设置压缩模式
    @param [in] oneMethodInfo 模式信息
    @param [in] numThreads 线程个数
    @param [in] totalSize 总大小
    */
    void SetCompressionMethod2(RCOneMethodInfo& oneMethodInfo, uint32_t numThreads, uint64_t totalSize);
    
public:

    /** 线程个数
    */
    uint32_t m_numThreads ;

    /** crc大小
    */
    uint32_t m_crcSize ;

    /** 模式
    */
    RCVector<RCOneMethodInfo> m_methods;

    /** 是否移除自解压块
    */
    bool m_removeSfxBlock;

    /** 固实压缩的文件数
    */
    uint64_t m_numSolidFiles;

    /** 固实压缩的字节数
    */
    uint64_t m_numSolidBytes;

    /** 是否定义固实压缩的字节数
    */
    bool m_numSolidBytesDefined;

    /** 是否有固实压缩扩展信息
    */ 
    bool m_solidExtension;

    /** 是否有压缩头
    */
    bool m_compressHeaders;

    /** 是否指定了加密头
    */
    bool m_encryptHeadersSpecified;

    /** 是否有加密头
    */
    bool m_encryptHeaders;

    /** 是否写创建时间
    */
    bool m_writeCTime;

    /** 是否写访问时间
    */
    bool m_writeATime;

    /** 是否写修改时间
    */
    bool m_writeMTime;

    /** 是否自动过滤
    */
    bool m_autoFilter;

    /** 压缩级别
    */
    uint32_t m_level;

    /** 是否分卷
    */
    bool m_volumeMode;

    /** 最小号
    */
    uint32_t m_minNumber;

    /** CPU个数
    */
    uint32_t m_numProcessors;

    /** 主字典大小
    */
    uint32_t m_mainDicSize;

    /** 主字典模式索引
    */
    uint32_t m_mainDicMethodIndex;

    /** 文件扩展名算法过滤器
    */
    static RCVector<RC7zExtFilterPtr> s_extFilters ;

    /** 文件算法过滤器
    */
    static RC7zFileFilter s_fileFilter ;

private:

    /** 是否设置了智能文件类型过滤
    */
    bool m_hasExtFilters ;

    /** 是否设置了文件压缩算法过滤
    */
    bool m_hasFileFilters ;
};

/** 设置模式的属性
@param [out] oneMethodInfo 输出模式属性
@param [in] propID 属性ID
@param [in] value 数值
*/
void SetOneMethodProp(RCOneMethodInfo& oneMethodInfo, 
                      RCPropertyID propID,
                      const RCVariant& value);
END_NAMESPACE_RCZIP

#endif //__RCArchiveOutHandler_h_
