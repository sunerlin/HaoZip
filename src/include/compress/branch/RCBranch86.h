/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCBranch86_h_
#define __RCBranch86_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** x86分支
*/
class RCBranch86
{
public:

    /**
    */
    uint32_t m_prevMask ;
    
    /** x86初始化
    */
    void x86Init() ;
};

END_NAMESPACE_RCZIP

#endif //__RCBranch86_h_
