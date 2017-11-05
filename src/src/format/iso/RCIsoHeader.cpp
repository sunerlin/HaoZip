/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoHeader.h"
#include "format/iso/RCIsoItem.h"

BEGIN_NAMESPACE_RCZIP

const char* RCIsoHeaderDefs::kElToritoSpec = "EL TORITO SPECIFICATION\0\0\0\0\0\0\0\0\0";

const wchar_t* RCIsoHeaderDefs::kMediaTypes[5] =
{
    L"NoEmulation",
    L"1.2M",
    L"1.44M",
    L"2.88M",
    L"HardDisk"
};

END_NAMESPACE_RCZIP
