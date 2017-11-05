/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdatePair2_h_
#define __RCUpdatePair2_h_ 1

#include "base/RCTypes.h"
#include "common/RCVector.h"
#include "archive/update/RCUpdatePair.h"

BEGIN_NAMESPACE_RCZIP

/** 包内包外更新比较
*/
class RCUpdatePair2
{
public:
    
    /** 默认构造函数
    */
    RCUpdatePair2(): 
        m_isAnti(false), 
        m_dirIndex(-1), 
        m_arcIndex(-1), 
        m_newNameIndex(-1) 
    {}

public:
    
    /** 磁盘上存在
    @return 存在与磁盘上返回true,否则返回false
    */
    bool ExistOnDisk() const 
    { 
        return m_dirIndex != -1; 
    }

    /** 包内存在
    @return 包内存在返回true,否则返回false
    */
    bool ExistInArchive() const 
    { 
        return m_arcIndex != -1; 
    }
    
    /** 更新处理
    @param [in] updatePairs 更新项列表
    @param [in] actionSet 更新操作
    @param [out] operationChain 更新结果
    @return 成功返回true,否则返回false
    */
    static bool UpdateProduce(const RCVector<RCUpdatePair> &updatePairs,
                              const RCActionSet &actionSet,
                              RCVector<RCUpdatePair2> &operationChain);

public:
    
    /** 返回是否为新数据
    @return 新数据返回true,否则返回false
    */
    bool GetNewData(void) const;
    
    /** 设置是否为新数据
    @param [in] newData 新数据
    */
    void SetNewData(bool newData);

    /** 返回是否为新属性
    @return 新属性返回true,否则返回false
    */
    bool GetNewProps(void) const;
    
    /** 设置新属性
    @param [in] newProps 新属性
    */
    void SetNewProps(bool newProps);

    /** 返回是否Anti
    @return Anti返回true,否则返回false
    */
    bool GetIsAnti(void) const;
    
    /** 设置是否为Anti
    @param [in] isAnti 是否Anti
    */
    void SetIsAnti(bool isAnti);

    /** 取得项编号
    @return 返回项编号
    */
    int32_t GetDirIndex(void) const;
    
    /** 设置项编号
    @param [in] dirIndex 项编号
    */
    void SetDirIndex(int32_t dirIndex);

    /** 取得包内编号
    @return 返回包内编号
    */
    int32_t GetArcIndex(void) const;
    
    /** 设置包内编号
    @param [in] arcIndex 包内编号
    */
    void SetArcIndex(int32_t arcIndex);

    /** 返回名称编号
    @return 返回名称编号
    */
    int32_t GetNewNameIndex(void) const;
    
    /** 设置名称编号
    @param [in] newNameIndex 名称编号
    */
    void SetNewNameIndex(int32_t newNameIndex);

private:
    
    /** 新数据
    */
    bool m_newData;
    
    /** 新属性
    */
    bool m_newProps;
    
    /** 是否Anti
    */
    bool m_isAnti;
    
    /** 项编号
    */
    int32_t m_dirIndex;
    
    /** 包内编号
    */
    int32_t m_arcIndex;
    
    /** 名称编号
    */
    int32_t m_newNameIndex;
};

END_NAMESPACE_RCZIP

#endif  //__RCUpdatePair2_h_
