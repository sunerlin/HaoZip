/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCIsoDir_h_
#define __RCIsoDir_h_ 1

#include "base/RCTypes.h"
#include "base/RCString.h"
#include "common/RCVector.h"
#include "common/RCStringBuffer.h"
#include "common/RCStringUtil.h"
#include "RCIsoItem.h"
#include "RCIsoHeader.h"

#ifdef RCZIP_OS_WIN
#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define WSTRING_PATH_SEPARATOR L"\\"
#else
#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define WSTRING_PATH_SEPARATOR L"/"
#endif

BEGIN_NAMESPACE_RCZIP

struct RCIsoDir :
    public RCIsoDirRecord
{
    /** 父路径
    */
    RCIsoDir* m_parent;

    /** 子项
    */
    RCVector<RCIsoDir> m_subItems;

    /** 清除
    */
    void Clear();

    /** 获取长度
    @param [in] checkSusp 是否检查susp
    @param [in] skipSize 跳过的大小
    @return 返回长度
    */
    int32_t GetLength(bool checkSusp, int32_t skipSize) const;

    /** 获取长度
    @return 返回长度
    */
    int32_t GetLengthU() const;

    /** 获取路径
    @param [in] checkSusp 是否检查susp
    @param [in] skipSize 跳过的大小
    @return 返回路径
    */
    RCStringA GetPath(bool checkSusp, int32_t skipSize) const;

    /** 获取unicode路径
    @return 返回unicode长度
    */
    RCString GetPathU() const;
};

END_NAMESPACE_RCZIP

#endif //__RCIsoDir_h_