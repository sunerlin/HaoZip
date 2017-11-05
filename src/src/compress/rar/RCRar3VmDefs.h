/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCRar3VmDefs_h_
#define __RCRar3VmDefs_h_ 1

#include "common/RCVector.h"

BEGIN_NAMESPACE_RCZIP

#define RARVM_STANDARD_FILTERS

class RCRar3VmDefs
{
public:
    static const int32_t  s_kNumRegBits = 3 ;
    static const uint32_t s_kNumRegs = 1 << s_kNumRegBits ;
    static const uint32_t s_kNumGpRegs = s_kNumRegs - 1 ;

    static const uint32_t s_kSpaceSize = 0x40000 ;
    static const uint32_t s_kSpaceMask = s_kSpaceSize -1 ;
    static const uint32_t s_kGlobalOffset = 0x3C000 ;
    static const uint32_t s_kGlobalSize = 0x2000 ;
    static const uint32_t s_kFixedGlobalSize = 64 ;
};

class RCRar3GlobalOffset
{
public:
    static const uint32_t s_kBlockSize = 0x1C;
    static const uint32_t s_kBlockPos  = 0x20;
    static const uint32_t s_kExecCount = 0x2C;
    static const uint32_t s_kGlobalMemOutSize = 0x30;
};

enum RCRar3CommandType
{
  CMD_MOV,  CMD_CMP,  CMD_ADD,  CMD_SUB,  CMD_JZ,   CMD_JNZ,  CMD_INC,  CMD_DEC,
  CMD_JMP,  CMD_XOR,  CMD_AND,  CMD_OR,   CMD_TEST, CMD_JS,   CMD_JNS,  CMD_JB,
  CMD_JBE,  CMD_JA,   CMD_JAE,  CMD_PUSH, CMD_POP,  CMD_CALL, CMD_RET,  CMD_NOT,
  CMD_SHL,  CMD_SHR,  CMD_SAR,  CMD_NEG,  CMD_PUSHA,CMD_POPA, CMD_PUSHF,CMD_POPF,
  CMD_MOVZX,CMD_MOVSX,CMD_XCHG, CMD_MUL,  CMD_DIV,  CMD_ADC,  CMD_SBB,  CMD_PRINT,

  CMD_MOVB, CMD_CMPB, CMD_ADDB, CMD_SUBB, CMD_INCB, CMD_DECB,
  CMD_XORB, CMD_ANDB, CMD_ORB,  CMD_TESTB,CMD_NEGB,
  CMD_SHLB, CMD_SHRB, CMD_SARB, CMD_MULB
};

enum RCRar3OpType
{
    OP_TYPE_REG, 
    OP_TYPE_INT, 
    OP_TYPE_REGMEM, 
    OP_TYPE_NONE
};

// Addr in COperand object can link (point) to CVm object!!!

class RCRar3Operand
{
public:
    RCRar3OpType Type ;
    uint32_t Data;
    uint32_t Base;
    RCRar3Operand():
        Type(OP_TYPE_NONE),
        Data(0),
        Base(0)
    {
    }
};

struct RCRar3Command
{
    RCRar3CommandType OpCode ;
    bool ByteMode ;
    RCRar3Operand Op1 ;
    RCRar3Operand Op2 ;
};

class RCRar3BlockRef
{
public:
    uint32_t Offset ;
    uint32_t Size ;
};

class RCRar3Program
{
public:
    RCVector<RCRar3Command> Commands ;
#ifdef RARVM_STANDARD_FILTERS
    int32_t StandardFilterIndex ;
#endif
    RCVector<byte_t> StaticData;
};

class RCRar3ProgramInitState
{
public:
    uint32_t InitR[RCRar3VmDefs::s_kNumGpRegs];
    RCVector<byte_t> GlobalData;

    void AllocateEmptyFixedGlobal()
    {
        GlobalData.clear();
        GlobalData.reserve(RCRar3VmDefs::s_kFixedGlobalSize);
        for (uint32_t i = 0; i < RCRar3VmDefs::s_kFixedGlobalSize; i++)
        {
            GlobalData.push_back(0) ;
        }
    }
};

END_NAMESPACE_RCZIP

#endif //__RCRar3VmDefs_h_
