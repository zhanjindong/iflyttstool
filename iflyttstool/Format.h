//---------------------------------------------------------------------------
#ifndef Format_H
#define Format_H
//---------------------------------------------------------------------------
struct FormatChunk
{
  char            chunkID[4];
  long            chunkSize;

  short           wFormatTag;
  unsigned short  wChannels;
  unsigned long   dwSamplesPerSec;
  unsigned long   dwAvgBytesPerSec;
  unsigned short  wBlockAlign;
  unsigned short  wBitsPerSample;

// Note: there may be additional fields here, depending upon wFormatTag.

};

// This is the start ID of a Wave file
// must contains 'RIFF' and 'WAVE'
char startID[12];

// contains the chunk id ('data', 'cue ' ...) and the chunk size
struct Chunk
{
  char            chunkID[4];
  long            chunkSize;
};

// a pointer to the samples in the data chunk
unsigned char     *WaveformData;

//---------------------------------------------------------------------------
#endif