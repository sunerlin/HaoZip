/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/xar/RCXarFile.h"

/////////////////////////////////////////////////////////////////
//RCXarFile class implementation

BEGIN_NAMESPACE_RCZIP

RCXarFile::RCXarFile():
    m_isDir(false), 
    m_hasData(false), 
    m_sha1IsDefined(false),
    m_parent(-1), 
    m_size(0), 
    m_packSize(0), 
    m_cTime(0), 
    m_mTime(0), 
    m_aTime(0) 
{
}

RCXarFile::~RCXarFile()
{
}

END_NAMESPACE_RCZIP
