    好压(HaoZip) V1.7 压缩文件格式与压缩解压算法部分源代码
--------------------------------------------------------------------------------
    好压(HaoZip)是一款Windows平台的压缩解压软件。
********************************************************************************
*  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   * 
******************************************************************************** 
*  作者    : HaoZip                                                            * 
*  版本    : 1.7                                                               * 
*  联系方式: haozip@gmail.com                                                  * 
*  官方网站: www.haozip.com                                                    * 
********************************************************************************

1. 如何编译源代码
(1) 获取第三方类库代码
    好压在实现时依赖boost库、UnRar库、WavePack库，首先需要获取这三个第三方类库的
    源代码，具体如下：
    A. (必须)获取boost库的代码，好压使用的是boost 1.38版本的类库，可以从
       http://sourceforge.net/projects/boost/files/boost/1.38.0/网址获取。
       获取后，将代码解压 vendor\include\boost 目录中，不需要编译boost库。
       
    B. (可选)获取UnRar库，好压在处理Rar格式时，调用了UnRar库的部分代码，可以从
       http://www.rarlab.com/rar_add.htm网站下载最新代码。
       下载后，需要编译代码为Lib库，供好压链接使用。
       对于debug 版本， 使用 /MDd选项编译
       对于Release版本，需要使用 /MD 选项编译
       编译后的Lib名称：Debug版本，名称为：UnRar_MDd.lib
                        Release版本，名称为：UnRar_MD.lib
       对于Win32，Debug版本的UnRar_MDd.lib放在vendor\lib\win32\debug目录
                  Release版本的UnRar_MD.lib放在vendor\lib\win32\release目录
       对于X64，Debug版本的UnRar_MDd.lib放在vendor\lib\x64\debug目录
                Release版本的UnRar_MD.lib放在vendor\lib\x64\release目录
                
       UnRar的头文件需要放在vendor\include\UnRar目录。
       
    C. (可选)获取WavePack类库，可以从http://www.wavpack.com/downloads.html下载。
       下载后，下载后，需要编译代码为Lib库，供好压链接使用。
       对于debug 版本， 使用 /MDd选项编译
       对于Release版本，需要使用 /MD 选项编译
       编译后的Lib名称：Debug版本，名称为：WavPack_MDd.lib
                        Release版本，名称为：WavPack_MD.lib
       对于Win32，Debug版本的WavPack_MDd.lib放在vendor\lib\win32\debug目录
                  Release版本的WavPack_MD.lib放在vendor\lib\win32\release目录
       对于X64，Debug版本的WavPack_MDd.lib放在vendor\lib\x64\debug目录
                Release版本的WavPack_MD.lib放在vendor\lib\x64\release目录
       
       头文件wavpack.h需要放在vendor\include\wavpack目录。

(2) 第三方类库内容准备好以后，进入build\MSVC目录，使用VC打开build.sln文件可以进行编译。
    所需的VC版本为 VC 2005.

2. 文件结构说明
(1) build 目录
    为HaoZip的VC 2005的工程文件
(2) doc 目录
    为HaoZip相关的开发文档资料
(3) include 目录
    为HaoZip的接口源文件，按照功能划分为以下子目录：
      algorithm 目录  -- 压缩与解压算法C实现的接口文件
      archive   目录  -- 压缩解压接口
      base 目录       -- 工程公用接口定义
      coder 目录      -- 压缩与解压相关的编码接口定义
      common 目录     -- 公用算法接口定义
      compress 目录   -- 压缩与解压算法接口
      crypto 目录     -- 加密与解密算法接口
      exception 目录  -- 异常与容错接口
      filesystem 目录 -- IO访问接口
      format 目录     -- 各种文件格式封装接口
      interface       -- 压缩与解压流程的接口定义
      language        -- 语言管理接口
      locked          -- 压缩与解压进度相关的多线程封装接口
      thread          -- 多线程封装接口
      version         -- 版本管理接口
(4) src 目录
      为HaoZip的实现文件目录，按照功能划分为以下子目录：
      algorithm 目录  -- 压缩与解压算法C实现
      coder 目录      -- 压缩与解压相关的编码实现
      compress 目录   -- 压缩与解压算法实现
      crypto 目录     -- 加密与解密算法实现
      format 目录     -- 各种文件格式封装实现
(5) vendor 目录
      该目录为HaoZip依赖的第三方类库。
      vendor\include\boost    -- boost 的接口文件，需自己下载后放在此目录
      vendor\include\UnRar    -- UnRar类库的接口文件
      vendor\include\wavpack  -- wavpack类库的接口文件
      
      vendor\lib\win32\debug    -- Win32的Debug版本Lib文件，应包含UnRar_MDd.lib和WavPack_MDd.lib两个文件
      vendor\lib\win32\release  -- Win32的Release版本Lib文件，应包含UnRar_MD.lib和WavPack_MD.lib两个文件
      vendor\lib\x64\debug      -- x64的Debug版本Lib文件，应包含UnRar_MDd.lib和WavPack_MDd.lib两个文件
      vendor\lib\win32\debug    -- x64的Debug版本Lib文件，应包含UnRar_MD.lib和WavPack_MD.lib两个文件
      
      vendor\depends\UnRar      -- UnRar库的源代码，使用时请遵循原作者的授权协议。
      vendor\depends\wavPack    -- WavPack库的源代码，使用时请遵循原作者的授权协议。
      
(6) windows 目录
      该目录包含链接DLL时使用的manifest文件。
      
      windows\manifest\HaoZip.dll.x64.manifest  --  x64环境下使用的manifest文件
      windows\manifest\HaoZip.dll.x86.manifest  --  Win32环境下使用的manifest文件
(7) lib 目录
      该目录包含HaoZip底层封装的部分预编译Lib文件。
      
      lib\win32\debug\RCArchive_MDd.lib     -- Win32环境, Debug版本，/MDd编译选项
      lib\win32\debug\RCCommon_MDd.lib      -- Win32环境, Debug版本，/MDd编译选项
      
      lib\win32\release\RCArchive_MD.lib    -- Win32环境, Release版本，/MD编译选项
      lib\win32\release\RCCommon_MD.lib     -- Win32环境, Release版本，/MD编译选项
      
      lib\x64\debug\RCArchive_MDd.lib       -- x64环境, Debug版本，/MDd编译选项
      lib\x64\debug\RCCommon_MDd.lib        -- x64环境, Debug版本，/MDd编译选项
      
      lib\x64\release\RCArchive_MD.lib      -- x64环境, Release版本，/MD编译选项
      lib\x64\release\RCCommon_MD.lib       -- x64环境, Release版本，/MD编译选项
      
      其中，RCArchive_MD.lib和RCArchive_MDd.lib 是HaoZip压缩与解压流程封装的预编译Lib。
            RCCommon_MD.lib和RCCommon_MDd.lib 是HaoZip底层基础算法实现类库的预编译Lib。
            以上两个Lib类库是好压底层类库的一部分，不开放源代码。
