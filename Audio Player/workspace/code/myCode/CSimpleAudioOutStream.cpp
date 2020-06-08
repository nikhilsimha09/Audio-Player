/*
 * CSimpleAudioPlayer.cpp
 */
#include <stdlib.h>
#include <memory.h>
#include <string>
using namespace std;

#include "CASDDException.h"
#include "CSimpleAudioOutStream.h"


CSimpleAudioOutStream::CSimpleAudioOutStream()
{
	m_stream=NULL;
	m_state=CSA_S_NOTREADY;
}

CSimpleAudioOutStream::~CSimpleAudioOutStream()
{
	close();
	Pa_Terminate();
}

void CSimpleAudioOutStream::open(int numChannels, float samplerate, long maxFramesPerBuffer, bool bClippingOff)
{
	if(m_state == CSA_S_NOTREADY)
	{
		// channels and sample rate are handled by the portaudio library
		if(maxFramesPerBuffer <= 0)throw(CASDDException(SRC_SimpleAudioDevice, paNoError, "Frames per buffer must be > 0."));

		PaError err=paNoError;
		err= Pa_Initialize();
		if(err != paNoError)throw(CASDDException(SRC_SimpleAudioDevice, err, Pa_GetErrorText(err)));

		PaStreamParameters outParams;
		outParams.device = Pa_GetDefaultOutputDevice(); /* default output device */
		if (outParams.device == paNoDevice)
			throw(CASDDException(SRC_SimpleAudioDevice, paNoDevice, Pa_GetErrorText(paNoDevice)));
		outParams.channelCount = numChannels;
		outParams.sampleFormat = paFloat32; /* 32 bit floating point output */
		// Default latency values for robust non-interactive applications (eg. playing sound files).
		outParams.suggestedLatency = Pa_GetDeviceInfo( outParams.device )->defaultHighOutputLatency;
		outParams.hostApiSpecificStreamInfo = NULL;

		PaStreamFlags flags;
		flags=(bClippingOff == true)?paClipOff:paNoFlag;
		err = Pa_OpenStream(&m_stream, NULL, /* no input */&outParams, samplerate, maxFramesPerBuffer,
							flags,			/* in case of filtering out of range samples may be possible)*/
							NULL, 			/* no callback, use blocking API */
							NULL ); 		/* no callback, so no callback userData */
		if(err != paNoError)throw(CASDDException(SRC_SimpleAudioDevice, err, Pa_GetErrorText(err)));

		m_state=CSA_S_READY;
	}
}

void CSimpleAudioOutStream::play(float* buf, long numFrames)
{
	PaError err= paNoError;
	if(m_state == CSA_S_NOTREADY)throw(CASDDException(SRC_SimpleAudioDevice, paNoError, "Device is not initialized!"));
	if(m_state == CSA_S_PLAYING)
	{
		err = Pa_WriteStream( m_stream, buf, numFrames);
		if( err != paNoError )throw(CASDDException(SRC_SimpleAudioDevice, err, Pa_GetErrorText(err)));
	}
}

void CSimpleAudioOutStream::start()
{
	PaError err= paNoError;
	if(m_state == CSA_S_NOTREADY)throw(CASDDException(SRC_SimpleAudioDevice, paNoError, "Device is not initialized!"));
	if(m_state == CSA_S_READY)
	{
		err= Pa_StartStream(m_stream);
		if(err != paNoError)throw(CASDDException(SRC_SimpleAudioDevice, err, Pa_GetErrorText(err)));
		else
			m_state= CSA_S_PLAYING;
	}
}

void CSimpleAudioOutStream::stop()
{
	PaError err;
	if(m_state == CSA_S_NOTREADY)throw(CASDDException(SRC_SimpleAudioDevice, paNoError, "Device is not initialized!"));
	if(m_state == CSA_S_PLAYING)
	{
		err=Pa_StopStream(m_stream);
		if(err != paNoError)throw(CASDDException(SRC_SimpleAudioDevice, err, Pa_GetErrorText(err)));
		else
			m_state= CSA_S_READY;
	}
}

void CSimpleAudioOutStream::resume()
{
	start();
}


void CSimpleAudioOutStream::close()
{
	if(m_state == CSA_S_NOTREADY)return;
	stop();
	Pa_CloseStream(m_stream);
	m_state= CSA_S_NOTREADY;
}

void CSimpleAudioOutStream::pause()
{
	stop();
}
