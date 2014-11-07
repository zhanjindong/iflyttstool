#include "StdAfx.h"
#include "WAVE_CLASS.h"
#include "errorcode.h"

WAVE_CLASS::WAVE_CLASS()
{
}

WAVE_CLASS::~WAVE_CLASS()
{
}

int WAVE_CLASS::WaveToMP3(const char *wavefname,const char *mp3savefname)
{
	FILE *fp,*fout;
	char startID[12];

	fp=fopen(wavefname,"rb");
	if (fp==NULL)
	{
		LastErrorInfo="Failed to open file: "+string(wavefname);
		return CUST_ERROR_OPENFILEFAIL;
	}

	//read the 12 character in front of the file
	fread(startID,sizeof(startID),1,fp);

	//get the format chunk
	FormatChunk fc;
	fread(&fc,sizeof(FormatChunk),1,fp);
	// the first chunk MUST be the format chunk
	if(strncmp(fc.chunkID,"fmt ",4)!=0)
	{
		LastErrorInfo="This is not a valid Wave file£º"+string(wavefname);
		return CUST_ERROR_INVALID_WAVEFILE;
	}
	if(fc.wFormatTag!=1)
	{
	/*	MessageBox(hWnd,"Cannot handle compressed Wave file",
			"Wav2Mp3 ERROR",MB_OK);*/
		return CUST_ERROR_NOT_HANDLE_COMPRESSED_WAVEFILE;
	}

	// initialization of Mp3 encoder
	BE_CONFIG bc;
	bc.dwConfig=BE_CONFIG_MP3;
	//32000, 44100 and 48000 are the only sample rate authorized
	//due to encoding limitations
	if(fc.dwSamplesPerSec==32000 || fc.dwSamplesPerSec==44100 ||
		fc.dwSamplesPerSec==48000||fc.dwSamplesPerSec==16000)
		bc.format.mp3.dwSampleRate=fc.dwSamplesPerSec;
	else
	{
		LastErrorInfo="Unsuported sample rate£º"+fc.dwSamplesPerSec;
		return CUST_ERROR_NOT_UNSUPORT_SAMPLE_RATE;
	}
	if(fc.wChannels==1)bc.format.mp3.byMode=BE_MP3_MODE_MONO;
	else bc.format.mp3.byMode=BE_MP3_MODE_STEREO;
	//the resulting file length depends on this parameter
	//higher the bitrate, better the result
	bc.format.mp3.wBitrate=192;
	bc.format.mp3.bCopyright=false;
	bc.format.mp3.bCRC=false;
	bc.format.mp3.bOriginal=false;
	bc.format.mp3.bPrivate=false;
	//skip extra formatchunk parameter, if any
	if(sizeof(FormatChunk)<8+fc.chunkSize)
	{
		char c;
		for(int i=0;i<8+fc.chunkSize-sizeof(FormatChunk);i++)
			fread(&c,1,1,fp);
	}

	//get next chunk
	Chunk chunk;
	fread(&chunk,sizeof(Chunk),1,fp);
	//check if it's the data chunk
	while(strncmp(chunk.chunkID,"data",4)!=0)
	{
		char c;
		for(int i=0;i<chunk.chunkSize;i++) fread(&c,1,1,fp);
		fread(&chunk,sizeof(Chunk),1,fp);
	}

	//process with the encoding
	DWORD dwNumberOfSamples;
	DWORD dwOutputBufferLength;
	HBE_STREAM hStream;

	HINSTANCE hInstance = LoadLibrary("../lib/LAME_ENC.DLL");

	typedef BE_ERR (*BeInitStream)(BE_CONFIG *beConfig, PDWORD dwSamples, PDWORD dwBufferSize, PHBE_STREAM phbeStream);
	BeInitStream beInitStreamDll=(BeInitStream)GetProcAddress(hInstance, "beInitStream");
	typedef BE_ERR	(*BeEncodeChunk)(HBE_STREAM hbeStream, DWORD nSamples, PSHORT pSamples, PBYTE pOutput, PDWORD pdwOutput);
	BeEncodeChunk beEncodeChunkDll=(BeEncodeChunk)GetProcAddress(hInstance, "beEncodeChunk");
	typedef BE_ERR	(*BeDeinitStream)(HBE_STREAM hbeStream, PBYTE pOutput, PDWORD pdwOutput);
	BeDeinitStream beDeinitStreamDll=(BeDeinitStream)GetProcAddress(hInstance, "beDeinitStream");
	typedef BE_ERR	(*BeCloseStream)(HBE_STREAM hbeStream);
	BeCloseStream beCloseStreamDll=(BeCloseStream)GetProcAddress(hInstance, "beCloseStream");

	if(beInitStreamDll(&bc,&dwNumberOfSamples,&dwOutputBufferLength,&hStream)
		!=BE_ERR_SUCCESSFUL)
	{
		LastErrorInfo="Cannot perform compression.";
		return CUST_ERROR_NOT_HANDLE_COMPRESSED_WAVEFILE;
	}

	fout=fopen(mp3savefname,"wb");
	if (fout==NULL)
	{
		LastErrorInfo="Failed to open file: "+string(mp3savefname);
		return CUST_ERROR_OPENFILEFAIL;
	}

	char *Mp3Buffer=new char[dwOutputBufferLength];
	SHORT *InputBuffer=new SHORT[dwNumberOfSamples];      //SHORT=short=16 bits
	int SamplesPerformed=0;
	DWORD dwNumberOfSamplesEncoded;

	while(SamplesPerformed<chunk.chunkSize)
	{
		fread(InputBuffer,dwNumberOfSamples*2,1,fp);
		SamplesPerformed+=dwNumberOfSamples*2;
		if(beEncodeChunkDll(hStream,dwNumberOfSamples,InputBuffer,
			(BYTE*)Mp3Buffer,&dwNumberOfSamplesEncoded)!=BE_ERR_SUCCESSFUL)
		{
			LastErrorInfo="Cannot perform compression.";
			return CUST_ERROR_NOT_HANDLE_COMPRESSED_WAVEFILE;
		}
		fwrite(Mp3Buffer,dwNumberOfSamplesEncoded,1,fout);
	}

	beDeinitStreamDll(hStream,(BYTE*)Mp3Buffer,&dwNumberOfSamplesEncoded);
	beCloseStreamDll(hStream);

	delete Mp3Buffer;
	delete InputBuffer;
	fclose(fp);
	fclose(fout);

	return 0;
}