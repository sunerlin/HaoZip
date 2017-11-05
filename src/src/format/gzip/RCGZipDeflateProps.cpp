/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/gzip/RCGZipDeflateProps.h"
#include "format/common/RCParseProperties.h"
#include "format/gzip/RCGZipDefs.h"
#include "common/RCStringUtil.h"

BEGIN_NAMESPACE_RCZIP

RCGZipDeflateProps::RCGZipDeflateProps()
{
    Init() ;
}

void RCGZipDeflateProps::Init()
{
    m_level = m_numPasses = m_fb = m_algo = m_mc = 0xFFFFFFFF ;
    m_mcDefined = false ;
}

void RCGZipDeflateProps::Normalize()
{
    uint32_t level = m_level;
    if (level == 0xFFFFFFFF)
    {
        level = 5;
    }
  
    if (m_algo == 0xFFFFFFFF)
    {
        m_algo = (level >= 5 ? RCGZipDefs::s_algo5 : RCGZipDefs::s_algo1);
    }

    if (m_numPasses == 0xFFFFFFFF)
    {
        m_numPasses = (level >= 9 ? RCGZipDefs::s_passes9 : 
                                    (level >= 7 ? RCGZipDefs::s_passes7 : RCGZipDefs::s_passes1)) ;
    }
    if (m_fb == 0xFFFFFFFF)
    {
        m_fb = (level >= 9 ? RCGZipDefs::s_fastBytes9 : 
                             (level >= 7 ? RCGZipDefs::s_fastBytes7 : RCGZipDefs::s_fastBytes1)) ;
    }
}

bool RCGZipDeflateProps::IsMaximum() const
{
    return m_algo > 0 ;
}

HResult RCGZipDeflateProps::SetCoderProperties(ICompressSetCoderProperties* setCoderProperties)
{
    Normalize() ;
    RCPropertyIDPairArray propArray ;
    propArray.push_back( RCPropertyIDPair(RCCoderPropID::kAlgorithm,    RCVariant( uint64_t(m_algo) )) ) ;
    propArray.push_back( RCPropertyIDPair(RCCoderPropID::kNumPasses,    RCVariant( uint64_t(m_numPasses) )) ) ;
    propArray.push_back( RCPropertyIDPair(RCCoderPropID::kNumFastBytes, RCVariant( uint64_t(m_fb) )) ) ;
    if(m_mcDefined)
    {
        propArray.push_back( RCPropertyIDPair(RCCoderPropID::kMatchFinderCycles, RCVariant( uint64_t(m_mc) )) ) ;
    }
    return setCoderProperties->SetCoderProperties(propArray) ;
}

HResult RCGZipDeflateProps::SetProperties(const RCPropertyNamePairArray& propertyArray)
{
    Init();
    RCPropertyNamePairArray::const_iterator pos = propertyArray.begin() ;
    for(; pos != propertyArray.end(); ++pos)
    {
        const RCPropertyNamePair& namePair = *pos ;
        RCString propName     = namePair.first ;
        const RCVariant& prop = namePair.second ;
        RCStringUtil::MakeUpper(propName) ;
        
        if(propName.empty())
        {
            return RC_E_INVALIDARG ;
        }
        
        if (propName[0] == _T('X'))
        {
            uint32_t a = 9 ;
            HResult hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(propName,1), prop, a) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_level = a ;
        }
        else if (RCStringUtil::Left(propName, 1) == _T("A"))
        {
            uint32_t a = RCGZipDefs::s_algo5;
            HResult hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(propName,1), prop, a) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_algo = a ;
        }
        else if (RCStringUtil::Left(propName, 4) == _T("PASS") )
        {
            uint32_t a = RCGZipDefs::s_passes9 ;
            HResult hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(propName, 4), prop, a) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_numPasses = a;
        }
        else if (RCStringUtil::Left(propName, 2) == _T("FB") )
        {
            uint32_t a = RCGZipDefs::s_fastBytes9;
            HResult hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(propName, 2), prop, a) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_fb = a;
        }
        else if (RCStringUtil::Left(propName, 2) == _T("MC") )
        {
            uint32_t a = 0xFFFFFFFF;
            HResult hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(propName, 2), prop, a) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            m_mc = a ;
            m_mcDefined = true ;
        }
        else
        {
            return RC_E_INVALIDARG ;
        }        
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
