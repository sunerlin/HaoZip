/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLzhItem_h_
#define __RCLzhItem_h_ 1

#include "common/RCBuffer.h"
#include "common/RCVector.h"
#include "common/RCStringUtilA.h"
#include "algorithm/CpuArch.h"
#include "format/lzh/RCLzhHeader.h"

BEGIN_NAMESPACE_RCZIP

struct RCLzhExtension
{
    /** 类型
    */
    byte_t m_type;

    /** 数据缓冲
    */
    RCByteBuffer m_data;

    /** 获取字符串
    @return 返回字符串
    */
    RCStringA GetString() const
    {
        RCStringA s;
        for (size_t i = 0; i < m_data.GetCapacity(); i++)
        {
            char c = (char)m_data[i];
            if (c == 0)
            {
                break;
            }
            s += c;
        }
        return s;
    }
};

struct RCLzhItem
{
public:

    /** 名字
    */
    RCStringA m_name;

    /** 方法
    */
    byte_t m_method[RCLzhHeader::s_methodIdSize];

    /** 压缩大小
    */
    uint32_t m_packSize;

    /** 大小
    */
    uint32_t m_size;

    /** 修改时间
    */
    uint32_t m_modifiedTime;

    /** 属性
    */
    byte_t m_attributes;

    /** 等级
    */
    byte_t m_level;

    /** crc
    */
    uint16_t m_crc;

    /** 操作系统id
    */
    byte_t m_osId;

    /** 扩展信息
    */
    RCVector<RCLzhExtension> m_extensions;

    /** 是否是有效方法
    @return 是返回true，否则返回false
    */
    bool IsValidMethod() const
    {
        return (m_method[0] == '-' && m_method[1] == 'l' && m_method[4] == '-');
    }

    /** 是否是lh方法
    @return 是返回true，否则返回false
    */
    bool IsLhMethod() const
    {
        return (IsValidMethod() && m_method[2] == 'h');
    }

    /** 是否是目录
    @return 是返回true，否则返回false
    */
    bool IsDir() const
    {
        return (IsLhMethod() && m_method[3] == 'd');
    }

    /** 是否是copy方法
    @return 是返回true，否则返回false
    */
    bool IsCopyMethod() const
    {
        return (IsLhMethod() && m_method[3] == '0') ||
            (IsValidMethod() && m_method[2] == 'z' && m_method[3] == '4');
    }

    /** 是否是lh1 group方法
    @return 是返回true，否则返回false
    */
    bool IsLh1GroupMethod() const
    {
        if (!IsLhMethod())
        {
            return false;
        }

        switch(m_method[3])
        {
        case '1':
            return true;
        }
        return false;
    }

    /** 是否是lh4 group方法
    @return 是返回true，否则返回false
    */
    bool IsLh4GroupMethod() const
    {
        if (!IsLhMethod())
        {
            return false;
        }

        switch(m_method[3])
        {
        case '4':
        case '5':
        case '6':
        case '7':
            return true;
        }
        return false;
    }

    /** 获取字典大小
    @return 成功返回字典大小，否则返回0
    */
    int32_t GetNumDictBits() const
    {
        if (!IsLhMethod())
        {
            return 0;
        }

        switch(m_method[3])
        {
        case '1':
            return 12;

        case '2':
            return 13;

        case '3':
            return 13;

        case '4':
            return 12;

        case '5':
            return 13;

        case '6':
            return 15;

        case '7':
            return 16;
        }
        return 0;
    }

    /** 查找扩展信息索引
    @param [in] type 类型
    @return 成功返回索引，否则返回-1
    */
    int32_t FindExt(byte_t type) const
    {
        for (int32_t i = 0; i < (int32_t)m_extensions.size(); i++)
        {
            if (m_extensions[i].m_type == type)
            {
                return i;
            }
        }
        return -1;
    }

    /** 获取unix时间
    @param [out] value 返回时间
    @return 成功返回true，否则返回false
    */
    bool GetUnixTime(uint32_t& value) const
    {
        int32_t index = FindExt(RCLzhHeader::s_extIdUnixTime);
        if (index < 0)
        {
            if (m_level == 2)
            {
                value = m_modifiedTime;
                return true;
            }
            return false;
        }
        const byte_t *data = (const byte_t *)(m_extensions[index].m_data.data());
        value = GetUi32(data);
        return true;
    }

    /** 获取目录名
    @return 返回目录名
    */
    RCStringA GetDirName() const
    {
        int32_t index = FindExt(RCLzhHeader::s_extIdDirName);
        if (index < 0)
        {
            return RCStringA();
        }
        return m_extensions[index].GetString();
    }

    /** 获取文件名
    @return 返回文件名
    */
    RCStringA GetFileName() const
    {
        int32_t index = FindExt(RCLzhHeader::s_extIdFileName);
        if (index < 0)
        {
            return m_name;
        }
        return m_extensions[index].GetString();
    }

    /** 获取名字
    @return 返回名字
    */
    RCStringA GetName() const
    {
        RCStringA dirName = GetDirName();
        for(size_t index = 0; index < dirName.size(); ++index)
        {
            if(0xFF == dirName[index])
            {
                dirName[index] = '\\' ;
            }
        }
        if (!dirName.empty())
        {
            RCStringA::value_type c = dirName[dirName.length() - 1];
            if (c != '\\')
            {
                dirName += '\\';
            }
        }
        return dirName + GetFileName();
    }
};

class RCLzhItemEx: public RCLzhItem
{
public:

    /** 数据偏移位置
    */
    uint64_t m_dataPosition;
};

END_NAMESPACE_RCZIP

#endif //__RCLzhItem_h_
