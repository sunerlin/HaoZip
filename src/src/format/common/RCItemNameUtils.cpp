/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCItemNameUtils.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCItemNameUtils class implementation

BEGIN_NAMESPACE_RCZIP

#define kOSDirDelimiter _T('\\')
#define kDirDelimiter   _T('/')

RCItemNameUtils::RCItemNameUtils()
{
}

RCItemNameUtils::~RCItemNameUtils()
{
}

RCString RCItemNameUtils::MakeLegalName(const RCString& name)
{
    RCString zipName = name ;
    RCString::size_type len = zipName.size() ;
    for (RCString::size_type index = 0; index < len; ++index)
    {
        if (kOSDirDelimiter == zipName[index])
        {
            zipName[index] = kDirDelimiter ;
        }
    }
    return zipName ;
}

RCString RCItemNameUtils::GetOSName(const RCString& name)
{
    RCString newName = name ;
    RCString::size_type len = newName.size() ;
    for (RCString::size_type index = 0; index < len; ++index)
    {
        if (kDirDelimiter == newName[index])
        {
            newName[index] = kOSDirDelimiter ;
        }
    }
    return newName ;
}

RCString RCItemNameUtils::GetOSName2(const RCString& name)
{
    if (name.empty())
    {
        return RCString() ;
    }
    RCString newName = GetOSName(name) ;
    if (newName[newName.size() - 1] == kOSDirDelimiter)
    {
        newName.erase(newName.size() - 1) ;
    }
    return newName ;
}

bool RCItemNameUtils::HasTailSlash(const RCStringA& name, uint32_t codePage)
{
    RCStringW wName = RCStringConvert::MultiByteToUnicodeString(name,codePage) ;
    if (wName.empty())
    {
        return false ;
    }
#ifdef RCZIP_OS_WIN
    return (wName[wName.size() - 1] == kOSDirDelimiter) ||
           (wName[wName.size() - 1] == kDirDelimiter) ;
#else
    return (wName[wName.size() - 1] == kDirDelimiter) ;
#endif
    return false ;
}

RCString RCItemNameUtils::WinNameToOSName(const RCString& name)
{
    RCString result(name) ;
    RCStringUtil::Replace(result, _T("\\"),_T("/")) ;
    return result ;
}

END_NAMESPACE_RCZIP
