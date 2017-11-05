/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFilePathUtils_h_
#define __RCFilePathUtils_h_ 1

#include "filesystem/RCFilePath.h"
#include "base/RCString.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCFilePathUtils
{
public:
    
    /** 判断路径是否存在
    @param [in] p 文件路径
    @return 如果存在返回true, 否则返回false
    */ 
    static bool IsExists(const RCFilePath& p) ;
    
    /** 判断是否为目录
    @param [in] p 文件路径
    @return 如果为目录返回true, 否则返回false
    */
    static bool IsDirectory(const RCFilePath& p) ;
    
    /** 判断是否为空，如果是目录，则目录中没有内容返回true，如果是文件，则当文件大小为0返回true
    @param [in] p 文件路径
    @return 如果为空返回true, 否则返回false
    */
    static bool IsEmpty(const RCFilePath& p) ;
        
    /** 获取文件大小
    @param [in] fileName 文件名
    @return 返回文件大小，若文件不存在，返回 0
    */
    static uint64_t FileSize(const RCFilePath& fileName) ;
    
    /** 删除目录，及子目录和所有文件。
    @param [in] path 路径名
    @return 返回删除的文件数量
    */
    static uint64_t RemoveAll(const RCFilePath& path) ;
    
    /** 重命名目录或者文件
    @param [in] from 需要重命名的文件
    @param [in] to 重名为新文件名
    @return 成功返回true, 否则返回false
    */
    static bool Rename(const RCFilePath& from, const RCFilePath& to) ;
    
    /** 复制文件或者目录
    @param [in] from 源文件路径
    @param [in] to 源文件路径
    @return 成功返回true, 否则返回false
    */
    static bool CopyPathFile(const RCFilePath& from, const RCFilePath& to) ;
    
    /** 创建目录，支持多级目录
    @param [in] path 目录名
    @return 成功返回true, 否则返回false
    */
    static bool CreateDirectories(const RCFilePath& path) ;
    
    /** 获取文件的扩展名
    @param [in] path 文件名
    @return 返回文件的扩展名
    */
    static RCString Extension(const RCFilePath& path) ;
    
    /** 获取文件名，去掉扩展名
    @param [in] path 文件名
    @return 返回文件的文件名部分，不含扩展名
    */
    static RCString BaseName(const RCFilePath& path) ;
    
    /** 修改扩展名，字符串操作，不对真实文件操作
    @param [in] path 文件名
    @param [in] newExtension 新的扩展名
    @return 返回修改扩展名后的新路径
    */
    static RCString ChangeExtension(const RCFilePath& path, const RCString& newExtension) ; 

    /** 获取所有的子对象
    @param [in] path 文件路径
    @param [out] sons 当前目录中，所有一层的文件路径和文件名
    */
    static void GetAllSons(const RCFilePath& path, RCVector<RCString>& sons);
};

END_NAMESPACE_RCZIP

#endif //__RCFilePathUtils_h_
