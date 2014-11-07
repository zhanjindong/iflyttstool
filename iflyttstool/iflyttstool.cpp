// tecentttstool.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <string>
#include "../include/qtts.h"
#include <conio.h>
#include <iostream>  
#include <fstream>
#include "get_config.h"
#include "wavehead.h"
#include "ttsconfig.h"
#include "WAVE_CLASS.h"
#include "TTSHELPER_CLASS.h"
#include "FileUtility_CLASS.h"
#include <time.h>
#include <sstream>
#include "errorcode.h"
#include "HttpHelper.h"
#include "stringtonum.h"

#pragma comment(lib,"../lib/msc.lib")

//默认音频头部数据
wave_pcm_hdr default_pcmwavhdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

static string LogPath;
static string CallBackHost;
static string CallBackPort;
static string CallBackApi;
static string BaseDownUrl;
static string Cid;

//音频合成
int Synthesis(ttsconfig ttsconf,const char *textfname,const char *synthfname,string &errorinfo)
{
	TTSHelper tts(ttsconf,default_pcmwavhdr);
	int ret;
	//添加头部音频
	string begintext=FileUtility::ReadFile("../res/begin.txt",&ret);
	if(ret!=0)
	{
		errorinfo="can not open file：../res/begin.txt.";
		return ret;
	}
	ret=tts.CreateWaveAudioFile(begintext.data(),synthfname);
	if(ret!=0)
	{
		errorinfo=tts.LastErrorInfo;
		return ret;
	}
	ret=tts.AppendWaveAudioFile("../res/叮咚声.wav",synthfname,false);
	if(ret!=0)
	{
		errorinfo=tts.LastErrorInfo;
		return ret;
	}

	TTSHelper tts2(ttsconf,default_pcmwavhdr);
	//合成新闻音频
	ifstream out;
	out.open(textfname,ios::in);
	string line;
	while(!out.eof())
	{
		getline(out,line);
		if(line!="")
		{
			ret=tts2.CreateWaveAudioFile(line.data(),"tmp.wav");
			if(ret!=0)
			{
				errorinfo=tts2.LastErrorInfo;
				return ret;
			}
			ret=tts.AppendWaveAudioBuffer(tts2.AudioBuffer,tts2.AudioSize,synthfname,false);
			if(ret!=0)
			{
				errorinfo=tts.LastErrorInfo;
				return ret;
			}
		}
		else
		{
			ret=tts.AppendWaveAudioFile("../res/叮咚声.wav",synthfname,false);
			if(ret!=0)
			{
				errorinfo=tts.LastErrorInfo;
				return ret;
			}
		}
	}
	out.close();

	ret=tts.AppendWaveAudioFile("../res/叮咚声.wav",synthfname,false);
	if(ret!=0)
	{
		errorinfo=tts.LastErrorInfo;
		return ret;
	}

	//添加尾部音频
	string endtext=FileUtility::ReadFile("../res/end.txt",&ret);
	if(ret!=0)
	{
		errorinfo=errorinfo="can not open file：../res/end.txt.";
		return ret;
	}
	ret=tts2.CreateWaveAudioFile(endtext.data(),"tmp.wav");
	if(ret!=0)
	{
		errorinfo=tts2.LastErrorInfo;
		return ret;
	}
	ret=tts.AppendWaveAudioBuffer(tts2.AudioBuffer,tts2.AudioSize,synthfname,false);
	if(ret!=0)
	{
		errorinfo=tts.LastErrorInfo;
		return ret;
	}
	/*ret=tts.AppendWaveAudioFile("../res/叮咚声.wav",synthfname,false);
	if(ret!=0)
	{
		errorinfo=tts.LastErrorInfo;
		return ret;
	}*/
	return 0;
}

//音频转码：wave2mp3
int Transcoding(const char *sourcefname,const char *destfname,string &errorinfo)
{
	WAVE_CLASS wave;
	int ret=wave.WaveToMP3(sourcefname,destfname);
	if(ret!=0)
	{
		errorinfo=wave.LastErrorInfo;
		return ret;
	}
	return 0;
}

//合成并转码
int SynthAndTranscoding(ttsconfig ttsconf, const char *textfname,const char *synthfname,const char *destfname,string &errorinfo)
{
	int ret=0;
	ret=Synthesis(ttsconf,textfname,synthfname,errorinfo);
	if(ret!=0)
	{
		return CUST_ERROR_AUDIO_SYNTHESIS_FAILED;
	}

	ret=Transcoding(synthfname,destfname,errorinfo);
	if(ret!=0)
	{
		return CUST_ERROR_AUDIO_TRANSCODING_FAILED;
	}

	return ret;
}

//-s:合成，-t:转码.-st合成同时转码
int _tmain(int argc, _TCHAR* argv[])
{
	//初始化配置信息
	map<string,string> configs;
	ReadConfig("../conf/config.cfg",configs);
	LogPath=configs["logpath"];
	ttsconfig ttsconf;
	ttsconf.m_configs=configs["m_configs"].data();
	ttsconf.szParam=configs["szParam"].data();

	if(argc<=1)
	{
		printf("-t or -r parameter is missing.\n");
		FileUtility::WriteLogToFlie(LogPath,"-t or-r parameter is missing.\n");
		return 0;
	}
	//audio synthesis
	if(string(argv[1])=="-s")
	{
		//text path
		const char *textfname=argv[2];
		if(textfname==NULL)
		{
			printf("audio synthesis:text filename is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis:filename is missing.\n");
			return CUST_ERROR_AUDIO_SYNTHESIS_TEXTFILENAME_MISSING;
		}
		//synthesis result path
		const char *synthfname=argv[3];
		if(synthfname==NULL)
		{
			printf("audio synthesis:synthesis result path is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis:synthesis result filename is missing.\n");
			return CUST_ERROR_AUDIO_SYNTHESIS_RESULTFILENAME_MISSING;
		}

		string errorinfo;
		int ret=Synthesis(ttsconf,textfname,synthfname,errorinfo);
		if(ret!=0)
		{
			printf("audio synthesis:failed!\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis:failed! "+errorinfo+"\n");
			return CUST_ERROR_AUDIO_SYNTHESIS_FAILED;
		}

		printf("audio synthesis:success!\n");
		FileUtility::WriteLogToFlie(LogPath,"audio synthesis:success!\n");
	}

	//audio transcoding
	if(string(argv[1])=="-t")
	{
		//source filename
		const char *sourcefname=argv[2];
		if(sourcefname==NULL)
		{
			printf("audio transcoding:source filename is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio transcoding:source filename is missing.\n");
			return CUST_ERROR_AUDIO_TRANSCODING_SOURCEFILENAME_MISSING;
		}
		//dest filename
		const char *destfname=argv[3];
		if(destfname==NULL)
		{
			printf("audio transcoding:dest filename is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio transcoding:dest filename is missing.\n");
			return CUST_ERROR_AUDIO_TRANSCODING_DESTFILENAME_MISSING;
		}

		string errorinfo;
		int ret=Transcoding(sourcefname,destfname,errorinfo);
		if(ret!=0)
		{
			printf("audio transcoding:failed!\n");
			FileUtility::WriteLogToFlie(LogPath,"audio transcoding:failed! "+errorinfo+"\n");
			return CUST_ERROR_AUDIO_TRANSCODING_FAILED;
		}

		printf("audio transcoding:success!\n");
		FileUtility::WriteLogToFlie(LogPath,"audio transcoding:success!\n");
	}

	//synthesis and transcoding
	if (string(argv[1])=="-st")
	{
		int ret=0;
		//text path
		const char *textfname=argv[2];
		if(textfname==NULL)
		{
			printf("audio synthesis and transcoding:text filename is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis and transcoding:filename is missing.\n");
			return CUST_ERROR_AUDIO_SYNTHESIS_TEXTFILENAME_MISSING;
		}
		//synthesis result path
		const char *synthfname=argv[3];
		if(synthfname==NULL)
		{
			printf("audio synthesis and transcoding:synthesis result path is missing.\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis and transcoding:synthesis result filename is missing.\n");
			return CUST_ERROR_AUDIO_SYNTHESIS_RESULTFILENAME_MISSING;
		}
		const char *destfname=argv[4];
		string tmp;
		if (destfname==NULL)
		{
			string synthfnameStr=string(synthfname);
			tmp=synthfnameStr.replace(synthfnameStr.length()-3,synthfnameStr.length(),"mp3");
			destfname=tmp.data();
		}

		string errorinfo;
		ret=SynthAndTranscoding(ttsconf,textfname,synthfname,destfname,errorinfo);

		if(ret!=0)
		{
			printf("audio synthesis and transcoding:failed!\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis and transcoding:failed! "+errorinfo+"\n");
		}
		else
		{
			printf("audio synthesis and transcoding:success!\n");
			FileUtility::WriteLogToFlie(LogPath,"audio synthesis and transcoding:success!\n");
		}

		return ret;
	}
	
	return 0;
}


