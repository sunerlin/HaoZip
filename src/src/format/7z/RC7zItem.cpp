/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zItem.h"

/////////////////////////////////////////////////////////////////
//RC7zItem class implementation

BEGIN_NAMESPACE_RCZIP

bool RC7zCoderInfo::IsSimpleCoder() const
{
    return (m_numInStreams == 1) && (m_numOutStreams == 1); 
}

RC7zFolder::RC7zFolder():
    m_unpackCRCDefined(false)
{
}

uint64_t RC7zFolder::GetUnpackSize() const
{
    if (m_unpackSizes.empty())
    {
        return 0;
    }
    for (int32_t i = (int32_t)m_unpackSizes.size() - 1; i >= 0; i--)
    {
        if (FindBindPairForOutStream(i) < 0)
        {
            return m_unpackSizes[i];
        }
    }
    _ThrowCode(RC_E_DataError) ;
    return 0 ;
}

RC7zNum RC7zFolder::GetNumOutStreams() const
{
    RC7zNum result = 0;
    uint32_t count = (uint32_t)m_coders.size() ;
    for (uint32_t i = 0; i < count; i++)
    {
        result += m_coders[i].m_numOutStreams;
    }
    return result;
}

int32_t RC7zFolder::FindBindPairForInStream(RC7zNum inStreamIndex) const
{
    uint32_t count = (uint32_t)m_bindPairs.size() ;
    for(int32_t i = 0; i < (int32_t)count; i++)
    {
        if (m_bindPairs[i].m_inIndex == inStreamIndex)
        {
            return i;
        }
    }
    return -1 ;
}

int32_t RC7zFolder::FindBindPairForOutStream(RC7zNum outStreamIndex) const
{
    uint32_t count = (uint32_t)m_bindPairs.size() ;
    for(int32_t i = 0; i < (int32_t)count; i++)
    {
        if (m_bindPairs[i].m_outIndex == outStreamIndex)
        {
            return i;
        }
    }
      return -1;
}

int32_t RC7zFolder::FindPackStreamArrayIndex(RC7zNum inStreamIndex) const
{
    uint32_t count = (uint32_t)m_packStreams.size() ;
    for(int32_t i = 0; i < (int32_t)count; i++)
    {
        if (m_packStreams[i] == inStreamIndex)
        {
            return i;
        }
    }
    return -1 ;
}

void RC7zUInt64DefVector::Clear()
{
    m_values.clear();
    m_defined.clear();
}

void RC7zUInt64DefVector::ReserveDown()
{
    RCVectorUtils::ReserveDown(m_values) ;
    RCVectorUtils::ReserveDown(m_defined) ;
}

bool RC7zUInt64DefVector::GetItem(int32_t index, uint64_t &value) const
{
    if (index < (int32_t)m_defined.size() && m_defined[index])
    {
        value = m_values[index];
        return true;
    }
    value = 0;
    return false;
}

void RC7zUInt64DefVector::SetItem(int32_t index, bool defined, uint64_t value)
{
    while (index >= (int32_t)m_defined.size())
    {
        m_defined.push_back(false);
    }
    m_defined[index] = defined;
    if (!defined)
    {
        return;
    }
    while (index >= (int32_t)m_values.size())
    {
        m_values.push_back(0);
    }
    m_values[index] = value;
}

bool RC7zUInt64DefVector::CheckSize(int32_t size) const
{
    return (int32_t)m_defined.size() == size || m_defined.size() == 0; 
}

RC7zFileItem::RC7zFileItem():
    m_hasStream(true),
    m_isDir(false),
    m_crcDefined(false),
    m_attribDefined(false)
{    
}

void RC7zFileItem::SetAttrib(uint32_t attrib)
{
    m_attribDefined = true;
    m_attrib = attrib;
}

void RC7zArchiveDatabase::Clear()
{
    m_packSizes.clear();
    m_packCRCsDefined.clear();
    m_packCRCs.clear();
    m_folders.clear();
    m_numUnpackStreamsVector.clear();
    m_files.clear();
    m_cTime.Clear();
    m_aTime.Clear();
    m_mTime.Clear();
    m_startPos.Clear();
    m_isAnti.clear();

    m_commentStartPos = 0;
    m_isCommented = false;
    m_comment.clear();
}

void RC7zArchiveDatabase::ReserveDown()
{
    RCVectorUtils::ReserveDown(m_packSizes);
    RCVectorUtils::ReserveDown(m_packCRCsDefined);
    RCVectorUtils::ReserveDown(m_packCRCs);
    RCVectorUtils::ReserveDown(m_folders);
    RCVectorUtils::ReserveDown(m_numUnpackStreamsVector);
    RCVectorUtils::ReserveDown(m_files);
    m_cTime.ReserveDown();
    m_aTime.ReserveDown();
    m_mTime.ReserveDown();
    m_startPos.ReserveDown();  
    RCVectorUtils::ReserveDown(m_isAnti);
}

bool RC7zArchiveDatabase::IsEmpty() const
{
    return (m_packSizes.empty() &&
            m_packCRCsDefined.empty() &&
            m_packCRCs.empty() &&
            m_folders.empty() &&
            m_numUnpackStreamsVector.empty() &&
            m_files.empty());
}

bool RC7zArchiveDatabase::CheckNumFiles() const
{
    int32_t size = (int32_t)m_files.size() ;

    return (m_cTime.CheckSize(size) &&
            m_aTime.CheckSize(size) &&
            m_mTime.CheckSize(size) &&
            m_startPos.CheckSize(size) &&
            (size == (int32_t)m_isAnti.size() || m_isAnti.size() == 0));
}

bool RC7zArchiveDatabase::IsSolid() const
{
    uint32_t count = (uint32_t)m_numUnpackStreamsVector.size() ;
    for (uint32_t i = 0; i < count; i++)
    {
        if (m_numUnpackStreamsVector[i] > 1)
        {
            return true ;
        }
    }
    return false ;
}

bool RC7zArchiveDatabase::IsItemAnti(int32_t index) const
{
    return (index < (int32_t)m_isAnti.size() && m_isAnti[index]) ;
}

void RC7zArchiveDatabase::SetItemAnti(int32_t index, bool isAnti)
{
    while (index >= (int32_t)m_isAnti.size())
    {
        m_isAnti.push_back(false);
    }
    m_isAnti[index] = isAnti ;
}

void RC7zArchiveDatabase::GetFile(int32_t index, RC7zFileItemPtr &file, RC7zFileItem2Ptr &file2) const
{
    file = m_files[index];
    file2->m_cTimeDefined = m_cTime.GetItem(index, file2->m_cTime);
    file2->m_aTimeDefined = m_aTime.GetItem(index, file2->m_aTime);
    file2->m_mTimeDefined = m_mTime.GetItem(index, file2->m_mTime);
    file2->m_startPosDefined = m_startPos.GetItem(index, file2->m_startPos);
    file2->m_isAnti = IsItemAnti(index);
}

void RC7zArchiveDatabase::AddFile(const RC7zFileItemPtr &file, const RC7zFileItem2Ptr &file2)
{
    int32_t index = (int32_t)m_files.size();
    m_cTime.SetItem(index, file2->m_cTimeDefined, file2->m_cTime);
    m_aTime.SetItem(index, file2->m_aTimeDefined, file2->m_aTime);
    m_mTime.SetItem(index, file2->m_mTimeDefined, file2->m_mTime);
    m_startPos.SetItem(index, file2->m_startPosDefined, file2->m_startPos);
    SetItemAnti(index, file2->m_isAnti);
    m_files.push_back(file);
}

END_NAMESPACE_RCZIP
