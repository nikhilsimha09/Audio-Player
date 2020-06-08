/*
 * CSimpleAudioPlayer.h
 */

#ifndef CSIMPLEAUDIOPLAYER_H_
#define CSIMPLEAUDIOPLAYER_H_

#include "PortAudio.h"
enum CSA_STATES{CSA_S_NOTREADY, CSA_S_READY, CSA_S_PLAYING};

class CSimpleAudioOutStream
{
	PaStream* m_stream;
	CSA_STATES m_state;

public:
	CSimpleAudioOutStream();
	~CSimpleAudioOutStream();
	void open(int numChannels,float samplerate,long maxFramesPerBuffer, bool bClippingOff=true);
	void start();
	void play(float* buf,long numFrames);
	void pause();
	void stop();
	void resume();
	void close();
};

#endif /* CSIMPLEAUDIOPLAYER_H_ */
