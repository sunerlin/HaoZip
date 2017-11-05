/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWinFileBase_h_
#define __RCWinFileBase_h_ 1

#include "base/RCWindowsDefs.h"
#include "base/RCNonCopyable.h"
#include "base/RCString.h"

BEGIN_NAMESPACE_RCZIP

class RCWinFileBase:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCWinFileBase() ;
    
    /** 默认析构函数
    */
    ~RCWinFileBase() ;
    
public:
    
    /** 关闭文件
    @return 关闭成功返回true, 否则返回false
    */
    bool Close() ;
    
    /** 获取当前文件位置偏移
    @param [out] position 返回当前文件位置偏移
    @return 成功返回true, 否则返回false
    */
    bool GetPosition(uint64_t& position) const;
    
    /** 获取当前文件长度
    @param [out] length 返回当前文件长度
    @return 成功返回true, 否则返回false
    */
    bool GetLength(uint64_t& length) const;
    
    /** 定位文件位置
    @param [in] distanceToMove 移动偏移量
    @param [in] moveMethod 移动方法
    @param [out] newPosition 移动后新的文件位置偏移
    @return 成功返回true, 否则返回false
    */
    bool Seek(int64_t distanceToMove, DWORD moveMethod, uint64_t& newPosition) const ;
    
    /** 定位文件位置
    @param [in] position 移动偏移量
    @param [out] newPosition 移动后新的文件位置偏移
    @return 成功返回true, 否则返回false
    */
    bool Seek(uint64_t position, uint64_t& newPosition);
    
    /** 定位文件偏移到文件首
    @return 成功返回true, 否则返回false
    */
    bool SeekToBegin();
    
    /** 定位文件偏移到文件尾
    @param [out] newPosition 移动后新的文件位置偏移
    @return 成功返回true, 否则返回false
    */
    bool SeekToEnd(uint64_t& newPosition);
    
    /** 取当前文件名
    @return 返回当前文件名，含完整路径
    */
    const RCString& GetFileName() const ;
  
protected:
    
    /** 创建文件
    @param [in] fileName 文件名
    @param [in] desiredAccess 访问属性
    @param [in] shareMode 共享属性
    @param [in] creationDisposition 创建属性
    @param [in] flagsAndAttributes 标记位
    @return 成功返回true, 否则返回false
    */
    bool Create(const RCString& fileName,
                DWORD desiredAccess,
                DWORD shareMode, 
                DWORD creationDisposition,  
                DWORD flagsAndAttributes) ;
    
protected:
    
    /** 文件句柄
    */
    HANDLE m_handle ;
    
    /** 文件名
    */
    RCString m_fileName ;
};

END_NAMESPACE_RCZIP

#endif //__RCWinFileBase_h_
