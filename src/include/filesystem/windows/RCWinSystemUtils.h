/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinSystemUtils_h_
#define __RCWinSystemUtils_h_ 1

#include "base/RCString.h"
#include "base/RCWindowsDefs.h"

BEGIN_NAMESPACE_RCZIP

/** windows平台系统相关工具类
*/
class RCWinSystemUtils
{
public:
    
    /** 获取Windows目录
    @param [out] path 返回路径名
    @return 成功返回true，失败返回false
    */
    static bool MyGetWindowsDirectory(RCString& path) ;
    
    /** 获取System目录
    @param [out] path 返回路径名
    @return 成功返回true，失败返回false
    */
    static bool MyGetSystemDirectory(RCString& path) ;
    
    /** 设置目录时间
    @param [in] fileName 目录名称
    @param [in] cTime 文件创建时间
    @param [in] aTime 文件最后访问时间
    @param [in] mTime 文件修改时间
    @return 成功返回true，失败返回false
    */
    static bool SetDirTime(const RCString& fileName, 
                           const FILETIME* cTime, 
                           const FILETIME* aTime,
                           const FILETIME* mTime) ;
   
    /** 设置文件属性
    @param [in] fileName 文件名
    @param [in] fileAttributes 文件属性
    @return 成功返回true，失败返回false
    */
    static bool MySetFileAttributes(const RCString& fileName, DWORD fileAttributes) ;
    
    /** 移动文件，如果目标存在则返回失败
    @param [in] existFileName 需要移动的文件
    @param [in] newFileName 目标文件名
    @return 成功返回true，失败返回false
    */
    static bool MyMoveFile(const RCString& existFileName, const RCString& newFileName) ;
    
    /** 移动文件,如果目标文件存在则覆盖
    @param [in] existFileName 需要移动的文件
    @param [in] newFileName 目标文件名
    @return 成功返回true，失败返回false
    */
    static bool MyMoveFileEx(const RCString& existFileName, const RCString& newFileName) ;
    
    /** 删除目录，仅能删除空目录
    @param [in] pathName 路径名
    @return 成功返回true，失败返回false
    */
    static bool MyRemoveDirectory(const RCString& pathName) ;
    
    /** 创建目录
    @param [in] pathName 路径名
    @return 成功返回true，失败返回false
    */
    static bool MyCreateDirectory(const RCString& pathName) ;
    
    /** 创建多级目录
    @param [in] pathName 路径名
    @return 成功返回true，失败返回false
    */
    static bool CreateComplexDirectory(const RCString& pathName) ;
    
    /** 删除文件
    @param [in] fileName 需要删除的文件
    @return 成功返回true，失败返回false
    */
    static bool DeleteFileAlways(const RCString& fileName);
    
    /** 删除文件夹，包含所有子目录及内容
    @param [in] pathName 路径名
    @return 成功返回true，失败返回false
    */
    static bool RemoveDirectoryWithSubItems(const RCString& pathName) ;
    
    /** 获取临时目录
    @param [in] resultPath 返回临时目录名
    @return 成功返回true，失败返回false
    */
    static bool MyGetTempPath(RCString& resultPath);
    
    /** 获取临时文件名
    @param [in] dirPath 临时文件所在目录
    @param [in] prefix 临时文件前缀
    @param [out] resultPath 临时文件名，含完整路径
    @return 成功返回true，失败返回false
    */
    static bool MyGetTempFileName(const RCString& dirPath, const RCString& prefix, RCString& resultPath) ;

    /** 获取文件的完整路径名
    @param [in] fileName 文件名
    @param [out] resultPath 完整路径名
    @param [out] fileNamePartStartIndex 文件名起始下标值
    @return 成功返回true，失败返回false
    */
    static bool MyGetFullPathName(const RCString& fileName, RCString& resultPath, int32_t& fileNamePartStartIndex);
    
    /** 设置当前路径
    @param [in] path 路径名
    @return 成功返回true，失败返回false
    */
    static bool MySetCurrentDirectory(const RCString& path) ;
    
    /** 在指定目录查找文件
    @param [in] path 需要查找的路径
    @param [in] fileName 需要查找的文件名
    @param [in] extension 需要查找文件的扩展名
    @param [out] resultPath 结果路径
    @param [out] filePart 文件名部分的起始下标值
    @return 成功返回true，失败返回false
    */
    static bool MySearchPath(const RCString& path, 
                             const RCString& fileName,
                             const RCString& extension,
                             RCString& resultPath,
                             int32_t& filePart) ;
};

END_NAMESPACE_RCZIP

#endif //__RCWinSystemUtils_h_
