/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCIsoItem_h_
#define __RCIsoItem_h_ 1

#include "base/RCTypes.h"
#include "filesystem/RCFileDefs.h"
#include "filesystem/RCFileTime.h"
#include "common/RCBuffer.h"
#include "format/iso/RCIsoHeader.h"

BEGIN_NAMESPACE_RCZIP

struct RCIsoRecordingDateTime
{
    /** 年
    */
    byte_t m_year;

    /** 月
    */
    byte_t m_month;

    /** 日
    */
    byte_t m_day;

    /** 小时
    */
    byte_t m_hour;

    /** 分钟
    */
    byte_t m_minute;

    /** 秒
    */
    byte_t m_second;

    /** gmt偏移
    */
    signed char m_gmtOffset;

    /** 获取文件时间
    @param [out] ft 返回文件时间
    @return 获取成功返回true，否则返回false
    */
    bool GetFileTime(RC_FILE_TIME& ft) const;
};

struct RCIsoDirRecord
{
    /** 扩展属性记录长度
    */
    byte_t m_extendedAttributeRecordLen;

    /** 扩展位置
    */
    uint32_t m_extentLocation;

    /** 数据长度
    */
    uint32_t m_dataLength;

    /** 日期时间
    */
    RCIsoRecordingDateTime m_dateTime;

    /** 文集那标志
    */
    byte_t m_fileFlags;

    /** 文件长度
    */
    byte_t m_fileUnitSize;

    /** 除留缺口的大小
    */
    byte_t m_interleaveGapSize;

    /** 卷序列号
    */
    uint16_t m_volSequenceNumber;

    /** 文件id
    */
    RCByteBuffer m_fileId;

    /** 系统用的缓冲区
    */
    RCByteBuffer m_systemUse;

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const;

    /** 是否是系统项
    @return 是返回true，否则返回false
    */
    bool IsSystemItem() const;

    /** 获取susp名字
    @param [in] skipSize 跳过的大小
    @param [out] lenRes 名字长度
    @return 成功返回susp名字，否则返回0
    */
    const byte_t* FindSuspName(int32_t skipSize, int32_t& lenRes) const;

    /** 获取当前长度
    @param [in] checkSusp 是否要检验susp
    @param [in] skipSize 跳过的大小
    @return 返回当前长度
    */
    int32_t GetLengthCur(bool checkSusp, int32_t skipSize) const;

    /** 获取当前名字
    @param [in] checkSusp 是否要检验susp
    @param [in] skipSize 跳过的大小
    @return 返回当前名字
    */
    const byte_t* GetNameCur(bool checkSusp, int32_t skipSize) const;

    /** 检验susp
    @param [in] p 内存数据
    @param [in] startPos 起始偏移
    @return 通过检验返回true，否则返回false
    */
    bool CheckSusp(const byte_t* p, int32_t& startPos) const;

    /** 检验susp
    @param [in] startPos 起始偏移
    @return 通过检验返回true，否则返回false
    */
    bool CheckSusp(int32_t& startPos) const;
};

END_NAMESPACE_RCZIP

#endif //__RCIsoItem_h_
