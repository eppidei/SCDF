/*
 * SensorAudioInputImplAndroid.h
 *
 *  Created on: Jun 22, 2014
 *      Author: athos
 */

#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "SensorAudioInput.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>



namespace scdf {



    class SensorAudioInputImpl : public SensorAudioInput
    {

    public:

        SensorAudioInputImpl();
        ~SensorAudioInputImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();
        s_int32 GetRate();
        s_int32 GetNumFramesPerCallback();
        s_int32 GetNumChannels();

    private:

        static void Callback(SLAndroidSimpleBufferQueueItf bq, void *context);
        static void TestCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

        void Reset();
        int GetBufferSize();

        // OpenSL interfaces:
        SLObjectItf audioRecorderItf;
        SLRecordItf recordItf;
        SLAndroidSimpleBufferQueueItf inBufferQueue;

        // Audio properties:
        SLDataFormat_PCM inputOpenSLFormat;

        // data structures and indexes:
        short** inputBuffer;
        int numBuffers;
        s_int32 framesPerBuffer;
        int currentBuff;

        s_int16 nGroupedCallbacks;
        s_uint64 startTimeReference;
        s_uint64 callbacksCount;
        s_uint64 lastCallbackTime;
        s_uint64 lastTimestamp;
        s_uint64 lastTimeId;
        s_bool	 lastCallbackWasGrouped;
    };

    SLuint32 ConvertToOpenSLSampleRate(s_int32 insr);
    SLuint16 ConvertToOpenSLSampleFormat(s_int32 sampleFormat);
    std::string StringizeSLresult(SLresult res);

    // the engine is shared between input and output, and it's a singleton:
    class OpenSLEngine;
    OpenSLEngine* theOpenSLEngine();

    class OpenSLEngine {

    	friend OpenSLEngine* theOpenSLEngine();
    	s_bool initialized;

    public:

    	s_bool Initialize();
    	void Destroy();

    	SLObjectItf objectInterface;		// base interface
    	SLEngineItf engineInterface;		// "engine" interface
    	SLuint32 numEngineOptions;         // number of optional features
    	SLEngineOption *engineOptions;     // array of optional features
    	SLuint32 numEngineExplicitInterfaces;	// number of explicit interfaces to retrieve
    	SLInterfaceID *engineInterfacesIDs;	// ids of the explicit interfaces
    	SLboolean *engineRequiredFlags;		// array of flags indicating required interfaces

    private:

    	OpenSLEngine()
    	{
    		// ENGINE OPTIONS
    		// interfaces are invalid before calling appropriate functions:
    		objectInterface = NULL;
    		engineInterface = NULL;
    		// no optional features needed by default:
    		numEngineOptions = 0;
    		engineOptions = NULL;
    		// no explicit interfaces needed by default:
    		numEngineExplicitInterfaces = 0;
    		engineInterfacesIDs = NULL;
    		engineRequiredFlags = NULL;
    		initialized=false;
    	}
    };

}




#endif /* SENSORAUDIOINPUTIMPLANDROID_H_ */
