/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/udf/RCUdfArchiveInfo.h"
#include "format/udf/RCUdfHandler.h"

BEGIN_NAMESPACE_RCZIP

static IInArchive* CreateRCUdfHandlerIn()
{
    RCUdfHandler* udf = new RCUdfHandler;
    if (udf)
    {
        return (IInArchive*)udf;
    }
    return 0;
}

static IOutArchive* CreateRCUdfHandlerOut()
{
    return 0;
}

RCUdfArchiveInfo::RCUdfArchiveInfo()
{
}

RCUdfArchiveInfo::~RCUdfArchiveInfo()
{
}

RCArchiveID RCUdfArchiveInfo::GetArchiveID() const 
{
    return RC_ARCHIVE_UDF;
}

RCString RCUdfArchiveInfo::GetName() const 
{
    return RC_ARCHIVE_TYPE_UDF ;
}

RCString RCUdfArchiveInfo::GetExt() const 
{
    return _T("iso");
}

RCString RCUdfArchiveInfo::GetAddExt() const 
{
    return _T("");
}

void RCUdfArchiveInfo::GetSignature(std::vector<byte_t>& signature) const 
{
    signature.clear();
    signature.reserve(5);
    signature.push_back(0);
    signature.push_back('N');
    signature.push_back('S');
    signature.push_back('R');
    signature.push_back('0');
}
    
bool RCUdfArchiveInfo::IsKeepName() const 
{
    return false;
}

PFNCreateInArchive RCUdfArchiveInfo::GetCreateInArchiveFunc(void) const 
{
    return CreateRCUdfHandlerIn;
}

PFNCreateOutArchive RCUdfArchiveInfo::GetCreateOutArchiveFunc(void) const 
{
    return CreateRCUdfHandlerOut;
}

END_NAMESPACE_RCZIP
