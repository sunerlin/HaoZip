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
#include "crypto/7zAES/RC7zAESInfo.h"
#include "crypto/Rar20/RCRar20DecoderInfo.h"
#include "crypto/RarAES/RCRarAESDecoderInfo.h"
#include "crypto/WzAES/RCWzAESInfo.h"
#include "crypto/Zip/RCZipCryptoInfo.h"
#include "compress/copy/RCCopyCoderInfo.h"
#include "compress/byteswap/RCByteSwap2Info.h"
#include "compress/byteswap/RCByteSwap4Info.h"
#include "compress/shrink/RCShrinkDecoderInfo.h"
#include "compress/lzh/RCLzhHuffmanDecoderInfo.h"
#include "compress/z/RCZDecoderInfo.h"
#include "compress/arj/RCArjDecoder1Info.h"
#include "compress/arj/RCArjDecoder2Info.h"
#include "compress/lzx/RCLzxDecoderInfo.h"
#include "compress/lzma/RCLzmaCoderInfo.h"
#include "compress/implode/RCImplodeDecoderInfo.h"
#include "compress/branch/RCBranchARMInfo.h"
#include "compress/branch/RCBranchARMTInfo.h"
#include "compress/branch/RCBranchBCJ2Info.h"
#include "compress/branch/RCBranchBCJInfo.h"
#include "compress/branch/RCBranchIA64Info.h"
#include "compress/branch/RCBranchPPCInfo.h"
#include "compress/branch/RCBranchSPARCInfo.h"
#include "compress/ppmd/RCPPMDCoderInfo.h"
#include "compress/ppmdi1/RCPPMDI1CoderInfo.h"
#include "compress/quantum/RCQuantumDecoderInfo.h"
#include "compress/rar/RCRar1DecoderInfo.h"
#include "compress/rar/RCRar2DecoderInfo.h"
#include "compress/rar/RCRar3DecoderInfo.h"
#include "compress/bzip2/RCBZip2CoderInfo.h"
#include "compress/deflate/RCDeflateCOMCoderInfo.h"
#include "compress/deflate/RCDeflateCOMCoder64Info.h"
#include "compress/deflate/RCDeflateNSISCoderInfo.h"
#include "compress/deflate/RCZlibCoderInfo.h"
#include "compress/wavpack/RCWavPackCoderInfo.h"
#include "compress/lzma2/RCLzma2CoderInfo.h"
#include "compress/delta/RCDeltaCoderInfo.h"
#include "compress/lz/RCAdcDecoderInfo.h"

/////////////////////////////////////////////////////////////////
//RCCodecsRegister class implementation

BEGIN_NAMESPACE_RCZIP

RCCodecsRegister::RCCodecsRegister()
{
}

RCCodecsRegister::~RCCodecsRegister()
{
}

HResult RCCodecsRegister::LoadCodecs(void)
{
    //在这里注册各种编码和解码器
    ICodecInfoPtr spCodecInfo ;
    spCodecInfo = new RC7zAESInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCRar20DecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCRarAESDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCWzAESInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCZipCryptoInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCCopyCoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCByteSwap2Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCByteSwap4Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCShrinkDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCLzhHuffmanDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCZDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCArjDecoder1Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCArjDecoder2Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCLzxDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCLzmaCoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCImplodeDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchARMInfo ;
    RegisterCodec(spCodecInfo) ;

    spCodecInfo = new RCBranchARMTInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchBCJ2Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchBCJInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchIA64Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchPPCInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBranchSPARCInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCPPMDCoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCQuantumDecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCRar1DecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCRar2DecoderInfo ;
    RegisterCodec(spCodecInfo) ; 
    
    spCodecInfo = new RCRar3DecoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCBZip2CoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCDeflateCOMCoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCDeflateCOMCoder64Info ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCDeflateNSISCoderInfo ;
    RegisterCodec(spCodecInfo) ;
    
    spCodecInfo = new RCZlibCoderInfo ;
    RegisterCodec(spCodecInfo) ;

    spCodecInfo = new RCPPMDI1CoderInfo;
    RegisterCodec(spCodecInfo) ;

    spCodecInfo = new RCWavPackCoderInfo;
    RegisterCodec(spCodecInfo);
     
    spCodecInfo = new RCLzma2CoderInfo;
    RegisterCodec(spCodecInfo);

    spCodecInfo = new RCDeltaCoderInfo;
    RegisterCodec(spCodecInfo);

    spCodecInfo = new RCAdcDecoderInfo;
    RegisterCodec(spCodecInfo);
    
    return RC_S_OK ;
}

END_NAMESPACE_RCZIP
