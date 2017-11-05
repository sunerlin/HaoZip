/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdateCallbackUI_h_
#define __RCUpdateCallbackUI_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "interface/RCPropertyID.h"
#include "common/RCVariant.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩文件过滤器
*/
class RCArchiveUpdateFilter ;

/** 压缩界面回调接口
*/
class RCUpdateCallbackUI
{
public:

    /** 设置总数
    @param [in] total 总数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetTotal(uint64_t total) = 0;

    /** 设置完成
    @param [in] completed 完成数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCompleted(uint64_t completed) = 0;

    /** 设置总体进度信息
    @param [in] inSize 输入字节数
    @param [in] outSize 输出字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetRatioInfo(uint64_t inSize, uint64_t outSize) = 0;
    
    /** 设置单个文件进度信息
    @param [in] index 文件编号
    @param [in] inSize 输入字节数
    @param [in] outSize 输出字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCompressRatioInfo(uint32_t index, uint64_t inSize, uint64_t outSize) = 0;

    /** 检查用户是否取消操作
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CheckBreak() = 0 ;

    /** 结束相关操作
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Finilize() = 0 ;

    /** 设置文件数
    @param [in] numFiles 文件总数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetNumFiles(uint64_t numFiles) = 0 ;

    /** 正在读取被压缩的文件
    @param [in] index 该文件的序号
    @param [in] name 该文件的文件名，含完整路径
    @param [in] isAnti Anti属性值
    @param [in] modifyTime 该文件的最后修改时间
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetStream(uint32_t index, 
                              const RCString& name, 
                              bool isAnti, 
                              const RC_FILE_TIME& modifyTime) = 0 ;
    
    /** 获取压缩文档属性
    @param [in] propID 属性ID
    @param [out] value 属性值
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetArchiveProperty(RCPropertyID propID, RCVariant& value) = 0 ;

    /** 打开文件结果
    @param [in] index 文件编号
    @param [in] name 文件名
    @param [in] result 打开结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenFileResult(uint32_t index, const RCString& name, HResult result) = 0 ;

    /** 设置操作结果
    @param [in] index 文件编号
    @param [in] operationResult 操作结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetOperationResult(uint32_t index, int32_t operationResult) = 0;

    /** 加密口令设置
    @param [out] passwordIsDefined 密码是否定义 返回0表示没有定义密码，返回1表示有密码
    @param [out] password 密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CryptoGetTextPassword2(int32_t* passwordIsDefined, RCString& password) = 0 ;
    
    /** 更新压缩文件时，7z固实压缩需要输入加密口令
    @param [out] password 密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CryptoGetTextPassword(RCString& password) = 0 ;

    /** 设置压缩解压多线程模式
    @param [in] isMultThread 是否为多线程
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetMultiThread(bool isMultThread) = 0 ;
    
    /** 设置压缩注释
    @param [in] commentValue 新的注释值
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetComment(const RCString& commentValue) = 0 ;
};

/** 压缩操作界面回调
*/
class RCUpdateCallbackUI2 : 
    public RCUpdateCallbackUI
{
public:
    
    /** 打开文档结果
    @param [in] name 文件名
    @param [in] result 打开结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenResult(const RCString& name, HResult result) = 0 ;

    /** 开始扫描磁盘
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult StartScanning() = 0 ;

    /** 扫描进度回调
    @param [in] numFolders 目录数
    @param [in] numFiles 文件数
    @param [in] path 扫描路径
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ScanProgress(uint64_t numFolders, uint64_t numFiles, const RCString& path) = 0 ;

    /** 扫描错误回调
    @param [in] name 扫描路径
    @param [in] systemError 系统错误号
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CanNotFindError(const RCString& name, HResult systemError) = 0 ;

    /** 扫描完成回调
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult FinishScanning() = 0 ;

    /** 开始文档操作
    @param [in] name 文件名
    @param [in] updating 是否更新
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult StartArchive(const RCString& name, bool updating) =0 ;

    /** 完成文档操作
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult FinishArchive() = 0 ;
    
    /** 询问图片转换接口
    @param [in] updateFilter 图片转换过滤器
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OnAskFileFilter(RCArchiveUpdateFilter* updateFilter) = 0 ;
    
    /** 开始转换图片
    @param [in] imageFile 图片文件名
    @param [in] destFile 转换后文件名
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OnConvertImageStart(const RCString& imageFile, const RCString& destFile) = 0 ;
    
    /** 结束转换图片
    @param [in] imageFile 图片文件名
    @param [in] result 转换结果
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OnConvertImageEnd(const RCString& imageFile, HResult result) = 0 ;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdateCallbackUI_h_
