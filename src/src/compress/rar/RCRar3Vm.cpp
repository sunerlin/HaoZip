/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "RCRar3Vm.h"
#include "RCRar3VmDefs.h"
#include "RCRar3VmMemBitDecoder.h"
#include "common/RCAlloc.h"
#include "RCRar3VmUtils.h"
#include "algorithm/CpuArch.h"
#include "algorithm/7zCrc.h"

/////////////////////////////////////////////////////////////////
//RCRar3Vm class implementation

BEGIN_NAMESPACE_RCZIP

static const uint32_t s_kRar3VmStackRegIndex = RCRar3VmDefs::s_kNumRegs - 1;

static const uint32_t RAR3_VM_FLAG_C = 1;
static const uint32_t RAR3_VM_FLAG_Z = 2;
static const uint32_t RAR3_VM_FLAG_S = 0x80000000;

static const byte_t RAR3_VM_CF_OP0 = 0;
static const byte_t RAR3_VM_CF_OP1 = 1;
static const byte_t RAR3_VM_CF_OP2 = 2;
static const byte_t RAR3_VM_CF_OPMASK = 3;
static const byte_t RAR3_VM_CF_BYTEMODE = 4;
static const byte_t RAR3_VM_CF_JUMP = 8;
static const byte_t RAR3_VM_CF_PROC = 16;
static const byte_t RAR3_VM_CF_USEFLAGS = 32;
static const byte_t RAR3_VM_CF_CHFLAGS = 64;

static byte_t s_kRar3VmCmdFlags[]=
{
  /* CMD_MOV   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE,
  /* CMD_CMP   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_ADD   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_SUB   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_JZ    */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JNZ   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_INC   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_DEC   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_JMP   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP,
  /* CMD_XOR   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_AND   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_OR    */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_TEST  */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_JS    */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JNS   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JB    */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JBE   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JA    */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_JAE   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_JUMP | RAR3_VM_CF_USEFLAGS,
  /* CMD_PUSH  */ RAR3_VM_CF_OP1,
  /* CMD_POP   */ RAR3_VM_CF_OP1,
  /* CMD_CALL  */ RAR3_VM_CF_OP1 | RAR3_VM_CF_PROC,
  /* CMD_RET   */ RAR3_VM_CF_OP0 | RAR3_VM_CF_PROC,
  /* CMD_NOT   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_BYTEMODE,
  /* CMD_SHL   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_SHR   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_SAR   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_NEG   */ RAR3_VM_CF_OP1 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_CHFLAGS,
  /* CMD_PUSHA */ RAR3_VM_CF_OP0,
  /* CMD_POPA  */ RAR3_VM_CF_OP0,
  /* CMD_PUSHF */ RAR3_VM_CF_OP0 | RAR3_VM_CF_USEFLAGS,
  /* CMD_POPF  */ RAR3_VM_CF_OP0 | RAR3_VM_CF_CHFLAGS,
  /* CMD_MOVZX */ RAR3_VM_CF_OP2,
  /* CMD_MOVSX */ RAR3_VM_CF_OP2,
  /* CMD_XCHG  */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE,
  /* CMD_MUL   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE,
  /* CMD_DIV   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE,
  /* CMD_ADC   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_USEFLAGS | RAR3_VM_CF_CHFLAGS ,
  /* CMD_SBB   */ RAR3_VM_CF_OP2 | RAR3_VM_CF_BYTEMODE | RAR3_VM_CF_USEFLAGS | RAR3_VM_CF_CHFLAGS ,
  /* CMD_PRINT */ RAR3_VM_CF_OP0
};


#define SET_IP(IP) \
                    if ((IP) >= numCommands) return true; \
                    if (--maxOpCount <= 0) return false; \
                    cmd = commands + (IP);

#define GET_FLAG_S_B(res) (((res) & 0x80) ? RAR3_VM_FLAG_S : 0)
#define SET_IP_OP1 { uint32_t val = GetOperand32(&cmd->Op1); SET_IP(val); }
#define FLAGS_UPDATE_SZ m_flags = res == 0 ? RAR3_VM_FLAG_Z : res & RAR3_VM_FLAG_S
#define FLAGS_UPDATE_SZ_B m_flags = (res & 0xFF) == 0 ? RAR3_VM_FLAG_Z : GET_FLAG_S_B(res)

RCRar3Vm::RCRar3Vm():
    m_mem(NULL),
    m_flags(0)
{
}

RCRar3Vm::~RCRar3Vm()
{
    RCAlloc::Instance().MyFree(m_mem);
}

uint32_t RCRar3Vm::GetValue(bool byteMode, const void *addr)
{
    if (byteMode)
    {
        return(*(const byte_t *)addr);
    }
    else
    {
        return GetUi32(addr);
    }
}

void RCRar3Vm::SetValue(bool byteMode, void* addr, uint32_t value)
{
    if (byteMode)
    {
        *(byte_t *)addr = (byte_t)value;
    }
    else
    {
        SetUi32(addr, value);
    }
}

uint32_t RCRar3Vm::GetFixedGlobalValue32(uint32_t globalOffset)
{
    return GetValue(false, &m_mem[RCRar3VmDefs::s_kGlobalOffset + globalOffset]);
}

void RCRar3Vm::SetBlockSize(uint32_t v)
{
    SetValue(&m_mem[RCRar3VmDefs::s_kGlobalOffset + RCRar3GlobalOffset::s_kBlockSize], v);
}

void RCRar3Vm::SetBlockPos(uint32_t v)
{
    SetValue(&m_mem[RCRar3VmDefs::s_kGlobalOffset + RCRar3GlobalOffset::s_kBlockPos], v);
}

void RCRar3Vm::SetValue(void* addr, uint32_t value)
{
    SetValue(false, addr, value);
}

uint32_t RCRar3Vm::GetOperand32(const RCRar3Operand* op) const
{
    switch(op->Type)
    {
        case OP_TYPE_REG: 
            return m_R[op->Data];
        case OP_TYPE_REGMEM: 
            return RCRar3VmUtils::GetValue32(&m_mem[(op->Base + m_R[op->Data]) & RCRar3VmDefs::s_kSpaceMask]);
        default: 
            return op->Data;
    }
}

void RCRar3Vm::SetOperand32(const RCRar3Operand *op, uint32_t val)
{
    switch(op->Type)
    {
        case OP_TYPE_REG: 
            m_R[op->Data] = val; 
            return;
        case OP_TYPE_REGMEM: 
            RCRar3VmUtils::SetValue32(&m_mem[(op->Base + m_R[op->Data]) & RCRar3VmDefs::s_kSpaceMask], val); 
            return;
    }
}

byte_t RCRar3Vm::GetOperand8(const RCRar3Operand* op) const
{
    switch(op->Type)
    {
        case OP_TYPE_REG: 
            return (byte_t)m_R[op->Data];
        case OP_TYPE_REGMEM: 
            return m_mem[(op->Base + m_R[op->Data]) & RCRar3VmDefs::s_kSpaceMask];
        default: 
            return (byte_t)op->Data;
    }
}

void RCRar3Vm::SetOperand8(const RCRar3Operand* op, byte_t val)
{
    switch(op->Type)
    {
        case OP_TYPE_REG: 
            m_R[op->Data] = (m_R[op->Data] & 0xFFFFFF00) | val; 
            return;
        case OP_TYPE_REGMEM: 
            m_mem[(op->Base + m_R[op->Data]) & RCRar3VmDefs::s_kSpaceMask] = val; 
            return;
    }
}

uint32_t RCRar3Vm::GetOperand(bool byteMode, const RCRar3Operand* op) const
{
    if (byteMode)
    {
        return GetOperand8(op);
    }
    return GetOperand32(op);
}

void RCRar3Vm::SetOperand(bool byteMode, const RCRar3Operand* op, uint32_t val)
{
    if (byteMode)
    {
        SetOperand8(op, (byte_t)(val & 0xFF));
    }
    else
    {
        SetOperand32(op, val);
    }
}

void RCRar3Vm::DecodeArg(RCRar3VmMemBitDecoder& inp, RCRar3Operand& op, bool byteMode)
{
    if (inp.ReadBit())
    {
        op.Type = OP_TYPE_REG;
        op.Data = inp.ReadBits(RCRar3VmDefs::s_kNumRegBits);
    }
    else if (inp.ReadBit() == 0)
    {
        op.Type = OP_TYPE_INT;
        if (byteMode)
        {
            op.Data = inp.ReadBits(8);
        }
        else
        {
            op.Data = RCRar3VmUtils::ReadEncodedUInt32(inp);
        }
    }
    else
    {
        op.Type = OP_TYPE_REGMEM;
        if (inp.ReadBit() == 0)
        {
            op.Data = inp.ReadBits(RCRar3VmDefs::s_kNumRegBits);
            op.Base = 0;
        }
        else
        {
            if (inp.ReadBit() == 0)
            {
                op.Data = inp.ReadBits(RCRar3VmDefs::s_kNumRegBits);
            }
            else
            {
                op.Data = RCRar3VmDefs::s_kNumRegs;
            }
            op.Base = RCRar3VmUtils::ReadEncodedUInt32(inp);
        }
    }
}

bool RCRar3Vm::ExecuteCode(const RCRar3Program* prg)
{
    int32_t maxOpCount = 25000000;
    const RCRar3Command *commands = &prg->Commands[0];
    const RCRar3Command *cmd = commands;
    uint32_t numCommands = (uint32_t)prg->Commands.size();
    for (;;)
    {
        switch(cmd->OpCode)
        {
#ifndef RARVM_NO_VM

      case CMD_MOV:
          SetOperand32(&cmd->Op1, GetOperand32(&cmd->Op2));
          break;
      case CMD_MOVB:
          SetOperand8(&cmd->Op1, GetOperand8(&cmd->Op2));
          break;
      case CMD_CMP:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              uint32_t res = v1 - GetOperand32(&cmd->Op2);
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : (res > v1) | (res & RAR3_VM_FLAG_S);
          }
          break;
      case CMD_CMPB:
          {
              byte_t v1 = GetOperand8(&cmd->Op1);
              byte_t res = v1 - GetOperand8(&cmd->Op2);
              res &= 0xFF;
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : (res > v1) | GET_FLAG_S_B(res);
          }
          break;
      case CMD_ADD:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              uint32_t res = v1 + GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
              m_flags = (res < v1) | (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S));
          }
          break;
      case CMD_ADDB:
          {
              byte_t v1 = GetOperand8(&cmd->Op1);
              byte_t res = v1 + GetOperand8(&cmd->Op2);
              res &= 0xFF;
              SetOperand8(&cmd->Op1, (byte_t)res);
              m_flags = (res < v1) | (res == 0 ? RAR3_VM_FLAG_Z : GET_FLAG_S_B(res));
          }
          break;
      case CMD_ADC:
          {
              uint32_t v1 = GetOperand(cmd->ByteMode, &cmd->Op1);
              uint32_t FC = (m_flags & RAR3_VM_FLAG_C);
              uint32_t res = v1 + GetOperand(cmd->ByteMode, &cmd->Op2) + FC;
              if (cmd->ByteMode)
                  res &= 0xFF;
              SetOperand(cmd->ByteMode, &cmd->Op1, res);
              m_flags = (res < v1 || res == v1 && FC) | (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S));
          }
          break;
      case CMD_SUB:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              uint32_t res = v1 - GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : (res > v1) | (res & RAR3_VM_FLAG_S);
          }
          break;
      case CMD_SUBB:
          {
              uint32_t v1 = GetOperand8(&cmd->Op1);
              uint32_t res = v1 - GetOperand8(&cmd->Op2);
              SetOperand8(&cmd->Op1, (byte_t)res);
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : (res > v1) | (res & RAR3_VM_FLAG_S);
          }
          break;
      case CMD_SBB:
          {
              uint32_t v1 = GetOperand(cmd->ByteMode, &cmd->Op1);
              uint32_t FC = (m_flags & RAR3_VM_FLAG_C);
              uint32_t res = v1 - GetOperand(cmd->ByteMode, &cmd->Op2) - FC;
              // m_flags = res == 0 ? RAR3_VM_FLAG_Z : (res > v1 || res == v1 && FC) | (res & RAR3_VM_FLAG_S);
              if (cmd->ByteMode)
                  res &= 0xFF;
              SetOperand(cmd->ByteMode, &cmd->Op1, res);
              m_flags = (res > v1 || res == v1 && FC) | (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S));
          }
          break;
      case CMD_INC:
          {
              uint32_t res = GetOperand32(&cmd->Op1) + 1;
              SetOperand32(&cmd->Op1, res);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_INCB:
          {
              byte_t res = GetOperand8(&cmd->Op1) + 1;
              SetOperand8(&cmd->Op1, res);;
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_DEC:
          {
              uint32_t res = GetOperand32(&cmd->Op1) - 1;
              SetOperand32(&cmd->Op1, res);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_DECB:
          {
              byte_t res = GetOperand8(&cmd->Op1) - 1;
              SetOperand8(&cmd->Op1, res);;
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_XOR:
          {
              uint32_t res = GetOperand32(&cmd->Op1) ^ GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_XORB:
          {
              byte_t res = GetOperand8(&cmd->Op1) ^ GetOperand8(&cmd->Op2);
              SetOperand8(&cmd->Op1, res);
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_AND:
          {
              uint32_t res = GetOperand32(&cmd->Op1) & GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_ANDB:
          {
              byte_t res = GetOperand8(&cmd->Op1) & GetOperand8(&cmd->Op2);
              SetOperand8(&cmd->Op1, res);
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_OR:
          {
              uint32_t res = GetOperand32(&cmd->Op1) | GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_ORB:
          {
              byte_t res = GetOperand8(&cmd->Op1) | GetOperand8(&cmd->Op2);
              SetOperand8(&cmd->Op1, res);
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_TEST:
          {
              uint32_t res = GetOperand32(&cmd->Op1) & GetOperand32(&cmd->Op2);
              FLAGS_UPDATE_SZ;
          }
          break;
      case CMD_TESTB:
          {
              byte_t res = GetOperand8(&cmd->Op1) & GetOperand8(&cmd->Op2);
              FLAGS_UPDATE_SZ_B;
          }
          break;
      case CMD_NOT:
          SetOperand(cmd->ByteMode, &cmd->Op1, ~GetOperand(cmd->ByteMode, &cmd->Op1));
          break;
      case CMD_NEG:
          {
              uint32_t res = 0 - GetOperand32(&cmd->Op1);
              SetOperand32(&cmd->Op1, res);
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : RAR3_VM_FLAG_C | (res & RAR3_VM_FLAG_S);
          }
          break;
      case CMD_NEGB:
          {
              byte_t res = (byte_t)(0 - GetOperand8(&cmd->Op1));
              SetOperand8(&cmd->Op1, res);
              m_flags = res == 0 ? RAR3_VM_FLAG_Z : RAR3_VM_FLAG_C | GET_FLAG_S_B(res);
          }
          break;

      case CMD_SHL:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand32(&cmd->Op2);
              uint32_t res = v1 << v2;
              SetOperand32(&cmd->Op1, res);
              m_flags = (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S)) | ((v1 << (v2 - 1)) & 0x80000000 ? RAR3_VM_FLAG_C : 0);
          }
          break;
      case CMD_SHLB:
          {
              byte_t v1 = GetOperand8(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand8(&cmd->Op2);
              byte_t res = (byte_t)(v1 << v2);
              SetOperand8(&cmd->Op1, res);
              m_flags = (res == 0 ? RAR3_VM_FLAG_Z : GET_FLAG_S_B(res)) | ((v1 << (v2 - 1)) & 0x80 ? RAR3_VM_FLAG_C : 0);
          }
          break;
      case CMD_SHR:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand32(&cmd->Op2);
              uint32_t res = v1 >> v2;
              SetOperand32(&cmd->Op1, res);
              m_flags = (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S)) | ((v1 >> (v2 - 1)) & RAR3_VM_FLAG_C);
          }
          break;
      case CMD_SHRB:
          {
              byte_t v1 = GetOperand8(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand8(&cmd->Op2);
              byte_t res = (byte_t)(v1 >> v2);
              SetOperand8(&cmd->Op1, res);
              m_flags = (res == 0 ? RAR3_VM_FLAG_Z : GET_FLAG_S_B(res)) | ((v1 >> (v2 - 1)) & RAR3_VM_FLAG_C);
          }
          break;
      case CMD_SAR:
          {
              uint32_t v1 = GetOperand32(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand32(&cmd->Op2);
              uint32_t res = uint32_t(((int32_t)v1) >> v2);
              SetOperand32(&cmd->Op1, res);
              m_flags= (res == 0 ? RAR3_VM_FLAG_Z : (res & RAR3_VM_FLAG_S)) | ((v1 >> (v2 - 1)) & RAR3_VM_FLAG_C);
          }
          break;
      case CMD_SARB:
          {
              byte_t v1 = GetOperand8(&cmd->Op1);
              int32_t v2 = (int32_t)GetOperand8(&cmd->Op2);
              byte_t res = (byte_t)(((signed char)v1) >> v2);
              SetOperand8(&cmd->Op1, res);
              m_flags= (res == 0 ? RAR3_VM_FLAG_Z : GET_FLAG_S_B(res)) | ((v1 >> (v2 - 1)) & RAR3_VM_FLAG_C);
          }
          break;

      case CMD_JMP:
          SET_IP_OP1;
          continue;
      case CMD_JZ:
          if ((m_flags & RAR3_VM_FLAG_Z) != 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JNZ:
          if ((m_flags & RAR3_VM_FLAG_Z) == 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JS:
          if ((m_flags & RAR3_VM_FLAG_S) != 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JNS:
          if ((m_flags & RAR3_VM_FLAG_S) == 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JB:
          if ((m_flags & RAR3_VM_FLAG_C) != 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JBE:
          if ((m_flags & (RAR3_VM_FLAG_C | RAR3_VM_FLAG_Z)) != 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JA:
          if ((m_flags & (RAR3_VM_FLAG_C | RAR3_VM_FLAG_Z)) == 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;
      case CMD_JAE:
          if ((m_flags & RAR3_VM_FLAG_C) == 0)
          {
              SET_IP_OP1;
              continue;
          }
          break;

      case CMD_PUSH:
          m_R[s_kRar3VmStackRegIndex] -= 4;
          RCRar3VmUtils::SetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex] & RCRar3VmDefs::s_kSpaceMask], GetOperand32(&cmd->Op1));
          break;
      case CMD_POP:
          SetOperand32(&cmd->Op1, RCRar3VmUtils::GetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex] & RCRar3VmDefs::s_kSpaceMask]));
          m_R[s_kRar3VmStackRegIndex] += 4;
          break;
      case CMD_CALL:
          m_R[s_kRar3VmStackRegIndex] -= 4;
          RCRar3VmUtils::SetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex] & RCRar3VmDefs::s_kSpaceMask], (uint32_t)(cmd - commands + 1));
          SET_IP_OP1;
          continue;

      case CMD_PUSHA:
          {
              for (uint32_t i = 0, SP = m_R[s_kRar3VmStackRegIndex] - 4; i < RCRar3VmDefs::s_kNumRegs; i++, SP -= 4)
                  RCRar3VmUtils::SetValue32(&m_mem[SP & RCRar3VmDefs::s_kSpaceMask], m_R[i]);
              m_R[s_kRar3VmStackRegIndex] -= RCRar3VmDefs::s_kNumRegs * 4;
          }
          break;
      case CMD_POPA:
          {
              for (uint32_t i = 0, SP = m_R[s_kRar3VmStackRegIndex]; i < RCRar3VmDefs::s_kNumRegs; i++, SP += 4)
                  m_R[s_kRar3VmStackRegIndex - i] = RCRar3VmUtils::GetValue32(&m_mem[SP & RCRar3VmDefs::s_kSpaceMask]);
          }
          break;
      case CMD_PUSHF:
          m_R[s_kRar3VmStackRegIndex] -= 4;
          RCRar3VmUtils::SetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex]&RCRar3VmDefs::s_kSpaceMask], m_flags);
          break;
      case CMD_POPF:
          m_flags = RCRar3VmUtils::GetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex] & RCRar3VmDefs::s_kSpaceMask]);
          m_R[s_kRar3VmStackRegIndex] += 4;
          break;

      case CMD_MOVZX:
          SetOperand32(&cmd->Op1, GetOperand8(&cmd->Op2));
          break;
      case CMD_MOVSX:
          SetOperand32(&cmd->Op1, (uint32_t)(int32_t)(signed char)GetOperand8(&cmd->Op2));
          break;
      case CMD_XCHG:
          {
              uint32_t v1 = GetOperand(cmd->ByteMode, &cmd->Op1);
              SetOperand(cmd->ByteMode, &cmd->Op1, GetOperand(cmd->ByteMode, &cmd->Op2));
              SetOperand(cmd->ByteMode, &cmd->Op2, v1);
          }
          break;
      case CMD_MUL:
          {
              uint32_t res = GetOperand32(&cmd->Op1) * GetOperand32(&cmd->Op2);
              SetOperand32(&cmd->Op1, res);
          }
          break;
      case CMD_MULB:
          {
              byte_t res = GetOperand8(&cmd->Op1) * GetOperand8(&cmd->Op2);
              SetOperand8(&cmd->Op1, res);
          }
          break;
      case CMD_DIV:
          {
              uint32_t divider = GetOperand(cmd->ByteMode, &cmd->Op2);
              if (divider != 0)
              {
                  uint32_t res = GetOperand(cmd->ByteMode, &cmd->Op1) / divider;
                  SetOperand(cmd->ByteMode, &cmd->Op1, res);
              }
          }
          break;

#endif

      case CMD_RET:
          {
              if (m_R[s_kRar3VmStackRegIndex] >= RCRar3VmDefs::s_kSpaceSize)
              {
                  return true;
              }
              uint32_t ip = RCRar3VmUtils::GetValue32(&m_mem[m_R[s_kRar3VmStackRegIndex] & RCRar3VmDefs::s_kSpaceMask]);
              SET_IP(ip);
              m_R[s_kRar3VmStackRegIndex] += 4;
              continue;
          }
      case CMD_PRINT:
          break;
        }
        cmd++;
        --maxOpCount;
    }
}

void RCRar3Vm::ReadVmProgram(const byte_t* code, uint32_t codeSize, RCRar3Program* prg)
{
    RCRar3VmMemBitDecoder inp;
    inp.Init(code, codeSize);

    prg->StaticData.clear();
    if (inp.ReadBit())
    {
        uint32_t dataSize = RCRar3VmUtils::ReadEncodedUInt32(inp) + 1;
        for (uint32_t i = 0; inp.Avail() && i < dataSize; i++)
        {
            prg->StaticData.push_back((byte_t)inp.ReadBits(8));
        }
    }
    while (inp.Avail())
    {
        prg->Commands.push_back(RCRar3Command());
        RCRar3Command *cmd = &prg->Commands.back();
        if (inp.ReadBit() == 0)
        {
            cmd->OpCode = (RCRar3CommandType)inp.ReadBits(3);
        }
        else
        {
            cmd->OpCode = (RCRar3CommandType)(8 + inp.ReadBits(5));
        }
        if (s_kRar3VmCmdFlags[cmd->OpCode] & RAR3_VM_CF_BYTEMODE)
        {
            cmd->ByteMode = (inp.ReadBit()) ? true : false;
        }
        else
        {
            cmd->ByteMode = 0;
        }
        int32_t opNum = (s_kRar3VmCmdFlags[cmd->OpCode] & RAR3_VM_CF_OPMASK);
        if (opNum > 0)
        {
            DecodeArg(inp, cmd->Op1, cmd->ByteMode);
            if (opNum == 2)
            {
                DecodeArg(inp, cmd->Op2, cmd->ByteMode);
            }
            else
            {
                if (cmd->Op1.Type == OP_TYPE_INT && (s_kRar3VmCmdFlags[cmd->OpCode] & (RAR3_VM_CF_JUMP | RAR3_VM_CF_PROC)))
                {
                    int32_t Distance = cmd->Op1.Data;
                    if (Distance >= 256)
                    {
                        Distance -= 256;
                    }
                    else
                    {
                        if (Distance >= 136)
                        {
                            Distance -= 264;
                        }
                        else if (Distance >= 16)
                        {
                            Distance -= 8;
                        }
                        else if (Distance >= 8)
                        {
                            Distance -= 16;
                        }
                        Distance += static_cast<int32_t>(prg->Commands.size()) - 1;
                    }
                    cmd->Op1.Data = Distance;
                }
            }
        }
        if (cmd->ByteMode)
        {
            switch (cmd->OpCode)
            {
                case CMD_MOV: cmd->OpCode = CMD_MOVB; break;
                case CMD_CMP: cmd->OpCode = CMD_CMPB; break;
                case CMD_ADD: cmd->OpCode = CMD_ADDB; break;
                case CMD_SUB: cmd->OpCode = CMD_SUBB; break;
                case CMD_INC: cmd->OpCode = CMD_INCB; break;
                case CMD_DEC: cmd->OpCode = CMD_DECB; break;
                case CMD_XOR: cmd->OpCode = CMD_XORB; break;
                case CMD_AND: cmd->OpCode = CMD_ANDB; break;
                case CMD_OR: cmd->OpCode = CMD_ORB; break;
                case CMD_TEST: cmd->OpCode = CMD_TESTB; break;
                case CMD_NEG: cmd->OpCode = CMD_NEGB; break;
                case CMD_SHL: cmd->OpCode = CMD_SHLB; break;
                case CMD_SHR: cmd->OpCode = CMD_SHRB; break;
                case CMD_SAR: cmd->OpCode = CMD_SARB; break;
                case CMD_MUL: cmd->OpCode = CMD_MULB; break;
            }
        }
    }
}

bool RCRar3Vm::Create()
{
    if (m_mem == NULL)
    {
        m_mem = (byte_t *)RCAlloc::Instance().MyAlloc(RCRar3VmDefs::s_kSpaceSize + 4);
    }
    return (m_mem != NULL);
}

void RCRar3Vm::PrepareProgram(const byte_t* code, uint32_t codeSize, RCRar3Program* prg)
{
    byte_t xorSum = 0;
    for (uint32_t i = 1; i < codeSize; i++)
    {
        xorSum ^= code[i];
    }

    prg->Commands.clear();
#ifdef RARVM_STANDARD_FILTERS
    prg->StandardFilterIndex = -1;
#endif

    if (xorSum == code[0] && codeSize > 0)
    {
#ifdef RARVM_STANDARD_FILTERS
        prg->StandardFilterIndex = FindStandardFilter(code, codeSize);
        if (prg->StandardFilterIndex >= 0)
        {
            return;
        }
#endif
        // 1 byte for checksum
        ReadVmProgram(code + 1, codeSize - 1, prg);
    }
    prg->Commands.push_back(RCRar3Command());
    RCRar3Command *cmd = &prg->Commands.back();
    cmd->OpCode = CMD_RET;
}

void RCRar3Vm::SetMemory(uint32_t pos, const byte_t *data, uint32_t dataSize)
{
    if (pos < RCRar3VmDefs::s_kSpaceSize && data != m_mem + pos)
    {
        memmove(m_mem + pos, data, MyMin(dataSize, RCRar3VmDefs::s_kSpaceSize - pos));
    }
}

bool RCRar3Vm::Execute(RCRar3Program* prg, 
                       const RCRar3ProgramInitState* initState,
                       RCRar3BlockRef& outBlockRef, 
                       RCVector<byte_t>& outGlobalData)
{
    memcpy(m_R, initState->InitR, sizeof(initState->InitR));
    m_R[s_kRar3VmStackRegIndex] = RCRar3VmDefs::s_kSpaceSize;
    m_R[RCRar3VmDefs::s_kNumRegs] = 0;
    m_flags = 0;

    uint32_t globalSize = MyMin((uint32_t)initState->GlobalData.size(), RCRar3VmDefs::s_kGlobalSize);
    if (globalSize != 0)
    {
        memcpy(m_mem + RCRar3VmDefs::s_kGlobalOffset, &initState->GlobalData[0], globalSize);
    }
    uint32_t staticSize = MyMin((uint32_t)prg->StaticData.size(), RCRar3VmDefs::s_kGlobalSize - globalSize);
    if (staticSize != 0)
        memcpy(m_mem + RCRar3VmDefs::s_kGlobalOffset + globalSize, &prg->StaticData[0], staticSize);

    bool res = true;
#ifdef RARVM_STANDARD_FILTERS
    if (prg->StandardFilterIndex >= 0)
    {
        ExecuteStandardFilter(prg->StandardFilterIndex);
    }
    else
#endif
    {
        res = ExecuteCode(prg);
        if (!res)
        {
            prg->Commands[0].OpCode = CMD_RET;
        }
    }
    uint32_t newBlockPos = GetFixedGlobalValue32(RCRar3GlobalOffset::s_kBlockPos) & RCRar3VmDefs::s_kSpaceMask;
    uint32_t newBlockSize = GetFixedGlobalValue32(RCRar3GlobalOffset::s_kBlockSize) & RCRar3VmDefs::s_kSpaceMask;
    if (newBlockPos + newBlockSize >= RCRar3VmDefs::s_kSpaceSize)
    {
        newBlockPos = newBlockSize = 0;
    }
    outBlockRef.Offset = newBlockPos;
    outBlockRef.Size = newBlockSize;

    outGlobalData.clear();
    uint32_t dataSize = GetFixedGlobalValue32(RCRar3GlobalOffset::s_kGlobalMemOutSize);
    dataSize = MyMin(dataSize, RCRar3VmDefs::s_kGlobalSize - RCRar3VmDefs::s_kFixedGlobalSize);
    if (dataSize != 0)
    {
        dataSize += RCRar3VmDefs::s_kFixedGlobalSize;
        outGlobalData.reserve(dataSize);
        for (uint32_t i = 0; i < dataSize; i++)
        {
            outGlobalData.push_back(m_mem[RCRar3VmDefs::s_kGlobalOffset + i]);
        }
    }
    return res;
}

const byte_t* RCRar3Vm::GetDataPointer(uint32_t offset) const
{
    return m_mem + offset;
}

#ifdef RARVM_STANDARD_FILTERS

enum EStandardFilter
{
    SF_E8,
    SF_E8E9,
    SF_ITANIUM,
    SF_RGB,
    SF_AUDIO,
    SF_DELTA,
    SF_UPCASE
};

struct StandardFilterSignature
{
    uint32_t Length;
    uint32_t CRC;
    EStandardFilter Type;
}

kStdFilters[]=
{
            53, 0xad576887, SF_E8,
            57, 0x3cd7e57e, SF_E8E9,
           120, 0x3769893f, SF_ITANIUM,
            29, 0x0e06077d, SF_DELTA,
           149, 0x1c2c5dc8, SF_RGB,
           216, 0xbc85e701, SF_AUDIO,
            40, 0x46b9c560, SF_UPCASE
};

void RCRar3Vm::ExecuteStandardFilter(int32_t filterIndex)
{
    uint32_t dataSize = m_R[4];
    if (dataSize >= RCRar3VmDefs::s_kGlobalOffset)
    {
        return;
    }
    EStandardFilter filterType = kStdFilters[filterIndex].Type;

    switch (filterType)
    {
    case SF_E8:
    case SF_E8E9:
        E8E9Decode(m_mem, dataSize, m_R[6], (filterType == SF_E8E9));
        break;
    case SF_ITANIUM:
        ItaniumDecode(m_mem, dataSize, m_R[6]);
        break;
    case SF_DELTA:
        if (dataSize >= RCRar3VmDefs::s_kGlobalOffset / 2)
        {
            break;
        }
        SetBlockPos(dataSize);
        DeltaDecode(m_mem, dataSize, m_R[0]);
        break;
    case SF_RGB:
        if (dataSize >= RCRar3VmDefs::s_kGlobalOffset / 2)
        {
            break;
        }
        {
            uint32_t width = m_R[0];
            if (width <= 3)
            {
                break;
            }
            SetBlockPos(dataSize);
            RgbDecode(m_mem, dataSize, width, m_R[1]);
        }
        break;
    case SF_AUDIO:
        if (dataSize >= RCRar3VmDefs::s_kGlobalOffset / 2)
        {
            break;
        }
        SetBlockPos(dataSize);
        AudioDecode(m_mem, dataSize, m_R[0]);
        break;
    case SF_UPCASE:
        if (dataSize >= RCRar3VmDefs::s_kGlobalOffset / 2)
        {
            break;
        }
        uint32_t destSize = UpCaseDecode(m_mem, dataSize);
        SetBlockSize(destSize);
        SetBlockPos(dataSize);
        break;
    }
}

int32_t RCRar3Vm::FindStandardFilter(const byte_t* code, uint32_t codeSize)
{
    uint32_t crc = CrcCalc(code, codeSize);
    for (int32_t i = 0; i < sizeof(kStdFilters) / sizeof(kStdFilters[0]); i++)
    {
        StandardFilterSignature &sfs = kStdFilters[i];
        if (sfs.CRC == crc && sfs.Length == codeSize)
        {
            return i;
        }
    }
    return -1 ;
}

void RCRar3Vm::E8E9Decode(byte_t *data, uint32_t dataSize, uint32_t fileOffset, bool e9)
{
    if (dataSize <= 4)
    {
        return;
    }
    dataSize -= 4;
    const uint32_t kFileSize = 0x1000000;
    byte_t cmpByte2 = (e9 ? 0xE9 : 0xE8);
    for (uint32_t curPos = 0; curPos < dataSize;)
    {
        byte_t curByte = *(data++);
        curPos++;
        if (curByte == 0xE8 || curByte == cmpByte2)
        {
            uint32_t offset = curPos + fileOffset;
            uint32_t addr = (int32_t)RCRar3VmUtils::GetValue32(data);
            if (addr < kFileSize)
            {
                RCRar3VmUtils::SetValue32(data, addr - offset);
            }
            else if ((int32_t)addr < 0 && (int32_t)(addr + offset) >= 0)
            {
                RCRar3VmUtils::SetValue32(data, addr + kFileSize);
            }
            data += 4;
            curPos += 4;
        }
    }
}

uint32_t RCRar3Vm::ItaniumGetOpType(const byte_t* data, int32_t bitPos)
{
    return (data[(uint32_t)bitPos >> 3] >> (bitPos & 7)) & 0xF;
}

void RCRar3Vm::ItaniumDecode(byte_t *data, uint32_t dataSize, uint32_t fileOffset)
{
    uint32_t curPos = 0;
    fileOffset >>= 4;
    while (curPos < dataSize - 21)
    {
        int32_t b = (data[0] & 0x1F) - 0x10;
        if (b >= 0)
        {
            static byte_t kCmdMasks[16] = {4,4,6,6,0,0,7,7,4,4,0,0,4,4,0,0};
            byte_t cmdMask = kCmdMasks[b];
            if (cmdMask != 0)
                for (int32_t i = 0; i < 3; i++)
                    if (cmdMask & (1 << i))
                    {
                        int32_t startPos = i * 41 + 18;
                        if (ItaniumGetOpType(data, startPos + 24) == 5)
                        {
                            const uint32_t kMask = 0xFFFFF;
                            byte_t *p = data + ((uint32_t)startPos >> 3);
                            uint32_t bitField =  ((uint32_t)p[0]) | ((uint32_t)p[1] <<  8) | ((uint32_t)p[2] << 16);
                            int32_t inBit = (startPos & 7);
                            uint32_t offset = (bitField >> inBit) & kMask;
                            uint32_t andMask = ~(kMask << inBit);
                            bitField = ((offset - fileOffset) & kMask) << inBit;
                            for (int32_t j = 0; j < 3; j++)
                            {
                                p[j] &= andMask;
                                p[j] |= bitField;
                                andMask >>= 8;
                                bitField >>= 8;
                            }
                        }
                    }
        }
        data += 16;
        curPos += 16;
        fileOffset++;
    }
}

void RCRar3Vm::DeltaDecode(byte_t* data, uint32_t dataSize, uint32_t numChannels)
{
    uint32_t srcPos = 0;
    uint32_t border = dataSize * 2;
    for (uint32_t curChannel = 0; curChannel < numChannels; curChannel++)
    {
        byte_t prevByte = 0;
        for (uint32_t destPos = dataSize + curChannel; destPos < border; destPos += numChannels)
        {
            data[destPos] = (prevByte = prevByte - data[srcPos++]);
        }
    }
}

void RCRar3Vm::RgbDecode(byte_t *srcData, uint32_t dataSize, uint32_t width, uint32_t posR)
{
    byte_t *destData = srcData + dataSize;
    const uint32_t numChannels = 3;
    for (uint32_t curChannel = 0; curChannel < numChannels; curChannel++)
    {
        byte_t prevByte = 0;

        for (uint32_t i = curChannel; i < dataSize; i+= numChannels)
        {
            uint32_t predicted;
            if (i < width)
            {
                predicted = prevByte;
            }
            else
            {
                uint32_t upperLeftByte = destData[i - width];
                uint32_t upperByte = destData[i - width + 3];
                predicted = prevByte + upperByte - upperLeftByte;
                int32_t pa = abs((int32_t)(predicted - prevByte));
                int32_t pb = abs((int32_t)(predicted - upperByte));
                int32_t pc = abs((int32_t)(predicted - upperLeftByte));
                if (pa <= pb && pa <= pc)
                {
                    predicted = prevByte;
                }
                else
                {
                    if (pb <= pc)
                    {
                        predicted = upperByte;
                    }
                    else
                    {
                        predicted = upperLeftByte;
                    }
                }
            }
            destData[i] = prevByte = (byte_t)(predicted - *(srcData++));
        }
    }
    if (dataSize < 3)
    {
        return;
    }
    for (uint32_t i = posR, border = dataSize - 2; i < border; i += 3)
    {
        byte_t g = destData[i + 1];
        destData[i] = destData[i] + g;
        destData[i + 2] = destData[i + 2] + g;
    }
}

void RCRar3Vm::AudioDecode(byte_t *srcData, uint32_t dataSize, uint32_t numChannels)
{
    byte_t *destData = srcData + dataSize;
    for (uint32_t curChannel = 0; curChannel < numChannels; curChannel++)
    {
        uint32_t prevByte = 0, prevDelta = 0, dif[7];
        int32_t D1 = 0, D2 = 0, D3;
        int32_t K1 = 0, K2 = 0, K3 = 0;
        memset(dif, 0, sizeof(dif));

        for (uint32_t i = curChannel, byteCount = 0; i < dataSize; i += numChannels, byteCount++)
        {
            D3 = D2;
            D2 = prevDelta - D1;
            D1 = prevDelta;

            uint32_t predicted = 8 * prevByte + K1 * D1 + K2 * D2 + K3 * D3;
            predicted = (predicted >> 3) & 0xFF;

            uint32_t curByte = *(srcData++);

            predicted -= curByte;
            destData[i] = (byte_t)predicted;
            prevDelta = (uint32_t)(int32_t)(signed char)(predicted - prevByte);
            prevByte = predicted;

            int32_t D = ((int32_t)(signed char)curByte) << 3;

            dif[0] += abs(D);
            dif[1] += abs(D - D1);
            dif[2] += abs(D + D1);
            dif[3] += abs(D - D2);
            dif[4] += abs(D + D2);
            dif[5] += abs(D - D3);
            dif[6] += abs(D + D3);

            if ((byteCount & 0x1F) == 0)
            {
                uint32_t minDif = dif[0], numMinDif = 0;
                dif[0] = 0;
                for (int32_t j = 1; j < sizeof(dif) / sizeof(dif[0]); j++)
                {
                    if (dif[j] < minDif)
                    {
                        minDif = dif[j];
                        numMinDif = j;
                    }
                    dif[j] = 0;
                }
                switch (numMinDif)
                {
                    case 1: if (K1 >= -16) K1--; break;
                    case 2: if (K1 <   16) K1++; break;
                    case 3: if (K2 >= -16) K2--; break;
                    case 4: if (K2 <   16) K2++; break;
                    case 5: if (K3 >= -16) K3--; break;
                    case 6: if (K3 <   16) K3++; break;
                }
            }
        }
    }
}

uint32_t RCRar3Vm::UpCaseDecode(byte_t *data, uint32_t dataSize)
{
    uint32_t srcPos = 0, destPos = dataSize;
    while (srcPos < dataSize)
    {
        byte_t curByte = data[srcPos++];
        if (curByte == 2 && (curByte = data[srcPos++]) != 2)
        {
            curByte -= 32;
        }
        data[destPos++] = curByte;
    }
    return destPos - dataSize;
}

#endif // end of RARVM_STANDARD_FILTERS

END_NAMESPACE_RCZIP
