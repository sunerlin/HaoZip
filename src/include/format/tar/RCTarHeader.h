/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#ifndef __RCTarHeader_h_
#define __RCTarHeader_h_ 1

#include "base/RCDefs.h"

BEGIN_NAMESPACE_RCZIP

/** Tar 常量定义
*/
namespace RCTarHeader
{
    const int32_t s_recordSize = 512;
    const int32_t s_nameSize = 100;
    const int32_t s_userNameSize = 32;
    const int32_t s_groupNameSize = 32;
    const int32_t s_prefixSize = 155;

    namespace NMode
    {
        /** Set UID on execution
        */
        const int32_t s_setUID   = 04000;  

        /** Set GID on execution
        */
        const int32_t s_setGID   = 02000;  

        /** Save text (sticky bit)
        */
        const int32_t s_saveText = 01000;  
    }

    namespace NFilePermissions
    {
        /** read by owner
        */
        const int32_t s_userRead     = 00400;  

        /** write by owner
        */
        const int32_t s_userWrite    = 00200;  

        /** execute/search by owner
        */
        const int32_t s_userExecute  = 00100;  

        /** read by group
        */
        const int32_t s_groupRead    = 00040;  

        /** write by group
        */
        const int32_t s_groupWrite   = 00020;  

        /** execute/search by group
        */
        const int32_t s_groupExecute = 00010;  

        /** read by other
        */
        const int32_t s_otherRead    = 00004;  

        /** write by other
        */
        const int32_t s_otherWrite   = 00002;  

        /** execute/search by other
        */
        const int32_t s_otherExecute = 00001;  
    }

    namespace NLinkFlag
    {
        /** Normal disk file, Unix compatible
        */
        const char s_oldNormal    = '\0'; 

        /** Normal disk file
        */
        const char s_normal       = '0'; 

        /** Link to previously dumped file
        */
        const char s_link         = '1'; 

        /** Symbolic link
        */
        const char s_symbolicLink = '2'; 

        /** Character special file
        */
        const char s_character    = '3'; 

        /** Block special file
        */
        const char s_block        = '4'; 

        /** Directory
        */
        const char s_directory    = '5'; 

        /** FIFO special file
        */
        const char s_fifo         = '6'; 

        /** Contiguous file
        */
        const char s_contiguous   = '7'; 

        /** GNUTYPE_DUMPDIR.
        */
        const char s_dumpDir       = 'D'; 
    }

    
    /**   = "        ";   // 8 blanks, no null
    */
    extern const char *s_checkSumBlanks;

    /**   = "././@LongLink";
    */
    extern const char *s_longLink;  

    /** = "@LongLink";
    */
    extern const char *s_longLink2; 

    // The magic field is filled with this if uname and gname are valid.
    namespace NMagic
    {
        extern const char *s_usTar; //   = "ustar"; // 5 chars
        extern const char *s_gnuTar; //  = "GNUtar "; // 7 chars and a null
        extern const char *s_empty; //  = "GNUtar "; // 7 chars and a null
    }
}

END_NAMESPACE_RCZIP

#endif //__RCTarHeader_h_
