/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/
//include files
#include "coder/RCCodecsRegister.h"
#include "algorithm/RCInitCRCTable.h"
#include "common/RCAlloc.h"
#include "common/RCBitlDecoderData.h"
#include "compress/bzip2/RCBZip2CRCTableInit.h"
#include "compress/range/RCRangeCoderDefs.h"
#include "crypto/AES/RCAesTableInit.h"
#include "crypto/hash/RCRandomGenerator.h"
#include "language/RCLanguage.h"

#ifdef RCZIP_OS_WIN
    #include "base/RCWindowsDefs.h"
#endif

/////////////////////////////////////////////////////////////////
//RCCodecsExports class implementation


#if !defined (RC_STATIC_SFX) && !defined (RC_BUILD_STATIC)

BEGIN_NAMESPACE_RCZIP

/** 创建编码器管理对象, 返回对象类型为ICompressCodecsInfo接口指针
*/
extern "C" {
    HResult DoCreateCodecs(void** outObject)
    {
        if(outObject == NULL)
        {
            return RC_E_FAIL ;
        }
        RCCodecsManager* codecsManager = NULL ;
        try
        {
            RCCodecsRegister* codecsRegister = new RCCodecsRegister() ;
            codecsManager = codecsRegister ;
            HResult hr = codecsRegister->LoadCodecs() ;
            if(IsSuccess(hr))
            {
                ICompressCodecsInfo* codecsInfo = (ICompressCodecsInfo*)codecsManager ;
                codecsInfo->AddRef() ;
                *outObject = codecsInfo ;
            }
            return hr ;
        }
        catch(...)
        {
            if(codecsManager)
            {
                delete codecsManager ;
                codecsManager = NULL ;
            }
            return RC_E_FAIL ;
        }
        return RC_E_FAIL ;
    }
}

END_NAMESPACE_RCZIP

#ifdef RCZIP_OS_WIN

/** DLL 入口函数
*/
BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD   ul_reason_for_call,
                       LPVOID  /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            //初始化
            (void)RCZIP_QUALIFIER RCLang::Instance() ;
            (void)RCZIP_QUALIFIER RCInitCRCTable::Instance() ;
            (void)RCZIP_QUALIFIER RCAlloc::Instance() ;
            (void)RCZIP_QUALIFIER RCBitlDefs::Instance() ;
            (void)RCZIP_QUALIFIER RCBZip2CRCInit::Instance() ;
            (void)RCZIP_QUALIFIER RCRangeCoderInit::Instance() ;
            (void)RCZIP_QUALIFIER RCAesTableInit::Instance() ;
            (void)RCZIP_QUALIFIER RCRandomGenerator::Instance() ;
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE ;
}

extern "C"{
    /** 创建编码器管理对象, 返回对象类型为ICompressCodecsInfo接口指针
    @param [out] outObject 返回ICompressCodecsInfo接口指针，并且引用计数加1
    @return 如果成功则返回RC_S_OK，否则返回错误号
    */
    RCZIP_API HResult CreateCodecs(void** outObject)
    {
        return RCZIP_QUALIFIER DoCreateCodecs(outObject) ;
    }

    /** 设置资源文件名
    */
    RCZIP_API void SetResFileName(const TCHAR* szFileName)
    {
        RCZIP_QUALIFIER RCLang::Instance().SetResFileName(szFileName) ;
    }
}

#else
    #error "Can't support other OS now!"
#endif

#endif // RC_STATIC_SFX
