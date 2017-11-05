/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef RCOutMultiVolStream_h_
#define RCOutMultiVolStream_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "interface/IStream.h"
#include "interface/IUnknownImpl.h"
#include "common/RCVector.h"
#include "archive/common/RCTempFiles.h"
#include "filesystem/RCOutFileStream.h"

BEGIN_NAMESPACE_RCZIP

/** 分卷流输出事件接口
*/
class IOutStreamEvent ;

/** 分卷输出流实现
*/
class RCOutMultiVolStream
    : public IUnknownImpl3<IOutStream, IMultiVolStream, IZipMultiVolStream>
{
public:
    
    /** 构造函数
    @param [in] outStreamEvent 分卷流输出事件接口指针
    */
    RCOutMultiVolStream(IOutStreamEvent* outStreamEvent) ;
    
public:
    
    /** 各分卷文件大小
    */
    RCVector<uint64_t> m_sizes;
    
    /** 分卷文件名前缀
    */
    RCString m_prefix;
    
    /** 临时文件
    */
    RCTempFiles* m_tempFiles;

    /** 初始化
    */
    void Init() ; 

    /** 关闭
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Close();
    
    /** 写操作
    @param [in] data 待写数据
    @param [in] size 数据大小
    @param [out] processedSize 返回写入字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Write(const void *data, uint32_t size, uint32_t *processedSize);
    
    /** 移动操作
    @param [in] offset 偏移量
    @param [in] seekOrigin 初始位置
    @param [out] newPosition 新的文件位置
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult Seek(int64_t offset, RC_STREAM_SEEK seekOrigin, uint64_t *newPosition);
    
    /** 设置文件大小
    @param [in] newSize 文件大小
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult SetSize(uint64_t newSize);

    /** 取得当前正在输出的卷编号
    @param [out] volIndex 返回当前卷编号
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetCurVolIndex(uint32_t& volIndex);

    /** 取得当前卷可写字节数
    @param [out] freeByte 返回当前卷可写字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetCurVolFreeByte(uint64_t& freeByte);

    /** 取得当前卷偏移量
    @param [out] position 返回当前卷的文件偏移量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetCurVolPosition(uint64_t& position);

    /** 迫使从新卷开始写文件
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult ForceStartNextVol();

    /** 检查从某位置开始的一段数据是否跨卷
    @param [in] beginPos 起始位置
    @param [in] dataLen 数据长度
    @param [out] result 跨卷返回true,否则返回false
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult CheckDataAcrossVol(uint64_t beginPos, uint64_t dataLen, bool& result);

    /** 取得分卷数量
    @param [out] volCount 分卷数量
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult GetVolumeCount(uint32_t& volCount);

    /** 设置是否生成zip 分卷
    @param [in] toZip  true生成zip分卷 false不生成zip分卷
    @return 成功返回RC_S_OK,否则返回错误号
    */
    virtual HResult SetToZipMultiVolFormat(bool toZip);

public:

    /** 当只有一个卷时，是否自动去掉.001后缀
    @param [in] isAutoRename 是否自动重命名唯一分卷
    */
    void SetAutoRename(bool isAutoRename) ;

private:

    /** 写7z兼容分卷函数实现
    @param [in] data 待写入数据
    @param [in] size 数据长度
    @param [out] processedSize 实际写入字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult  Write7zMultiVolImpl(const void *data, uint32_t size, uint32_t *processedSize);

    /** 写win zip兼容分卷函数实现
    @param [in] data 待写入数据
    @param [in] size 数据长度
    @param [out] processedSize 实际写入字节数
    @return 成功返回RC_S_OK,否则返回错误号
    */
    HResult  WriteZipMultiVolImpl(const void *data, uint32_t size, uint32_t *processedSize);

    /** 如果只有一个分卷,自动去掉.001后缀
    *   如果和压缩包重名文件存在,则取消操作
    @return 重命名成功返回true,否则返回false
    */
    bool RenameSingleVolumn(void);

    /** 重命名zip格式最后一个分卷名
    @return 重命名成功返回true,否则返回false
    */
    bool RenameLastZipVolume(void);

private:
    
    /** 流编号
    */
    int32_t m_streamIndex;
    
    /** 偏移量
    */
    uint64_t m_offsetPos;
    
    /** 决定位置
    */
    uint64_t m_absPos;
    
    /** 文件流长度
    */
    uint64_t m_length;
    
    /** 分卷信息结构
    */
    struct RCSubStreamInfo
    {
        /** 输出文件流
        */
        RCOutFileStream * m_streamSpec;
        
        /** 输出文件流指针
        */
        IOutStreamPtr m_stream;
        
        /** 分卷文件名
        */
        RCString m_name;
        
        /** 分卷文件偏移
        */
        uint64_t m_pos;
        
        /** 分卷文件实际大小
        */
        uint64_t m_RealSize;
        
        /** 分卷是否固定大小
        */
        bool     m_fixed;
    };
    
    /** 分卷信息列表
    */
    RCVector<RCSubStreamInfo> m_streams;
    
private:
    
    /** 写盘事件处理
    */
    IOutStreamEvent* m_outStreamEvent ;

    /** 当只有一个卷时，是否自动去掉.001后缀
    */
    bool m_isAutoRename ;

    /** 是否生成zip分卷
    */
    bool m_isZipSplit;

};

END_NAMESPACE_RCZIP

#endif //RCOutMultiVolStream_h_
