/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
#ifndef __RCUpdateAction_h
#define __RCUpdateAction_h 1

#include "base/RCSingleton.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩更新常量枚举定义
*/
class RCUpdateDefs
{
public:
    
    enum 
    {
        KNUMVALUES = 7,
    };

    /** 比较结果
    */
    enum enum_pairstate
    {
        KNOTMASKED = 0,
        KONLYINARCHIVE,
        KONLYONDISK,
        KNEWINARCHIVE,
        KOLDINARCHIVE,
        KSAMEFILES,
        KUNKNOWNEWERFILES
    };

    /** 更新操作
    */
    enum enum_pairaction
    {
        KIGNORE = 0,
        KCOPY,
        KCOMPRESS,
        KCOMPRESSASANTI
    };
};

typedef RCUpdateDefs::enum_pairstate    RCUpdatePairState;
typedef RCUpdateDefs::enum_pairaction   RCUpdatePairAction;

/** 更新操作集合
*/
class RCActionSet
{
public:
    
    /** 默认构造函数
    */
    RCActionSet(): 
        m_StateActions(RCUpdateDefs::KNUMVALUES) 
    {}
    
public:
    
    /** =操作符重载
    */
    RCActionSet& operator=(const RCActionSet& val);
    
public:
    
    /** 是否需扫描磁盘
    @return 是返回true,否则返回false
    */
    bool NeedScanning() const;
    
    /** 返回更新操作列表
    @return 返回更新操作列表
    */
    RCVector<RCUpdatePairAction> & GetStateActions(void);
    
    /** 返回更新操作列表
    @return 返回更新操作列表
    */
    const RCVector<RCUpdatePairAction>& GetStateActions(void) const;
    
private:
    
    /** 更新操作列表
    */
    RCVector<RCUpdatePairAction> m_StateActions;
};

/** 全局更新操作定义
*/
struct RCGlobalActionSet
{
    /** 新建操作
    */
    RCActionSet m_AddActionSet;
    
    /** 更新操作
    */
    RCActionSet m_UpdateActionSet;
    
    /** 刷新操作
    */
    RCActionSet m_FreshActionSet;
    
    /** 同步操作
    */
    RCActionSet m_SynchronizeActionSet;
    
    /** 删除操作
    */
    RCActionSet m_DeleteActionSet;
    
    /** 默认构造函数
    */
    RCGlobalActionSet();
};

/** 定义单例作为全局访问点
*/
typedef RCSingleton<RCGlobalActionSet> Singleton_GlobalActionSet;

END_NAMESPACE_RCZIP

#endif //__RCUpdateAction_h
