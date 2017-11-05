/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCArchivePath_h_
#define __RCArchivePath_h_ 1

#include "base/RCDefs.h"
#include "base/RCTypes.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

/** 文档路径
*/
class RCArchivePath
{
public:

    /** 默认构造函数
    */
    RCArchivePath() ;
    
    /** 默认析构函数
    */
    ~RCArchivePath() ;
    
    /** 拷贝构造函数
    */
    RCArchivePath(const RCArchivePath& rhs) ;
    
    /** 赋值操作符
    */
    RCArchivePath& operator= (const RCArchivePath& rhs) ;
    
public:

    /** 从字符串加载信息
    @param [in] path 路径
    */
    void ParseFromPath(const RCString& path);

    /** 取得文件路径,不含扩展名
    @return 返回文件路径,不含扩展名
    */
    RCString GetPathWithoutExt() const;

    /** 取得临时目录
    @return 返回临时目录
    */
    RCString GetTempPath_() const;

    /** 取得最终路径
    @return 返回最终路径
    */
    RCString GetFinalPath() const;

public:
    
    /** 设置文件(目录)前缀包含\
    @param [in] prefix 前缀
    */
    void SetPrefix(const RCString& prefix) ;

    /** 设置文件名
    @param [in] name 文件名
    */
    void SetName(const RCString& name) ;

    /** 设置文件扩展名
    @param [in] baseExtension 扩展名
    */
    void SetBaseExtension(const RCString& baseExtension) ;

    /** 设置分卷文件扩展名
    @param [in] volExtension 分卷扩展名
    */
    void SetVolExtension(const RCString& volExtension) ;

    /** 设置是否使用临时目录
    @param [in] temp 是否使用临时目录
    */
    void SetTemp(bool temp) ;

    /** 设置临时目录前缀
    @param [in] tempPrefix 临时目录前缀
    */
    void SetTempPrefix(const RCString& tempPrefix) ;

    /** 设置临时文件后缀名
    @param [in] tempPostfix 临时目录后缀
    */
    void SetTempPostfix(const RCString& tempPostfix) ;

public:
    
    /** 获取文件(目录)前缀包含\
    @return 返回前缀
    */
    const RCString& GetPrefix(void) const ;

    /** 获取文件名
    @return 返回文件名
    */
    const RCString& GetName(void) const ;

    /** 获取文件扩展名
    @return 返回扩展名
    */
    const RCString& GetBaseExtension(void) const ;

    /** 获取分卷文件扩展名
    @return 返回分卷扩展名
    */
    const RCString& GetVolExtension(void) const ;

    /** 获取是否使用临时目录
    @return 使用临时目录返回true,否则返回false
    */
    bool GetTemp(void) const;

    /** 获取临时目录前缀
    @return 返回临时目录前缀
    */
    RCString& GetTempPrefix(void);

    /** 获取临时文件后缀名
    @return 返回临时目录后缀
    */
    const RCString& GetTempPostfix(void) const;

private:
    
    /** 文件(目录)前缀包含\
    */
    RCString m_prefix ;

    /** 文件名
    */
    RCString m_name ;

    /** 文件扩展名
    */
    RCString m_baseExtension ;

    /** 分卷文件扩展名
    */
    RCString m_volExtension ;

    /** 是否使用临时目录
    */
    bool m_temp ;

    /** 临时目录前缀
    */
    RCString m_tempPrefix ;

    /** 临时文件后缀名
    */
    RCString m_tempPostfix ;
};

END_NAMESPACE_RCZIP

#endif //__RCArchivePath_h_
