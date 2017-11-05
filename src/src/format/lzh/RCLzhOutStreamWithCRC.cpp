/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/lzh/RCLzhOutStreamWithCRC.h"

/////////////////////////////////////////////////////////////////
//RCLzhOutStreamWithCRC class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCLzhOutStreamWithCRC::Write(const void* data, uint32_t size, uint32_t* processedSize)
{
    HResult hr;
    uint32_t realProcessedSize;
    if(!m_stream)
    {
        realProcessedSize = size;
        hr = RC_S_OK;
    }
    else
    {
        hr = m_stream->Write(data, size, &realProcessedSize);
    }

    m_crc.Update(data, realProcessedSize);
    if(processedSize != NULL)
    {
        *processedSize = realProcessedSize;
    }

    return hr;
}

END_NAMESPACE_RCZIP
