#include "stdafx.h"
#include <string>
#include "../include/qtts.h"
#include <conio.h>
#include "get_config.h"
#include "wavehead.h"
#include <iostream>  
#include <stdio.h>
#include <fstream>  
#include "TTSHELPER_CLASS.h"
#include "FILEUTILITY_CLASS.h"

#pragma comment(lib,"../lib/msc.lib")

TTSHelper::~TTSHelper()
{
	if(AudioBuffer!=NULL)
	{
		delete AudioBuffer;
		AudioBuffer=NULL;
	}
}

TTSHelper::TTSHelper(ttsconfig &ttsconf,wave_pcm_hdr &wph):LastErrorInfo(""),AudioBuffer(NULL),AudioSize(0)
{
	this->ttsconf=ttsconf;
	this->pcmwavhdr=wph;
}

int TTSHelper::CreateWaveAudioFile(const char *szText,const char *audioFileName)
{
	const char* m_configs = ttsconf.m_configs;
	const char* szParam = ttsconf.szParam;

	printf("begin to synth...\n");

	int ret = QTTSInit(m_configs);
	if ( ret != MSP_SUCCESS )
	{
		LastErrorInfo="QTTSInit: qtts init failed, ret = "+ret;
		return ret;
	}
	//每次合成时，需要将这两个数据清零
	pcmwavhdr.size_8=0;
	pcmwavhdr.data_size=0;
	
	const char* sess_id = QTTSSessionBegin(szParam, &ret);
	if ( ret != MSP_SUCCESS )
	{
		//printf("QTTSSessionBegin: qtts begin session failed Error code %d.\n",ret);
		LastErrorInfo="QTTSSessionBegin: qtts begin session failed Error code: "+ret;
		QTTSFini();
		return ret;
	}

	int len = strlen(szText);
	ret = QTTSTextPut(sess_id, szText, len, NULL );
	if ( ret != MSP_SUCCESS )
	{
		//printf("QTTSTextPut: qtts put text failed Error code %d.\n",ret);
		LastErrorInfo="QTTSTextPut: qtts put text failed Error code: "+ret;
		QTTSSessionEnd(sess_id, "");
		QTTSFini();
		return ret;
	}

	char* audio_data;
	unsigned int audio_len = 0;
	int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	const char*  tmpname = audioFileName;
	FILE* fp = fopen(tmpname, "wb");
	if ( fp == NULL )
	{
		LastErrorInfo="Failed to open file: "+string(tmpname);
		QTTSSessionEnd(sess_id, "");
		QTTSFini();
		return ret;
	}

	//写入音频头部数据
	fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);

	int pos = 0;			//用于标记上一次已经合成到的位置
	int loop_count = 0;		//用于标记，取了几次结果
	int upload_flow = 0,download_flow = 0;//上传流量和下载流量
	char param_value[32] = "";//参数值的字符串形式
	size_t value_len = 32;	//字符串长度或buffer长度

	while ( true )
	{
		audio_data = (char*)QTTSAudioGet( sess_id ,&audio_len , &synth_status , &ret );

		if ( ret != MSP_SUCCESS )
		{
			LastErrorInfo="QTTSAudioGet: qtts get audio failed Error code: "+ret;
			char key = _getch();
			break;
		}

		fwrite(audio_data, 1, audio_len, fp);
		pcmwavhdr.data_size += audio_len;//修正pcm数据的大小
		if ( MSP_TTS_FLAG_DATA_END == synth_status )
		{
			printf("QTTSAudioGet: get end of data.\n");
			break;
		}
	}

	//修正pcm文件头数据的大小
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;

	//将修正过的数据写回文件头部
	fseek(fp, 4, 0);
	fwrite(&(pcmwavhdr.size_8),sizeof(pcmwavhdr.size_8), 1, fp);
	fseek(fp, 40, 0);
	fwrite(&(pcmwavhdr.data_size),sizeof(pcmwavhdr.data_size), 1, fp);

	fclose(fp);

	ret = QTTSSessionEnd(sess_id, "");
	if ( ret != MSP_SUCCESS )
	{
		//printf("QTTSSessionEnd: qtts end failed Error code %d.\n",ret);
		LastErrorInfo="QTTSSessionEnd: qtts end failed Error code: "+ret;
		char key = _getch();
		QTTSFini();
		return ret;
	}

	ret = QTTSFini();
	if ( ret != MSP_SUCCESS )
	{
		//printf("CMSPAudioCtrl::PlayText: qtts fini failed Error code %d.\n",ret);
		LastErrorInfo="CMSPAudioCtrl::PlayText: qtts fini failed Error code: "+ret;
		char key = _getch();
		return ret;
	}

	  filebuf *pbuf;  
	  ifstream filestr;  
	  long size;  
	  // 要读入整个文件，必须采用二进制打开   
	  filestr.open (audioFileName, ios::binary);  
	  // 获取filestr对应buffer对象的指针   
	  pbuf=filestr.rdbuf();  
    
	  // 调用buffer对象方法获取文件大小  
	  size=pbuf->pubseekoff (0,ios::end,ios::in);  
	  pbuf->pubseekpos (0,ios::in);  
     
	  // 分配内存空间  
	  AudioBuffer=new char[size];  
	  AudioSize=size;
	  // 获取文件内容  
	  pbuf->sgetn (AudioBuffer,size);  
	  filestr.close();  

	  return 0;
}

int TTSHelper::AppendWaveAudioFile(const char *appendWaveAudioFileName,const char *destAudioFileName,bool isCreateDestFile)
{
	long size; 
	//获取音频纯数据
	wave_pcm_hdr tmp_wph;
	FILE *fp_wph=fopen(appendWaveAudioFileName,"rb");
	if (fp_wph==NULL)
	{
		return -1;
	}
	fread(&tmp_wph,sizeof(wave_pcm_hdr),1,fp_wph);
	size=tmp_wph.data_size;

	char *buffer=new char[size];  
	fseek(fp_wph,sizeof(wave_pcm_hdr),0);
	fread(buffer,size,1,fp_wph);
	fclose(fp_wph);

	FILE* fp = fopen(destAudioFileName, "ab");//文件末尾追加二进制
	if(fp==NULL)
	{
		return -1;
	}
	//如果是产生一个空文件然后追加音频数据需先写入pcm头部信息
	if(isCreateDestFile)
	{
		//写入音频头部数据
		fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);
	}

	fwrite(buffer, 1,size, fp);

	delete buffer;
	fclose(fp);

	//修正pcm
	pcmwavhdr.data_size +=size;
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;
	//将修正过的数据写回文件头部
	FILE* fppcm = fopen(destAudioFileName, "rb+");//二进制追加
	if(fppcm==NULL)
	{
		return -1;
	}
	fseek(fppcm, 4, 0);
	fwrite(&(pcmwavhdr.size_8),sizeof(pcmwavhdr.size_8), 1, fppcm);
	fseek(fppcm, 40, 0);
	fwrite(&(pcmwavhdr.data_size),sizeof(pcmwavhdr.data_size), 1, fppcm);
	fclose(fppcm);

	return 0;
}

int TTSHelper::AppendWaveAudioBuffer(const char *appendWaveAudioBuffer,int audioBufferSize,const char *destAudioFileName,bool isCreateDestFile)
{
	FILE* fp = fopen(destAudioFileName, "ab");//文件末尾追加二进制
	if(fp==NULL)
	{
		return -1;
	}
	//如果是产生一个空文件然后追加音频数据需先写入pcm头部信息
	if(isCreateDestFile)
	{
		//写入音频头部数据
		fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);
	}

	//去掉追加的音频的头信息
	audioBufferSize-=sizeof(wave_pcm_hdr);
	appendWaveAudioBuffer+=sizeof(wave_pcm_hdr);

	fwrite(appendWaveAudioBuffer, 1,audioBufferSize, fp);
	fclose(fp);

	//修正pcm
	pcmwavhdr.data_size +=audioBufferSize;
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;
	//将修正过的数据写回文件头部
	FILE* fppcm = fopen(destAudioFileName, "rb+");//二进制追加
	if(fppcm==NULL)
	{
		return -1;
	}
	fseek(fppcm, 4, 0);
	fwrite(&(pcmwavhdr.size_8),sizeof(pcmwavhdr.size_8), 1, fppcm);
	fseek(fppcm, 40, 0);
	fwrite(&(pcmwavhdr.data_size),sizeof(pcmwavhdr.data_size), 1, fppcm);
	fclose(fppcm);

	return 0;
}
