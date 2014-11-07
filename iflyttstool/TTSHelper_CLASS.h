#ifndef __TTSHELPER_H__
#define __TTSHELPER_H__

#include "ttsconfig.h"
#include "wavehead.h"

class TTSHelper{
public:
	TTSHelper(ttsconfig &ttsconf, wave_pcm_hdr &wph);
	~TTSHelper();

public:
	int CreateWaveAudioFile(const char *szText,const char *audioFileName);
	int AppendWaveAudioFile(const char *appendWaveAudioFileName,const char *destAudioFileName,bool isCreateDestFile);
	int AppendWaveAudioBuffer(const char *appendWaveAudioBuffer,int audioBufferSize,const char *destAudioFileName,bool isCreateDestFile);

	char *AudioBuffer;
	int AudioSize;
	string LastErrorInfo;

private:
	ttsconfig ttsconf;
	wave_pcm_hdr pcmwavhdr;

};
#endif