/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/cpio/RCCpioArchiveInfo.h"
#include "format/cpio/RCCpioHandler.h"

/////////////////////////////////////////////////////////////////
//RCCpioArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCCpioHandlerIn()
{
    RCCpioHandler* cpio = new RCCpioHandler;
    if (cpio)
    {
        return (IInArchive*)cpio;
    }
    return 0;
}

IOutArchive* CreateRCCpioHandlerOut()
{
    return 0;
}

RCCpioArchiveInfo::RCCpioArchiveInfo()
{
}

RCCpioArchiveInfo::~RCCpioArchiveInfo()
{
}

RCArchiveID RCCpioArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_CPIO ;
}

RCString RCCpioArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_CPIO ;
}

RCString RCCpioArchiveInfo::GetExt() const
{
    return _T("cpio") ;
}

RCString RCCpioArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RCCpioArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
}

bool RCCpioArchiveInfo::IsKeepName() const
{
    return false ;
}

PFNCreateInArchive RCCpioArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCCpioHandlerIn ;
}

PFNCreateOutArchive RCCpioArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCCpioHandlerOut ;
}

END_NAMESPACE_RCZIP
