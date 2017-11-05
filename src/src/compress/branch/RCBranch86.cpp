/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/branch/RCBranch86.h"
#include "algorithm/Bra.h"

/////////////////////////////////////////////////////////////////
//RCBranch86 class implementation

BEGIN_NAMESPACE_RCZIP

void RCBranch86::x86Init()
{
    x86_Convert_Init(m_prevMask);
}

END_NAMESPACE_RCZIP
