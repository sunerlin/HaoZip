/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCMethodProps.h"
#include "interface/RCMethodDefs.h"
#include "interface/ICoder.h"

/////////////////////////////////////////////////////////////////
//RCMethodProps class implementation

BEGIN_NAMESPACE_RCZIP

HResult RCMethodProps::SetMethodProperties(const RCArchiveMethod& method, const uint64_t* inSizeForReduce, IUnknown *coder)
{
    if (coder == NULL)
    {
        return RC_E_INVALIDARG ;
    }

    bool tryReduce = false;
    uint64_t reducedDictionarySize = UINT64_C(1) << 10 ;
    if ( (inSizeForReduce != NULL) && 
         ( (method.m_id == RCMethod::ID_COMPRESS_LZMA) ||(method.m_id == RCMethod::ID_COMPRESS_LZMA2) ) )
    {
        for (;;)
        {
            const uint64_t step = (reducedDictionarySize >> 1);
            if (reducedDictionarySize >= *inSizeForReduce)
            {
                tryReduce = true;
                break;
            }
            reducedDictionarySize += step;
            if (reducedDictionarySize >= *inSizeForReduce)
            {
                tryReduce = true;
                break;
            }
            if (reducedDictionarySize >= (UINT64_C(3) << 30))
            {
                break;
            }
            reducedDictionarySize += step;
        }
    }

    {
        size_t numProperties = method.m_properties.size() ;
        ICompressSetCoderPropertiesPtr setCoderProperties ;
        coder->QueryInterface(IID_ICompressSetCoderProperties, (void**)setCoderProperties.GetAddress()) ;
        if (setCoderProperties == NULL)
        {
            if (numProperties != 0)
            {
                return RC_E_INVALIDARG ;
            }
        }
        else
        {
            RCPropertyIDPairArray propertyArray ;
            HResult res = RC_S_OK ;
            for (size_t i = 0; i < numProperties; i++)
            {
                const RCArchiveProp& prop = method.m_properties[i];
                RCVariant value = prop.m_value;
                if (tryReduce)
                {
                    if (prop.m_id == RCCoderPropID::kDictionarySize)
                    {
                        if (IsInteger64Type(value))
                        {
                            if (reducedDictionarySize < static_cast<uint64_t>(GetInteger64Value(value)))
                            {
                                value = reducedDictionarySize ;
                            }
                        }
                    }
                }
                propertyArray.push_back(RCPropertyIDPair(prop.m_id,value)) ;
            }

            ICompressSetCoderPropertiesPtr setCoderProperties ;
            HResult hr = coder->QueryInterface(IID_ICompressSetCoderProperties, (void **)setCoderProperties.GetAddress());
            if (IsSuccess(hr))
            {
                res = setCoderProperties->SetCoderProperties(propertyArray);
            }
            else
            {
                res = hr ;
            }

            if (!IsSuccess(res))
            {
                return res ;
            }
        }
    }
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
