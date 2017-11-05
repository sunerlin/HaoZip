/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3Filter_h_
#define __RCRar3Filter_h_ 1

#include "RCRar3VmDefs.h"

BEGIN_NAMESPACE_RCZIP

class RCRar3Filter:
    public RCRar3Program
{
public:

    RCVector<byte_t> GlobalData;
    uint32_t BlockStart;
    uint32_t BlockSize;
    uint32_t ExecCount;
    
public:
    RCRar3Filter():
        BlockStart(0),
        BlockSize(0),
        ExecCount(0)
    {        
    }
};

END_NAMESPACE_RCZIP

#endif //__RCRar3Filter_h_
