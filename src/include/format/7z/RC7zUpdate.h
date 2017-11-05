/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zUpdate_h_
#define __RC7zUpdate_h_ 1

#include "base/RCString.h"
#include "base/RCSmartPtr.h"
#include "format/7z/RC7zCompressionMode.h"
#include "format/7z/RC7zOutArchive.h"
#include "format/7z/RC7zArchiveDatabaseEx.h"
#include "interface/IArchive.h"
#include "interface/ICoder.h"
#include "common/RCBuffer.h"
#include "format/7z/RC7zItem.h"

BEGIN_NAMESPACE_RCZIP

struct RC7zUpdateItem
{
    /** 在文档中的索引
    */
    int32_t m_indexInArchive;

    /** 在clinet中的索引
    */
    int32_t m_indexInClient;

    /** 创建时间
    */
    uint64_t m_cTime;

    /** 访问时间按
    */
    uint64_t m_aTime;

    /** 修改时间
    */
    uint64_t m_mTime;

    /** 大小
    */
    uint64_t m_size;

    /** 名字
    */
    RCString m_name;

    /** 属性
    */
    uint32_t m_attrib;

    /** 是否是新数据
    */
    bool m_newData;

    /** 是否是新属性
    */
    bool m_newProperties;

    /** 是否是anti
    */
    bool m_isAnti;

    /** 是否是目录
    */
    bool m_isDir;

    /** 是否有属性
    */
    bool m_attribDefined;

    /** 是否有创建时间
    */
    bool m_cTimeDefined;

    /** 是否有访问时间
    */
    bool m_aTimeDefined;

    /** 是否有修改时间
    */
    bool m_mTimeDefined;

public:

    /** 默认构造函数
    */
    RC7zUpdateItem() ;

public:

    /** 是否有流
    */
    bool HasStream() const ;

    /** 从属性中设置路径状态
    */
    void SetDirStatusFromAttrib() ;

    /** 获取扩展偏移
    */
    int32_t GetExtensionPos() const;

    /** 获取扩展信息
    */
    RCString GetExtension() const;
};

/** RC7zUpdateItem智能指针
*/
typedef RCSharedPtr<RC7zUpdateItem> RC7zUpdateItemPtr ;

struct RC7zUpdateOptions
{
    /** 模式
    */
    const RC7zCompressionMethodMode* m_method;

    /** 主属性 
    */
    const RC7zCompressionMethodMode* m_headerMethod;

    /** 是否用过滤
    */
    bool m_useFilters;

    /** 是否最大过滤
    */
    bool m_maxFilter;

    /** 头操作
    */
    RC7zHeaderOptions m_headerOptions;

    /** 固实文件数
    */
    uint64_t m_numSolidFiles;

    /** 固实字节数
    */
    uint64_t m_numSolidBytes;

    /** 是否有固实扩展
    */
    bool m_solidExtension;

    /** 是否移除自解压模块
    */
    bool m_removeSfxBlock;

    /** 是否是分卷模式
    */
    bool m_volumeMode;
};

struct RC7zFolderRepack
{
    /** 文件夹中索引
    */
    int32_t m_folderIndex;

    /** 分组索引
    */
    int32_t m_group;

    /** 拷贝的文件
    */
    RC7zNum m_numCopyFiles;
};

class RC7zUpdate
{
public:

    /** 更新
    @param [in] codecsInfo 编码管理器
    @param [in] inStream 输入流
    @param [in] db 基本数据
    @param [in] updateItems 更新项
    @param [in] archive 输出文档
    @param [in] newDatabase 新基本数据
    @param [in] seqOutStream 输出流
    @param [in] updateCallback 文档更新管理器回调接口
    @param [in] options 操作
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult Update(ICompressCodecsInfo* codecsInfo,
                          IInStream* inStream,
                          const RC7zArchiveDatabaseEx* db,
                          const RCVector<RC7zUpdateItemPtr>& updateItems,
                          RC7zOutArchive& archive,
                          RC7zArchiveDatabase& newDatabase,
                          ISequentialOutStream* seqOutStream,
                          IArchiveUpdateCallback* updateCallback,
                          const RC7zUpdateOptions& options) ;

private:

    /** 写范围
    @param [in] codecsInfo 编码管理器
    @param [in] inStream 输入流
    @param [in] outStream 输出流
    @param [in] position 偏移位置
    @param [in] size 大小
    @param [in] progress 压缩进度管理器
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult WriteRange(ICompressCodecsInfo* codecsInfo,
                              IInStream* inStream, 
                              ISequentialOutStream* outStream,
                              uint64_t position, 
                              uint64_t size, 
                              ICompressProgressInfo* progress) ;

    /** 比较内存
    @param [in] a1 内存一
    @param [in] a2 内存二
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareBuffers(const RCByteBuffer& a1, const RCByteBuffer& a2) ;

    /** 比较编码
    @param [in] c1 编码一
    @param [in] c2 编码二
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareCoders(const RC7zCoderInfo& c1, const RC7zCoderInfo& c2) ;

    /** 比较绑定配对信息
    @param [in] b1 绑定配对信息一
    @param [in] b2 绑定配对信息二
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareBindPairs(const RC7zBindPair& b1, const RC7zBindPair& b2) ;

    /** 比较文件夹
    @param [in] f1 文件夹一
    @param [in] f2 文件夹二
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareFolders(const RC7zFolder& f1, const RC7zFolder& f2) ;

    /** 比较空项
    @param [in] p1 数值一
    @param [in] p2 数值二
    @param [in] param 参数
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareEmptyItems(const int32_t* p1, const int32_t* p2, void* param) ;
 
    /** 比较文件夹
    @param [in] p1 文件夹一
    @param [in] p2 文件夹二
    @param [in] param 参数
    @return 相等返回0，否则返回其它
    */
    static int32_t CompareFolderRepacks(const RC7zFolderRepack* p1, const RC7zFolderRepack* p2, void* param) ;

    /** 从更新项到文件项
    @param [in] ui 更新项
    @param [in] file 文件项
    @param [in] file2 文件项
    */
    static void FromUpdateItemToFileItem(const RC7zUpdateItemPtr& ui, 
                                         RC7zFileItemPtr& file, 
                                         RC7zFileItem2Ptr& file2) ;

    /** 获取扩展索引
    @param [in] ext 扩展
    @return 返回索引
    */
    static int32_t GetExtIndex(const char* ext) ;

    /** 比较字符串
    @param [in] s1 字符串一
    @param [in] s2 字符串二
    @return 相等返回0，否则返回其它
    */
    static int32_t MyStringCompareNoCase(const RCString::value_type* s1, const RCString::value_type* s2) ;

    struct RC7zRefItem ;

    class RC7zSolidGroup ;

    /** 比较更新项
    @param [in] p1 引用项二
    @param [in] p2 引用项二
    @param [in] param 参数
    */
    static int32_t CompareUpdateItems(const RC7zRefItem* p1, const RC7zRefItem* p2, void* param) ;

    /** 是否是exe文件
    */
    static bool IsExeFile(const RCString& ext) ;

    /** 获取模式属性
    @param [in] methodID 模式id
    @param [in] numInStreams 输入流的个数
    @param [out] methodResult 输出模式属性
    @return 返回true
    */
    static bool GetMethodFull(uint64_t methodID,
                              uint32_t numInStreams, 
                              RC7zArchiveMethodFull& methodResult) ; 

    /** 设置exe的模式属性
    @param [in] method 模式属性
    @param [in] bcj2Filter 是否是bcj filter
    @param [out] exeMethod 输出模式属性
    @return 成功返回true，否则返回false
    */
    static bool MakeExeMethod(const RC7zCompressionMethodMode& method,
                              bool bcj2Filter, 
                              RC7zCompressionMethodMode& exeMethod) ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zUpdate_h_
