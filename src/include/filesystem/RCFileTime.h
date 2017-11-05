/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileTime_h_
#define __RCFileTime_h_ 1

#include "filesystem/RCFileDefs.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCFileTime
{
public:

    /** 将Dos文件时间转换为本地文件时间
    @param [in] dosTime Unix文件时间
    @param [out] fileTime 返回本地文件时间
    @return 成功返回true, 否则返回false
    */
    static bool DosTimeToFileTime(uint32_t dosTime, RC_FILE_TIME& fileTime);
    
    /** 将本地文件时间转换为Dos文件时间
    @param [in] fileTime 本地文件时间
    @param [out] dosTime Dos文件时间
    @return 成功返回true, 否则返回false
    */
    static bool FileTimeToDosTime(const RC_FILE_TIME& fileTime, uint32_t& dosTime);
    
    /** 将Unix文件时间转换为本地文件时间
    @param [in] unixTime Unix文件时间
    @param [out] fileTime 返回本地文件时间
    */
    static void UnixTimeToFileTime(uint32_t unixTime, RC_FILE_TIME& fileTime);
    
    /** 将本地文件时间转换为Unix文件时间
    @param [in] fileTime 本地文件时间
    @param [out] unixTime Unix文件时间
    @return 成功返回true, 否则返回false
    */
    static bool FileTimeToUnixTime(const RC_FILE_TIME& fileTime, uint32_t& unixTime);
    
    /** 获取从1601年起的秒数
    @param [in] year 年
    @param [in] month 月
    @param [in] day 日
    @param [in] hour 时
    @param [in] min 分
    @param [in] sec 秒
    @param [out] resSeconds 返回UTC文件时间
    @return 成功返回true, 否则返回false
    */
    static bool GetSecondsSince1601(uint32_t year, 
                                    uint32_t month, 
                                    uint32_t day,
                                    uint32_t hour, 
                                    uint32_t min, 
                                    uint32_t sec, 
                                    uint64_t& resSeconds);
    
    /** 获取本地UTC文件时间
    @param [out] fileTime 返回UTC文件时间
    */
    static void GetCurUtcFileTime(RC_FILE_TIME& fileTime);
    
    /** 将RC_FILE_TIME转换成64位无符号整型表示
    @param [in] fileTime 文件时间
    @return 返回转换结果
    */
    static uint64_t ConvertFromFileTime(const RC_FILE_TIME& fileTime) ;
    
    /** 将64位无符号整型表示转换成RC_FILE_TIME
    @param [in] time 64位无符号整型表示的文件时间
    @param [out] fileTime 返回文件时间
    */
    static void ConvertToFileTime(uint64_t time, RC_FILE_TIME& fileTime) ;
    
    /**  本地时间转换成UTC时间
    @param [in] fileTime 本地文件时间
    @param [out] utcFileTime 返回UTC 文件时间
    @return 成功返回true, 否则返回false
    */
    static bool LocalFileTimeToFileTime(const RC_FILE_TIME& fileTime, RC_FILE_TIME& utcFileTime) ;
    
    /**  UTC时间转换成本地时间
    @param [in] utcFileTime UTC 文件时间
    @param [out] fileTime 返回本地文件时间
    @return 成功返回true, 否则返回false
    */
    static bool FileTimeToLocalFileTime(const RC_FILE_TIME& utcFileTime, RC_FILE_TIME& fileTime) ;
    
    /** 比较文件时间
    @param [in] time1 第一个文件时间
    @param [in] time2 第二个文件时间
    @return 如果相等返回0, 如果 time1 > time2 返回1，如果time1 < time2 返回 -1
    */
    static int32_t CompareFileTime(const RC_FILE_TIME* time1, const RC_FILE_TIME* time2) ;
};

END_NAMESPACE_RCZIP

#endif //__RCFileTime_h_
