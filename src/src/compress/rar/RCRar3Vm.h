/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3Vm_h_
#define __RCRar3Vm_h_ 1

#include "RCRar3VmDefs.h"
#include "base/RCNonCopyable.h"
#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

class RCRar3Operand ;
class RCRar3VmMemBitDecoder ;
class RCRar3Program ;
class RCRar3BlockRef ;

class RCRar3Vm:
    private RCNonCopyable
{
public:

    /** 默认构造函数
    */
    RCRar3Vm() ;
    
    /** 默认析构函数
    */
    ~RCRar3Vm() ;
    
public:
    bool Create() ;
    void PrepareProgram(const byte_t* code, uint32_t codeSize, RCRar3Program* prg);
    void SetMemory(uint32_t pos, const byte_t* data, uint32_t dataSize) ;
    bool Execute(RCRar3Program* prg, 
                 const RCRar3ProgramInitState* initState,
                 RCRar3BlockRef& outBlockRef, 
                 RCVector<byte_t>& outGlobalData);
    const byte_t* GetDataPointer(uint32_t offset) const ;
      
public:
    static void SetValue(void* addr, uint32_t value) ;
    
private:
    static uint32_t GetValue(bool byteMode, const void* addr) ;
    static void SetValue(bool byteMode, void* addr, uint32_t value) ;
    
private:
    uint32_t GetFixedGlobalValue32(uint32_t globalOffset) ;
    void SetBlockSize(uint32_t v) ;
    void SetBlockPos(uint32_t v) ;
    uint32_t GetOperand32(const RCRar3Operand* op) const ;
    void SetOperand32(const RCRar3Operand *op, uint32_t val);
    byte_t GetOperand8(const RCRar3Operand* op) const ;
    void SetOperand8(const RCRar3Operand* op, byte_t val);
    uint32_t GetOperand(bool byteMode, const RCRar3Operand* op) const ;
    void SetOperand(bool byteMode, const RCRar3Operand* op, uint32_t val) ;
    void DecodeArg(RCRar3VmMemBitDecoder& inp, RCRar3Operand& op, bool byteMode) ;
    bool ExecuteCode(const RCRar3Program* prg) ;
    void ReadVmProgram(const byte_t* code, uint32_t codeSize, RCRar3Program* prg) ;
    
#ifdef RARVM_STANDARD_FILTERS
    void ExecuteStandardFilter(int32_t filterIndex) ;
    static int32_t FindStandardFilter(const byte_t* code, uint32_t codeSize) ;
    static void E8E9Decode(byte_t* data, uint32_t dataSize, uint32_t fileOffset, bool e9) ;
    static uint32_t ItaniumGetOpType(const byte_t* data, int32_t bitPos) ;
    static void ItaniumDecode(byte_t* data, uint32_t dataSize, uint32_t fileOffset) ;
    static void DeltaDecode(byte_t* data, uint32_t dataSize, uint32_t numChannels) ;
    static void RgbDecode(byte_t* srcData, uint32_t dataSize, uint32_t width, uint32_t posR) ;
    static void AudioDecode(byte_t* srcData, uint32_t dataSize, uint32_t numChannels) ;
    static uint32_t UpCaseDecode(byte_t* data, uint32_t dataSize) ;
    
#endif
    
private:
    byte_t* m_mem ;
    uint32_t m_R[RCRar3VmDefs::s_kNumRegs + 1] ; // R[kNumRegs] = 0 always (speed optimization)
    uint32_t m_flags ;
};

END_NAMESPACE_RCZIP

#endif //__RCRar3Vm_h_
