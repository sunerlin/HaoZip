/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/rar/RCRarArchiveInfo.h"
#include "format/rar/RCRarHandler.h"

/////////////////////////////////////////////////////////////////
//RCRarArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCRarHandlerIn()
{
    RCRarHandler* rar = new RCRarHandler;
    if (rar)
    {
        return (IInArchive*)rar;
    }
    return 0;
}

IOutArchive* CreateRCRarHandlerOut()
{
    return 0;
}

RCRarArchiveInfo::RCRarArchiveInfo()
{
}

RCRarArchiveInfo::~RCRarArchiveInfo()
{
}

RCArchiveID RCRarArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_RAR ;
}

RCString RCRarArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_RAR ;
}

RCString RCRarArchiveInfo::GetExt() const
{
    return _T("rar") ;
}

RCString RCRarArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RCRarArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
    signature.reserve(7) ;
    signature.push_back(0x52) ;
    signature.push_back(0x61) ;
    signature.push_back(0x72) ;
    signature.push_back(0x21) ;
    signature.push_back(0x1a) ;
    signature.push_back(0x07) ;
    signature.push_back(0x00) ;
}

bool RCRarArchiveInfo::IsKeepName() const
{
    return false ;
}

PFNCreateInArchive RCRarArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCRarHandlerIn ;
}

PFNCreateOutArchive RCRarArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCRarHandlerOut ;
}

END_NAMESPACE_RCZIP
