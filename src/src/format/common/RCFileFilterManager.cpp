/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/common/RCFileFilterManager.h"
#include "common/RCStringUtil.h"
#include "archive/common/RCWildcardUtils.h"

/////////////////////////////////////////////////////////////////
//RCFileFilterManager class implementation

BEGIN_NAMESPACE_RCZIP

RCFileFilterManager::RCFileFilterManager(const RCString& fileList)
{
    ParseList(fileList);
}

RCFileFilterManager::~RCFileFilterManager()
{
    m_fileNames.clear();
}

bool RCFileFilterManager::IsFilter(const RCString& absolutePath, const RCString& relativePath)
{
    if (m_fileNames.size() > 0)
    {
        RCVector<RCString> parts;
        RCVector<RCString> absoluteParts;
        RCVector<RCString> relativeParts;
                    
        RCWildcardUtils::SplitPathToParts(absolutePath, absoluteParts);
        RCWildcardUtils::SplitPathToParts(relativePath, relativeParts);

        for (RCString::size_type i = 0; i < m_fileNames.size(); i++)
        {
            if (m_fileNames[i]->m_parts.size() == 0)
            {
                //不含路径分隔符，只比较文件名
                parts = relativeParts;

                if (CompareStrings(m_fileNames[i]->m_name.c_str(), 
                                   parts[parts.size()-1].c_str(), 
                                   !RCWildcardUtils::IsCaseSensitive()))
                {
                    return true;
                }
            }
            else
            {
                if ( (m_fileNames[i]->m_name.size() > 1) && (m_fileNames[i]->m_name[1] == ':') )
                {
                    //全路径与absolutePath比较
                    parts = absoluteParts;
                }
                else
                {
                    //相对路径与relativePath比较
                    parts = relativeParts;
                }

                size_t size = m_fileNames[i]->m_parts.size();
                size_t partsSize = parts.size();

                if (size > partsSize)
                {
                    continue;
                }

                RCString::size_type j;
                RCString::size_type k;
                for (j = 0; j <= partsSize - size; j++)
                {
                    for (k = 0; k < size; k++)
                    {
                        if (!CompareStrings(m_fileNames[i]->m_parts[k].c_str(), 
                                            parts[j + k].c_str(), 
                                            !RCWildcardUtils::IsCaseSensitive()))
                        {
                            break;
                        }
                    }

                    if (k == size)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool RCFileFilterManager::ParseList(const RCString& fileList)
{
    RCString list = fileList;
    bool last = false;
    while (!last)
    {
        RCString name;
        RCString::size_type size = list.size();
        RCString::size_type pos = list.find_first_of('|');
        if (pos != RCString::npos)
        {
            name = RCStringUtil::Left(list, (int32_t)pos);

            list = RCStringUtil::Mid(list, (int32_t)pos+1);
        }
        else
        {
            name = list;
            last = true;
        }

        if (name.size() > 0)
        {
            RCFileNamePartsPtr nameParts(new RCFileNameParts);
            if (!nameParts)
            {
                return false;
            }

            if (list.find_first_of('\\') != RCString::npos ||
                list.find_first_of('/') != RCString::npos)
            {
                RCWildcardUtils::SplitPathToParts(name, nameParts->m_parts);
            }
            nameParts->m_name = name;

            m_fileNames.push_back(nameParts);
        }
    }
    return true;
}

bool RCFileFilterManager::CompareStrings(const char_t* sPattern, 
                                         const char_t* sFileName, 
                                         bool bNoCase)
{
    return RCStringUtil::WildcardCompare(sPattern, sFileName, bNoCase) ;
}

END_NAMESPACE_RCZIP
