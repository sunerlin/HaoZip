/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "compress/rar/RCRar3Decoder.h"
#include "common/RCAlloc.h"
#include "RCRar3Vm.h"
#include "RCRar3TempFilter.h"
#include "RCRar3Filter.h"
#include "RCRar3VmUtils.h"
#include "compress/rar/RCRar3Defs.h"
#include "filesystem/RCStreamUtils.h"
#include "RCRar3VmMemBitDecoder.h"

/////////////////////////////////////////////////////////////////
//RCRar3Decoder class implementation

BEGIN_NAMESPACE_RCZIP

static const uint32_t s_kRar3NumAlignReps = 15;

static const uint32_t s_kRar3SymbolReadTable = 256;
static const uint32_t s_kRar3SymbolRep = 259;
static const uint32_t s_kRar3SymbolLen2 = s_kRar3SymbolRep + RCRar3Defs::s_kNumReps;

static const byte_t s_kRar3LenStart[RCRar3Defs::s_kLenTableSize]      = {0,1,2,3,4,5,6,7,8,10,12,14,16,20,24,28,32,40,48,56,64,80,96,112,128,160,192,224};
static const byte_t s_kRar3LenDirectBits[RCRar3Defs::s_kLenTableSize] = {0,0,0,0,0,0,0,0,1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5};

static const byte_t s_kRar3DistDirectBits[RCRar3Defs::s_kDistTableSize] =
  {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  18,18,18,18,18,18,18,18,18,18,18,18};

static const byte_t s_kRar3LevelDirectBits[RCRar3Defs::s_kLevelTableSize] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

static const byte_t s_kRar3Len2DistStarts[RCRar3Defs::s_kNumLen2Symbols]={0,4,8,16,32,64,128,192};
static const byte_t s_kRar3Len2DistDirectBits[RCRar3Defs::s_kNumLen2Symbols]={2,2,3, 4, 5, 6,  6,  6};

static const uint32_t s_kRar3DistLimit3 = 0x2000 - 2;
static const uint32_t s_kRar3DistLimit4 = 0x40000 - 2;

static const uint32_t s_kRar3NormalMatchMinLen = 3;

static const uint32_t s_kRar3VmDataSizeMax = 1 << 16;
static const uint32_t s_kRar3VmCodeSizeMax = 1 << 16;

class RCRar3DecoderReleaser
{
public:
    RCRar3DecoderReleaser(RCRar3Decoder* coder):
        m_coder(coder)
    {
    }
    ~RCRar3DecoderReleaser()
    {
        m_coder->ReleaseStreams();
    }
private:
    RCRar3Decoder* m_coder;
};

uint32_t s_kRar3DistStart[RCRar3Defs::s_kDistTableSize];

class RCRar3DistInit
{
public:
    RCRar3DistInit()
    {
        Init();
    }
    void Init()
    {
        uint32_t start = 0;
        for (uint32_t i = 0; i < RCRar3Defs::s_kDistTableSize; i++)
        {
            s_kRar3DistStart[i] = start;
            start += (1 << s_kRar3DistDirectBits[i]);
        }
    }
} g_rar3DistInit ;

RCRar3Decoder::RCRar3Decoder():
    m_window(NULL),
    m_winPos(0),
    m_wrPtr(0),
    m_lzSize(0),
    m_unpackSize(0),
    m_writtenFileSize(0),
    m_lastLength(0),
    m_vmData(NULL),
    m_vmCode(NULL),
    m_vm( *(new RCRar3Vm) ),
    m_lastFilter(0),
    m_isSolid(false),
    m_lzMode(0),
    m_prevAlignBits(0),
    m_prevAlignCount(0),
    m_isTablesRead(false),
    m_ppmEscChar(0)
{
}

RCRar3Decoder::~RCRar3Decoder()
{
    InitFilters();
    RCAlloc::Instance().MidFree(m_vmData);
    RCAlloc::Instance().MidFree(m_window);
    delete &m_vm ;
}

HResult RCRar3Decoder::WriteDataToStream(const byte_t* data, uint32_t size)
{
    return RCStreamUtils::WriteStream(m_outStream.Get(), data, size);
}

HResult RCRar3Decoder::WriteData(const byte_t* data, uint32_t size)
{
    HResult res = RC_S_OK;
    if (m_writtenFileSize < m_unpackSize)
    {
        uint32_t curSize = size;
        uint64_t remain = m_unpackSize - m_writtenFileSize;
        if (remain < curSize)
        {
            curSize = (uint32_t)remain;
        }
        res = WriteDataToStream(data, curSize);
    }
    m_writtenFileSize += size;
    return res;
}

HResult RCRar3Decoder::WriteArea(uint32_t startPtr, uint32_t endPtr)
{
    if (startPtr <= endPtr)
    {
        return WriteData(m_window + startPtr, endPtr - startPtr);
    }
    HResult hr = WriteData(m_window + startPtr, RCRar3Defs::s_kWindowSize - startPtr) ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    return WriteData(m_window, endPtr);
}

void RCRar3Decoder::ExecuteFilter(int32_t tempFilterIndex, RCRar3BlockRef& outBlockRef)
{
    RCRar3TempFilter* tempFilter = m_tempFilters[tempFilterIndex];
    tempFilter->InitR[6] = (uint32_t)m_writtenFileSize;
    RCRar3VmUtils::SetValue32(&tempFilter->GlobalData[0x24], (uint32_t)m_writtenFileSize);
    RCRar3VmUtils::SetValue32(&tempFilter->GlobalData[0x28], (uint32_t)(m_writtenFileSize >> 32));
    RCRar3Filter *filter = m_filters[tempFilter->FilterIndex];
    m_vm.Execute(filter, tempFilter, outBlockRef, filter->GlobalData);
    delete tempFilter;
    m_tempFilters[tempFilterIndex] = 0;
}

HResult RCRar3Decoder::WriteBuf()
{
    uint32_t writtenBorder = m_wrPtr;
    uint32_t writeSize = (m_winPos - writtenBorder) & RCRar3Defs::s_kWindowMask;
    int32_t count = (int32_t)m_tempFilters.size() ;
    for (int32_t i = 0; i < count ; i++)
    {
        RCRar3TempFilter* filter = m_tempFilters[i];
        if (filter == NULL)
        {
            continue;
        }
        if (filter->NextWindow)
        {
            filter->NextWindow = false;
            continue;
        }
        uint32_t blockStart = filter->BlockStart;
        uint32_t blockSize = filter->BlockSize;
        if (((blockStart - writtenBorder) & RCRar3Defs::s_kWindowMask) < writeSize)
        {
            if (writtenBorder != blockStart)
            {
                HResult hr = WriteArea(writtenBorder, blockStart) ;
                if(!IsSuccess(hr))
                {
                    return hr ;
                }
                writtenBorder = blockStart;
                writeSize = (m_winPos - writtenBorder) & RCRar3Defs::s_kWindowMask;
            }
            if (blockSize <= writeSize)
            {
                uint32_t blockEnd = (blockStart + blockSize) & RCRar3Defs::s_kWindowMask;
                if (blockStart < blockEnd || blockEnd == 0)
                {
                    m_vm.SetMemory(0, m_window + blockStart, blockSize);
                }
                else
                {
                    uint32_t tailSize = RCRar3Defs::s_kWindowSize - blockStart;
                    m_vm.SetMemory(0, m_window + blockStart, tailSize);
                    m_vm.SetMemory(tailSize, m_window, blockEnd);
                }
                RCRar3BlockRef outBlockRef;
                ExecuteFilter(i, outBlockRef);
                while (i + 1 < (int32_t)m_tempFilters.size())
                {
                    RCRar3TempFilter* nextFilter = m_tempFilters[i + 1];
                    if (nextFilter == NULL || nextFilter->BlockStart != blockStart ||
                        nextFilter->BlockSize != outBlockRef.Size || nextFilter->NextWindow)
                    {
                        break;
                    }
                    m_vm.SetMemory(0, m_vm.GetDataPointer(outBlockRef.Offset), outBlockRef.Size);
                    ExecuteFilter(++i, outBlockRef);
                }
                WriteDataToStream(m_vm.GetDataPointer(outBlockRef.Offset), outBlockRef.Size);
                m_writtenFileSize += outBlockRef.Size;
                writtenBorder = blockEnd;
                writeSize = (m_winPos - writtenBorder) & RCRar3Defs::s_kWindowMask;
            }
            else
            {
                for (int32_t j = i; j < (int32_t)m_tempFilters.size(); j++)
                {
                    RCRar3TempFilter *filter = m_tempFilters[j];
                    if (filter != NULL && filter->NextWindow)
                    {
                        filter->NextWindow = false;
                    }
                }
                m_wrPtr = writtenBorder;
                return RC_S_OK; // check it
            }
        }
    }

    m_wrPtr = m_winPos;
    return WriteArea(writtenBorder, m_winPos);
}

void RCRar3Decoder::InitFilters()
{
    m_lastFilter = 0;
    int32_t i = 0 ;
    int32_t count = (int32_t)m_tempFilters.size() ;
    for (i = 0; i < count ; i++)
    {
        delete m_tempFilters[i];
    }
    m_tempFilters.clear();
    
    count = (int32_t)m_filters.size() ;
    for (i = 0; i < count ; i++)
    {
        delete m_filters[i];
    }
    m_filters.clear();
}

bool RCRar3Decoder::AddVmCode(uint32_t firstByte, uint32_t codeSize)
{
    RCRar3VmMemBitDecoder inp;
    inp.Init(m_vmData, codeSize);

    uint32_t filterIndex;
    if (firstByte & 0x80)
    {
        filterIndex = RCRar3VmUtils::ReadEncodedUInt32(inp);
        if (filterIndex == 0)
        {
            InitFilters();
        }
        else
        {
            filterIndex--;
        }
    }
    else
    {
        filterIndex = m_lastFilter;
    }
    if (filterIndex > (uint32_t)m_filters.size())
    {
        return false;
    }
    m_lastFilter = filterIndex;
    bool newFilter = (filterIndex == (uint32_t)m_filters.size());

    RCRar3Filter *filter = NULL;
    if (newFilter)
    {
        // check if too many filters
        if (filterIndex > 1024)
        {
            return false;
        }
        filter = new RCRar3Filter ;
        m_filters.push_back(filter);
    }
    else
    {
        filter = m_filters[filterIndex];
        filter->ExecCount++;
    }

    int32_t numEmptyItems = 0;
    int32_t i = 0 ;
    int32_t count = (int32_t)m_tempFilters.size() ;
    for (i = 0; i < count; i++)
    {
        m_tempFilters[i - numEmptyItems] = m_tempFilters[i];
        if (m_tempFilters[i] == NULL)
        {
            numEmptyItems++;
        }
        if (numEmptyItems > 0)
        {
            m_tempFilters[i] = NULL;
        }
    }
    if (numEmptyItems == 0)
    {
        m_tempFilters.push_back(NULL);
        numEmptyItems = 1;
    }
    RCRar3TempFilter* tempFilter = new RCRar3TempFilter;
    m_tempFilters[m_tempFilters.size() - numEmptyItems] = tempFilter;
    tempFilter->FilterIndex = filterIndex;
    tempFilter->ExecCount = filter->ExecCount;

    uint32_t blockStart = RCRar3VmUtils::ReadEncodedUInt32(inp);
    if (firstByte & 0x40)
    {
        blockStart += 258;
    }
    tempFilter->BlockStart = (blockStart + m_winPos) & RCRar3Defs::s_kWindowMask;
    if (firstByte & 0x20)
    {
        filter->BlockSize = RCRar3VmUtils::ReadEncodedUInt32(inp);
    }
    tempFilter->BlockSize = filter->BlockSize;
    tempFilter->NextWindow = m_wrPtr != m_winPos && ((m_wrPtr - m_winPos) & RCRar3Defs::s_kWindowMask) <= blockStart;

    memset(tempFilter->InitR, 0, sizeof(tempFilter->InitR));
    tempFilter->InitR[3] = RCRar3VmDefs::s_kGlobalOffset;
    tempFilter->InitR[4] = tempFilter->BlockSize;
    tempFilter->InitR[5] = tempFilter->ExecCount;
    if (firstByte & 0x10)
    {
        uint32_t initMask = inp.ReadBits(RCRar3VmDefs::s_kNumGpRegs);
        for (int32_t i = 0; i < RCRar3VmDefs::s_kNumGpRegs; i++)
        {
            if (initMask & (1 << i))
            {
                tempFilter->InitR[i] = RCRar3VmUtils::ReadEncodedUInt32(inp);
            }
        }
    }
    if (newFilter)
    {
        uint32_t vmCodeSize = RCRar3VmUtils::ReadEncodedUInt32(inp);
        if (vmCodeSize >= s_kRar3VmCodeSizeMax || vmCodeSize == 0)
        {
            return false;
        }
        for (uint32_t i = 0; i < vmCodeSize; i++)
        {
            m_vmCode[i] = (byte_t)inp.ReadBits(8);
        }
        m_vm.PrepareProgram(m_vmCode, vmCodeSize, filter);
    }

    tempFilter->AllocateEmptyFixedGlobal();

    byte_t *globalData = &tempFilter->GlobalData[0];
    for (i = 0; i < RCRar3VmDefs::s_kNumGpRegs; i++)
    {
        RCRar3VmUtils::SetValue32(&globalData[i * 4], tempFilter->InitR[i]);
    }
    RCRar3VmUtils::SetValue32(&globalData[RCRar3GlobalOffset::s_kBlockSize], tempFilter->BlockSize);
    RCRar3VmUtils::SetValue32(&globalData[RCRar3GlobalOffset::s_kBlockPos], 0); // It was commented. why?
    RCRar3VmUtils::SetValue32(&globalData[RCRar3GlobalOffset::s_kExecCount], tempFilter->ExecCount);

    if (firstByte & 8)
    {
        uint32_t dataSize = RCRar3VmUtils::ReadEncodedUInt32(inp);
        if (dataSize > RCRar3VmDefs::s_kGlobalSize - RCRar3VmDefs::s_kFixedGlobalSize)
        {
            return false;
        }
        RCVector<byte_t>& globalData = tempFilter->GlobalData;
        int32_t requredSize = (int32_t)(dataSize + RCRar3VmDefs::s_kFixedGlobalSize);
        if ((int32_t)globalData.size() < requredSize)
        {
            globalData.reserve(requredSize);
            for (; (int32_t)globalData.size() < requredSize; i++)
            {
                globalData.push_back(0);
            }
        }
        for (uint32_t i = 0; i < dataSize; i++)
        {
            globalData[RCRar3VmDefs::s_kFixedGlobalSize + i] = (byte_t)inp.ReadBits(8);
        }
    }
    return true;
}

bool RCRar3Decoder::ReadVmCodeLZ()
{
    uint32_t firstByte = m_inBitStream.ReadBits(8);
    uint32_t length = (firstByte & 7) + 1;
    if (length == 7)
    {
        length = m_inBitStream.ReadBits(8) + 7;
    }
    else if (length == 8)
    {
        length = m_inBitStream.ReadBits(16);
    }
    if (length > s_kRar3VmDataSizeMax)
    {
        return false;
    }
    for (uint32_t i = 0; i < length; i++)
    {
        m_vmData[i] = (byte_t)m_inBitStream.ReadBits(8);
    }
    return AddVmCode(firstByte, length);
}

bool RCRar3Decoder::ReadVmCodePPM()
{
    int32_t firstByte = DecodePpmSymbol();
    if (firstByte == -1)
    {
        return false;
    }
    uint32_t length = (firstByte & 7) + 1;
    if (length == 7)
    {
        int32_t b1 = DecodePpmSymbol();
        if (b1 == -1)
        {
            return false;
        }
        length = b1 + 7;
    }
    else if (length == 8)
    {
        int32_t b1 = DecodePpmSymbol();
        if (b1 == -1)
        {
            return false;
        }
        int32_t b2 = DecodePpmSymbol();
        if (b2 == -1)
        {
            return false;
        }
        length = b1 * 256 + b2;
    }
    if (length > s_kRar3VmDataSizeMax)
    {
        return false;
    }
    for (uint32_t i = 0; i < length; i++)
    {
        int32_t b = DecodePpmSymbol();
        if (b == -1)
        {
            return false;
        }
        m_vmData[i] = (byte_t)b;
    }
    return AddVmCode(firstByte, length);
}

uint32_t RCRar3Decoder::ReadBits(int32_t numBits)
{
    return m_inBitStream.ReadBits(numBits);
}

HResult RCRar3Decoder::InitPPM()
{
    byte_t maxOrder = (byte_t)ReadBits(7);

    bool reset = ((maxOrder & 0x20) != 0);
    int32_t maxMB = 0;
    if (reset)
    {
        maxMB = (byte_t)ReadBits(8);
    }
    else
    {
        if (m_ppm.m_subAllocator.GetSubAllocatorSize()== 0)
        {
            return RC_S_FALSE;
        }
    }
    if (maxOrder & 0x40)
    {
        m_ppmEscChar = (byte_t)ReadBits(8);
    }
    m_inBitStream.InitRangeCoder();
    /*
    if (m_inBitStream.m_BitPos != 0)
    return RC_S_FALSE;
    */
    if (reset)
    {
        maxOrder = (maxOrder & 0x1F) + 1;
        if (maxOrder > 16)
        {
            maxOrder = 16 + (maxOrder - 16) * 3;
        }
        if (maxOrder == 1)
        {
            // SubAlloc.StopSubAllocator();
            m_ppm.m_subAllocator.StopSubAllocator();
            return RC_S_FALSE;
        }
        // SubAlloc.StartSubAllocator(MaxMB+1);
        // StartModelRare(maxOrder);

        if (!m_ppm.m_subAllocator.StartSubAllocator((maxMB + 1) << 20))
        {
            return RC_E_OUTOFMEMORY;
        }
        m_ppm.m_maxOrder = 0;
        m_ppm.StartModelRare(maxOrder);

    }
    // return (minContext != NULL);

    return RC_S_OK;
}

int32_t RCRar3Decoder::DecodePpmSymbol()
{
    return m_ppm.DecodeSymbol(&m_inBitStream);
}

HResult RCRar3Decoder::DecodePPM(int32_t num, bool &keepDecompressing)
{
    keepDecompressing = false;
    do
    {
        if (((m_wrPtr - m_winPos) & RCRar3Defs::s_kWindowMask) < 260 && m_wrPtr != m_winPos)
        {
            HResult hr = WriteBuf() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (m_writtenFileSize > m_unpackSize)
            {
                keepDecompressing = false;
                return RC_S_OK;
            }
        }
        int32_t c = DecodePpmSymbol();
        if (c == -1)
        {
            // Original code sets PPMError=true here and then it returns RC_S_OK. Why ???
            // return RC_S_OK;
            return RC_S_FALSE;
        }
        if (c == m_ppmEscChar)
        {
            int32_t nextCh = DecodePpmSymbol();
            if (nextCh == 0)
            {
                return ReadTables(keepDecompressing);
            }
            if (nextCh == 2 || nextCh == -1)
            {
                return RC_S_OK;
            }
            if (nextCh == 3)
            {
                if (!ReadVmCodePPM())
                {
                    return RC_S_FALSE;
                }
                continue;
            }
            if (nextCh == 4 || nextCh == 5)
            {
                uint32_t distance = 0;
                uint32_t length = 4;
                if (nextCh == 4)
                {
                    for (int32_t i = 0; i < 3; i++)
                    {
                        int32_t c = DecodePpmSymbol();
                        if (c == -1)
                        {
                            return RC_S_OK;
                        }
                        distance = (distance << 8) + (byte_t)c;
                    }
                    distance++;
                    length += 28;
                }
                int32_t c = DecodePpmSymbol();
                if (c == -1)
                {
                    return RC_S_OK;
                }
                length += c;
                if (distance >= m_lzSize)
                {
                    return RC_S_FALSE;
                }
                CopyBlock(distance, length);
                num -= (int32_t)length;
                continue;
            }
        }
        PutByte((byte_t)c);
        num--;
    }
    while (num >= 0);
    keepDecompressing = true;
    return RC_S_OK;
}

HResult RCRar3Decoder::ReadTables(bool &keepDecompressing)
{
    keepDecompressing = true;
    ReadBits((8 - m_inBitStream.GetBitPosition()) & 7);
    if (ReadBits(1) != 0)
    {
        m_lzMode = false;
        return InitPPM();
    }

    m_lzMode = true;
    m_prevAlignBits = 0;
    m_prevAlignCount = 0;

    byte_t levelLevels[RCRar3Defs::s_kLevelTableSize];
    byte_t newLevels[RCRar3Defs::s_kTablesSizesSum];

    if (ReadBits(1) == 0)
    {
        memset(m_lastLevels, 0, RCRar3Defs::s_kTablesSizesSum);
    }

    int32_t i = 0 ;
    for (i = 0; i < RCRar3Defs::s_kLevelTableSize; i++)
    {
        uint32_t length = ReadBits(4);
        if (length == 15)
        {
            uint32_t zeroCount = ReadBits(4);
            if (zeroCount != 0)
            {
                zeroCount += 2;
                while (zeroCount-- > 0 && i < RCRar3Defs::s_kLevelTableSize)
                {
                    levelLevels[i++]=0;
                }
                i--;
                continue;
            }
        }
        levelLevels[i] = (byte_t)length;
    }
    if(!m_levelDecoder.SetCodeLengths(levelLevels))
    {
        return RC_S_FALSE ;
    }
    i = 0;
    while (i < RCRar3Defs::s_kTablesSizesSum)
    {
        uint32_t number = m_levelDecoder.DecodeSymbol(&m_inBitStream);
        if (number < 16)
        {
            newLevels[i] = byte_t((number + m_lastLevels[i]) & 15);
            i++;
        }
        else if (number > RCRar3Defs::s_kLevelTableSize)
        {
            return RC_S_FALSE;
        }
        else
        {
            int32_t num;
            if (((number - 16) & 1) == 0)
            {
                num = ReadBits(3) + 3;
            }
            else
            {
                num = ReadBits(7) + 11;
            }
            if (number < 18)
            {
                if (i == 0)
                {
                    return RC_S_FALSE;
                }
                for (; num > 0 && i < RCRar3Defs::s_kTablesSizesSum; num--, i++)
                {
                    newLevels[i] = newLevels[i - 1];
                }
            }
            else
            {
                for (; num > 0 && i < RCRar3Defs::s_kTablesSizesSum; num--)
                {
                    newLevels[i++] = 0;
                }
            }
        }
    }
    m_isTablesRead = true;

    // original code has check here:
    /*
    if (InAddr > ReadTop)
    {
    keepDecompressing = false;
    return true;
    }
    */

    if(!m_mainDecoder.SetCodeLengths(&newLevels[0]))
    {
        return RC_S_FALSE ;
    }
    if(!m_distDecoder.SetCodeLengths(&newLevels[RCRar3Defs::s_kMainTableSize]))
    {
        return RC_S_FALSE ;
    }
    if(!m_alignDecoder.SetCodeLengths(&newLevels[RCRar3Defs::s_kMainTableSize + RCRar3Defs::s_kDistTableSize]))
    {
        return RC_S_FALSE ; 
    }
    if(!m_lenDecoder.SetCodeLengths(&newLevels[RCRar3Defs::s_kMainTableSize + RCRar3Defs::s_kDistTableSize + RCRar3Defs::s_kAlignTableSize]))
    {
        return RC_S_FALSE ;
    }

    memcpy(m_lastLevels, newLevels, RCRar3Defs::s_kTablesSizesSum);
    return RC_S_OK;
}

HResult RCRar3Decoder::ReadEndOfBlock(bool& keepDecompressing)
{
    if (ReadBits(1) != 0)
    {
        // old file
        m_isTablesRead = false;
        return ReadTables(keepDecompressing);
    }
    // new file
    keepDecompressing = false;
    m_isTablesRead = (ReadBits(1) == 0);
    return RC_S_OK;
}

HResult RCRar3Decoder::DecodeLZ(bool& keepDecompressing)
{
    uint32_t rep0 = m_reps[0];
    uint32_t rep1 = m_reps[1];
    uint32_t rep2 = m_reps[2];
    uint32_t rep3 = m_reps[3];
    uint32_t length = m_lastLength;
    for (;;)
    {
        if (((m_wrPtr - m_winPos) & RCRar3Defs::s_kWindowMask) < 260 && m_wrPtr != m_winPos)
        {
            HResult hr = WriteBuf() ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            if (m_writtenFileSize > m_unpackSize)
            {
                keepDecompressing = false;
                return RC_S_OK;
            }
        }
        uint32_t number = m_mainDecoder.DecodeSymbol(&m_inBitStream);
        if (number < 256)
        {
            PutByte(byte_t(number));
            continue;
        }
        else if (number == s_kRar3SymbolReadTable)
        {
            HResult hr = ReadEndOfBlock(keepDecompressing) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
            break;
        }
        else if (number == 257)
        {
            if (!ReadVmCodeLZ())
            {
                return RC_S_FALSE;
            }
            continue;
        }
        else if (number == 258)
        {
        }
        else if (number < s_kRar3SymbolRep + 4)
        {
            if (number != s_kRar3SymbolRep)
            {
                uint32_t distance;
                if (number == s_kRar3SymbolRep + 1)
                {
                    distance = rep1;
                }
                else
                {
                    if (number == s_kRar3SymbolRep + 2)
                    {
                        distance = rep2;
                    }
                    else
                    {
                        distance = rep3;
                        rep3 = rep2;
                    }
                    rep2 = rep1;
                }
                rep1 = rep0;
                rep0 = distance;
            }

            uint32_t number = m_lenDecoder.DecodeSymbol(&m_inBitStream);
            if (number >= RCRar3Defs::s_kLenTableSize)
            {
                return RC_S_FALSE;
            }
            length = 2 + s_kRar3LenStart[number] + m_inBitStream.ReadBits(s_kRar3LenDirectBits[number]);
        }
        else
        {
            rep3 = rep2;
            rep2 = rep1;
            rep1 = rep0;
            if (number < 271)
            {
                number -= 263;
                rep0 = s_kRar3Len2DistStarts[number] + m_inBitStream.ReadBits(s_kRar3Len2DistDirectBits[number]);
                length = 2;
            }
            else if (number < 299)
            {
                number -= 271;
                length = s_kRar3NormalMatchMinLen + (uint32_t)s_kRar3LenStart[number] + m_inBitStream.ReadBits(s_kRar3LenDirectBits[number]);
                uint32_t number = m_distDecoder.DecodeSymbol(&m_inBitStream);
                if (number >= RCRar3Defs::s_kDistTableSize)
                {
                    return RC_S_FALSE;
                }
                rep0 = s_kRar3DistStart[number];
                int32_t numBits = s_kRar3DistDirectBits[number];
                if (number >= (RCRar3Defs::s_kNumAlignBits * 2) + 2)
                {
                    if (numBits > RCRar3Defs::s_kNumAlignBits)
                    {
                        rep0 += (m_inBitStream.ReadBits(numBits - RCRar3Defs::s_kNumAlignBits) << RCRar3Defs::s_kNumAlignBits);
                    }
                    if (m_prevAlignCount > 0)
                    {
                        m_prevAlignCount--;
                        rep0 += m_prevAlignBits;
                    }
                    else
                    {
                        uint32_t number = m_alignDecoder.DecodeSymbol(&m_inBitStream);
                        if (number < (1 << RCRar3Defs::s_kNumAlignBits))
                        {
                            rep0 += number;
                            m_prevAlignBits = number;
                        }
                        else if (number  == (1 << RCRar3Defs::s_kNumAlignBits))
                        {
                            m_prevAlignCount = s_kRar3NumAlignReps;
                            rep0 += m_prevAlignBits;
                        }
                        else
                        {
                            return RC_S_FALSE;
                        }
                    }
                }
                else
                {
                    rep0 += m_inBitStream.ReadBits(numBits);
                }
                length += ((s_kRar3DistLimit4 - rep0) >> 31) + ((s_kRar3DistLimit3 - rep0) >> 31);
            }
            else
            {
                return RC_S_FALSE;
            }
        }
        if (rep0 >= m_lzSize)
        {
            return RC_S_FALSE;
        }
        CopyBlock(rep0, length);
    }
    m_reps[0] = rep0;
    m_reps[1] = rep1;
    m_reps[2] = rep2;
    m_reps[3] = rep3;
    m_lastLength = length;

    return RC_S_OK;
}

HResult RCRar3Decoder::CodeReal(ICompressProgressInfo *progress)
{
    m_writtenFileSize = 0;
    if (!m_isSolid)
    {
        m_lzSize = 0;
        m_winPos = 0;
        m_wrPtr = 0;
        for (int32_t i = 0; i < RCRar3Defs::s_kNumReps; i++)
        {
            m_reps[i] = 0;
        }
        m_lastLength = 0;
        memset(m_lastLevels, 0, RCRar3Defs::s_kTablesSizesSum);
        m_isTablesRead = false;
        m_ppmEscChar = 2;
        InitFilters();
    }
    if (!m_isSolid || !m_isTablesRead)
    {
        bool keepDecompressing;
        HResult hr = ReadTables(keepDecompressing) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (!keepDecompressing)
        {
            return RC_S_OK;
        }
    }

    for(;;)
    {
        bool keepDecompressing;
        if (m_lzMode)
        {
            HResult hr = DecodeLZ(keepDecompressing) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        else
        {
            HResult hr = DecodePPM(1 << 18, keepDecompressing) ;
            if(!IsSuccess(hr))
            {
                return hr ;
            }
        }
        uint64_t packSize = m_inBitStream.GetProcessedSize();
        HResult hr = progress->SetRatioInfo(packSize, m_writtenFileSize) ;
        if(!IsSuccess(hr))
        {
            return hr ;
        }
        if (!keepDecompressing)
        {
            break;
        }
    }
    uint64_t packSize = m_inBitStream.GetProcessedSize() ;
    HResult hr = WriteBuf() ;
    if(!IsSuccess(hr))
    {
        return hr ;
    }
    if (m_writtenFileSize < m_unpackSize)
    {
        return RC_S_FALSE;
    }
    progress->SetRatioInfo(packSize, m_writtenFileSize) ;
    return RC_S_OK;
}

void RCRar3Decoder::ReleaseStreams()
{
    m_outStream.Release();
    m_inBitStream.ReleaseStream();
}

void RCRar3Decoder::CopyBlock(uint32_t distance, uint32_t len)
{
    if(len == 0)
    {
        return ;
    }
    m_lzSize += len;
    uint32_t pos = (m_winPos - distance - 1) & RCRar3Defs::s_kWindowMask;
    byte_t *window = m_window;
    uint32_t winPos = m_winPos;
    if (RCRar3Defs::s_kWindowSize - winPos > len && RCRar3Defs::s_kWindowSize - pos > len)
    {
        const byte_t *src = window + pos;
        byte_t *dest = window + winPos;
        m_winPos += len;
        do
        {
            *dest++ = *src++;
        } while(--len != 0);
        return;
    }
    do
    {
        window[winPos] = window[pos];
        winPos = (winPos + 1) & RCRar3Defs::s_kWindowMask;
        pos = (pos + 1) & RCRar3Defs::s_kWindowMask;
    } while(--len != 0);
    m_winPos = winPos;
}

void RCRar3Decoder::PutByte(byte_t b)
{
    m_window[m_winPos] = b;
    m_winPos = (m_winPos + 1) & RCRar3Defs::s_kWindowMask;
    m_lzSize++;
}

HResult RCRar3Decoder::Code(ISequentialInStream* inStream,
                            ISequentialOutStream* outStream, 
                            const uint64_t* inSize, 
                            const uint64_t* outSize,
                            ICompressProgressInfo* progress) 
{
    try
    {
        if (inSize == NULL || outSize == NULL)
        {
            return RC_E_INVALIDARG;
        }

        if (m_vmData == 0)
        {
            m_vmData = (byte_t *)RCAlloc::Instance().MidAlloc(s_kRar3VmDataSizeMax + s_kRar3VmCodeSizeMax);
            if (m_vmData == 0)
            {
                return RC_E_OUTOFMEMORY;
            }
            m_vmCode = m_vmData + s_kRar3VmDataSizeMax;
        }

        if (m_window == 0)
        {
            m_window = (byte_t *)RCAlloc::Instance().MidAlloc(RCRar3Defs::s_kWindowSize);
            if (m_window == 0)
            {
                return RC_E_OUTOFMEMORY;
            }
        }
        if (!m_inBitStream.Create(1 << 20))
        {
            return RC_E_OUTOFMEMORY;
        }
        if (!m_vm.Create())
        {
            return RC_E_OUTOFMEMORY;
        }

        m_inBitStream.SetStream(inStream);
        m_inBitStream.Init();
        m_outStream = outStream;

        RCRar3DecoderReleaser coderReleaser(this);
        m_unpackSize = *outSize;
        return CodeReal(progress);
    }
    catch(HResult errorCode)
    {
        return errorCode ;
    }
    catch(...)
    {
        return RC_E_FAIL ;
    }
}

HResult RCRar3Decoder::SetDecoderProperties2(const byte_t* data, uint32_t size)
{
    if (size < 1)
    {
        return RC_E_INVALIDARG;
    }
    m_isSolid = (data[0] != 0) ;
    return RC_S_OK;
}
  
END_NAMESPACE_RCZIP
