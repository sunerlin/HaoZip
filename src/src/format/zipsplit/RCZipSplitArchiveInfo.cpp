/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/zipsplit/RCZipSplitArchiveInfo.h"
#include "format/zipsplit/RCZipSplitHandler.h"
#include "common/RCStringUtil.h"

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRCZipSplitHandlerIn()
{
    RCZipSplitHandler* split = new RCZipSplitHandler;
    if (split)
    {
        return (IInArchive*)split;
    }
    return 0;
}

IOutArchive* CreateRCZipSplitHandlerOut()
{
    return 0;
}

RCZipSplitArchiveInfo::RCZipSplitArchiveInfo()
{
}

RCZipSplitArchiveInfo::~RCZipSplitArchiveInfo()
{
}

RCArchiveID RCZipSplitArchiveInfo::GetArchiveID() const 
{
    return RC_ARCHIVE_ZIP_SPLIT;
}

RCString RCZipSplitArchiveInfo::GetName() const 
{
    return RC_ARCHIVE_TYPE_ZIP_SPLIT;
}

RCString RCZipSplitArchiveInfo::GetExt() const 
{   //注册最多99个分卷的后缀名
    RCString ret;
    for (uint32_t i = 1; i < 100; i++)
    {
        RCString ext;
        RCStringUtil::Format(ext, _T("z%.2d"), i);
        if (!ret.empty())
        {
            ret += _T(" ");
        }
        ret += ext;
    }
    return ret;
}

RCString RCZipSplitArchiveInfo::GetAddExt() const 
{
    return _T("");
}

void RCZipSplitArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
}
    
bool RCZipSplitArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCZipSplitArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCZipSplitHandlerIn;
}

PFNCreateOutArchive RCZipSplitArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCZipSplitHandlerOut;
}

END_NAMESPACE_RCZIP
