/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCVector_h_
#define __RCVector_h_ 1

#include "base/RCDefs.h"
#include <vector>

#define RCVector std::vector
    
BEGIN_NAMESPACE_RCZIP

typedef RCVector<int32_t>   RCIntVector ;
typedef RCVector<uint32_t>  RCUIntVector ;
typedef RCVector<bool>      RCBoolVector ; //注意：该类型不能以C风格数组使用
typedef RCVector<byte_t>    RCByteVector ;
typedef RCVector<void*>     RCPointerVector ;

namespace RCVectorUtils
{
    /** 从Vector中查找元素，线性查找
    @param [in] array 需要查找的Vector数组
    @param [in] item 需要查找的元素
    @return 返回该元素的下标，从0开始，如果查找失败返回-1
    */
    template <typename T>
    int32_t Find(const RCVector<T>& array, const T& item)  
    {
        int32_t count = (int32_t)array.size() ;
        for (int32_t i = 0; i < count ; ++i)
        {
            if (item == array[i])
            {
                return i ;
            }
        }
        return -1 ;
    }
    
    /** 从Vector中删除指定元素
    @param [in] array 需要操作的Vector数组
    @param [in] index 需要删除的起始下标值
    @param [in] num 需要删除的元素个数
    */
    template <typename T>
    void Delete(RCVector<T>& array, int32_t index, int32_t num = 1)
    {
        if( (index < 0) || ( num < 1) )
        {
            return ;
        }
        int32_t count = (int32_t)array.size() ;
        if( (num + index) > count )
        {
            num = count - index ;
        }
        array.erase(array.begin() + index, array.begin() + index + num) ;
    }
    
    /** 向Vector中增加元素
    @param [in] array 需要操作的Vector数组
    @param [in] index 需要插入的下标值位置
    @param [in] item 需要插入的元素值
    */
    template <typename T>
    void Insert(RCVector<T>& array, int32_t index, const T& item)
    {
        if(index <= 0)
        {
            array.insert(array.begin(),item) ;
            return ;
        }
        else if(index >= (int32_t)array.size())
        {
            array.push_back(item) ;
            return ;
        }
        array.insert(array.begin() + index,item) ;
    }
    
    /** 从Vector中查找元素，二分法查找，前提是Vector内的数据必须是有序的
    @param [in] array 需要查找的Vector数组
    @param [in] item 需要查找的元素
    @return 返回该元素的下标，从0开始，如果查找失败返回-1
    */
    template <typename T>
    int32_t FindInSorted(const RCVector<T>& array, const T& item)
    {
        int32_t left = 0 ;
        int32_t right = (int32_t)array.size() ;
        while (left != right)
        {
            int32_t mid = (left + right) / 2;
            const T& midValue = array[mid];
            if (item == midValue)
            {
                return mid;
            }
            if (item < midValue)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        return -1 ;
    }
    
    /** 向Vector中添加元素，确保Vector中的数据是有序的
    @param [in] array 需要操作的Vector数组
    @param [in] item 需要插入的元素值
    @return 返回插入元素的下标位置
    */
    template <typename T>
    int32_t AddToSorted(RCVector<T>& array, const T& item)
    {
        int32_t left = 0 ;
        int32_t right = (int32_t)array.size() ;
        while (left != right)
        {
            int32_t mid = (left + right) / 2;
            const T& midValue = array[mid];
            if (item == midValue)
            {
                right = mid + 1 ;
                break ;
            }
            if (item < midValue)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        array.insert(array.begin() + right, item) ;
        return right ;
    }
    
    /** 向Vector中添加元素，确保Vector中的数据是有序而且唯一的
    @param [in] array 需要操作的Vector数组
    @param [in] item 需要插入的元素值
    @return 返回插入元素的下标位置
    */
    template <typename T>
    int32_t AddToUniqueSorted(RCVector<T>& array, const T& item)
    {
        int32_t left = 0 ;
        int32_t right = (int32_t)array.size() ;
        while (left != right)
        {
            int32_t mid = (left + right) / 2;
            const T& midValue = array[mid];
            if (item == midValue)
            {
                return mid;
            }
            if (item < midValue)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        array.insert(array.begin() + right, item) ;
        return right ;
    }
    
    /** 释放多余内存
    @param [in] array 需要操作的Vector数组
    */
    template <typename T>
    void ReserveDown(RCVector<T>& array)
    {
        //什么也不做
    }
    
    /** 排序算法
    @param [in] p 数组起始地址
    @param [in] k 数组元素起始位置
    @param [in] size 数组长度
    @param [in] functor 比较函数
    */
    template<class T,
             class TCompareFunctors>
    void SortRefDown(T* p, int32_t k, int32_t size, TCompareFunctors functor)
    {
        T temp = p[k] ;
        for (;;)
        {
            int32_t s = (k << 1);
            if (s > size)
            {
                break;
            }
            if (s < size && functor(*(p + s + 1), *(p + s)) > 0)
            {
                s++;
            }
            if (functor(temp, *(p + s)) >= 0)
            {
                break;
            }
            p[k] = p[s] ;
            k = s ;
        }
        p[k] = temp ;
    }
    
    /** 排序算法
    @param [in] array 需要排序的数组
    @param [in] functor 比较函数
    */
    template<class T,
             class TCompareFunctors>
    void Sort(RCVector<T>& array, TCompareFunctors functor)
    {
        int32_t size = (int32_t)array.size() ;
        if (size <= 1)
        {
            return ;
        }
        T* p = (&array.front()) - 1 ;
        {
            int32_t i = size / 2;
            do
            {
                SortRefDown<T>(p, i, size,functor);
            } while (--i != 0) ;
        }
        do
        {
            T temp = p[size] ;
            p[size--] = p[1] ;
            p[1] = temp ;
            SortRefDown<T>(p, 1, size,functor) ;
        } while (size > 1) ;
    }
    
}// end of namespace RCVectorUtils

END_NAMESPACE_RCZIP

#endif //__RCVector_h_
