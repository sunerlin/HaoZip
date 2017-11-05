/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCWavUnpack_h_
#define __RCWavUnpack_h_ 1

#if defined(WIN32)
#include <windows.h>
#include <io.h>
#endif

#include <math.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "base/RCTypes.h"
#include "interface/IStream.h"
#include "interface/ICoder.h"
#include "wavpack/wavpack.h"

#define NO_ERROR 0L
#define SOFT_ERROR 1
#define HARD_ERROR 2
#define USER_ABORT 3

BEGIN_NAMESPACE_RCZIP

/** WavPack 解压
@param [in] inFileStream 输入流
@param [in] outFileStream 输出流
@param [in] progressCallback 进度回调接口
@return 返回解压结果
*/
int RcWavUnpackFile(void* inFileStream, void* outFileStream, ICompressProgressInfo* progressCallback);

END_NAMESPACE_RCZIP

#endif //__RCWavUnpack_h_
