/********************************************************************************
 *  版权所有(C)2008,2009,2010，好压软件工作室，保留所有权利。                   *
 ********************************************************************************
 *  作者    : HaoZip                                                            *
 *  版本    : 1.7                                                               *
 *  联系方式: haozip@gmail.com                                                  *
 *  官方网站: www.haozip.com                                                    *
 ********************************************************************************/

#include "compress/wavpack/RCWavUnpack.h"
#include "compress/wavpack/RCWavUnpackStreamReader.h"

#define CLEAR(destin) memset (&destin, 0, sizeof (destin));

BEGIN_NAMESPACE_RCZIP

static int DoWriteStream(void *hFile, void *lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten)
{
    uint32_t write_size = 0;
    ISequentialOutStream* stream = static_cast<ISequentialOutStream*>(hFile);
    HResult ret = stream->Write(lpBuffer, nNumberOfBytesToWrite, &write_size);
    if (IsSuccess(ret))
    {
        *lpNumberOfBytesWritten = write_size;
        return write_size;
    }
    else
    {
        return -1;
    }
}

static int write_riff_header (void *outfile, WavpackContext *wpc, uint32_t total_samples)
{
    RiffChunkHeader riffhdr;
    ChunkHeader datahdr, fmthdr;
    WaveHeader wavhdr;
    uint32_t bcount;

    uint32_t total_data_bytes;
    int num_channels = WavpackGetNumChannels (wpc);
    int32_t channel_mask = WavpackGetChannelMask (wpc);
    int32_t sample_rate = WavpackGetSampleRate (wpc);
    int bytes_per_sample = WavpackGetBytesPerSample (wpc);
    int bits_per_sample = WavpackGetBitsPerSample (wpc);
    int format = WavpackGetFloatNormExp (wpc) ? 3 : 1;
    int wavhdrsize = 16;

    if (format == 3 && WavpackGetFloatNormExp (wpc) != 127) 
    {
        return FALSE;
    }

    if (total_samples == (uint32_t) -1)
    {
        total_samples = 0x7ffff000 / (bytes_per_sample * num_channels);
    }

    total_data_bytes = total_samples * bytes_per_sample * num_channels;

    CLEAR (wavhdr);

    wavhdr.FormatTag = static_cast<ushort>(format);
    wavhdr.NumChannels = static_cast<ushort>(num_channels);
    wavhdr.SampleRate = sample_rate;
    wavhdr.BytesPerSecond = sample_rate * num_channels * bytes_per_sample;
    wavhdr.BlockAlign = static_cast<ushort>(bytes_per_sample * num_channels);
    wavhdr.BitsPerSample = static_cast<ushort>(bits_per_sample);

    if (num_channels > 2 || channel_mask != 0x5 - num_channels) 
    {
        wavhdrsize = sizeof (wavhdr);
        wavhdr.cbSize = 22;
        wavhdr.ValidBitsPerSample = static_cast<ushort>(bits_per_sample);
        wavhdr.SubFormat = static_cast<ushort>(format);
        wavhdr.ChannelMask = channel_mask;
        wavhdr.FormatTag = 0xfffe;
        wavhdr.BitsPerSample = static_cast<ushort>(bytes_per_sample * 8);
#pragma warning( push )  
#pragma warning (disable: 4309)
        wavhdr.GUID [4] = 0x10;
        wavhdr.GUID [6] = 0x80;
        wavhdr.GUID [9] = 0xaa;
        wavhdr.GUID [11] = 0x38;
        wavhdr.GUID [12] = 0x9b;
        wavhdr.GUID [13] = 0x71;
#pragma warning( pop )
    }

#ifdef RCZIP_OS_WIN
    strncpy_s (riffhdr.ckID, sizeof (riffhdr.ckID), "RIFF", sizeof (riffhdr.ckID));
    strncpy_s (riffhdr.formType, sizeof (riffhdr.formType), "WAVE", sizeof (riffhdr.formType));
#else
    strncpy (riffhdr.ckID, "RIFF", sizeof (riffhdr.ckID));
    strncpy (riffhdr.formType, "WAVE", sizeof (riffhdr.formType));
#endif
    riffhdr.ckSize = sizeof (riffhdr) + wavhdrsize + sizeof (datahdr) + total_data_bytes;
#ifdef RCZIP_OS_WIN
    strncpy_s (fmthdr.ckID, sizeof (fmthdr.ckID), "fmt ", sizeof (fmthdr.ckID));
#else
    strncpy (fmthdr.ckID, "fmt ", sizeof (fmthdr.ckID));
#endif
    fmthdr.ckSize = wavhdrsize;

#ifdef RCZIP_OS_WIN
    strncpy_s (datahdr.ckID, sizeof (datahdr.ckID), "data", sizeof (datahdr.ckID));
#else
    strncpy (datahdr.ckID, "data", sizeof (datahdr.ckID));
#endif
    datahdr.ckSize = total_data_bytes;

    // write the RIFF chunks up to just before the data starts

    WavpackNativeToLittleEndian (&riffhdr, ChunkHeaderFormat);
    WavpackNativeToLittleEndian (&fmthdr, ChunkHeaderFormat);
    WavpackNativeToLittleEndian (&wavhdr, WaveHeaderFormat);
    WavpackNativeToLittleEndian (&datahdr, ChunkHeaderFormat);

    if (!DoWriteStream (outfile, &riffhdr, sizeof (riffhdr), &bcount) || bcount != sizeof (riffhdr) ||
        !DoWriteStream (outfile, &fmthdr, sizeof (fmthdr), &bcount) || bcount != sizeof (fmthdr) ||
        !DoWriteStream (outfile, &wavhdr, wavhdrsize, &bcount) || bcount != static_cast<uint32_t>(wavhdrsize) ||
        !DoWriteStream (outfile, &datahdr, sizeof (datahdr), &bcount) || bcount != sizeof (datahdr)) 
    {
            return FALSE;
    }

    return TRUE;
}

static uchar *format_samples (int bps, uchar *dst, int32_t *src, uint32_t samcnt)
{
    int32_t temp;

    switch (bps) 
    {

    case 1:
        while (samcnt--)
            *dst++ = static_cast<uchar>(*src++ + 128);

        break;

    case 2:
        while (samcnt--) 
        {
            *dst++ = (uchar) (temp = *src++);
            *dst++ = (uchar) (temp >> 8);
        }

        break;

    case 3:
        while (samcnt--) 
        {
            *dst++ = (uchar) (temp = *src++);
            *dst++ = (uchar) (temp >> 8);
            *dst++ = (uchar) (temp >> 16);
        }

        break;

    case 4:
        while (samcnt--) 
        {
            *dst++ = (uchar) (temp = *src++);
            *dst++ = (uchar) (temp >> 8);
            *dst++ = (uchar) (temp >> 16);
            *dst++ = (uchar) (temp >> 24);
        }

        break;
    }

    return dst;
}

/** wavpack解压算法实现
*/
int RcWavUnpackFile(void* inFileStream, void* outFileStream, ICompressProgressInfo* progressCallback)
{
    int result = NO_ERROR ;
    int created_riff_header = 0 ;
    WavpackContext *wpc = NULL;
    char error[80] = {0} ;
    int bytes_per_sample = 0 ;
    int num_channels = 0 ;
    int wvc_mode = 0 ;
    int bps = 0 ;
    uint32_t output_buffer_size = 0 ;
    uint32_t bcount = 0 ;
    uint32_t total_unpacked_samples = 0;
    uint32_t until_samples_total = 0;
    uchar* output_buffer = NULL ;
    uchar* output_pointer = NULL;
    double progress = -1.0;
    int32_t* temp_buffer = NULL ;
    uint32_t total_write_size = 0;

    ISequentialInStream* file_stream = static_cast<ISequentialInStream*>(inFileStream);
    RCWavPackBufferStream buffer_stream(file_stream);

    wpc = RcWavpackOpenInputStream(&buffer_stream, error, OPEN_WRAPPER, 0);

    if (!wpc) 
    {
        return SOFT_ERROR;
    }

    wvc_mode = WavpackGetMode(wpc) & MODE_WVC;
    num_channels = WavpackGetNumChannels(wpc);
    bps = WavpackGetBytesPerSample(wpc);
    bytes_per_sample = num_channels * bps;

    output_buffer_size = 0x40000;

    output_pointer = output_buffer = (uchar*)malloc(output_buffer_size);

    if (outFileStream) 
    {
        if (until_samples_total) 
        {
            if (!write_riff_header (outFileStream, wpc, until_samples_total)) 
            {
                result = HARD_ERROR;
            }
            else 
            {
                created_riff_header = TRUE;
            }
        }
        else if (WavpackGetWrapperBytes(wpc)) 
        {
            if (!DoWriteStream(outFileStream, WavpackGetWrapperData(wpc), WavpackGetWrapperBytes(wpc), &bcount) ||
                bcount != WavpackGetWrapperBytes(wpc)) 
            {
                result = HARD_ERROR;
            }
            total_write_size += bcount;
            WavpackFreeWrapper(wpc);
        }
        else if (!write_riff_header (outFileStream, wpc, WavpackGetNumSamples (wpc))) 
        {
            result = HARD_ERROR;
        }
        else 
        {
            created_riff_header = TRUE;
        }
    }

    temp_buffer = (int32_t*)malloc (4096L * num_channels * 4);

    while(result == NO_ERROR)
    {
        uint32_t samples_to_unpack, samples_unpacked;
        if (output_buffer) 
        {
            samples_to_unpack = (output_buffer_size - static_cast<uint32_t>((output_pointer - output_buffer))) / bytes_per_sample;        
            if (samples_to_unpack > 4096) 
            {
                samples_to_unpack = 4096;
            }
        }
        else 
        {
            samples_to_unpack = 4096;
        }

        samples_unpacked = WavpackUnpackSamples(wpc, temp_buffer, samples_to_unpack);
        total_unpacked_samples += samples_unpacked;

        if (output_buffer) 
        {
            if (samples_unpacked) 
            {
                output_pointer = format_samples(bps, output_pointer, temp_buffer, samples_unpacked * num_channels);
            }

            if (!samples_unpacked || (output_buffer_size - (output_pointer - output_buffer)) < (uint32_t) bytes_per_sample) {
                if (!DoWriteStream(outFileStream, output_buffer, (uint32_t)(output_pointer - output_buffer), &bcount) ||
                    bcount != static_cast<uint32_t>(output_pointer - output_buffer)) 
                {
                        result = HARD_ERROR;
                        break;
                }
                total_write_size += bcount;
                output_pointer = output_buffer;
            }
        }

        if (!samples_unpacked) 
        {
            break;
        }

        if (progressCallback)
        {
            HResult hr = progressCallback->SetRatioInfo(buffer_stream.GetPos(), total_write_size);
            if (!IsSuccess(hr))
            {
                result = USER_ABORT;
                break;
            }
        }
    }

    if (output_buffer) 
    {
        free(output_buffer);
    }

    while(!created_riff_header && WavpackGetWrapperBytes(wpc)) 
    {
        if (outFileStream && result == NO_ERROR && (!DoWriteStream(outFileStream, WavpackGetWrapperData(wpc), WavpackGetWrapperBytes(wpc), &bcount) ||
            bcount != WavpackGetWrapperBytes(wpc))) 
        {
                result = HARD_ERROR;
        }

        total_write_size += bcount;
        WavpackFreeWrapper(wpc);
        WavpackUnpackSamples(wpc, temp_buffer, 1);

        if (progressCallback)
        {
            HResult hr = progressCallback->SetRatioInfo(buffer_stream.GetPos(), total_write_size);
            if (!IsSuccess(hr))
            {
                result = USER_ABORT;
                break;
            }
        }
    }

    if (temp_buffer) 
    {
        free(temp_buffer);
    }

    WavpackCloseFile(wpc);

    return result;
}

END_NAMESPACE_RCZIP
