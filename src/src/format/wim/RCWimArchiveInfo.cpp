/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/wim/RCWimArchiveInfo.h"
#include "format/wim/RCWimHandler.h"

/////////////////////////////////////////////////////////////////
//RCWimArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCWimHandlerIn()
{
    RCWimHandler* wim = new RCWimHandler;
    if (wim)
    {
        return (IInArchive*)wim;
    }
    return 0;
}

IOutArchive* CreateRCWimHandlerOut()
{
    return 0;
}

RCWimArchiveInfo::RCWimArchiveInfo()
{
}

RCWimArchiveInfo::~RCWimArchiveInfo()
{
}

RCArchiveID RCWimArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_WIM ;
}

RCString RCWimArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_WIM ;
}

RCString RCWimArchiveInfo::GetExt() const
{
    return _T("wim swm") ;
}

RCString RCWimArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RCWimArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
    signature.push_back('M') ;
    signature.push_back('S') ;
    signature.push_back('W') ;
    signature.push_back('I') ;
    signature.push_back('M') ;
    signature.push_back(0) ;
    signature.push_back(0) ;
    signature.push_back(0) ;
}

bool RCWimArchiveInfo::IsKeepName() const
{
    return false ;
}

PFNCreateInArchive RCWimArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCWimHandlerIn ;
}

PFNCreateOutArchive RCWimArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCWimHandlerOut ;
}

END_NAMESPACE_RCZIP
