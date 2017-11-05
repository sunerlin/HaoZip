/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/xar/RCXarArchiveInfo.h"
#include "format/xar/RCXarHandler.h"

/////////////////////////////////////////////////////////////////
//RCXarArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCXarHandlerIn()
{
    RCXarHandler* xar = new RCXarHandler;
    if (xar)
    {
        return (IInArchive*)xar;
    }
    return 0;
}

IOutArchive* CreateRCXarHandlerOut()
{
    return 0;
}

RCXarArchiveInfo::RCXarArchiveInfo()
{
}

RCXarArchiveInfo::~RCXarArchiveInfo()
{
}

RCArchiveID RCXarArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_XAR ;
}

RCString RCXarArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_XAR ;
}

RCString RCXarArchiveInfo::GetExt() const
{
    return _T("xar") ;
}

RCString RCXarArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RCXarArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
    signature.push_back('x') ;
    signature.push_back('a') ;
    signature.push_back('r') ;
    signature.push_back('!') ;
    signature.push_back(0) ;
    signature.push_back(0x1C) ;
}

bool RCXarArchiveInfo::IsKeepName() const
{
    return false ;
}

PFNCreateInArchive RCXarArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCXarHandlerIn ;
}

PFNCreateOutArchive RCXarArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCXarHandlerOut ;
}

END_NAMESPACE_RCZIP
