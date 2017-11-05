/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCMethodDefs_h_
#define __RCMethodDefs_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Method ID 定义
*/
class RCMethod
{
public:
    
    typedef enum
    {
        /** 加密解密算法
        */
        ID_CRYPTO_7Z_AES    = 0x06F10701,
        ID_CRYPTO_RAR_20    = 0x0501 ,
        ID_CRYPTO_RAR_29    = 0x0502 ,
        ID_CRYPTO_WZ_AES    = 0x0503 ,
        ID_CRYPTO_ZIP       = 0x0504 ,
        
        /** Filter算法
        */
        ID_COMPRESS_BYTE_SWAP_2     = 0x020302,
        ID_COMPRESS_BYTE_SWAP_4     = 0x020304,        
        ID_COMPRESS_BRANCH_PPC      = 0x03030000 + (0x02 * 256) + 0x05,
        ID_COMPRESS_BRANCH_IA64     = 0x03030000 + (0x04 * 256) + 1,
        ID_COMPRESS_BRANCH_ARM      = 0x03030000 + (0x05 * 256) + 1,
        ID_COMPRESS_BRANCH_ARMT     = 0x03030000 + (0x07 * 256) + 1,
        ID_COMPRESS_BRANCH_SPARC    = 0x03030000 + (0x08 * 256) + 0x05,
        ID_COMPRESS_BRANCH_BCJ      = 0x03030103,
        ID_COMPRESS_BRANCH_BCJ_2    = 0x0303011B,

        /** 压缩解压算法
        */
        ID_COMPRESS_LZMA            = 0x030101,
        ID_COMPRESS_PPMD            = 0x030401,
        ID_COMPRESS_PPMDI1          = 0x030402,
        ID_COMPRESS_WAVPACK         = 0x030403,
        ID_COMPRESS_RAR_1           = 0x040300 + 1,
        ID_COMPRESS_RAR_2           = 0x040300 + 2, 
        ID_COMPRESS_RAR_3           = 0x040300 + 3,
        ID_COMPRESS_BZIP2           = 0x040202,
        ID_COMPRESS_DEFLATE_COM     = 0x040108,
        ID_COMPRESS_DEFLATE_COM_64  = 0x040109,
        ID_COMPRESS_DEFLATE_NSIS    = 0x040901,
        ID_COMPRESS_COPY            = 0x0000 ,
        ID_COMPRESS_SHRINK          = 0x0001 ,        
        ID_COMPRESS_LZH             = 0x0002 ,
        ID_COMPRESS_DELTA           = 0x0003 ,
        ID_COMPRESS_ARJ_1           = 0x0004 ,
        ID_COMPRESS_ARJ_2           = 0x0005 ,
        ID_COMPRESS_LZX             = 0x0006 ,
        ID_COMPRESS_IMPLODE         = 0x0007 ,        
        ID_COMPRESS_QUANTUM         = 0x0008 ,
        ID_COMPRESS_ZLIB            = 0x0009 ,
        ID_COMPRESS_Z               = 0x000A ,
        ID_COMPRESS_ADC             = 0x000B ,
        ID_COMPRESS_LZMA2           = 0x0021
    }ID ;
};

/** 编码属性枚举值定义
*/
class RCMethodProperty
{
public:
    
    enum
    {
        ID_SET_PARAMS           = 0 , // int64_t 类型
        ID_SET_KEEP_HISTORY     = 1 , // bool 类型
        ID_SET_RAR_350_MODE     = 2 , // bool 类型
        ID_SET_DICTIONARY_SIZE  = 3 , // int64_t 类型
        ID_SET_LZX_WIM_MODE     = 4 , // bool 类型
        ID_SET_NSIS_DEFLATE     = 5 , // bool 类型
        ID_SET_ISZ_BIZP2         =6   // bool 类型
    };
};

/** 算法名称定义
*/
#define RC_METHOD_NAME_CRYPTO_7Z_AES            _T("7zAES") 
#define RC_METHOD_NAME_CRYPTO_RAR_20            _T("Rar20") 
#define RC_METHOD_NAME_CRYPTO_RAR_29            _T("Rar29") 
#define RC_METHOD_NAME_CRYPTO_WZ_AES            _T("WzAES") 
#define RC_METHOD_NAME_CRYPTO_ZIP               _T("Zip") 
#define RC_METHOD_NAME_COMPRESS_COPY            _T("Copy") 
#define RC_METHOD_NAME_COMPRESS_SHRINK          _T("Shrink") 
#define RC_METHOD_NAME_COMPRESS_BYTE_SWAP_2     _T("ByteSwap2") 
#define RC_METHOD_NAME_COMPRESS_BYTE_SWAP_4     _T("ByteSwap4") 
#define RC_METHOD_NAME_COMPRESS_LZH             _T("Lzh") 
#define RC_METHOD_NAME_COMPRESS_Z               _T("Z") 
#define RC_METHOD_NAME_COMPRESS_ARJ_1           _T("Arj1") 
#define RC_METHOD_NAME_COMPRESS_ARJ_2           _T("Arj2") 
#define RC_METHOD_NAME_COMPRESS_LZX             _T("Lzx") 
#define RC_METHOD_NAME_COMPRESS_LZMA            _T("LZMA") 
#define RC_METHOD_NAME_COMPRESS_IMPLODE         _T("Implode") 
#define RC_METHOD_NAME_COMPRESS_BRANCH_ARM      _T("ARM")
#define RC_METHOD_NAME_COMPRESS_BRANCH_ARMT     _T("ARMT")
#define RC_METHOD_NAME_COMPRESS_BRANCH_PPC      _T("PPC")
#define RC_METHOD_NAME_COMPRESS_BRANCH_IA64     _T("IA64")
#define RC_METHOD_NAME_COMPRESS_BRANCH_SPARC    _T("SPARC")
#define RC_METHOD_NAME_COMPRESS_BRANCH_BCJ      _T("BCJ")
#define RC_METHOD_NAME_COMPRESS_BRANCH_BCJ_2    _T("BCJ2")
#define RC_METHOD_NAME_COMPRESS_PPMD            _T("PPMd") 
#define RC_METHOD_NAME_COMPRESS_QUANTUM         _T("Quantum") 
#define RC_METHOD_NAME_COMPRESS_RAR_1           _T("Rar1") 
#define RC_METHOD_NAME_COMPRESS_RAR_2           _T("Rar2") 
#define RC_METHOD_NAME_COMPRESS_RAR_3           _T("Rar3") 
#define RC_METHOD_NAME_COMPRESS_BZIP2           _T("BZip2")
#define RC_METHOD_NAME_COMPRESS_DEFLATE_COM     _T("Deflate")
#define RC_METHOD_NAME_COMPRESS_DEFLATE_COM_64  _T("Deflate64")
#define RC_METHOD_NAME_COMPRESS_DEFLATE_NSIS    _T("DeflateNSIS")
#define RC_METHOD_NAME_COMPRESS_ZLIB            _T("Zlib")
#define RC_METHOD_NAME_COMPRESS_LZMA2           _T("LZMA2") 
#define RC_METHOD_NAME_COMPRESS_DELTA           _T("Delta") 

#define RC_METHOD_NAME_COMPRESS_PPMDI1          _T("PPMDI1")
#define RC_METHOD_NAME_COMPRESS_WAVPACK         _T("WAVPACK")
#define RC_METHOD_NAME_COMPRESS_ADC             _T("ADC") 

END_NAMESPACE_RCZIP

#endif //__RCMethodDefs_h_
