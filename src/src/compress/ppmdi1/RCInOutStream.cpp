/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
#include "RCInOutStream.h"
#include <stdio.h>

BEGIN_NAMESPACE_RCZIP

int32_t stream_getc(RCInOutStream* inStream)
{
    if (inStream && inStream->m_InStream)
    {
        byte_t val;
        uint32_t proc_size;
        HResult hr = inStream->m_InStream->Read(&val, sizeof(byte_t), &proc_size);
        if (IsSuccess(hr))
        {
            inStream->m_InCount++;
            return val;
        }
    }
    return EOF;
}

int32_t stream_putc(int32_t val ,RCInOutStream* outStream)
{
    if (outStream && outStream->m_OutStream)
    {
        byte_t wval = (byte_t)(val & 0xFF);
        uint32_t proc_size;
        HResult hr = outStream->m_OutStream->Write(&wval, sizeof(wval), &proc_size);
        if (IsSuccess(hr))
        {
            outStream->m_OutCount++;
            if (outStream->m_Progress && (outStream->m_OutCount & 0x1FFF) > 8190)
            {
                hr = outStream->m_Progress->SetRatioInfo(outStream->m_InCount, outStream->m_OutCount);
                if (!IsSuccess(hr))
                {
                    outStream->m_IsCanceled = true;
                }
            }
            return val;
        }
        else
        {
            outStream->m_IsCanceled = true;
        }
    }
    return EOF;
}

END_NAMESPACE_RCZIP
