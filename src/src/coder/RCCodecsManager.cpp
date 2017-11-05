/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "coder/RCCodecsManager.h"

/////////////////////////////////////////////////////////////////
//RCCodecsManager class implementation

BEGIN_NAMESPACE_RCZIP

RCCodecsManager::RCCodecsManager()
{
}

RCCodecsManager::~RCCodecsManager()
{
    m_codecArray.clear() ;
}

HResult RCCodecsManager::GetNumberOfMethods(uint32_t& numMethods) const
{
    numMethods = static_cast<uint32_t>(m_codecArray.size()) ;
    return RC_S_OK ;
}

HResult RCCodecsManager::GetMethod(uint32_t index, ICodecInfoPtr& spCodecInfo) const
{
    if(index < m_codecArray.size())
    {
        spCodecInfo = m_codecArray[index] ;
        return RC_S_OK ;
    }
    else
    {
        return RC_E_INVALIDARG ;
    }
}

HResult RCCodecsManager::DoCreateCoder(uint32_t index, RC_IID iid, bool isEncoder, void** coder) const
{
    if(coder == NULL)
    {
        return RC_E_INVALIDARG ;
    }
    bool isCoder = (iid == IID_ICompressCoder) ;
    bool isCoder2 = (iid == IID_ICompressCoder2) ;
    bool isFilter = (iid == IID_ICompressFilter) ;
    if(!isCoder && !isCoder2 && !isFilter)
    {
        return RC_E_NOINTERFACE ;
    }
    
    ICodecInfoPtr spCodecInfo ;
    HResult hr = GetMethod(index,spCodecInfo) ;
    if(hr != RC_S_OK)
    {
        return hr ;
    }
    if( (!isFilter && spCodecInfo->IsFilter()) || 
        (isFilter && !spCodecInfo->IsFilter())   ||
        (spCodecInfo->GetNumInStreams() != 1 && !isCoder2) || 
        (spCodecInfo->GetNumInStreams() == 1 && isCoder2) )
    {
        return RC_E_NOINTERFACE ;
    }
    if(isEncoder)
    {
        if(spCodecInfo->GetEncoderIID() != iid)
        {
            return RC_E_NOINTERFACE ;
        }
        PFNCreateCodec pfnCreateCoder = spCodecInfo->GetCreateEncoderFunc() ;
        if(pfnCreateCoder == NULL)
        {
            return RC_E_CLASSNOTAVAILABLE ;
        }
        *coder = (*pfnCreateCoder)() ;        
    }
    else
    {
        if(spCodecInfo->GetDecoderIID() != iid)
        {
            return RC_E_NOINTERFACE ;
        }
        PFNCreateCodec pfnCreateCoder = spCodecInfo->GetCreateDecoderFunc() ;
        if(pfnCreateCoder == NULL)
        {
            return RC_E_CLASSNOTAVAILABLE ;
        }
        *coder = (*pfnCreateCoder)() ;
    }
    if(*coder == NULL)
    {
        return RC_E_FAIL ;
    }
    if (isCoder)
    {
        ((ICompressCoder *)*coder)->AddRef() ;
    }
    else if (isCoder2)
    {
        ((ICompressCoder2 *)*coder)->AddRef() ;
    }
    else
    {
        ((ICompressFilter *)*coder)->AddRef() ;
    }
    return RC_S_OK ;
}

HResult RCCodecsManager::CreateDecoder(uint32_t index, RC_IID iid, void** coder) const
{
    return DoCreateCoder(index,iid,false,coder) ;
}

HResult RCCodecsManager::CreateEncoder(uint32_t index, RC_IID iid, void** coder) const
{
    return DoCreateCoder(index,iid,true,coder) ;
}

const RCCodecsManager::container_type& RCCodecsManager::GetCodecArray(void) const
{
    return m_codecArray ;
}

bool RCCodecsManager::RegisterCodec(const ICodecInfoPtr& spCodecInfo)
{
    if(spCodecInfo != NULL)
    {
        m_codecArray.push_back(spCodecInfo) ;
        return true ;
    }
    return false ;
}

END_NAMESPACE_RCZIP
