/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "format/tar/RCTarHandler.h"
#include "common/RCCreateCoder.h"
#include "interface/RCMethodDefs.h"

BEGIN_NAMESPACE_RCZIP

HResult RCTarHandler::SetCompressCodecsInfo(ICompressCodecsInfo* compressCodecsInfo)
{
    if (!compressCodecsInfo)
    {
        return RC_S_FALSE;
    }

    m_compressCodecsInfo = compressCodecsInfo ;
    HResult hr = RCCreateCoder::CreateCoder(m_compressCodecsInfo.Get(), RCMethod::ID_COMPRESS_COPY, m_spCopyCoder, false);
    if ( !IsSuccess(hr) || (m_spCopyCoder == NULL) )
    {
        return hr ;
    }
    return RC_S_OK;
}

END_NAMESPACE_RCZIP