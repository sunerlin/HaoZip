/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCInArchiveUtils_h_
#define __RCInArchiveUtils_h_ 1

#include "base/RCString.h"
#include "filesystem/RCFileDefs.h"
#include "interface/RCPropertyID.h"

BEGIN_NAMESPACE_RCZIP

/** 输入文档接口
*/
class IInArchive ;

/** 输入文档辅助类
*/
class RCInArchiveUtils
{
public:

    /** 取包内文件路径
    @param [in] archive 输入文档
    @param [in] index 序号
    @param [out] result 返回包内文件路径
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemPath(IInArchive* archive, 
                                      uint32_t index, 
                                      RCString& result);
                                      
    /** 取包内文件路径
    @param [in] archive 输入文档
    @param [in] index 包内文件序号
    @param [in] defaultName 缺省名称
    @param [out] result 返回文件路径
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemPath(IInArchive* archive, 
                                      uint32_t index,
                                      const RCString& defaultName, 
                                      RCString& result);
    
    /** 取包内文件时间
    @param [in] archive 输入文档
    @param [in] index 文件序号
    @param [in] defaultFileTime 缺省时间
    @param [out] fileTime 返回文件时间
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetArchiveItemFileTime(IInArchive* archive, 
                                          uint32_t index,
                                          const RC_FILE_TIME& defaultFileTime,
                                          RC_FILE_TIME& fileTime);
    
    /** 包内文件是否有某属性
    @param [in] archive 输入文档
    @param [in] index 序号
    @param [in] propID 属性编号
    @param [out] result 有该属性为true,否则为false
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemProp(IInArchive* archive, 
                                     uint32_t index, 
                                     RCPropertyID propID, 
                                     bool& result);
    
    /** 包内项是否为目录
    @param [in] archive 输入文档
    @param [in] index 包内文件序号
    @param [out] result 是目录为true,否则为false
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemFolder(IInArchive* archive, 
                                       uint32_t index, 
                                       bool& result);
    
    /** 包内文件是否反序
    @param [in] archive 输入文档
    @param [in] index 文件序号
    @param [out] result 是为true,否则为false
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult IsArchiveItemAnti(IInArchive* archive, 
                                     uint32_t index, 
                                     bool& result);
    
    /** 取压缩包内文件总数
    @param [in] inArchive 输入文档接口
    @param [out] fileCount 文件总数
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveFileCount(IInArchive* inArchive, 
                                         uint32_t& fileCount ) ;
                                      
    /** 取压缩文件总大小
    @param [in] inArchive 输入文档接口
    @param [out] unPackSize 压缩文件解压后总大小
    @param [out] packSize 压缩文件压缩后总大小
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveFileSize(IInArchive* inArchive, 
                                        uint64_t& unPackSize,
                                        uint64_t& packSize ) ;

    /** 取压缩文件压缩平台
    @param [in] inArchive 输入文档接口
    @param [out] hostOS 压缩平台
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveHostOS(IInArchive* inArchive, 
                                      RCString& hostOS) ;
    
    /** 取压缩文件的主注释属性
    @param [in] inArchive 输入文档接口
    @param [out] isCommented 返回true表示有主注释，否则没有主注释
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveIsCommented(IInArchive* inArchive, 
                                           bool& isCommented) ;

    /** 取压缩文件的主注释
    @param [in] inArchive 输入文档接口
    @param [out] comment 返回主注释
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveComment(IInArchive* inArchive, 
                                       RCString& comment) ;
   
    /** 是否可以更新注释
    @param [in] inArchive 输入文档
    @return 注释可更新返回true,否则返回false
    */ 
    static bool GetInArchiveCanUpdateComment(IInArchive* inArchive) ;
    
    /** 取压缩文件的密码属性
    @param [in] inArchive 输入文档接口
    @param [out] isEncrypted 返回true表示有密码，否则没有密码
    @return 如果成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetInArchiveIsEncrypted(IInArchive* inArchive, 
                                           bool& isEncrypted) ;

    /** 判断是否有文档属性
    @param [in] inArchive 输入文档接口
    @param [in] propID 属性ID
    @return 有文档属性返回true,否则返回false
    */
    static bool HasArchiveProperty(IInArchive* inArchive, RCPropertyID propID) ;
};

END_NAMESPACE_RCZIP

#endif //__RCInArchiveUtils_h_
