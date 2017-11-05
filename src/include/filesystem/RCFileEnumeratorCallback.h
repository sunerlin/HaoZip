/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCFileEnumeratorCallback_h_
#define __RCFileEnumeratorCallback_h_ 1

#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCFileInfo ;

/** 遍历文件回调接口
*/
class RCFileEnumeratorCallback
{
protected:
    
    /** 默认析构函数
    */
    ~RCFileEnumeratorCallback() {} ;
    
public:
    
    /** 读取一个文件
    @param [in] filePath 文件路径，最后一个字符为路径分隔符
    @param [in] fileName 文件名，不含路径
    @param [out] fileInfo 返回取到的文件属性信息
    @return 成功RC_S_OK, 如果返回不是RC_S_OK，则遍历过程立即终止
    */
    virtual HResult OnFindFile(const RCString& filePath, 
                               const RCString& fileName, 
                               RCFileInfo& fileInfo) = 0 ;
};

END_NAMESPACE_RCZIP

#endif //__RCFileEnumeratorCallback_h_
