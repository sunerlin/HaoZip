/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/arj/RCArjArchiveInfo.h"
#include "format/arj/RCArjHandler.h"

BEGIN_NAMESPACE_RCZIP

static IInArchive* CreateRCArjHandlerIn()
{
    RCArjHandler* arj = new RCArjHandler;
    if (arj)
    {
        return (IInArchive*)arj;
    }
    return 0;
}

static IOutArchive* CreateRCArjHandlerOut()
{
    return 0;
}

RCArjArchiveInfo::RCArjArchiveInfo()
{
}

RCArjArchiveInfo::~RCArjArchiveInfo()
{
}

RCArchiveID RCArjArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_ARJ;
}

RCString RCArjArchiveInfo::GetName() const 
{
    return RC_ARCHIVE_TYPE_ARJ ;
}

RCString RCArjArchiveInfo::GetExt() const 
{
    return _T("arj");
}

RCString RCArjArchiveInfo::GetAddExt() const 
{
    return _T("");
}

void RCArjArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
    signature.push_back(0x60);
    signature.push_back(0xEA);
}

bool RCArjArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCArjArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCArjHandlerIn;
}

PFNCreateOutArchive RCArjArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCArjHandlerOut;
}

END_NAMESPACE_RCZIP
