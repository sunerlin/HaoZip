/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/bzip2/RCBZip2ArchiveInfo.h"
#include "format/bzip2/RCBZip2Handler.h"

/////////////////////////////////////////////////////////////////
//RCBZip2ArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCBZip2HandlerIn()
{
    RCBZip2Handler* bzip2 = new RCBZip2Handler;
    if (bzip2)
    {
        return (IInArchive*)bzip2;
    }
    return 0;
}

IOutArchive* CreateRCBZip2HandlerOut()
{
    RCBZip2Handler* bzip2 = new RCBZip2Handler;
    if (bzip2)
    {
        return (IOutArchive*)bzip2;
    }
    return 0;
}

RCBZip2ArchiveInfo::RCBZip2ArchiveInfo()
{
}

RCBZip2ArchiveInfo::~RCBZip2ArchiveInfo()
{
}

RCArchiveID RCBZip2ArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_BZIP2 ;
}

RCString RCBZip2ArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_BZIP2 ;
}

RCString RCBZip2ArchiveInfo::GetExt() const
{
    return _T("bz2 bzip2 tbz2 tbz") ;
}

RCString RCBZip2ArchiveInfo::GetAddExt() const
{
    return _T("* * .tar .tar") ;
}

void RCBZip2ArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
    signature.reserve(3) ;
    signature.push_back('B') ;
    signature.push_back('Z') ;
    signature.push_back('h') ;
}

bool RCBZip2ArchiveInfo::IsKeepName() const
{
    return true ;
}

PFNCreateInArchive RCBZip2ArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCBZip2HandlerIn ;
}

PFNCreateOutArchive RCBZip2ArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCBZip2HandlerOut ;
}

END_NAMESPACE_RCZIP
