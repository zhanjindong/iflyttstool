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
	//ÿ�κϳ�ʱ����Ҫ����������������
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

	//д����Ƶͷ������
	fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);

	int pos = 0;			//���ڱ����һ���Ѿ��ϳɵ���λ��
	int loop_count = 0;		//���ڱ�ǣ�ȡ�˼��ν��
	int upload_flow = 0,download_flow = 0;//�ϴ���������������
	char param_value[32] = "";//����ֵ���ַ�����ʽ
	size_t value_len = 32;	//�ַ������Ȼ�buffer����

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
		pcmwavhdr.data_size += audio_len;//����pcm���ݵĴ�С
		if ( MSP_TTS_FLAG_DATA_END == synth_status )
		{
			printf("QTTSAudioGet: get end of data.\n");
			break;
		}
	}

	//����pcm�ļ�ͷ���ݵĴ�С
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;

	//��������������д���ļ�ͷ��
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
	  // Ҫ���������ļ���������ö����ƴ�   
	  filestr.open (audioFileName, ios::binary);  
	  // ��ȡfilestr��Ӧbuffer�����ָ��   
	  pbuf=filestr.rdbuf();  
    
	  // ����buffer���󷽷���ȡ�ļ���С  
	  size=pbuf->pubseekoff (0,ios::end,ios::in);  
	  pbuf->pubseekpos (0,ios::in);  
     
	  // �����ڴ�ռ�  
	  AudioBuffer=new char[size];  
	  AudioSize=size;
	  // ��ȡ�ļ�����  
	  pbuf->sgetn (AudioBuffer,size);  
	  filestr.close();  

	  return 0;
}

int TTSHelper::AppendWaveAudioFile(const char *appendWaveAudioFileName,const char *destAudioFileName,bool isCreateDestFile)
{
	long size; 
	//��ȡ��Ƶ������
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

	FILE* fp = fopen(destAudioFileName, "ab");//�ļ�ĩβ׷�Ӷ�����
	if(fp==NULL)
	{
		return -1;
	}
	//����ǲ���һ�����ļ�Ȼ��׷����Ƶ��������д��pcmͷ����Ϣ
	if(isCreateDestFile)
	{
		//д����Ƶͷ������
		fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);
	}

	fwrite(buffer, 1,size, fp);

	delete buffer;
	fclose(fp);

	//����pcm
	pcmwavhdr.data_size +=size;
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;
	//��������������д���ļ�ͷ��
	FILE* fppcm = fopen(destAudioFileName, "rb+");//������׷��
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
	FILE* fp = fopen(destAudioFileName, "ab");//�ļ�ĩβ׷�Ӷ�����
	if(fp==NULL)
	{
		return -1;
	}
	//����ǲ���һ�����ļ�Ȼ��׷����Ƶ��������д��pcmͷ����Ϣ
	if(isCreateDestFile)
	{
		//д����Ƶͷ������
		fwrite(&pcmwavhdr, 1, sizeof(pcmwavhdr), fp);
	}

	//ȥ��׷�ӵ���Ƶ��ͷ��Ϣ
	audioBufferSize-=sizeof(wave_pcm_hdr);
	appendWaveAudioBuffer+=sizeof(wave_pcm_hdr);

	fwrite(appendWaveAudioBuffer, 1,audioBufferSize, fp);
	fclose(fp);

	//����pcm
	pcmwavhdr.data_size +=audioBufferSize;
	pcmwavhdr.size_8 += pcmwavhdr.data_size + 36;
	//��������������д���ļ�ͷ��
	FILE* fppcm = fopen(destAudioFileName, "rb+");//������׷��
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
