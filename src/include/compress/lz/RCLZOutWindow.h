/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCLZOutWindow_h_
#define __RCLZOutWindow_h_ 1

#include "common/RCOutBuffer.h"

BEGIN_NAMESPACE_RCZIP

/** RCLZOutWindow
*/
class RCLZOutWindow:
    public RCOutBuffer
{
public:

    /** 默认构造函数
    */
    RCLZOutWindow() ;
    
    /** 默认析构函数
    */
    ~RCLZOutWindow() ;
    
public:
    
    /** 初始化
    @param [in] solid 是否固实
    */
    void Init(bool solid = false) ;
    
    /** 复制数据
    @param [in] distance distance >= 0
    @param [in] len len > 0
    @return 成功返回true，失败返回false
    */
    bool CopyBlock(uint32_t distance, uint32_t len) ;
    
    /** 存入字节
    @param [in] byte 字节值
    */
    void PutByte(byte_t byte) ;
    
    /** 获取字节
    @param [in] distance 距当前位置的距离（向前）
    @return 返回获取的字节值
    */
    byte_t GetByte(uint32_t distance) const ;
};

END_NAMESPACE_RCZIP

#endif //__RCLZOutWindow_h_
