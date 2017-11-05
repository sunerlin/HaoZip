/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzma/RCLzmaArchiveInfo.h"
#include "format/lzma/RCLzmaHandler.h"

/////////////////////////////////////////////////////////////////
//RCLzmaArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCLzmaHandlerIn()
{
    RCLzmaHandler* lzma = new RCLzmaHandler(false) ;
    if (lzma)
    {
        return (IInArchive*)lzma;
    }
    return 0;
}

IOutArchive* CreateRCLzmaHandlerOut()
{
    return 0;
}

RCLzmaArchiveInfo::RCLzmaArchiveInfo()
{
}

RCLzmaArchiveInfo::~RCLzmaArchiveInfo()
{
}

RCArchiveID RCLzmaArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_LZMA ;
}

RCString RCLzmaArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_LZMA ;
}

RCString RCLzmaArchiveInfo::GetExt() const
{
    return _T("lzma") ;
}

RCString RCLzmaArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RCLzmaArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
}

bool RCLzmaArchiveInfo::IsKeepName() const
{
    return true ;
}

PFNCreateInArchive RCLzmaArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRCLzmaHandlerIn ;
}

PFNCreateOutArchive RCLzmaArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRCLzmaHandlerOut ;
}

END_NAMESPACE_RCZIP
