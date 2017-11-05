/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/iso/RCIsoArchiveInfo.h"
#include "format/iso/RCIsoHandler.h"

BEGIN_NAMESPACE_RCZIP

static IInArchive* CreateRCIsoHandlerIn()
{
    RCIsoHandler* iso = new RCIsoHandler;
    if (iso)
    {
        return (IInArchive*)iso;
    }
    return 0;
}

static IOutArchive* CreateRCIsoHandlerOut()
{
    return 0;
}

RCIsoArchiveInfo::RCIsoArchiveInfo()
{
}

RCIsoArchiveInfo::~RCIsoArchiveInfo()
{
}

RCArchiveID RCIsoArchiveInfo::GetArchiveID() const 
{
    return RC_ARCHIVE_ISO;
}

RCString RCIsoArchiveInfo::GetName() const 
{
    return RC_ARCHIVE_TYPE_ISO ;
}

RCString RCIsoArchiveInfo::GetExt() const 
{
    return _T("iso");
}

RCString RCIsoArchiveInfo::GetAddExt() const 
{
    return _T("");
}

void RCIsoArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
    signature.reserve(7);
    signature.push_back('C');
    signature.push_back('D');
    signature.push_back('0');
    signature.push_back('0');
    signature.push_back('1');
    signature.push_back(0x1);
    signature.push_back(0);
}

bool RCIsoArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCIsoArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCIsoHandlerIn;
}

PFNCreateOutArchive RCIsoArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCIsoHandlerOut;
}

END_NAMESPACE_RCZIP
