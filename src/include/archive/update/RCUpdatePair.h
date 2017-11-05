/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdatePair_h_
#define __RCUpdatePair_h_ 1

#include "base/RCTypes.h"
#include "common/RCVector.h"
#include "interface/IArchive.h"
#include "filesystem/RCFileInfo.h"
#include "archive/update/RCUpdateAction.h"
#include "archive/common/RCDirItems.h"
#include "archive/common/RCArcItem.h"

BEGIN_NAMESPACE_RCZIP

/** 包内包外文件更新比较
*/
class RCUpdatePair
{
public:
    
    /** 默认构造函数
    */
    RCUpdatePair() : 
        m_arcIndex(-1), 
        m_dirIndex(-1) 
    {}
    
public:

    /** 比较字符串
    @param [in] s1 字符串1
    @param [in] s2 字符串2
    @return = 0, 两串匹配
            < 0, s1 < s2
            > 0, s1 > s2
    */
    static bool CompareStrings(const RCString &s1, const RCString& s2);
    
    /** 比较文件时间
    @param [in] ft1 文件时间1
    @param [in] ft2 文件时间2
    @return = 0, 相等
            < 0, ft1 < ft2
            > 0, ft1 > ft2
    */
    static int32_t CompareFileTime(const RC_FILE_TIME* ft1, const RC_FILE_TIME* ft2);

    /** 比较文件名
    @param [in] s1 文件名1
    @param [in] s2 文件名2
    @return = 0, 文件名相等
            < 0, s1 < s2
            > 0, s1 > s2
    */
    static int32_t CompareFileNames(const RCString &s1, const RCString &s2);

    /** 根据类型比较文件时间
    @param [in] fileTimeType 文件时间类型
    @param [in] time1 文件时间1
    @param [in] time2 文件时间2
    @return = 0, 相等
            < 0, time1 < time2
            > 0, time1 > time2
    */
    static int32_t MyCompareTime(RCArchiveFileTimeType fileTimeType, const RC_FILE_TIME& time1, const RC_FILE_TIME& time2);
    
    /** 文件名排序
    @param [in] strings 文件名列表
    @param [in] indices 序号列表
    */
    static void SortFileNames(RCVector<RCString> &strings, RCIntVector &indices);
    
    /** 检查文件名是否重复
    @param [in] strings 文件名列表
    @param [in] indices 序号列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult TestDuplicateString(const RCVector<RCString> &strings, const RCIntVector &indices);
    
    /** 设置需要更新的文档列表
    @param [in] dirItems 更新项列表
    @param [in] arcItems 包内项列表
    @param [in] fileTimeType 文档时间类型
    @param [out] updatePairs 更新项列表
    @return 成功返回RC_S_OK,否则返回错误号
    */
    static HResult GetUpdatePairInfoList(const RCDirItems &dirItems, 
                                         const RCVector<RCArcItem> &arcItems, 
                                         RCArchiveFileTimeType fileTimeType, 
                                         RCVector<RCUpdatePair> &updatePairs);

public:
    
    /** 对比结果
    */
    RCUpdateDefs::enum_pairstate m_state;
        
    /** 包内项编号
    */
    int32_t m_arcIndex;
    
    /** 更新项编号
    */
    int32_t m_dirIndex;
};

END_NAMESPACE_RCZIP

#endif //__RCUpdatePair_h_
