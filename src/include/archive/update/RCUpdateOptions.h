/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCUpdateOptions_h_
#define __RCUpdateOptions_h_ 1

#include "base/RCTypes.h"
#include "archive/update/RCCompressMethodMode.h"
#include "archive/update/RCUpdateArchiveCommand.h"
#include "archive/update/RCArchivePath.h"
#include "archive/update/RCArchiveUpdateFilter.h"
#include "archive/common/IEnumDirItemFilter.h"

BEGIN_NAMESPACE_RCZIP

class RCArchiveCodecs;
class IOutStreamEvent ;

/** 压缩update选项参数
*/
class RCUpdateOptions
{
public:
    /** 构造函数
    */
    RCUpdateOptions() ;

public:
    
    /** 保存路径模式
    */
    enum path_mode
    {
        RC_UPDATE_RELATIVE_PATH = 0 ,
        RC_UPDATE_ABSOLUTE_PATH = 1 ,
        RC_UPDATE_NO_PATH       = 2 ,
        RC_UPDATE_ABSOLUTE_FULL_PATH = 3
    };
    
    /** 压缩更新模式
    */
    enum update_mode
    {
        RC_COMPRESS_FILES_ADD_REPLACE       = 0 ,
        RC_COMPRESS_FILES_ADD_UPDATE        = 1 ,
        RC_COMPRESS_FILES_UPDATE_EXIST_ONLY = 2 ,
        RC_COMPRESS_FILES_SYNC_CONTENT      = 3
    };
    
public:
    /** 初始化
    @param [in] codecs 编码管理器
    @param [in] formatIndices 文档格式列表
    @param [in] arcPath 文档路径
    @param [in] pathMode 保存路径模式
    @param [in] updateMode 压缩更新模式
    @return 成功返回true,否则返回false
    */
    bool Init(const RCArchiveCodecs* codecs,
              const RCUIntVector& formatIndices, 
              const RCString& arcPath,
              RCUpdateOptions::path_mode pathMode = RC_UPDATE_RELATIVE_PATH,
              RCUpdateOptions::update_mode updateMode = RC_COMPRESS_FILES_ADD_REPLACE);
                
public:
    
    /** 取得压缩方式
    @return 返回压缩方式
    */
    const RCCompressMethodMode& GetMethodMode(void) const;
    
    /** 取得压缩方式
    @return 返回压缩方式
    */
    RCCompressMethodMode& GetMethodMode(void);

    /** 取得压缩命令列表
    @return 返回压缩命令列表
    */
    const std::vector<RCUpdateArchiveCommand>& GetCommands(void) const;
        
    /** 取得压缩命令列表
    @return 返回压缩命令列表
    */
    std::vector<RCUpdateArchiveCommand>& GetCommands(void);

    /** 是否更新文档
    @return 更新文档自身返回true,否则返回false
    */
    bool GetUpdateArchiveItSelf(void) const;
    
    /** 设置更新文档
    @param [in] updateArchiveItSelf 更新文档
    */
    void SetUpdateArchiveItSelf(bool updateArchiveItSelf);

    /** 返回文档路径
    @return 返回文档路径
    */
    const RCArchivePath& GetArchivePath(void) const;
    
    /** 返回文档路径
    @return 返回文档路径
    */
    RCArchivePath& GetArchivePath(void);

    /** 自解压模式
    @return 自解压模式返回true,否则返回false
    */
    bool GetSfxMode(void) const;
    
    /** 设置自解压模式
    @param [in] sfxMode 自解压模式
    */
    void SetSfxMode(bool sfxMode);

    /** 返回自解压模块文件名
    @return 返回自解压模块文件名
    */
    const RCString& GetSfxModule(void) const;
    
    /** 设置自解压模块文件名
    @param [in] sfxModule 自解压模块
    */
    void SetSfxModule(const RCString& sfxModule);

    /** 返回共享写
    @return 共享写返回true,否则返回false
    */
    bool GetOpenShareForWrite(void) const;
    
    /** 设置共享写
    @param [in] openShareForWrite 共享写
    */
    void SetOpenShareForWrite(bool openShareForWrite);

    /** 标准输入模式
    @return 标准输入模式返回true,否则返回false
    */
    bool GetStdInMode(void) const;
    
    /** 设置标准输入模式
    @param [in] stdInMode 标准输入模式
    */
    void SetStdInMode(bool stdInMode);

    /** 返回标准输入文件名
    @return 返回标准输入文件名
    */
    const RCString& GetStdInFileName(void) const;
    
    /** 设置标准输入文件名
    @param [in] stdInFileName 标准输入文件名
    */
    void SetStdInFileName(const RCString& stdInFileName);

    /** 是否标准输出
    @return 标准输出返回true,否则返回false
    */
    bool GetStdOutMode(void) const;
    
    /** 设置标准输出
    @param [in] stdOutMode 标准输出模式
    */
    void SetStdOutMode(bool stdOutMode);
    
    /** 是否测试模式
    @return 测试模式返回true,否则返回false
    */
    bool IsTestMode(void) const ;
    
    /** 设置测试模式
    @param [in] isTestMode 测试模式
    */
    void SetIsTestMode(bool isTestMode);

    /** 是否Email 模式
    @return Email模式返回true，否则返回false
    */
    bool GetEmailMode(void) const;
    
    /** 设置Email模式
    @param [in] emailMode Email模式
    */
    void SetEmailMode(bool emailMode);

    /** 是否发生后删除
    @return email后删除返回true，否则返回false
    */
    bool GetEmailRemoveAfter(void) const;
    
    /** 设置Email后删除
    @param [in] emailRemoveAfter 发生后删除
    */
    void SetEmailRemoveAfter(bool emailRemoveAfter);

    /** 返回邮件地址
    @return 返回邮件发生地址
    */
    const RCString& GetEmailAddress(void) const;
    
    /** 设置邮件地址
    @param [in] emailAddress 邮件发送地址
    */
    void SetEmailAddress(const RCString& emailAddress);

    /** 返回工作目录
    @return 返回工作目录
    */
    const RCString& GetWorkDir(void) const;
    
    /** 设置工作目录
    @param [in] workDir 工作目录
    */
    void SetWorkDir(const RCString& workDir);

    /** 取得分卷大小列表
    @return 返回分卷大小列表
    */
    const std::vector<uint64_t>& GetVolumnSizes(void) const;
    
    /** 取得分卷大小列表
    @return 返回分卷大小列表
    */
    std::vector<uint64_t>& GetVolumnSizes(void);
        
    /** 获取保存路径模式
    @return 返回路径保存模式
    */
    RCUpdateOptions::path_mode GetPathMode() const ;
        
    /** 获取更新方式
    @return 返回更新方式
    */
    RCUpdateOptions::update_mode GetUpdateMode() const ;
        
    /** 设置IO写入事件处理
    @param [in] outStreamEvent 输出流事件接口
    */
    void SetOutStreamEvent(IOutStreamEvent* outStreamEvent) ;
    
    /** 获取IO写入事件处理
    @return 返回输出流事件接口
    */
    IOutStreamEvent* GetOutStreamEvent() const ;
    
    /** 设置压缩文件过滤器
    @param [in] spUpdateFilter 压缩文件过滤器
    */
    void SetUpdateFileter(const RCArchiveUpdateFilterPtr& spUpdateFilter) ;
    
    /** 获取压缩文件过滤器
    @return 返回压缩文件过滤器
    */
    RCArchiveUpdateFilterPtr GetUpdateFileter(void) const ;
    
    /** 设置被压缩的文件过滤器
    @param [in] spEnumDirItemFilter 文件枚举过滤器
    */
    void SetEnumDirItemFilter(const IEnumDirItemFilterPtr& spEnumDirItemFilter) ;
    
    /** 获取被压缩的文件过滤器
    @return 返回被压缩的文件过滤器
    */
    IEnumDirItemFilterPtr GetEnumDirItemFilter(void) const ;

private:
    
    /** 压缩方法模式
    */
    RCCompressMethodMode m_methodMode;

    /** 命令
    */
    std::vector<RCUpdateArchiveCommand> m_commands;

    /** 
    */
    bool m_updateArchiveItSelf;

    /** 文档路径信息
    */
    RCArchivePath m_archivePath;

    /** 自解压模式
    */
    bool m_sfxMode;

    /** 自解压模块名称
    */
    RCString m_sfxModule;

    /** 共享写入
    */
    bool m_openShareForWrite;

    /** 标准输入模式
    */
    bool m_stdInMode;

    /** 标准输入文件名称
    */
    RCString m_stdInFileName;

    /** 标准输出模式
    */
    bool m_stdOutMode;
    
    /** 是否为测试模式, 测试模式不生成压缩包，只做压缩测试
    */
    bool m_isTestMode ;

    /** 电子邮件模式
    */
    bool m_emailMode;

    /** 发送后删除文档
    */
    bool m_emailRemoveAfter;

    /** 电子邮件地址
    */
    RCString m_emailAddress;

    /** 工作目录
    */
    RCString m_workingDir;

    /** 分卷大小
    */
    std::vector<uint64_t> m_volumnsSizes;
        
    /** 保存路径模式
    */
    RCUpdateOptions::path_mode m_pathMode ;
        
    /** 更新方式
    */
    RCUpdateOptions::update_mode m_updateMode ;

    /** IO 事件处理
    */
    IOutStreamEvent* m_outStreamEvent ;
    
    /** 压缩文件过滤器
    */
    RCArchiveUpdateFilterPtr m_spUpdateFilter ;
    
    /** 缺省文档类型
    */
    static RCString s_kDefaultArchiveType;
    
    /** 自解压扩展名
    */
    static RCString s_kSFXExtension;
    
    /** 被压缩的文件过滤器
    */
    IEnumDirItemFilterPtr m_spEnumDirItemFilter ;
};

END_NAMESPACE_RCZIP

#endif  //__RCUpdateOptions_h_
