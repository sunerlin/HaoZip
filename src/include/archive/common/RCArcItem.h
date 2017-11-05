/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArcItem_h_
#define __RCArcItem_h_ 1

#include "base/RCString.h"
#include "filesystem/RCFileDefs.h"

BEGIN_NAMESPACE_RCZIP

/** 包内项信息
*/
class RCArcItem
{
public:

    /** 默认构造函数
    */
    RCArcItem() ;
    
    /** 默认析构函数
    */
    ~RCArcItem() ;
    
public:
    
    /** 设置大小
    @param [in] size 项大小
    */
    void SetSize(uint64_t size) ;
    
    /** 设置修改实现
    @param [in] mTime 修改实现
    */
    void SetModifyTime(const RC_FILE_TIME& mTime);
    
    /** 设置项名称
    @param [in] name 项名称
    */
    void SetName(const RCString& name) ;
    
    /** 设置是否为目录
    @param [in] isDir 是否为目录
    */
    void SetIsDir(bool isDir);
    
    /** 设置是否定义大小
    @param [in] sizeDefined 大小是否定义
    */
    void SetSizeDefined(bool sizeDefined) ;
    
    /** 设置是否定义修改时间
    @param [in] mTimeDefined 修改时间是否定义
    */
    void SetMTimeDefined(bool mTimeDefined) ;
    
    /** 设置是否通配符匹配
    @param [in] censored 是否通配符匹配
    */
    void SetCensored(bool censored) ;
    
    /** 设置是否有序号
    @param [in] indexInServer 索引序号
    */
    void SetIndexInServer(uint32_t indexInServer);
    
    /** 设置时间类型
    @param [in] timeType 时间类型
    */
    void SetTimeType(int32_t timeType) ;
    
    /** 返回项大小
    @return 返回项大小
    */
    uint64_t GetSize(void) const ;
    
    /** 返回项修改时间
    @return 返回项修改时间
    */
    const RC_FILE_TIME& GetModifyTime(void) const ;
    
    /** 返回项名称
    @return 返回项名称引用
    */
    RCString& GetName(void);
    
    /** 返回项名称
    @return 返回项名称
    */
    const RCString& GetName(void) const;
    
    /** 返回是否为目录
    @return 是目录返回 true,否则返回 false
    */
    bool IsDir(void) const ;
    
    /** 返回是否定义大小
    @return 定义大小返回 true,否则返回 false
    */
    bool GetSizeDefined(void) const ;
    
    /** 返回是否定义修改时间
    @return 定义修改时间返回 true,否则返回 false
    */
    bool GetMTimeDefined(void) const ;
    
    /** 返回是否通配符匹配
    @return 匹配返回true,否则返回false
    */
    bool GetCensored(void) const ;
    
    /** 返回序号
    @return 返回序号
    */
    uint32_t GetIndexInServer(void) const ;
    
    /** 返回时间类型
    @return 返回时间类型
    */
    int32_t GetTimeType(void) const ;
    
private:
    
    /** 大小
    */
    uint64_t m_size ;

    /** 修改时间
    */
    RC_FILE_TIME m_mTime ;

    /** 名称
    */
    RCString m_name ;

    /** 是否目录
    */
    bool m_isDir ;

    /** 是否含有大小值
    */
    bool m_sizeDefined ;
    
    /** 是否含有修改时间
    */
    bool m_mTimeDefined ;

    /** 是否通配符匹配
    */
    bool m_censored;

    /** 序号
    */
    uint32_t m_indexInServer;

    /** 时间类型
    */
    int32_t m_timeType ;
};

END_NAMESPACE_RCZIP

#endif //__RCArcItem_h_
