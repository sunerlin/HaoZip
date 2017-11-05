/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/bzip2/RCBZip2Handler.h"
#include "format/bzip2/RCBZip2ConstDefs.h"
#include "format/common/RCParseProperties.h"
#include "common/RCStringUtil.h"

/////////////////////////////////////////////////////////////////
//RCBZip2Handler class implementation

BEGIN_NAMESPACE_RCZIP

RCBZip2Handler::RCBZip2Handler()
{
    InitMethodProperties();
}

HResult RCBZip2Handler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo;

    return RC_S_OK;
}

void RCBZip2Handler::InitMethodProperties()
{
    m_level     = 5;
    m_dicSize   = 0xFFFFFFFF;
    m_numPasses = 0xFFFFFFFF;

#ifdef COMPRESS_MT
    m_numThreads = RCSystemUtils::GetNumberOfProcessors();
#else
    m_numThreads = 1 ;
#endif
    m_packSizeDefined = false ;
    m_packSize = 0 ;
    m_startPosition = 0 ;
}

HResult RCBZip2Handler::SetProperties(const RCPropertyNamePairArray& propertyArray)
{
    HResult hr;

    InitMethodProperties() ;
    for (int32_t i = 0; i < (int32_t)propertyArray.size(); i++)
    {
        RCString name = propertyArray[i].first;
        RCStringUtil::MakeUpper(name);

        if (name.empty())
        {
            return RC_E_INVALIDARG;
        }

        const RCVariant& prop = propertyArray[i].second;

        if (name[0] == _T('X'))
        {
            uint32_t level = 9;
            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 1), prop, level);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_level = level;
            continue;
        }

        if (name[0] == _T('D'))
        {
            uint32_t dicSize = RCBZip2ConstDefs::s_bzip2DicSizeX5;
            hr = RCParseProperties::ParsePropDictionaryValue(RCStringUtil::Mid(name, 1), prop, dicSize);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_dicSize = dicSize;
            continue;
        }

        if (RCStringUtil::Left(name, 4) == _T("PASS"))
        {
            uint32_t num = RCBZip2ConstDefs::s_bzip2NumPassesX9;
            hr = RCParseProperties::ParsePropValue(RCStringUtil::Mid(name, 4), prop, num);
            if (hr != RC_S_OK)
            {
                return hr;
            }

            m_numPasses = num;
            continue;
        }

        if (RCStringUtil::Left(name, 2) == _T("MT"))
        {
#ifdef COMPRESS_MT
            uint32_t numProcessors = RCSystemUtils::GetNumberOfProcessors() ;
            hr = RCParseProperties::ParseMtProp(RCStringUtil::Mid(name, 2), prop, numProcessors, m_numThreads);
            if (hr != RC_S_OK)
            {
                return hr;
            }
#endif
            continue;
        }
        return RC_E_INVALIDARG ;
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
