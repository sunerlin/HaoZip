/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/z/RCZArchiveInfo.h"
#include "format/z/RCZHandler.h"

BEGIN_NAMESPACE_RCZIP

static IInArchive* CreateRCZHandlerIn()
{
    RCZHandler* z = new RCZHandler;
    if (z)
    {
        return (IInArchive*)z;
    }
    return 0;
}

static IOutArchive* CreateRCZHandlerOut()
{
    return 0;
}

RCZArchiveInfo::RCZArchiveInfo()
{
}

RCZArchiveInfo::~RCZArchiveInfo()
{
}

RCArchiveID RCZArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_Z;
}

RCString RCZArchiveInfo::GetName() const 
{
    return RC_ARCHIVE_TYPE_Z ;
}

RCString RCZArchiveInfo::GetExt() const 
{
    return _T("z taz");
}

RCString RCZArchiveInfo::GetAddExt() const 
{
    //return _T("*.tar");
    return _T("");
}

void RCZArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
    signature.push_back(0x1F);
    signature.push_back(0x9D);
}

bool RCZArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCZArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCZHandlerIn;
}

PFNCreateOutArchive RCZArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCZHandlerOut;
}

END_NAMESPACE_RCZIP