/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMethodProps_h_
#define __RCMethodProps_h_ 1

#include "interface/RCPropertyID.h"
#include "common/RCVariant.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

struct RCArchiveProp
{
    /** 属性id
    */
    RCPropertyID m_id ;

    /** 属性值
    */
    RCVariant m_value ;
};

struct RCArchiveMethod
{
    /** 方法id
    */
    RCMethodID m_id ;

    /** 方法值
    */
    RCVector<RCArchiveProp> m_properties ;
};

class IUnknown ;

class RCMethodProps
{
public:

    /** 设置方法属性
    @param [in] method 方法
    @param [in] inSizeForReduce 输入大小
    @param [in] coder 编码
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult SetMethodProperties(const RCArchiveMethod& method, 
                                       const uint64_t* inSizeForReduce, 
                                       IUnknown* coder);
};

END_NAMESPACE_RCZIP

#endif //__RCMethodProps_h_
