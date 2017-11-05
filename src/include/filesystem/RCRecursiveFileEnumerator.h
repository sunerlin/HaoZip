/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRecursiveFileEnumerator_h_
#define __RCRecursiveFileEnumerator_h_ 1

#include "filesystem/RCFileInfo.h"

BEGIN_NAMESPACE_RCZIP

class RCFileEnumeratorCallback ;

/** 遍历获取一个目录以及子目录中的所有文件
    采取递归遍历模式
*/
class RCRecursiveFileEnumerator
{    
public:
    
    /** 遍历目录以及递归其子目录获取文件
    @param [in] path 需要遍历的目录, 如C:\
    @param [in] callback 遍历回调函数, 不可以为NULL
    @return 成功返回 RC_S_OK, 失败返回错误码
    */
    HResult Recusive(RCString path, 
                     RCFileEnumeratorCallback* callback) ;
    

};

END_NAMESPACE_RCZIP

#endif //__RCRecursiveFileEnumerator_h_
