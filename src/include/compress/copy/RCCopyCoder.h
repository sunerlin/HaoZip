/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCopyCoder_h_
#define __RCCopyCoder_h_ 1

#include "interface/ICoder.h"
#include "interface/IUnknownImpl.h"

BEGIN_NAMESPACE_RCZIP

/** Copy 编码器
*/
class RCCopyCoder:
    public IUnknownImpl2<ICompressCoder,
                         ICompressGetInStreamProcessedSize>
{
public:

    /** 默认构造函数
    */
    RCCopyCoder() ;
    
    /** 默认析构函数
    */
    ~RCCopyCoder() ;
    
public:
    
    /** 获取已经处理的大小
    @return 返回已经处理大小
    */
    uint64_t GetTotalSize(void) const ; 
    
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

    /** 获取输入流读取的数据长度
    @param [out] size 返回数据长度
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetInStreamProcessedSize(uint64_t& size) ;
 
private:
    
    /** 缓冲区
    */
    byte_t* m_buffer ;
    
    /** 已经处理的大小
    */
    uint64_t m_totalSize ;
};

END_NAMESPACE_RCZIP

#endif //__RCCopyCoder_h_
