/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __IOpenCallbackUI_h_
#define __IOpenCallbackUI_h_ 1

#include "base/RCString.h"
#include "interface/IArchive.h"

BEGIN_NAMESPACE_RCZIP

/** 界面打开回调接口
*/
class IOpenCallbackUI
{
public:

    /** 检查是否终止
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenCheckBreak() = 0 ;
    
    /** 设置总体进度
    @param [in] files 文件总数
    @param [in] bytes 字节总数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenSetTotal(const uint64_t* files, const uint64_t* bytes) = 0 ;
    
    /** 设置完成进度
    @param [in] files 完成文件数
    @param [in] bytes 完成字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenSetCompleted(const uint64_t *files, const uint64_t *bytes) = 0 ;
    
    /** 获取密码
    @param [out] password 密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenCryptoGetTextPassword(RCString& password) = 0 ;
    
    /** 获取密码
    @param [out] password 密码
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenGetPasswordIfAny(RCString& password) = 0 ;
    
    /** 是否已经询问过密码
    @return 已经询问密码返回 true,否则返回 false
    */
    virtual bool OpenWasPasswordAsked() = 0 ;
    
    /** 清除询问密码标记
    */
    virtual void OpenClearPasswordWasAskedFlag() = 0 ;

    /** 修复分卷
    @param [in] items 修复前的分卷序列
    @param [out] volumeFirst 第一个分卷
    @param [out] outputFilenames 修复后的分卷序列
    @return 基类未实现,返回RC_S_FALSE
    */
    virtual HResult VolumeRepair(const RCVector<RCArchiveVolumeItem>& items, RCString& volumeFirst, RCVector<RCString>& outputFilenames)
    {
        return RC_S_FALSE;
    };

    /** 设置错误
    @param [in] errorCode 错误号
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult OpenSetError(int32_t errorCode) = 0 ;
    
protected:
       
    /** 默认析构函数
    */
    ~IOpenCallbackUI() {} ;
};

END_NAMESPACE_RCZIP

#endif //__IOpenCallbackUI_h_
