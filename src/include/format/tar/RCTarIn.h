/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTarIn_h_
#define __RCTarIn_h_ 1

#include "interface/IStream.h"
#include "RCTarItem.h"

BEGIN_NAMESPACE_RCZIP

/** Tar 输入
*/
class RCTarIn
{
public:
    
    /** 读取项
    @param [in] stream 输入流
    @param [out] filled 是否填充数据
    @param [out] itemInfo 项列表
    @return 成功返回RC_S_OK，否则返回错误号
    */
    static HResult ReadItem(ISequentialInStream *stream, bool &filled, RCTarItemEx &itemInfo);
};

END_NAMESPACE_RCZIP

#endif //__RCTarIn_h_
