/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchiveOpen_h_
#define __RCArchiveOpen_h_ 1

#include "archive/manager/RCArchiveCodecs.h"
#include "archive/common/RCArchiveLink.h"
#include "base/RCNonCopyable.h"
#include "interface/IStream.h"
#include "interface/RCPropertyID.h"
#include <set>

BEGIN_NAMESPACE_RCZIP

/** 界面打开回调接口
*/
class IOpenCallbackUI ;

/** 打开压缩包
*/
class RCArchiveOpen:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCArchiveOpen() ;
    
    /** 默认析构函数
    */
    ~RCArchiveOpen() ;
    
public:

    /** 打开压缩包
    @param [in] archiveCodecs 文档格式管理器
    @param [in] formatIndices 文档格式编号
    @param [in] archiveName 文档名称
    @param [out] archiveLink 文档链接
    @param [in] openCallbackUI 界面打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult OpenArchive(const RCArchiveCodecsPtr& archiveCodecs,
                               const RCVector<RCArchiveID>& formatIndices,
                               const RCString& archiveName,
                               RCArchiveLink& archiveLink,
                               IOpenCallbackUI* openCallbackUI) ;

public:
    
    /** 根据文件签名内容匹配可能的文件格式
    @param [in] fileName 文件名
    @param [in] archiveCodecs 文档格式管理器
    @param [out] matchFormatIndices 匹配签名的文件格式
    @param [in] inputFormatIndices 文档格式编号集合
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult FindFormatsBySignature(const RCString& fileName,
                                          RCArchiveCodecsPtr archiveCodecs,
                                          RCIntVector& matchFormatIndices,
                                          const std::set<RCArchiveID>* inputFormatIndices = NULL) ;

#ifndef RC_STATIC_SFX

    /** 是否是RAR格式的分卷压缩
    @param [in] fileName 文件名
    @param [in] archiveCodecs 编码管理器
    @param [in,out] result 是分卷压缩返回ture，否则返回false
    @return 是rar格式返回RC_S_OK，否则返回错误号
    */
    static HResult RCArchiveOpen::IsRarVolumeType(const RCString& fileName,
                                                  RCArchiveCodecsPtr archiveCodecs,
                                                  bool& result);
#endif
    
public:
    
    /** 返回包内项名称
    @param [in] archive 输入文档指针
    @param [in] index 包内项序号
    @param [in] defaultName 缺省名称
    @param [out] result 返回包内项名称
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemPath(IInArchive* archive,
                                      uint32_t index,
                                      const RCString& defaultName,
                                      RCString& result) ;

    /** 返回包内项是否为目录
    @param [in] archive 输入文档指针
    @param [in] index 包内项序号
    @param [out] result 是目录返回true,否则返回false
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemFolder(IInArchive* archive,
                                       uint32_t index,
                                       bool& result) ;

    /** 返回包内项是否为反向
    @param [in] archive 输入文档指针
    @param [in] index 包内项序号
    @param [out] result 是返回true,否则返回false
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemAnti(IInArchive* archive,
                                     uint32_t index,
                                     bool& result) ;

    /** 返回包内项路径
    @param [in] archive 输入文档指针
    @param [in] index 包内项序号
    @param [out] result 返回包内项路径
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemPathNoDefault(IInArchive* archive,
                                               uint32_t index,
                                               RCString& result) ;

    /** 返回包内项目扩展名
    @param [in] archive 输入文档指针
    @param [in] index 包内项目序号
    @param [out] result 返回扩展名
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemPathExtension(IInArchive* archive,
                                               uint32_t index,
                                               RCString& result) ;

    /** 返回包内项目是否有某属性
    @param [in] archive 输入文档指针
    @param [in] index 包内项序号
    @param [in] propID 属性序号
    @param [out] result 有某属性返回true,否则返回false
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemProp(IInArchive* archive,
                                     uint32_t index,
                                     RCPropertyID propID,
                                     bool& result) ;
    
private:

    /** 打开文档
    @param [in] archiveCodecs 文档格式编码管理器
    @param [in] formatIndices 文档格式列表
    @param [in] archiveName 文档名
    @param [out] archive0 输入文档1
    @param [out] archive1 输入文档2
    @param [out] defaultItemName0 缺省名称1
    @param [out] defaultItemName1 缺省名称2
    @param [out] volumePaths 各分卷路径
    @param [out] volumesSize 分卷大小
    @param [in] openCallbackUI 界面打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult OpenArchive(const RCArchiveCodecsPtr& archiveCodecs,
                               const RCVector<RCArchiveID>& formatIndices,
                               const RCString& archiveName,
                               IInArchivePtr& archive0,
                               IInArchivePtr& archive1,
                               RCString& defaultItemName0,
                               RCString& defaultItemName1,
                               RCVector<RCString>& volumePaths,
                               uint64_t& volumesSize,
                               IOpenCallbackUI* openCallbackUI) ;

    /** 打开文档
    @param [in] archiveCodecs 文档格式编码管理器
    @param [in] formatIndices 文档格式列表
    @param [in] fileName 文件名
    @param [out] archive0 输入文档1
    @param [out] archive1 输入文档2
    @param [out] formatIndex0 文档1格式编号
    @param [out] formatIndex1 文档2格式编号
    @param [out] defaultItemName0 缺省名称1
    @param [out] defaultItemName1 缺省名称2
    @param [in] archiveOpenCallback 打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult OpenArchive(const RCArchiveCodecsPtr& archiveCodecs,
                               const RCVector<RCArchiveID>& formatIndices,
                               const RCString& fileName,
                               IInArchivePtr& archive0,
                               IInArchivePtr& archive1,
                               int32_t& formatIndex0,
                               int32_t& formatIndex1,
                               RCString& defaultItemName0,
                               RCString& defaultItemName1,
                               IArchiveOpenCallbackPtr archiveOpenCallback) ;

    /** 打开文档
    @param [in] archiveCodecs 文档格式编码管理器
    @param [in] arcTypeIndex 文档格式编号
    @param [in] fileName 文件名
    @param [out] archiveResult 打开的文件
    @param [out] formatIndex 文档格式序号
    @param [out] defaultItemName 项缺省名称
    @param [in] archiveOpenCallback 打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult OpenArchive(const RCArchiveCodecsPtr& archiveCodecs,
                               int32_t arcTypeIndex,
                               const RCString& fileName,
                               IInArchivePtr& archiveResult,
                               int32_t& formatIndex,
                               RCString& defaultItemName,
                               IArchiveOpenCallbackPtr archiveOpenCallback) ;

    /** 打开文档
    @param [in] archiveCodecs 文档格式编码管理器
    @param [in] arcTypeIndex 格式序号
    @param [in] inStream 输入文件流指针
    @param [in] fileName 文件名
    @param [out] archiveResult 打开的文件
    @param [out] formatIndex 文档格式序号
    @param [out] defaultItemName 缺省项目名称
    @param [in] archiveOpenCallback 打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult OpenArchive(const RCArchiveCodecsPtr& archiveCodecs,
                               int32_t arcTypeIndex,
                               IInStreamPtr inStream,
                               const RCString& fileName,
                               IInArchivePtr& archiveResult,
                               int32_t& formatIndex,
                               RCString& defaultItemName,
                               IArchiveOpenCallbackPtr archiveOpenCallback) ;

    /** 设置回调
    @param [in] fileName 文件名称
    @param [in] openCallbackUI 界面打开回调接口指针
    @param [in] reArchiveOpenCallback 打开回调接口指针
    @param [in] archiveOpenCallback 打开回调接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult SetCallback( const RCString& fileName,
                                IOpenCallbackUI* openCallbackUI,
                                IArchiveOpenCallbackPtr reArchiveOpenCallback,
                                IArchiveOpenCallbackPtr& archiveOpenCallback) ;
        
#ifndef RC_STATIC_SFX

    /** 测试文档格式签名
    @param [in] p1 待匹配串1
    @param [in] p2 待匹配串2
    @param [in] size 需匹配的长度
    @return p1 p2匹配成功返回 true,否则返回 false
    */
    static bool TestSignature(const byte_t* p1, const byte_t* p2, size_t size) ;
    
#endif

    /** 生成缺省名称
    @param [in] name 项名称
    */
    static void MakeDefaultName(RCString& name) ;
    
    /** 根据文件前面内容匹配可能的文件格式
    @param [in] inStream 输入流
    @param [in] archiveCodecs 文档格式管理器
    @param [in] fileExt 文件扩展名
    @param [out] formatIndices 包含全部的以扩展名查找到的格式，把有签名的放在前面
    @param [out] matchFormatIndices 匹配签名的文件格式
    @param [in] inputFormatIndices 文档格式列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult FindFormatsBySignature(IInStreamPtr inStream,
                                          RCArchiveCodecsPtr archiveCodecs,
                                          const RCString& fileExt,
                                          RCIntVector& formatIndices,
                                          RCIntVector* matchFormatIndices = NULL,
                                          const std::set<RCArchiveID>* inputFormatIndices = NULL) ;

    /** 获取文件的扩展名, 不含'.'
    @param [in] fileName 文件名
    @return 返回文件扩展名,不含'.'
    */
    static RCString GetFileExtension(const RCString& fileName) ;
    
private:
    
    /** 签名检查最大偏移量
    */
    static const uint64_t s_maxCheckStartPosition ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchiveOpen_h_
