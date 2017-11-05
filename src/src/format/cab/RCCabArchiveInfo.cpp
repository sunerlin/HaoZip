/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/cab/RCCabArchiveInfo.h"
#include "format/cab/RCCabHandler.h"

BEGIN_NAMESPACE_RCZIP

static IInArchive* CreateRCCabHandlerIn()
{
    RCCabHandler* cab = new RCCabHandler;
    if (cab)
    {
        return (IInArchive*)cab;
    }
    return 0;
}

static IOutArchive* CreateRCCabHandlerOut()
{
    return 0;
}

RCCabArchiveInfo::RCCabArchiveInfo() 
{
}

RCCabArchiveInfo::~RCCabArchiveInfo() 
{
}

RCArchiveID RCCabArchiveInfo::GetArchiveID() const 
{
    return RC_ARCHIVE_CAB;
}

RCString RCCabArchiveInfo::GetName() const 
{
    return _T("Cab");
}

RCString RCCabArchiveInfo::GetExt() const 
{
    return RC_ARCHIVE_TYPE_CAB ;
}

RCString RCCabArchiveInfo::GetAddExt() const 
{
    return _T("");
}

void RCCabArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
    signature.reserve(4);
    signature.push_back(0x4D);
    signature.push_back(0x53);
    signature.push_back(0x43);
    signature.push_back(0x46);
}
    
bool RCCabArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCCabArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCCabHandlerIn;
}

PFNCreateOutArchive RCCabArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCCabHandlerOut;
}

END_NAMESPACE_RCZIP