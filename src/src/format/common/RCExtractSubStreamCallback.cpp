/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
#include "format/common/RCExtractSubStreamCallback.h"
#include "archive/extract/RCExtractDefs.h"
#include "filesystem/RCOutFileStream.h"

BEGIN_NAMESPACE_RCZIP

RCExtractSubStreamCallback::RCExtractSubStreamCallback():
    m_openCallback(NULL),
    m_fileSize( uint64_t( int64_t(-1)) )
{
    
}

RCExtractSubStreamCallback::~RCExtractSubStreamCallback()
{
    if (m_spOutFileStream != NULL)
    {
        RCOutFileStream* realOut = (RCOutFileStream*)m_spOutFileStream.Get() ;
        realOut->Close() ;
        m_spOutFileStream = NULL ;
    }
}

bool RCExtractSubStreamCallback::Init(const RCString& destFileName, 
                                      uint64_t fileSize,
                                      IArchiveOpenCallback* openCallback)
{
    m_openCallback = openCallback ;
    m_fileSize     = fileSize ;
    m_destFileName = destFileName ;
    m_spOutFileStream = NULL ;
    return true ;
}

HResult RCExtractSubStreamCallback::SetTotal(uint64_t total)
{
    if (m_openCallback)
    {
        return m_openCallback->SetTotal(static_cast<uint64_t>(-1), total);
    }
    else
    {
        return RC_S_OK ;
    }
}
    
HResult RCExtractSubStreamCallback::SetCompleted(uint64_t completed)
{
    if (m_openCallback)
    {
        return m_openCallback->SetCompleted(static_cast<uint64_t>(-1), completed);
    }
    else
    {
        return RC_S_OK ;
    }
}
    
HResult RCExtractSubStreamCallback::GetStream(uint32_t index, ISequentialOutStream** outStream, int32_t /*askExtractMode*/)
{
    RCOutFileStream* realOut = new RCOutFileStream ;
    ISequentialOutStreamPtr spOutFileStream(realOut) ;
    if (!realOut->Open(m_destFileName, OPEN_ALWAYS))
    {
        //如果打开临时文件失败,退出
        return RC_S_FALSE;
    }

    if (outStream)
    {
        if (m_fileSize != uint64_t( int64_t(-1)) )
        {
            realOut->SetSize(m_fileSize) ;
        }
        *outStream = realOut ;
        realOut->AddRef() ;
        m_spOutFileStream = spOutFileStream ;
    }
    return RC_S_OK ;
}
    
HResult RCExtractSubStreamCallback::PrepareOperation(uint32_t index, int32_t askExtractMode)
{
    return RC_S_OK ;
}
    
HResult RCExtractSubStreamCallback::SetOperationResult(uint32_t index, int32_t resultEOperationResult)
{
    if (m_spOutFileStream != NULL)
    {
        RCOutFileStream* realOut = (RCOutFileStream*)m_spOutFileStream.Get() ;
        realOut->Close() ;
        m_spOutFileStream = NULL ;
    }
    return resultEOperationResult == RC_ARCHIVE_EXTRACT_RESULT_OK ? RC_S_OK : RC_E_FAIL ;
}
    
HResult RCExtractSubStreamCallback::SetMultiThreadMode(bool isMultiThreads)
{
    return RC_S_OK  ;
}

HResult RCExtractSubStreamCallback::GetDestinationDirectory(RCString& path)
{
    RCZIP_ASSERT(!"path") ;
    return RC_E_FAIL ;
}
    
HResult RCExtractSubStreamCallback::GetRemovePathParts(RCVector<RCString>& removePathParts)
{
    removePathParts.clear() ;
    return RC_S_OK ;
}

HResult RCExtractSubStreamCallback::GetPathMode(RCExractPathMode& pathMode)
{
    pathMode = RCExtractDefs::RC_EXTRACT_FULL_PATH ;
    return RC_S_OK ;
}

HResult RCExtractSubStreamCallback::GetOverwriteMode(RCExractOverwriteMode& overwriteMode)
{
    overwriteMode = RCExtractDefs::RC_WITHOUT_PROMPT ;
    return RC_S_OK ;
}

HResult RCExtractSubStreamCallback::GetUpdateMode(RCExractUpdateMode& updateMode)
{
    updateMode = RCExtractDefs::RC_EXTRACT_REPLACE ;
    return RC_S_OK ;
}
    
bool RCExtractSubStreamCallback::IsKeepDamageFiles(void)
{
    return false ;
}

END_NAMESPACE_RCZIP
