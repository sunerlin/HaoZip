/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFile_h_
#define __RCFile_h_ 1

#include "base/RCNonCopyable.h"
#include "base/RCString.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 文件访问权限
*/
typedef enum eRCDesiredAccess
{
    RC_GENERIC_READ,
    RC_GENERIC_WRITE,
    RC_GENERIC_EXECUTE
}RC_DESIRED_ACCESS;

/** 文件共享模式
*/
typedef enum eRCShareMode
{
    RC_SHARE_READ,
    RC_SHARE_WRITE,
    RC_SHARE_DELETE,
}RC_SHARE_MODE;

/** 文件共享创建描述
*/
typedef enum eRCCreationDisposition
{
    RC_CREATE_ALWAYS,
    RC_CREATE_NEW,
    RC_OPEN_ALWAYS,
    RC_OPEN_EXISTING
}RC_CREATION_DISPOSITION;

/** 文件标志和属性
*/
typedef enum eRCFlagsAndAttributes
{
    RC_ATTRIBUTE_ARCHIVE,
    RC_ATTRIBUTE_NORMAL,
    RC_ATTRIBUTE_READONLY,
    RC_FLAG_DELETE_ON_CLOSE,
    RC_FLAG_NO_BUFFERING,
    RC_FLAG_WRITE_THROUGH
}RC_FLAGS_AND_ATTRIBUTES ;

/** 文件移动模式
*/
typedef enum eRCMoveMethod
{
    RC_FILE_BEGIN,
    RC_FILE_CURRENT,
    RC_FILE_END
}RC_MOVE_METHOD ;

/** 文件操作，支持32位和64位文件系统 
*/
class RCFile:
    private RCNonCopyable
{
public:
    
    /** 默认构造函数
    */
    RCFile();
    
    /** 默认析构函数
    */
    ~RCFile();
    
public:
    
    /** 创建或打开文件
    @param [in] rcsFileName 文件名
    @param [in] rdaDesiredAccess 访问权限
    @param [in] rsdShareMode 共享模式
    @param [in] rcdCreationDisposition 创建描述
    @param [in] rfaFlagsAndAttributes 文件标志和属性
    @return 成功返回true, 否则返回false
    */
    bool Create(const RCString& rcsFileName,
                RC_DESIRED_ACCESS rdaDesiredAccess,
                RC_SHARE_MODE rsdShareMode,
                RC_CREATION_DISPOSITION rcdCreationDisposition,
                RC_FLAGS_AND_ATTRIBUTES rfaFlagsAndAttributes );
    
    /** 读文件
    @param [out] pData 文件内容缓冲
    @param [in]  u32Size 读取的大小
    @param [out] u32ProcessedSize 实际读出的大小
    @return 成功返回true, 否则返回false
    */
    bool Read(void* pData, uint32_t u32Size, uint32_t& u32ProcessedSize);
    
    /** 写文件
    @param [in]  pData 数据传冲
    @param [in]  u32Size 写入的大小
    @param [out] u32ProcessedSize 实际写入的大小
    @return 成功返回true, 否则返回false
    */
    bool Write(const void* pData, uint32_t u32Size, uint32_t& u32ProcessedSize);
    
    /** 关闭文件
    @return 成功返回true, 否则返回false
    */
    bool Close();
    
public:
    
    /** 获得文件指针位置
    @param [out] u64Position 文件指针位置
    @return 成功返回true, 否则返回false
    */
    bool GetPosition(uint64_t& u64Position) const;
    
    /** 获得文件大小
    @param [out] u64Size 文件大小
    @return 成功返回true, 否则返回false
    */
    bool GetSize(uint64_t& u64Size) const;
    
    /** 移动文件指针
    @param [in] n64DistanceToMove 移动的字节数
    @param [in] rcmdMoveMethod 移动模式
    @param [out] u64NewPosition 移动到的新位置
    @return 成功返回true, 否则返回false
    */
    bool Seek(int64_t n64DistanceToMove, RC_MOVE_METHOD rcmdMoveMethod, uint64_t& u64NewPosition) const;
    
    /** 移动文件指针到文件头
    @return 成功返回true, 否则返回false
    */
    bool SeekBeginOfFile();
    
    /** 移动文件指针到文件尾
    @param [in] u64NewPosition 移动到的新位置
    @return 成功返回true, 否则返回false
    */
    bool SeekEndOfFile(uint64_t& u64NewPosition);
    
    /** 设置文件时间属性
    @param [in] rcftCreationTime 创建时间
    @param [in] rcftLastAccessTime 最后一次访问时间
    @param [in] rcftLastModifyTime 最后一次修改时间
    @return 成功返回true, 否则返回false
    */
    bool SetTime(const RC_FILE_TIME rcftCreationTime, const RC_FILE_TIME rcftLastAccessTime, const RC_FILE_TIME rcftLastModifyTime);
    
    /** 获取文件时间属性
    @param [in] rcftCreationTime 创建时间
    @param [in] rcftLastAccessTime 最后一次访问时间
    @param [in] rcftLastModifyTime 最后一次修改时间
    @return 成功返回true, 否则返回false
    */
    bool GetTime(RC_FILE_TIME& rcftCreationTime, RC_FILE_TIME& rcftLastAccessTime, RC_FILE_TIME& rcftLastModifyTime);

    /** 设置文件长度
    @param [in] u64Length 文件长度
    @return 成功返回true, 否则返回false
    */
    bool SetLength(uint64_t u64Length);
    
    /** 移动文件指针到文件尾
    @return 成功返回true, 否则返回false
    */
    bool SetEndOfFile();
    
    /** 文件是否已打开
    @return 如果已经打开返回true, 否则返回false
    */
    bool IsOpen() const ;
    
private:
    
    /** 文件句柄类
    */
    class RCFileHandle;
    
    /** 文件句柄
    */
    RCFileHandle* m_prcfhFile;
};

END_NAMESPACE_RCZIP

#endif //__RCFile_h_