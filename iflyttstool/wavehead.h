#ifndef __WAVEHEAD_H__
#define __WAVEHEAD_H__

typedef int SR_DWORD;
typedef short int SR_WORD ;

typedef struct wave_pcm_hdr
        {
                char            riff[4];                        // = "RIFF"
                SR_DWORD        size_8;                         // = FileSize - 8
                char            wave[4];                        // = "WAVE"

                char            fmt[4];                         // = "fmt "
                SR_DWORD        dwFmtSize;                      // = 下一个结构体的大小 : 16

                SR_WORD         format_tag;              // = PCM : 1,0代表压缩的wave文件
                SR_WORD         channels;                       // = 通道数 : 1
                SR_DWORD        samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
                SR_DWORD        avg_bytes_per_sec;      // = 每秒字节数 : dwSamplesPerSec * wBitsPerSample / 8
                SR_WORD         block_align;            // = 每采样点字节数 : wBitsPerSample / 8
                SR_WORD         bits_per_sample;         // = 量化比特数: 8 | 16

                char            data[4];                        // = "data";
                SR_DWORD        data_size;                // = 纯数据长度 : FileSize - 44,播放器的进度条 
        } wave_pcm_hdr;

#endif
