/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zArchiveInfo.h"
#include "format/7z/RC7zHandler.h"

/////////////////////////////////////////////////////////////////
//RC7zArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

IInArchive* CreateRC7zHandlerIn()
{
    RC7zHandler* in7z = new RC7zHandler;
    if (in7z)
    {
        return (IInArchive*)in7z;
    }
    return NULL ;
}

IOutArchive* CreateRC7zHandlerOut()
{
#ifndef RC_STATIC_SFX 
    RC7zHandler* out7z = new RC7zHandler;
    if (out7z)
    {
        return (IOutArchive*)out7z;
    }
#endif
    return NULL ;
}

RC7zArchiveInfo::RC7zArchiveInfo()
{
}

RC7zArchiveInfo::~RC7zArchiveInfo()
{
}

RCArchiveID RC7zArchiveInfo::GetArchiveID() const
{
    return RC_ARCHIVE_7Z ;
}

RCString RC7zArchiveInfo::GetName() const
{
    return RC_ARCHIVE_TYPE_7Z ;
}

RCString RC7zArchiveInfo::GetExt() const
{
    return _T("7z") ;
}

RCString RC7zArchiveInfo::GetAddExt() const
{
    return _T("") ;
}

void RC7zArchiveInfo::GetSignature(std::vector<byte_t>& signature) const
{
    signature.clear() ;
    signature.reserve(6) ;
    signature.push_back('7') ;
    signature.push_back('z') ;
    signature.push_back(0xBC) ;
    signature.push_back(0xAF) ;
    signature.push_back(0x27) ;
    signature.push_back(0x1C) ;
}

bool RC7zArchiveInfo::IsKeepName() const
{
    return false ;
}

PFNCreateInArchive RC7zArchiveInfo::GetCreateInArchiveFunc(void) const
{
    return CreateRC7zHandlerIn ;
}

PFNCreateOutArchive RC7zArchiveInfo::GetCreateOutArchiveFunc(void) const
{
    return CreateRC7zHandlerOut ;
}

END_NAMESPACE_RCZIP
