/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzmaEncoder_h_
#define __RCLzmaEncoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "algorithm/LzmaEnc.h"
#include "common/RCVariant.h"
#include "interface/RCPropertyID.h"

BEGIN_NAMESPACE_RCZIP

/** Lzma 编码器
*/
class RCLzmaEncoder:
    public IUnknownImpl3<ICompressCoder,
                         ICompressSetCoderProperties,
                         ICompressWriteCoderProperties>
{
public:

    /** 默认构造函数
    */
    RCLzmaEncoder() ;
    
    /** 默认析构函数
    */
    ~RCLzmaEncoder() ;
    
public:
    
    /** 压缩/解压数据
    @param [in] inStream 数据输入流
    @param [in] outStream 数据输出流
    @param [in] inSize 输入数据大小
    @param [in] outSize 输出数据大小 
    @param [in] progress 进度回调接口
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult Code(ISequentialInStream* inStream,
                         ISequentialOutStream* outStream, 
                         const uint64_t* inSize, 
                         const uint64_t* outSize,
                         ICompressProgressInfo* progress) ;
    
    /** 设置压缩编码属性
    @param [in] propertyArray 压缩编码属性列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetCoderProperties(const RCPropertyIDPairArray& propertyArray) ;
    
    /** 将压缩编码属性写入输出流
    @param [in] outStream 输出流接口指针
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult WriteCoderProperties(ISequentialOutStream* outStream) ;

private:

    /** 大块内存申请
    @param [in] p 指针
    @param [in] size 申请大小
    @return 返回申请的内存指针
    */
    static void* SzBigAlloc(void* p, size_t size) ;

    /** 大块内存释放
    @param [in] p 指针
    @param [in] address 内存地址
    */
    static void  SzBigFree(void* p, void* address) ;

    /** 内存申请
    @param [in] p 指针
    @param [in] size 申请内存大小
    @return 返回申请的内存地址
    */
    static void* SzAlloc(void* p, size_t size) ;

    /** 内存释放
    @param [in] p 指针
    @param [in] address 内存地址
    */
    static void  SzFree(void* p, void *address) ;

    /** 转为大写字符
    @param [in] c 待转字符
    @return 返回大写字符
    */
    static RCString::value_type GetUpperChar(RCString::value_type c) ;

    /** 匹配分析
    @param [in] s 字符串
    @param [in] btMode 模式
    @param [in] numHashBytes 校验位
    @return 返回匹配结果
    */
    static int32_t ParseMatchFinder(const RCString::value_type* s, int32_t* btMode, int32_t* numHashBytes) ;
        
    /** 设置Lzma属性
    @param [in] propID 属性编号 
    @param [in] prop 属性
    @param [in] ep 编码属性
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetLzmaProp(RCCoderPropIDEnumType propID, const RCVariant& prop, CLzmaEncProps& ep) ;
   
private:

    /** Lzma 编码器
    */
    CLzmaEncHandle m_encoder ; 
    
private:

    /** 大内存申请
    */
    static ISzAlloc s_bigAlloc ;

    /** 内存申请
    */
    static ISzAlloc s_alloc ;
};

END_NAMESPACE_RCZIP

#endif //__RCLzmaEncoder_h_

