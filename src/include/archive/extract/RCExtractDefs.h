/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCExtractMode_h
#define __RCExtractMode_h 1

#include "base/RCTypes.h"

BEGIN_NAMESPACE_RCZIP

/** 解压常量定义
*/
class RCExtractDefs
{
public:
    
    /** 更新方式
    */
    enum update_type
    {
        /** 解压并替换文件
        */
        RC_EXTRACT_REPLACE      = 0 ,
        
        /** 解压并更新文件
        */
        RC_EXTRACT_UPDATE       = 1 ,
        
        /** 仅仅更新已经存在的文件
        */
        RC_EXTRACT_UPDATE_EXIST = 2
    };
    
    /** 解压路径模式
    */
    enum path_type
    {
        /** 解压完全路径
        */
        RC_EXTRACT_FULL_PATH     = 0 ,
    
        /** 不解压路径
        */
        RC_EXTRACT_NO_PATH       = 1 ,
    
        /** 解压绝对路径
        */
        RC_EXTRACT_ABSOLUTE_PATH = 2 ,
        
        /** 解压当前路径
        */
        RC_EXTRACT_CURRENT_PATH = 3
    };
    
    /** 解压覆盖模式
    */
    enum overwrite_type
    {
        /** 覆盖前提示确认
        */
        RC_ASK_BEFORE           = 0 ,
        
        /** 覆盖前不提示
        */
        RC_WITHOUT_PROMPT       = 1 ,
        
        /** 跳过已经存在的文件，不覆盖
        */
        RC_SKIP_EXISTING        = 2 ,
        
        /** 对新文件自动重命名
        */
        RC_AUTO_RENAME          = 3 ,
        
        /** 对旧文件自动重命名
        */
        RC_AUTO_RENAME_EXISTING = 4
    };
    
    /** 文件覆盖回答类型
    */
    enum overwrite_answer_type
    {
        RC_ANSWER_YES               = 0 ,
        RC_ANSWER_YES_TO_ALL        = 1 ,
        RC_ANSWER_NO                = 2 ,
        RC_ANSWER_NO_TO_ALL         = 3 ,
        RC_ANSWER_AUTO_RENAME       = 4 ,
        RC_ANSWER_AUTO_RENAME_ALL   = 5 ,
        RC_ANSWER_USER_RENAME       = 6 ,
        RC_ANSWER_CANCEL            = 7
    };
    
    /** 解压询问类型
    */
    enum ask_mode_type
    {
        RC_ASK_EXTRACT  = 0 ,
        RC_ASK_TEST     = 1 ,
        RC_ASK_SKIP     = 2
    };
    
    /** 解压结果
    */
    enum result_type
    {
        RC_EXTRACT_OK                   = 0 ,
        RC_EXTRACT_UNSUPPORTED_METHOD   = 1 ,
        RC_EXTRACT_DATA_ERROR           = 2 ,
        RC_EXTRACT_CRC_ERROR            = 3
    };
    
    /** 解压文件时间选项
    */
    enum file_time_flag
    {
        /** 保存文件修改时间
        */
        RC_EXTRACT_FILE_SAVE_MTIME      = 1 << 1 ,
        
        /** 保存文件创建时间
        */
        RC_EXTRACT_FILE_SAVE_CTIME      = 1 << 2 ,

        /** 保存文件最后访问时间
        */
        RC_EXTRACT_FILE_SAVE_ATIME      = 1 << 3
    };
};

typedef RCExtractDefs::update_type              RCExractUpdateMode ;
typedef RCExtractDefs::path_type                RCExractPathMode ;
typedef RCExtractDefs::overwrite_type           RCExractOverwriteMode ;
typedef RCExtractDefs::overwrite_answer_type    RCExractOverwriteAnswer ;
typedef RCExtractDefs::ask_mode_type            RCExractAskMode ;
typedef RCExtractDefs::result_type              RCExractOperationResult ;
typedef uint32_t                                RCExractFileTimeFlag ;

END_NAMESPACE_RCZIP
#endif
