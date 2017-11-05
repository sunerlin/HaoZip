/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCreateCoder_h_
#define __RCCreateCoder_h_ 1

#include "interface/ICodecInfo.h"
#include "interface/ICoder.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCCreateCoder
{
public:
    
    /** 根据ID查找编码方法信息
    @param [in] codecInfo 编码器查询接口
    @param [in] methodID 编码ID
    @param [out] spCodecInfo 返回该方法接口指针
    @param [out] codecIndex 返回该编码器的下标值
    @return 成功返回true，如果不存在该方法则返回false
    */
    static bool FindMethodByID(ICompressCodecsInfo* codecInfo, 
                               RCMethodID methodID, 
                               ICodecInfoPtr& spCodecInfo,
                               uint32_t* codecIndex = NULL) ;
                               
    /** 根据Name查找编码方法信息
    @param [in] codecInfo 编码器查询接口
    @param [in] methodName 编码名称，不区分大小写
    @param [out] spCodecInfo 返回该方法接口指针
    @param [out] codecIndex 返回该编码器的下标值
    @return 成功返回true，如果不存在该方法则返回false
    */
    static bool FindMethodByName(ICompressCodecsInfo* codecInfo, 
                                 const RCString& methodName, 
                                 ICodecInfoPtr& spCodecInfo,
                                 uint32_t* codecIndex = NULL) ;

    /** 根据Name查找编码ID
    @param [in] codecInfo 编码器查询接口
    @param [in] methodName 编码名称，不区分大小写
    @param [out] methodID 返回编码名称对应的编码ID
    @param [out] codecIndex 返回该编码器的下标值
    @return 成功返回true，如果不存在该方法则返回false
    */
    static bool FindMethodID(ICompressCodecsInfo* codecInfo, 
                             const RCString& methodName, 
                             RCMethodID& methodID,
                             uint32_t* codecIndex = NULL) ;

    /** 创建编码/解码器
    @param [in] codecInfo 编码器查询接口
    @param [in] methodID 编码ID
    @param [out] coder  返回ICompressCoder接口指针
    @param [in] isEncoder 如果为true，创建编码器接口指针，如果为false，则创建解码器接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult CreateCoder(ICompressCodecsInfo* codecInfo,
                               RCMethodID methodID,
                               ICompressCoderPtr& coder,
                               bool isEncoder);
                    
    /** 创建编码/解码器
    @param [in] codecsInfo 编码器查询接口
    @param [in] methodID 编码ID
    @param [out] coder  返回ICompressCoder接口指针
    @param [out] coder2 返回ICompressCoder2接口指针
    @param [in] isEncoder 如果为true，创建编码器接口指针，如果为false，则创建解码器接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult CreateCoder(ICompressCodecsInfo* codecsInfo, 
                               RCMethodID methodID,
                               ICompressCoderPtr& coder,
                               ICompressCoder2Ptr& coder2,
                               bool isEncoder);
    
    /** 创建编码/解码器的过滤器
    @param [in] codecsInfo 编码器查询接口
    @param [in] methodID 编码ID
    @param [out] filter 返回ICompressFilter接口指针
    @param [in] isEncoder 如果为true，创建编码器接口指针，如果为false，则创建解码器接口指针
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult CreateFilter(ICompressCodecsInfo* codecsInfo, 
                                RCMethodID methodID,
                                ICompressFilterPtr& filter,
                                bool isEncoder);
private:
    
    /** 创建编码/解码器
    @param [in] codecsInfo 编码器查询接口
    @param [in] methodID 编码ID
    @param [out] filter 返回ICompressFilter接口指针
    @param [out] coder  返回ICompressCoder接口指针
    @param [out] coder2 返回ICompressCoder2接口指针
    @param [in] isEncoder 如果为true，创建编码器接口指针，如果为false，则创建解码器接口指针
    @param [in] isOnlyCoder 如果为true，创建RCFilterCoder作为coder接口指针，
                            如果为false,如果编码器是Filter，则不创建coder接口
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult CreateCoder(ICompressCodecsInfo* codecsInfo, 
                               RCMethodID methodID,
                               ICompressFilterPtr& filter,
                               ICompressCoderPtr& coder,
                               ICompressCoder2Ptr& coder2,
                               bool isEncoder,
                               bool isOnlyCoder) ;
};

END_NAMESPACE_RCZIP

#endif //__RCCreateCoder_h_
