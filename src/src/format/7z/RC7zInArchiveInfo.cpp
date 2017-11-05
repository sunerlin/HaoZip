/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/7z/RC7zInArchiveInfo.h"

/////////////////////////////////////////////////////////////////
//RC7zInArchiveInfo class implementation

BEGIN_NAMESPACE_RCZIP

RC7zInArchiveInfo::RC7zInArchiveInfo():
    m_startPosition(0),
    m_startPositionAfterHeader(0),
    m_dataStartPosition(0),
    m_dataStartPosition2(0)
{
}

RC7zInArchiveInfo::~RC7zInArchiveInfo()
{
}

void RC7zInArchiveInfo::Clear()
{
    m_fileInfoPopIDs.clear();
}

END_NAMESPACE_RCZIP
