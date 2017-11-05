/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/rpm/RCRpmHeader.h"
#include "filesystem/RCStreamUtils.h"
#include "algorithm/CpuArch.h"

#define Get16(p) GetBe16(p)
#define Get32(p) GetBe32(p)

#define RPMSIG_NONE         0  /* Do not change! */
#define RPMSIG_PGP262_1024  1  /* No longer generated */ /* 256 byte */
#define RPMSIG_HEADERSIG    5  /* New Header style signature */

BEGIN_NAMESPACE_RCZIP

bool RCRpmLead::MagicCheck() const
{ 
    return m_magic[0] == 0xed && m_magic[1] == 0xab && m_magic[2] == 0xee && m_magic[3] == 0xdb; 
}

bool RCRpmSigHeaderSig::MagicCheck()
{ 
    return m_magic[0] == 0x8e && m_magic[1] == 0xad && m_magic[2] == 0xe8 && m_magic[3] == 0x01; 
}

uint32_t RCRpmSigHeaderSig::GetLostHeaderLen()
{ 
    return m_indexLen * RCRpmDefs::s_entryInfoSize + m_dataLen;  
}

HResult RCRpmFuncWrapper::ReadSigHeaderSig(IInStream *inStream, RCRpmSigHeaderSig &h)
{
    HResult result;
    char dat[RCRpmDefs::s_csigHeaderSigSize];
    char *cur = dat;
    result = (RCStreamUtils::ReadStream_FALSE(inStream, dat, RCRpmDefs::s_csigHeaderSigSize));
    if (!IsSuccess(result))
    {
        return result;
    }
    memmove(h.m_magic, cur, 4);
    cur += 4;
    cur += 4;
    h.m_indexLen = Get32(cur);
    cur += 4;
    h.m_dataLen = Get32(cur);
    return RC_S_OK;
}

HResult RCRpmFuncWrapper::OpenArchive(IInStream *inStream)
{
    HResult result;
    uint64_t pos;
    char leadData[RCRpmDefs::s_leadSize];
    char *cur = leadData;
    RCRpmLead lead;
    result = (RCStreamUtils::ReadStream_FALSE(inStream, leadData, RCRpmDefs::s_leadSize));
    if (!IsSuccess(result))
    {
        return result;
    }
    memmove(lead.m_magic, cur, 4);
    cur += 4;
    lead.m_major = *cur++;
    lead.m_minor = *cur++;
    lead.m_type = Get16(cur);
    cur += 2;
    lead.m_archNum = Get16(cur);
    cur += 2;
    memmove(lead.m_name, cur, sizeof(lead.m_name));
    cur += sizeof(lead.m_name);
    lead.m_osNum = Get16(cur);
    cur += 2;
    lead.m_signatureType = Get16(cur);
    cur += 2;

    if (!lead.MagicCheck() || lead.m_major < 3)
    {
        return RC_S_FALSE;
    }

    RCRpmSigHeaderSig sigHeader, header;
    if(lead.m_signatureType == RPMSIG_NONE)
    {
        ;
    }
    else if(lead.m_signatureType == RPMSIG_PGP262_1024)
    {
        uint64_t pos;
        result = inStream->Seek(256, RC_STREAM_SEEK_CUR, &pos) ;
        if (!IsSuccess(result))
        {
            return result;
        }
    }
    else if(lead.m_signatureType == RPMSIG_HEADERSIG)
    {
        result = (ReadSigHeaderSig(inStream, sigHeader));
        if (!IsSuccess(result))
        {
            return result;
        }
        if(!sigHeader.MagicCheck())
        {
            return RC_S_FALSE;
        }
        uint32_t len = sigHeader.GetLostHeaderLen();
        result = inStream->Seek(len, RC_STREAM_SEEK_CUR, &pos) ;
        if (!IsSuccess(result))
        {
            return result;
        }
        if((pos % 8) != 0)
        {
            result = inStream->Seek((pos / 8 + 1) * 8 - pos, RC_STREAM_SEEK_CUR, &pos) ;
            if (!IsSuccess(result))
            {
                return result;
            }
        }
    }
    else
    {
        return RC_S_FALSE;
    }
    result = ReadSigHeaderSig(inStream, header) ;
    if (!IsSuccess(result))
    {
        return result;
    }
    if(!header.MagicCheck())
    {
        return RC_S_FALSE;
    }
    int32_t headerLen = header.GetLostHeaderLen();
    if(headerLen == -1)
    {
        return RC_S_FALSE;
    }
    result = inStream->Seek(headerLen, RC_STREAM_SEEK_CUR, &pos) ;
    if (!IsSuccess(result))
    {
        return result;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP

