/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RC7zAESEncoder_h_
#define __RC7zAESEncoder_h_ 1

#include "crypto/7zAES/RC7zAESBaseCoder.h"

BEGIN_NAMESPACE_RCZIP

/** 7z AES 编码器
*/
class RC7zAESEncoder:
    public RC7zAESBaseCoder,
    public ICompressWriteCoderProperties,
    public ICryptoResetInitVector
{
public:

    /** 默认构造函数
    */
    RC7zAESEncoder() ;
    
    /** 默认析构函数
    */
    ~RC7zAESEncoder() ;
    
public:
    
    /** 根据ID查询接口
    @param [in] iid 被查询的接口ID
    @param [out] outObject 如果查询成功，则保存结果接口指针，并增加引用计数
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    virtual HResult QueryInterface(RC_IID iid, void** outObject) ;
    
    /** 增加引用计数
    */
    virtual void AddRef(void) ;
    
    /** 减少引用计数
    */
    virtual void Release(void) ;
    
    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) ;
    
    /** 重置加密初始化数据
    @return 成功返回RC_S_OK，否则返回错误号 
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ResetInitVector() ;
    
private:

    /** 创建过滤器
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CreateFilter() ;
};

END_NAMESPACE_RCZIP

#endif //__RC7zAESEncoder_h_
