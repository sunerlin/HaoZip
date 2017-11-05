/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBranchBCJx86Encoder_h_
#define __RCBranchBCJx86Encoder_h_ 1

#include "compress/branch/RCBranchConverter.h"
#include "compress/branch/RCBranch86.h"

BEGIN_NAMESPACE_RCZIP

/** BCJ x86 编码器
*/
class RCBranchBCJx86Encoder:
    public RCBranchConverter,
    public RCBranch86
{
public:
    
    /** 数据过滤器
    @param [in,out] data 数据缓冲区
    @param [in] size 数据长度
    @return 返回过滤字节数
    */
    virtual uint32_t SubFilter(byte_t* data, uint32_t size) ;
    
    /** 初始化
    */
    virtual void SubInit() ;
};

END_NAMESPACE_RCZIP

#endif //__RCBranchBCJx86Encoder_h_
