#ifndef __WAVE_CLASS_H__
#define __WAVE_CLASS_H__

#include "stdafx.h"

class WAVE_CLASS{
public:
	WAVE_CLASS(void);
	~WAVE_CLASS(void);
	int WaveToMP3(const char *wavefname,const char *mp3savefname);

	string LastErrorInfo;
};
#endif