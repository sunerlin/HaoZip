/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

//include files
#include "format/nsis/RCNsisIn.h"
#include "filesystem/RCStreamUtils.h"
#include "common/RCStringConvert.h"
#include "common/RCStringUtilA.h"
#include "algorithm/CpuArch.h"
#include <functional>

#define Get32(p) GetUi32(p)

/////////////////////////////////////////////////////////////////
//RCNsisIn class implementation

BEGIN_NAMESPACE_RCZIP

#ifdef NSIS_SCRIPT
static const char* g_crLf = "\x0D\x0A";
#endif

#define NS_UN_SKIP_CODE  0xE000
#define NS_UN_VAR_CODE   0xE001
#define NS_UN_SHELL_CODE 0xE002
#define NS_UN_LANG_CODE  0xE003
#define NS_UN_CODES_START NS_UN_SKIP_CODE
#define NS_UN_CODES_END   NS_UN_LANG_CODE

byte_t RCNsisIn::ReadByte()
{
    if (m_posInData >= m_size)
    {
        _ThrowCode(RC_E_ReadStreamError) ;
    }
    return m_data[m_posInData++];
}

uint32_t RCNsisIn::ReadUInt32()
{
    uint32_t value = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        value |= ((uint32_t)(ReadByte()) << (8 * i));
    }
    return value;
}

void RCNsisIn::ReadBlockHeader(RCNsisBlockHeader& bh)
{
    bh.m_offset = ReadUInt32();
    bh.m_num = ReadUInt32();
}

#define RINOZ(x) { int32_t __tt = (x); if (__tt < 0) return true; else if(__tt > 0) return false;}

class RCCompareItems:
    public std::binary_function<RCNsisItem, RCNsisItem, bool>
{
public:

    /** 重载()操作符
    @param [in] i1 nsisItem一
    @param [in] i2 nsisItem二
    */
    bool operator()(const RCNsisItem& i1 , const RCNsisItem& i2) const
    {
        RINOZ(MyCompare(i1.m_pos, i2.m_pos)) ;
        if (i1.m_isUnicode)
        {
            RINOZ(RCStringUtil::Compare(i1.m_prefixU, i2.m_prefixU));
            RINOZ(RCStringUtil::Compare(i1.m_nameU, i2.m_nameU));
        }
        else
        {
            RINOZ(RCStringUtilA::Compare(i1.m_prefixA, i2.m_prefixA));
            RINOZ(RCStringUtilA::Compare(i1.m_nameA, i2.m_nameA));
        }
        return false;
    }
};

static RCStringA UIntToString(uint32_t v)
{
    char sz[32];
    RCStringUtilA::ConvertUInt64ToString(v, sz);
    return sz;
}

static RCStringA IntToString(int32_t v)
{
    char sz[32];
    RCStringUtilA::ConvertInt64ToString(v, sz);
    return sz;
}

RCStringA RCNsisIn::ReadStringA(uint32_t pos) const
{
    RCStringA s;
    if (pos >= m_size)
    {
        return IntToString((int32_t)pos);
    }
    uint32_t offset = GetOffset() + m_stringsPos + pos;
    for (;;)
    {
        if (offset >= m_size)
        {
            break;
        }
        char c = m_data[offset++];
        if (c == 0)
        {
            break;
        }
        s += c;
    }
    return s;
}

RCString RCNsisIn::ReadStringU(uint32_t pos) const
{
    RCString s;
    uint32_t offset = GetOffset() + m_stringsPos + (pos * 2);
    for (;;)
    {
        if (offset >= m_size || offset + 1 >= m_size)
        {
            return s;
        }
        char c0 = m_data[offset++];
        char c1 = m_data[offset++];
        char_t c = (c0 | ((char_t)c1 << 8));
        if (c == 0)
        {
            break;
        }
        s += c;
    }
    return s;
}

struct CCommandPair
{
    /** 参数个数
    */
    int32_t m_numParams;

    /** 名字
    */
    const char* m_name;
};

enum
{
    // 0
    EW_INVALID_OPCODE,    // zero is invalid. useful for catching errors. (otherwise an all zeroes instruction
    // does nothing, which is easily ignored but means something is wrong.
    EW_RET,               // return from function call
    EW_NOP,               // Nop/Jump, do nothing: 1, [?new address+1:advance one]
    EW_ABORT,             // Abort: 1 [status]
    EW_QUIT,              // Quit: 0
    EW_CALL,              // Call: 1 [new address+1]
    EW_UPDATETEXT,        // Update status text: 2 [update str, ui_st_updateflag=?ui_st_updateflag:this]
    EW_SLEEP,             // Sleep: 1 [sleep time in milliseconds]
    EW_BRINGTOFRONT,      // BringToFront: 0
    EW_CHDETAILSVIEW,     // SetDetailsView: 2 [listaction,buttonaction]

    // 10
    EW_SETFILEATTRIBUTES, // SetFileAttributes: 2 [filename, attributes]
    EW_CREATEDIR,         // Create directory: 2, [path, ?update$INSTDIR]
    EW_IFFILEEXISTS,      // IfFileExists: 3, [file name, jump amount if exists, jump amount if not exists]
    EW_SETFLAG,           // Sets a flag: 2 [id, data]
    EW_IFFLAG,            // If a flag: 4 [on, off, id, new value mask]
    EW_GETFLAG,           // Gets a flag: 2 [output, id]
    EW_RENAME,            // Rename: 3 [old, new, rebootok]
    EW_GETFULLPATHNAME,   // GetFullPathName: 2 [output, input, ?lfn:sfn]
    EW_SEARCHPATH,        // SearchPath: 2 [output, filename]
    EW_GETTEMPFILENAME,   // GetTempFileName: 2 [output, base_dir]

    // 20
    EW_EXTRACTFILE,       // File to extract: 6 [overwriteflag, output filename, compressed filedata, filedatetimelow, filedatetimehigh, allow ignore]
    //  overwriteflag: 0x1 = no. 0x0=force, 0x2=try, 0x3=if date is newer
    EW_DELETEFILE,        // Delete File: 2, [filename, rebootok]
    EW_MESSAGEBOX,        // MessageBox: 5,[MB_flags,text,retv1:retv2,moveonretv1:moveonretv2]
    EW_RMDIR,             // RMDir: 2 [path, recursiveflag]
    EW_STRLEN,            // StrLen: 2 [output, input]
    EW_ASSIGNVAR,         // Assign: 4 [variable (0-9) to assign, string to assign, maxlen, startpos]
    EW_STRCMP,            // StrCmp: 5 [str1, str2, jump_if_equal, jump_if_not_equal, case-sensitive?]
    EW_READENVSTR,        // ReadEnvStr/ExpandEnvStrings: 3 [output, string_with_env_variables, IsRead]
    EW_INTCMP,            // IntCmp: 6 [val1, val2, equal, val1<val2, val1>val2, unsigned?]
    EW_INTOP,             // IntOp: 4 [output, input1, input2, op] where op: 0=add, 1=sub, 2=mul, 3=div, 4=bor, 5=band, 6=bxor, 7=bnot input1, 8=lnot input1, 9=lor, 10=land], 11=1%2

    // 30
    EW_INTFMT,            // IntFmt: [output, format, input]
    EW_PUSHPOP,           // Push/Pop/Exchange: 3 [variable/string, ?pop:push, ?exch]
    EW_FINDWINDOW,        // FindWindow: 5, [outputvar, window class,window name, window_parent, window_after]
    EW_SENDMESSAGE,       // SendMessage: 6 [output, hwnd, msg, wparam, lparam, [wparamstring?1:0 | lparamstring?2:0 | timeout<<2]
    EW_ISWINDOW,          // IsWindow: 3 [hwnd, jump_if_window, jump_if_notwindow]
    EW_GETDLGITEM,        // GetDlgItem:        3: [outputvar, dialog, item_id]
    EW_SETCTLCOLORS,      // SerCtlColors:      3: [hwnd, pointer to struct colors]
    EW_SETBRANDINGIMAGE,  // SetBrandingImage:  1: [Bitmap file]
    EW_CREATEFONT,        // CreateFont:        5: [handle output, face name, height, weight, flags]
    EW_SHOWWINDOW,        // ShowWindow:        2: [hwnd, show state]

    // 40
    EW_SHELLEXEC,         // ShellExecute program: 4, [shell action, complete commandline, parameters, showwindow]
    EW_EXECUTE,           // Execute program: 3,[complete command line,waitflag,>=0?output errorcode]
    EW_GETFILETIME,       // GetFileTime; 3 [file highout lowout]
    EW_GETDLLVERSION,     // GetDLLVersion: 3 [file highout lowout]
    EW_REGISTERDLL,       // Register DLL: 3,[DLL file name, string ptr of function to call, text to put in display (<0 if none/pass parms), 1 - no unload, 0 - unload]
    EW_CREATESHORTCUT,    // Make Shortcut: 5, [link file, target file, parameters, icon file, iconindex|show mode<<8|hotkey<<16]
    EW_COPYFILES,         // CopyFiles: 3 [source mask, destination location, flags]
    EW_REBOOT,            // Reboot: 0
    EW_WRITEINI,          // Write INI String: 4, [Section, Name, Value, INI File]
    EW_READINISTR,        // ReadINIStr: 4 [output, section, name, ini_file]

    // 50
    EW_DELREG,            // DeleteRegValue/DeleteRegKey: 4, [root key(int32_t), KeyName, ValueName, delkeyonlyifempty]. ValueName is -1 if delete key
    EW_WRITEREG,          // Write Registry value: 5, [RootKey(int32_t),KeyName,ItemName,ItemData,typelen]
    //  typelen=1 for str, 2 for dword, 3 for binary, 0 for expanded str
    EW_READREGSTR,        // ReadRegStr: 5 [output, rootkey(int32_t), keyname, itemname, ==1?int32_t::str]
    EW_REGENUM,           // RegEnum: 5 [output, rootkey, keyname, index, ?key:value]
    EW_FCLOSE,            // FileClose: 1 [handle]
    EW_FOPEN,             // FileOpen: 4  [name, openmode, createmode, outputhandle]
    EW_FPUTS,             // FileWrite: 3 [handle, string, ?int32_t:string]
    EW_FGETS,             // FileRead: 4  [handle, output, maxlen, ?getchar:gets]
    EW_FSEEK,             // FileSeek: 4  [handle, offset, mode, >=0?positionoutput]
    EW_FINDCLOSE,         // FindClose: 1 [handle]

    // 60
    EW_FINDNEXT,          // FindNext: 2  [output, handle]
    EW_FINDFIRST,         // FindFirst: 2 [filespec, output, handleoutput]
    EW_WRITEUNINSTALLER,  // WriteUninstaller: 3 [name, offset, icon_size]
    EW_LOG,               // LogText: 2 [0, text] / LogSet: [1, logstate]
    EW_SECTIONSET,        // SectionSetText:    3: [idx, 0, text]
    // SectionGetText:    3: [idx, 1, output]
    // SectionSetFlags:   3: [idx, 2, flags]
    // SectionGetFlags:   3: [idx, 3, output]
    EW_INSTTYPESET,       // InstTypeSetFlags:  3: [idx, 0, flags]
    // InstTypeGetFlags:  3: [idx, 1, output]
    // instructions not actually implemented in exehead, but used in compiler.
    EW_GETLABELADDR,      // both of these get converted to EW_ASSIGNVAR
    EW_GETFUNCTIONADDR,

    EW_LOCKWINDOW
};

#ifdef NSIS_SCRIPT
static CCommandPair kCommandPairs[] =
{
    { 0, "Invalid" },
    { 0, "Return" },
    { 1, "Goto" },
    { 0, "Abort" },
    { 0, "Quit" },
    { 1, "Call" },
    { 2, "UpdateSatusText" },
    { 1, "Sleep" },
    { 0, "BringToFront" },
    { 2, "SetDetailsView" },

    { 2, "SetFileAttributes" },
    { 2, "SetOutPath" },
    { 3, "IfFileExists" },
    { 2, "SetFlag" },
    { 4, "IfFlag" },
    { 2, "GetFlag" },
    { 3, "Rename" },
    { 2, "GetFullPathName" },
    { 2, "SearchPath" },
    { 2, "GetTempFileName" },

    { 6, "File" },
    { 2, "Delete" },
    { 5, "MessageBox" },
    { 2, "RMDir" },
    { 2, "StrLen" },
    { 4, "StrCpy" },
    { 5, "StrCmp" },
    { 3, "ReadEnvStr" },
    { 6, "IntCmp" },
    { 4, "IntOp" },

    { 3, "IntFmt" },
    { 3, "PushPop" },
    { 5, "FindWindow" },
    { 6, "SendMessage" },
    { 3, "IsWindow" },
    { 3, "GetDlgItem" },
    { 3, "SerCtlColors" },
    { 1, "SetBrandingImage" },
    { 5, "CreateFont" },
    { 2, "ShowWindow" },

    { 4, "ShellExecute" },
    { 3, "Execute" },
    { 3, "GetFileTime" },
    { 3, "GetDLLVersion" },
    { 3, "RegisterDLL" },
    { 5, "CreateShortCut" },
    { 3, "CopyFiles" },
    { 0, "Reboot" },
    { 4, "WriteINIStr" },
    { 4, "ReadINIStr" },

    { 4, "DelReg" },
    { 5, "WriteReg" },
    { 5, "ReadRegStr" },
    { 5, "RegEnum" },
    { 1, "FileClose" },
    { 4, "FileOpen" },
    { 3, "FileWrite" },
    { 4, "FileRead" },
    { 4, "FileSeek" },
    { 1, "FindClose" },

    { 2, "FindNext" },
    { 2, "FindFirst" },
    { 3, "WriteUninstaller" },
    { 2, "LogText" },
    { 3, "Section?etText" },
    { 3, "InstType?etFlags" },
    { 6, "GetLabelAddr" },
    { 2, "GetFunctionAddress" },
    { 6, "LockWindow" }
};

#endif

static const char *kShellStrings[] =
{
    "",
    "",

    "SMPROGRAMS",
    "",
    "PRINTERS",
    "DOCUMENTS",
    "FAVORITES",
    "SMSTARTUP",
    "RECENT",
    "SENDTO",
    "",
    "STARTMENU",
    "",
    "MUSIC",
    "VIDEO",
    "",

    "DESKTOP",
    "",
    "",
    "NETHOOD",
    "FONTS",
    "TEMPLATES",
    "COMMONSTARTMENU",
    "COMMONFILES",
    "COMMON_STARTUP",
    "COMMON_DESKTOPDIRECTORY",
    "QUICKLAUNCH",
    "PRINTHOOD",
    "LOCALAPPDATA",
    "ALTSTARTUP",
    "ALTSTARTUP",
    "FAVORITES",

    "INTERNET_CACHE",
    "COOKIES",
    "HISTORY",
    "APPDATA",
    "WINDIR",
    "SYSDIR",
    "PROGRAMFILES",
    "PICTURES",
    "PROFILE",
    "",
    "",
    "COMMONFILES",
    "",
    "TEMPLATES",
    "DOCUMENTS",
    "ADMINTOOLS",

    "ADMINTOOLS",
    "",
    "",
    "",
    "",
    "MUSIC",
    "PICTURES",
    "VIDEO",
    "RESOURCES",
    "RESOURCES_LOCALIZED",
    "",
    "CDBURN_AREA"
};

static const int32_t kNumShellStrings = sizeof(kShellStrings) / sizeof(kShellStrings[0]);

/*
# define CMDLINE 20 // everything before here doesn't have trailing slash removal
# define INSTDIR 21
# define OUTDIR 22
# define EXEDIR 23
# define LANGUAGE 24
# define TEMP   25
# define PLUGINSDIR 26
# define HWNDPARENT 27
# define _CLICK 28
# define _OUTDIR 29
*/

static const char *kVarStrings[] =
{
    "CMDLINE",
    "INSTDIR",
    "OUTDIR",
    "EXEDIR",
    "LANGUAGE",
    "TEMP",
    "PLUGINSDIR",
    "EXEPATH", // test it
    "EXEFILE", // test it
    "HWNDPARENT",
    "_CLICK",
    "_OUTDIR"
};

static const int32_t kNumVarStrings = sizeof(kVarStrings) / sizeof(kVarStrings[0]);


static RCStringA GetVar(uint32_t index)
{
    RCStringA res = "$";
    if (index < 10)
    {
        res += UIntToString(index);
    }
    else if (index < 20)
    {
        res += "R";
        res += UIntToString(index - 10);
    }
    else if (index < 20 + kNumVarStrings)
    {
        res += kVarStrings[index - 20];
    }
    else
    {
        res += "[";
        res += UIntToString(index);
        res += "]";
    }
    return res;
}

#define NS_SKIP_CODE  252
#define NS_VAR_CODE   253
#define NS_SHELL_CODE 254
#define NS_LANG_CODE  255
#define NS_CODES_START NS_SKIP_CODE

static RCStringA GetShellString(int32_t index)
{
    RCStringA res = "$";
    if (index < kNumShellStrings)
    {
        const char *sz = kShellStrings[index];
        if (sz[0] != 0)
        {
            return res + sz;
        }
    }
    res += "SHELL[";
    res += UIntToString(index);
    res += "]";
    return res;
}

// Based on Dave Laundon's simplified process_string
RCStringA GetNsisString(const RCStringA& s)
{
    RCStringA res;
    for (int32_t i = 0; i < (int32_t)s.length();)
    {
        unsigned char nVarIdx = s[i++];
        if (nVarIdx > NS_CODES_START && i + 2 <= (int32_t)s.length())
        {
            int32_t nData = s[i++] & 0x7F;
            unsigned char c1 = s[i++];
            nData |= (((int32_t)(c1 & 0x7F)) << 7);

            if (nVarIdx == NS_SHELL_CODE)
                res += GetShellString(c1);
            else if (nVarIdx == NS_VAR_CODE)
                res += GetVar(nData);
            else if (nVarIdx == NS_LANG_CODE)
                res += "NS_LANG_CODE";
        }
        else if (nVarIdx == NS_SKIP_CODE)
        {
            if (i < (int32_t)s.length())
                res += s[i++];
        }
        else // Normal char
        {
            res += (char)nVarIdx;
        }
    }
    return res;
}

RCString GetNsisString(const RCString &s)
{
    RCString res;
    for (int32_t i = 0; i < (int32_t)s.length();)
    {
        char_t nVarIdx = s[i++];
        if (nVarIdx > NS_UN_CODES_START && nVarIdx <= NS_UN_CODES_END)
        {
            if (i == (int32_t)s.length())
                break;
            int32_t nData = s[i++] & 0x7FFF;

            if (nVarIdx == NS_UN_SHELL_CODE)
                res += RCStringConvert::MultiByteToUnicodeString(GetShellString(nData >> 8));
            else if (nVarIdx == NS_UN_VAR_CODE)
                res += RCStringConvert::MultiByteToUnicodeString(GetVar(nData));
            else if (nVarIdx == NS_UN_LANG_CODE)
                res += _T("NS_LANG_CODE");
        }
        else if (nVarIdx == NS_UN_SKIP_CODE)
        {
            if (i == (int32_t)s.length())
                break;
            res += s[i++];
        }
        else // Normal char
        {
            res += (char)nVarIdx;
        }
    }
    return res;
}

RCStringA RCNsisIn::ReadString2A(uint32_t pos) const
{
    return GetNsisString(ReadStringA(pos));
}

RCString RCNsisIn::ReadString2U(uint32_t pos) const
{
    return GetNsisString(ReadStringU(pos));
}

RCStringA RCNsisIn::ReadString2(uint32_t pos) const
{
    if (m_isUnicode)
        return RCStringConvert::UnicodeStringToMultiByte(ReadString2U(pos));
    else
        return ReadString2A(pos);
}

RCStringA RCNsisIn::ReadString2Qw(uint32_t pos) const
{
    return "\"" + ReadString2(pos) + "\"";
}

#define DEL_DIR 1
#define DEL_RECURSE 2
#define DEL_REBOOT 4

static const int32_t s_numEntryParams = 6;

struct RCNsisEntry
{
    /** 哪一个
    */
    uint32_t m_which;

    /** 参数
    */
    uint32_t m_params[s_numEntryParams];

    /** 获取参数字符串
    @param [in] numParams 参数个数
    @return 返回参数字符串
    */
    RCStringA GetParamsString(int32_t numParams);

    /** 默认构造函数
    */
    RCNsisEntry()
    {
        m_which = 0;
        for (uint32_t j = 0; j < s_numEntryParams; j++)
        {
            m_params[j] = 0;
        }
    }
};

RCStringA RCNsisEntry::GetParamsString(int32_t numParams)
{
    RCStringA s;
    for (int32_t i = 0; i < numParams; i++)
    {
        s += " ";
        uint32_t v = m_params[i];
        if (v > 0xFFF00000)
            s += IntToString((int32_t)m_params[i]);
        else
            s += UIntToString(m_params[i]);
    }
    return s;
}

#ifdef NSIS_SCRIPT

static RCStringA GetRegRootID(uint32_t val)
{
    const char *s;
    switch(val)
    {
    case 0:  s = "SHCTX"; break;
    case 0x80000000:  s = "HKCR"; break;
    case 0x80000001:  s = "HKCU"; break;
    case 0x80000002:  s = "HKLM"; break;
    case 0x80000003:  s = "HKU";  break;
    case 0x80000004:  s = "HKPD"; break;
    case 0x80000005:  s = "HKCC"; break;
    case 0x80000006:  s = "HKDD"; break;
    case 0x80000050:  s = "HKPT"; break;
    case 0x80000060:  s = "HKPN"; break;
    default:
        return UIntToString(val); break;
    }
    return s;
}

#endif

HResult RCNsisIn::ReadEntries(const RCNsisBlockHeader& bh)
{
    m_posInData = bh.m_offset + GetOffset();
    RCStringA prefixA;
    RCString prefixU;
    for (uint32_t i = 0; i < bh.m_num; i++)
    {
        RCNsisEntry e;
        e.m_which = ReadUInt32();
        for (uint32_t j = 0; j < s_numEntryParams; j++)
            e.m_params[j] = ReadUInt32();
#ifdef NSIS_SCRIPT
        if (e.Which != EW_PUSHPOP && e.Which < sizeof(kCommandPairs) / sizeof(kCommandPairs[0]))
        {
            const CCommandPair &pair = kCommandPairs[e.Which];
            Script += pair.Name;
        }
#endif

        switch (e.m_which)
        {
        case EW_CREATEDIR:
            {
                if (m_isUnicode)
                {
                    prefixU.clear();
                    prefixU = ReadString2U(e.m_params[0]);
                }
                else
                {
                    prefixA.clear();
                    prefixA = ReadString2A(e.m_params[0]);
                }
#ifdef NSIS_SCRIPT
                Script += " ";
                if (m_isUnicode)
                    Script += UnicodeStringToMultiByte(prefixU);
                else
                    Script += prefixA;
#endif
                break;
            }

        case EW_EXTRACTFILE:
            {
                RCNsisItem item;
                item.m_isUnicode = m_isUnicode;
                if (m_isUnicode)
                {
                    item.m_prefixU = prefixU;
                    item.m_nameU = ReadString2U(e.m_params[1]);
                }
                else
                {
                    item.m_prefixA = prefixA;
                    item.m_nameA = ReadString2A(e.m_params[1]);
                }
                /* uint32_t overwriteFlag = e.Params[0]; */
                item.m_pos = e.m_params[2];
                item.m_mTime.u32LowDateTime = e.m_params[3];
                item.m_mTime.u32HighDateTime = e.m_params[4];
                /* uint32_t allowIgnore = e.Params[5]; */
                if (m_items.size() > 0)
                {
                    /*
                    if (item.Pos == m_items.Back().Pos)
                    continue;
                    */
                }
                m_items.push_back(item);
#ifdef NSIS_SCRIPT
                Script += " ";

                if (m_isUnicode)
                    Script += UnicodeStringToMultiByte(item.NameU);
                else
                    Script += item.NameA;
#endif
                break;
            }


#ifdef NSIS_SCRIPT
        case EW_UPDATETEXT:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += UIntToString(e.Params[1]);
                break;
            }
        case EW_SETFILEATTRIBUTES:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += UIntToString(e.Params[1]);
                break;
            }
        case EW_IFFILEEXISTS:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += UIntToString(e.Params[1]);
                Script += " ";
                Script += UIntToString(e.Params[2]);
                break;
            }
        case EW_RENAME:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);
                Script += " ";
                Script += UIntToString(e.Params[2]);
                break;
            }
        case EW_GETFULLPATHNAME:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);
                Script += " ";
                Script += UIntToString(e.Params[2]);
                break;
            }
        case EW_SEARCHPATH:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);
                break;
            }
        case EW_GETTEMPFILENAME:
            {
                RCStringA s;
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);
                break;
            }

        case EW_DELETEFILE:
            {
                uint64_t flag = e.Params[1];
                if (flag != 0)
                {
                    Script += " ";
                    if (flag == DEL_REBOOT)
                        Script += "/REBOOTOK";
                    else
                        Script += UIntToString(e.Params[1]);
                }
                Script += " ";
                Script += ReadString2(e.Params[0]);
                break;
            }
        case EW_RMDIR:
            {
                uint64_t flag = e.Params[1];
                if (flag != 0)
                {
                    if ((flag & DEL_REBOOT) != 0)
                        Script += " /REBOOTOK";
                    if ((flag & DEL_RECURSE) != 0)
                        Script += " /r";
                }
                Script += " ";
                Script += ReadString2(e.Params[0]);
                break;
            }
        case EW_STRLEN:
            {
                Script += " ";
                Script += GetVar(e.Params[0]);;
                Script += " ";
                Script += ReadString2Qw(e.Params[1]);
                break;
            }
        case EW_ASSIGNVAR:
            {
                Script += " ";
                Script += GetVar(e.Params[0]);;
                Script += " ";
                Script += ReadString2Qw(e.Params[1]);
                RCStringA maxLen, startOffset;
                if (e.Params[2] != 0)
                    maxLen = ReadString2(e.Params[2]);
                if (e.Params[3] != 0)
                    startOffset = ReadString2(e.Params[3]);
                if (!maxLen.IsEmpty() || !startOffset.IsEmpty())
                {
                    Script += " ";
                    if (maxLen.IsEmpty())
                        Script += "\"\"";
                    else
                        Script += maxLen;
                    if (!startOffset.IsEmpty())
                    {
                        Script += " ";
                        Script += startOffset;
                    }
                }
                break;
            }
        case EW_STRCMP:
            {
                Script += " ";

                Script += " ";
                Script += ReadString2Qw(e.Params[0]);

                Script += " ";
                Script += ReadString2Qw(e.Params[1]);

                for (int32_t j = 2; j < 5; j++)
                {
                    Script += " ";
                    Script += UIntToString(e.Params[j]);
                }
                break;
            }
        case EW_INTCMP:
            {
                if (e.Params[5] != 0)
                    Script += "U";

                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);

                for (int32_t i = 2; i < 5; i++)
                {
                    Script += " ";
                    Script += UIntToString(e.Params[i]);
                }
                break;
            }
        case EW_INTOP:
            {
                Script += " ";
                Script += GetVar(e.Params[0]);
                Script += " ";
                int32_t numOps = 2;
                RCStringA op;
                switch (e.Params[3])
                {
                case 0: op = '+'; break;
                case 1: op = '-'; break;
                case 2: op = '*'; break;
                case 3: op = '/'; break;
                case 4: op = '|'; break;
                case 5: op = '&'; break;
                case 6: op = '^'; break;
                case 7: op = '~'; numOps = 1; break;
                case 8: op = '!'; numOps = 1; break;
                case 9: op = "||"; break;
                case 10: op = "&&"; break;
                case 11: op = '%'; break;
                default: op = UIntToString(e.Params[3]);
                }
                RCStringA p1 = ReadString2(e.Params[1]);
                if (numOps == 1)
                {
                    Script += op;
                    Script += p1;
                }
                else
                {
                    Script += p1;
                    Script += " ";
                    Script += op;
                    Script += " ";
                    Script += ReadString2(e.Params[2]);
                }
                break;
            }

        case EW_PUSHPOP:
            {
                int32_t isPop = (e.Params[1] != 0);
                if (isPop)
                {
                    Script += "Pop";
                    Script += " ";
                    Script += GetVar(e.Params[0]);;
                }
                else
                {
                    int32_t isExch = (e.Params[2] != 0);
                    if (isExch)
                    {
                        Script += "Exch";
                    }
                    else
                    {
                        Script += "Push";
                        Script += " ";
                        Script += ReadString2(e.Params[0]);
                    }
                }
                break;
            }

        case EW_SENDMESSAGE:
            {
                // SendMessage: 6 [output, hwnd, msg, wparam, lparam, [wparamstring?1:0 | lparamstring?2:0 | timeout<<2]
                Script += " ";
                // Script += ReadString2(e.Params[0]);
                // Script += " ";
                Script += ReadString2(e.Params[1]);
                Script += " ";
                Script += ReadString2(e.Params[2]);

                Script += " ";
                uint32_t spec = e.Params[5];
                // if (spec & 1)
                Script += IntToString(e.Params[3]);
                // else
                //   Script += ReadString2(e.Params[3]);

                Script += " ";
                // if (spec & 2)
                Script += IntToString(e.Params[4]);
                // else
                //   Script += ReadString2(e.Params[4]);

                if ((int32_t)e.Params[0] >= 0)
                {
                    Script += " ";
                    Script += GetVar(e.Params[1]);
                }

                spec >>= 2;
                if (spec != 0)
                {
                    Script += " /TIMEOUT=";
                    Script += IntToString(spec);
                }
                break;
            }

        case EW_GETDLGITEM:
            {
                Script += " ";
                Script += GetVar(e.Params[0]);;
                Script += " ";
                Script += ReadString2(e.Params[1]);
                Script += " ";
                Script += ReadString2(e.Params[2]);
                break;
            }


        case EW_REGISTERDLL:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                Script += " ";
                Script += ReadString2(e.Params[1]);
                Script += " ";
                Script += UIntToString(e.Params[2]);
                break;
            }

        case EW_CREATESHORTCUT:
            {
                RCStringA s;

                Script += " ";
                Script += ReadString2Qw(e.Params[0]);

                Script += " ";
                Script += ReadString2Qw(e.Params[1]);

                for (int32_t j = 2; j < 5; j++)
                {
                    Script += " ";
                    Script += UIntToString(e.Params[j]);
                }
                break;
            }

            /*
            case EW_DELREG:
            {
            RCStringA keyName, valueName;
            keyName = ReadString2(e.Params[1]);
            bool isValue = (e.Params[2] != -1);
            if (isValue)
            {
            valueName = ReadString2(e.Params[2]);
            Script += "Key";
            }
            else
            Script += "Value";
            Script += " ";
            Script += UIntToString(e.Params[0]);
            Script += " ";
            Script += keyName;
            if (isValue)
            {
            Script += " ";
            Script += valueName;
            }
            Script += " ";
            Script += UIntToString(e.Params[3]);
            break;
            }
            */

        case EW_WRITEREG:
            {
                RCStringA s;
                switch(e.Params[4])
                {
                case 1:  s = "Str"; break;
                case 2:  s = "ExpandStr"; break;
                case 3:  s = "Bin"; break;
                case 4:  s = "DWORD"; break;
                default: s = "?" + UIntToString(e.Params[4]); break;
                }
                Script += s;
                Script += " ";
                Script += GetRegRootID(e.Params[0]);
                Script += " ";

                RCStringA keyName, valueName;
                keyName = ReadString2Qw(e.Params[1]);
                Script += keyName;
                Script += " ";

                valueName = ReadString2Qw(e.Params[2]);
                Script += valueName;
                Script += " ";

                valueName = ReadString2Qw(e.Params[3]);
                Script += valueName;
                Script += " ";

                break;
            }

        case EW_WRITEUNINSTALLER:
            {
                Script += " ";
                Script += ReadString2(e.Params[0]);
                for (int32_t j = 1; j < 3; j++)
                {
                    Script += " ";
                    Script += UIntToString(e.Params[j]);
                }
                break;
            }

        default:
            {
                int32_t numParams = kNumEntryParams;
                if (e.Which < sizeof(kCommandPairs) / sizeof(kCommandPairs[0]))
                {
                    const CCommandPair &pair = kCommandPairs[e.Which];
                    // Script += pair.Name;
                    numParams = pair.NumParams;
                }
                else
                {
                    Script += "Unknown";
                    Script += UIntToString(e.Which);
                }
                Script += e.GetParamsString(numParams);
            }
#endif
        }
#ifdef NSIS_SCRIPT
        Script += g_crLf;
#endif
    }

    {
        std::sort(m_items.begin(), m_items.end() ,RCCompareItems()) ;
        // m_items.Sort(CompareItems, 0);
        int32_t i;
        // if (m_isSolid)
        for (i = 0; i + 1 < (int32_t)m_items.size();)
        {
            bool sameName = m_isUnicode ?
                (m_items[i].m_nameU == m_items[i + 1].m_nameU) :
            (m_items[i].m_nameA == m_items[i + 1].m_nameA);
            if (m_items[i].m_pos == m_items[i + 1].m_pos && (m_isSolid || sameName))
                RCVectorUtils::Delete(m_items, i + 1);
            else
                i++;
        }
        for (i = 0; i + 1 < (int32_t)m_items.size(); i++)
        {
            RCNsisItem &item = m_items[i];
            item.m_estimatedSizeIsDefined = true;
            item.m_estimatedSize = m_items[i + 1].m_pos - item.m_pos - 4;
        }

        if (!m_isSolid)
        {
            for (i = 0; i < (int32_t)m_items.size(); i++)
            {
                RCNsisItem& item = m_items[i];
                HResult hr = m_stream->Seek(GetPosOfNonSolidItem(i), RC_STREAM_SEEK_SET, NULL);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                const uint32_t kSigSize = 4 + 1 + 5;
                byte_t sig[kSigSize];
                size_t processedSize = kSigSize;
                hr = RCStreamUtils::ReadStream(m_stream.Get(), sig, &processedSize);
                if (hr != RC_S_OK)
                {
                    return hr;
                }

                if (processedSize < 4)
                {
                    return RC_S_FALSE;
                }

                uint32_t size = Get32(sig);
                if ((size & 0x80000000) != 0)
                {
                    item.m_isCompressed = true;
                    // is compressed;
                    size &= ~0x80000000;
                    if (m_method == RCNsisMethodType::kLZMA)
                    {
                        if (processedSize < 9)
                        {
                            return RC_S_FALSE;
                        }
                        if (m_filterFlag)
                        {
                            item.m_useFilter = (sig[4] != 0);
                        }
                        item.m_dictionarySize = Get32(sig + 5 + (m_filterFlag ? 1 : 0));
                    }
                }
                else
                {
                    item.m_isCompressed = false;
                    item.m_size = size;
                    item.m_sizeIsDefined = true;
                }
                item.m_compressedSize = size;
                item.m_compressedSizeIsDefined = true;
            }
        }
    }

    return RC_S_OK;
}

HResult RCNsisIn::Parse()
{
    // uint32_t offset = ReadUInt32();
    // ???? offset == m_firstHeader.HeaderLength
    /* uint32_t ehFlags = */ ReadUInt32();
    RCNsisBlockHeader bhPages, bhSections, bhEntries, bhStrings, bhLangTables, bhCtlColors, bhData;
    // RCNsisBlockHeader bgFont;
    ReadBlockHeader(bhPages);
    ReadBlockHeader(bhSections);
    ReadBlockHeader(bhEntries);
    ReadBlockHeader(bhStrings);
    ReadBlockHeader(bhLangTables);
    ReadBlockHeader(bhCtlColors);
    // ReadBlockHeader(bgFont);
    ReadBlockHeader(bhData);

    m_stringsPos = bhStrings.m_offset;
    uint32_t pos = GetOffset() + m_stringsPos;
    int32_t numZeros0 = 0;
    int32_t numZeros1 = 0;
    int32_t i;
    const int32_t kBlockSize = 256;
    for (i = 0; i < kBlockSize; i++)
    {
        if (pos >= m_size || pos + 1 >= m_size)
            break;
        char c0 = m_data[pos++];
        char c1 = m_data[pos++];
        char_t c = (c0 | ((char_t)c1 << 8));

        if (c >= NS_UN_CODES_START && c < NS_UN_CODES_END)
        {
            if (pos >= m_size || pos + 1 >= m_size)
                break;
            pos += 2;
            numZeros1++;
        }
        else
        {
            if (c0 == 0 && c1 != 0)
                numZeros0++;
            if (c1 == 0)
                numZeros1++;
        }
        // printf("\nnumZeros0 = %2x %2x", m_data[pos + 0],  m_data[pos + 1]);
    }
    m_isUnicode = (numZeros1 > numZeros0 * 3 + kBlockSize / 16);
    // printf("\nnumZeros0 = %3d    numZeros1 = %3d", numZeros0,  numZeros1);
    return ReadEntries(bhEntries);
}

static bool IsLZMA(const byte_t *p, uint32_t &dictionary)
{
    dictionary = Get32(p + 1);
    return (p[0] == 0x5D && p[1] == 0x00 && p[2] == 0x00 && p[5] == 0x00);
}

static bool IsLZMA(const byte_t *p, uint32_t &dictionary, bool &thereIsFlag)
{
    if (IsLZMA(p, dictionary))
    {
        thereIsFlag = false;
        return true;
    }
    if (IsLZMA(p + 1, dictionary))
    {
        thereIsFlag = true;
        return true;
    }
    return false;
}

HResult RCNsisIn::Open2(ICompressCodecsInfo* codecsInfo)
{
    HResult hr = m_stream->Seek(0, RC_STREAM_SEEK_CUR, &m_streamOffset);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    const uint32_t kSigSize = 4 + 1 + 5 + 1; // size, flag, lzma props, lzma first byte
    byte_t sig[kSigSize];

    hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), sig, kSigSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    uint64_t position;
    hr = m_stream->Seek(m_streamOffset, RC_STREAM_SEEK_SET, &position);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_headerIsCompressed = true;
    m_isSolid = true;
    m_filterFlag = false;

    uint32_t compressedHeaderSize = Get32(sig);

    if (compressedHeaderSize == m_firstHeader.m_headerLength)
    {
        m_headerIsCompressed = false;
        m_isSolid = false;
        m_method = RCNsisMethodType::kCopy;
    }
    else if (IsLZMA(sig, m_dictionarySize, m_filterFlag))
    {
        m_method = RCNsisMethodType::kLZMA;
    }
    else if (IsLZMA(sig + 4, m_dictionarySize, m_filterFlag))
    {
        m_isSolid = false;
        m_method = RCNsisMethodType::kLZMA;
    }
    else if (sig[3] == 0x80)
    {
        m_isSolid = false;
        m_method = RCNsisMethodType::kDeflate;
    }
    else
    {
        m_method = RCNsisMethodType::kDeflate;
    }

    m_posInData = 0;
    if (!m_isSolid)
    {
        m_headerIsCompressed = ((compressedHeaderSize & 0x80000000) != 0);
        if (m_headerIsCompressed)
        {
            compressedHeaderSize &= ~0x80000000;
        }
        m_nonSolidStartOffset = compressedHeaderSize;
        hr = m_stream->Seek(m_streamOffset + 4, RC_STREAM_SEEK_SET, NULL);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }

    uint32_t unpackSize = m_firstHeader.m_headerLength;
    if (m_headerIsCompressed)
    {
        m_data.SetCapacity(unpackSize);

        hr = m_decoder.Init(codecsInfo,
                            m_stream.Get(),
                            m_method,
                            m_filterFlag,
                            m_useFilter);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        size_t processedSize = unpackSize;
        hr = m_decoder.Read(m_data.data(), &processedSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (processedSize != unpackSize)
        {
            return RC_S_FALSE;
        }

        m_size = processedSize;
        if (m_isSolid)
        {
            uint32_t size2 = ReadUInt32();
            if (size2 < m_size)
            {
                m_size = size2;
            }
        }
    }
    else
    {
        m_data.SetCapacity(unpackSize);
        m_size = (size_t)unpackSize;
        hr = RCStreamUtils::ReadStream_FALSE(m_stream.Get(), (byte_t *)m_data.data(), unpackSize);
        if (hr != RC_S_OK)
        {
            return hr;
        }
    }
    return Parse();
}

/*
NsisExe =
{
ExeStub
Archive  // must start from 512 * N
#ifndef NSIS_CONFIG_CRC_ANAL
{
Some additional data
}
}

Archive
{
FirstHeader
Data
#ifdef NSIS_CONFIG_CRC_SUPPORT && m_firstHeader.ThereIsCrc()
{
CRC
}
}

FirstHeader
{
uint32_t Flags;
byte_t Signature[16];
// points to the header+sections+entries+stringtable in the datablock
uint32_t HeaderLength;
uint32_t ArchiveSize;
}
*/

HResult RCNsisIn::Open(ICompressCodecsInfo* codecsInfo,
                       IInStream* inStream,
                       const uint64_t* maxCheckStartPosition)
{
    Clear();
    HResult hr = inStream->Seek(0, RC_STREAM_SEEK_SET, NULL);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    uint64_t maxSize = ((maxCheckStartPosition != 0) ? *maxCheckStartPosition : 0);
    const uint32_t kStep = 512;
    byte_t buffer[kStep];

    uint64_t position = 0;
    for (; position <= maxSize; position += kStep)
    {
        hr = RCStreamUtils::ReadStream_FALSE(inStream, buffer, kStep);
        if (hr != RC_S_OK)
        {
            return hr;
        }

        if (memcmp(buffer + 4, RCNsisSignature::kSignature, RCNsisSignature::kSignatureSize) == 0)
        {
            break;
        }
    }

    if (position > maxSize)
    {
        return RC_S_FALSE;
    }

    const uint32_t kStartHeaderSize = 4 * 7;
    hr = inStream->Seek(0, RC_STREAM_SEEK_END, &m_archiveSize);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    hr = inStream->Seek(position + kStartHeaderSize, RC_STREAM_SEEK_SET, 0);
    if (hr != RC_S_OK)
    {
        return hr;
    }

    m_firstHeader.m_flags = Get32(buffer);
    m_firstHeader.m_headerLength = Get32(buffer + RCNsisSignature::kSignatureSize + 4);
    m_firstHeader.m_archiveSize = Get32(buffer + RCNsisSignature::kSignatureSize + 8);
    if (m_archiveSize - position < m_firstHeader.m_archiveSize)
    {
        return RC_S_FALSE;
    }

    m_stream = inStream;
    hr = Open2(codecsInfo);
    if (hr != RC_S_OK)
    {
        Clear();
    }
    m_stream.Release();
    return hr;
}

void RCNsisIn::Clear()
{
#ifdef NSIS_SCRIPT
    Script.Empty();
#endif
    m_items.clear();
    m_stream.Release();
}

END_NAMESPACE_RCZIP
