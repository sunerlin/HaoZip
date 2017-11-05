/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/rar/RCRarGetComment.h"
#include "UnRar/rar.hpp"

BEGIN_NAMESPACE_RCZIP

//rar取注释密码询问回调函数
bool RC_OpenCallbackImpl(void* callback_data, char* pwd, size_t pwd_size)
{
    if (callback_data)
    {
        ICryptoGetTextPassword* call = static_cast<ICryptoGetTextPassword*>(callback_data);
        RCString defaultPwd;
        HResult hr = call->CryptoGetTextPassword(defaultPwd);
        if (!IsSuccess(hr))
        {
            return false;
        }
        return WideToChar(defaultPwd.c_str(), pwd, (int)pwd_size);
    }
    return false;
}

#ifndef RCZIP_UNICODE

bool RCRarGetComment::GetRarComment(const char* filename, RCStringA& comment, ICryptoGetTextPassword* callback)
{
    Archive rar_archive;
    rar_archive.m_OpenCallback = RC_OpenCallbackImpl;
    rar_archive.m_CallbackData = callback;
    if (!rar_archive.Open(filename, NULL) || !rar_archive.IsArchive(false))
    {
        return false;
    }
    Array<byte> comment_data;
    if (rar_archive.GetComment(&comment_data, NULL))
    {
        char* pdata;
        int32_t buf_size = comment_data.Size() + 1;
        pdata = new char[buf_size];
        memset(pdata, 0, sizeof(char)*buf_size);
        memcpy(pdata, &comment_data[0], comment_data.Size());
        comment = pdata;
        delete [] pdata;
    }
    else
    {
        return false;
    }
    return true;
}

#else 

bool RCRarGetComment::GetRarComment(const wchar_t* filename, RCStringW& comment, ICryptoGetTextPassword* callback)
{
    Archive rar_archive;
    rar_archive.m_OpenCallback = RC_OpenCallbackImpl;
    rar_archive.m_CallbackData = callback;
    if (!rar_archive.Open(NULL, filename) || !rar_archive.IsArchive(false))
    {
        return false;
    }
    Array<byte> cd;
    Array<wchar_t> comment_data;
    if (rar_archive.GetComment(&cd, &comment_data))
    {
        wchar_t* pdata;
        int32_t buf_size = comment_data.Size() + sizeof(wchar_t);
        pdata = new wchar_t[buf_size];
        memset(pdata, 0, sizeof(wchar_t)*buf_size);
        for (int32_t i = 0; i < comment_data.Size(); i++)
        {
            pdata[i] = comment_data[i];
            if (comment_data[i] == 0)
            {
                break;
            }
        }
        comment = pdata;
        delete [] pdata;
    }
    else
    {
        return false;
    }
    return true;
}

#endif


END_NAMESPACE_RCZIP
