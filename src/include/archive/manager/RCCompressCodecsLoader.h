/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCCompressCodecsLoader_h_
#define __RCCompressCodecsLoader_h_ 1

#include "coder/RCCodecsManager.h"

BEGIN_NAMESPACE_RCZIP

/** 压缩编码器信息接口
*/
class ICompressCodecsInfo ;

/** 从接口中载入编解码 
*/
class RCCompressCodecsLoader:
    public RCCodecsManager
{
public:

    /** 默认构造函数
    */
    RCCompressCodecsLoader() ;
    
    /** 默认析构函数
    */
    virtual ~RCCompressCodecsLoader() ;

public:
    
    /** 从ICompressCodecsInfo中载入编码解码器
    @param [in] codecsInfo 编码解码器的接口指针
    @return 返回成功加载的编码解码器个数
    */
    int32_t LoadCodecs(ICompressCodecsInfo* codecsInfo) ;
};

END_NAMESPACE_RCZIP

#endif //__RCCompressCodecsLoader_h_
