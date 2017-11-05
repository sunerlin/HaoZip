/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMultiStream_h_
#define __RCMultiStream_h_ 1

#include "interface/IStream.h"
#include "interface/IArchive.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCMultiStream:
    public IUnknownImpl3<IInStream,
                         ICloneStream,
                         IMultiStream>
{
public:

    struct CSubStreamInfo
    {
        /** 输入流
        */
        IInStreamPtr m_spStream ;

        /** 位置
        */
        uint64_t m_pos ;

        /** 大小
        */
        uint64_t m_size ;
    } ;

public:

    /** 默认构造函数
    */
    RCMultiStream() ;
    
    /** 默认析构函数
    */
    ~RCMultiStream() ;
    
public:

    /** 初始化
    */
    void Init() ;
    
    /** 读取数据 
    @param [out] data 存放数据的缓冲区
    @param [in] size 缓冲区大小
    @param [out] processedSize 实际读取的大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Read(void* data, uint32_t size, uint32_t* processedSize) ;
    
    /** 调整指针位置
    @param [in] offset 需要调整的偏移量
    @param [in] seekOrigin 调整的基准位置
    @param [out] newPosition 调整后的文件偏移
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t* newPosition) ;
    
    /** Clone数据流 
    @return Clone 的数据流，如果失败返回NULL
    */
    virtual IUnknown* Clone(void) ;

    /** 取得某分卷大小
    @param [in] index 索引
    @param [in] volumeSize 分卷大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetVolumeSize(uint32_t index, uint64_t& volumeSize);

    /** 取得从开始到某分卷的所有分卷的总大小，计算区间 [0, index)
    @param [in] index 索引
    @param [out] volumeTotalSize 总大小
    @return 成功返回RC_S_OK，否则返回错误号
    */
    virtual HResult GetVolumeTotalSize(uint32_t index, uint64_t& volumeTotalSize);
    
public:

    /** 流的容器
    */
    RCVector<CSubStreamInfo> m_streams ;
    
private:

    /** 流索引
    */
    int32_t  m_streamIndex ;

    /** 位置
    */
    uint64_t m_pos ;

    /** 定位位置
    */
    uint64_t m_seekPos ;

    /** 总长度
    */
    uint64_t m_totalLength ;
};

END_NAMESPACE_RCZIP

#endif //__RCMultiStream_h_
