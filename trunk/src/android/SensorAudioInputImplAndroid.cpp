/* SensorAudioInputImplAndroid.cpp
 *  Created on: Jun 22, 2014
 */

#include "SensorAudioInputImplAndroid.h"
#include "CustomPipe.h"
#include "PipesManager.h"
#include <time.h>
#include <climits>
#include "UDPSender.h"

#define TEST_ANDROID_AUDIO_SEND_DIRECT

s_uint64 now_ns(void); // definition in sensorstandardimplandroid.cpp

//SLAndroidSimpleBufferQueueState state;
//(*ai->inBufferQueue)->GetState(ai->inBufferQueue,&state);
//LOGI("Callback %llu, There are %d buffers in the queue",ai->callbacksCount,state.count);
//usleep(500000); // 20 ms sleep

void scdf::SensorAudioInputImpl::Callback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	//LOGI("----> Audio callback");

	s_uint64 now = now_ns();
	SensorAudioInputImpl* ai = (SensorAudioInputImpl*)context;
    ai->callbacksCount++;

	SensorData *s=(SensorData*) thePipesManager()->ReadFromReturnPipe(AudioInput); // data buffer size decided in setup

    s->type = AudioInput;
    s->num_frames = ai->framesPerBuffer;
    s->num_channels = ai->inputOpenSLFormat.numChannels;
    s->rate = (s_int32)(ai->inputOpenSLFormat.samplesPerSec/1000); // opensl sr is in millihertz!

    s_double buff_len_ns = (((s_double)ai->framesPerBuffer) / s->rate) * 1000000000.0;
    s_uint64 buff_len_ns_int = (s_uint64) buff_len_ns;

    s_uint64 timeFromLastCallback = now - ai->lastCallbackTime;

    std::string caseIs;
    s_double ratio = timeFromLastCallback / buff_len_ns;
    s_int32 ratio_rounded = (s_int32) (ratio+0.5);

    if (ratio_rounded == 1) { // normal situation

    	s->timeid = ai->lastCallbackTime;
    	s->timestamp[0] = now;
    	caseIs = "normal";

    } else if ( ratio_rounded == 0 && ai->nGroupedCallbacks > 0) {

    	s->timeid = now - (ai->nGroupedCallbacks * buff_len_ns_int);
    	s->timestamp[0] = s->timeid + buff_len_ns_int;

    	if (s->timestamp[0] > now)
    		s->timestamp[0] = now;

    	ai->nGroupedCallbacks -= 1;
    	ai->lastCallbackWasGrouped = true;
    	caseIs = "grouped";

    } else if ( ratio_rounded >= 2) { // the first of n grouped callbacks!

    	if (ai->nGroupedCallbacks==0 || ai->lastCallbackWasGrouped) {

    		s->timeid = now - (ratio_rounded * buff_len_ns);
    		s->timestamp[0] = s->timeid + buff_len_ns_int;
    		ai->nGroupedCallbacks = ratio_rounded - 1; // one "consumed" right now
    		caseIs = "first of grouped";
    		ai->lastCallbackWasGrouped = false;

   		} else { // there has been a delay in a grouped callback!

    		s->timeid = ai->lastTimestamp + buff_len_ns_int;
    		s->timestamp[0] = s->timeid + buff_len_ns_int;
    		ai->nGroupedCallbacks -= 1;

    		if (s->timestamp[0] > now)
   	    		s->timestamp[0] = now;

    		caseIs = "delayed grouped";
   		}

    } else { // ratio_rounded == 0 but no ngrouped callbacks!

    	// this is a callback coming after a delayed callback
    	s->timestamp[0] = now;
    	s->timeid = now - buff_len_ns_int;

    	if (s->timeid < ai->lastTimestamp) // avoid buffer time spans overlapping
    		s->timeid = ai->lastTimestamp;

    	caseIs = "after delayed";
    	ai->lastCallbackWasGrouped = false;
    }

    ai->lastTimestamp = s->timestamp[0];
    ai->lastTimeId = s->timeid;
    ai->lastCallbackTime = now;

    /*static int countz = 0;
    countz++;
    if (countz>=8) {
    LOGI("AUDIO - %s [last: %lld ms ago, ratio: %f, nGrouped: %d]",
    		caseIs.c_str(),timeFromLastCallback/1000000,ratio,ai->nGroupedCallbacks);
    	countz = 0;
    }*/


    // convert to floating point and copy audio data:
    s_sample scale = ((s_sample)(1.0))/((s_sample)(-SHRT_MIN));
    int bufferSize = ai->GetBufferSize();
    for (int i=0; i<bufferSize; i++)
    	s->data[i] = ai->inputBuffer[ai->currentBuff][i]*scale;

	/*s_double error;
	std::string sign;
	if (ai->lastTimestamp > now) { error = (ai->lastTimestamp - now)/1000000.0;	sign="+";}
	else { error = (now - ai->lastTimestamp)/1000000.0;	sign="-";}
	*/

	//s_double calc_bufflen = (ai->lastTimestamp - ai->lastTimeId)/1000000.0;
	//if (ai->callbacksCount==100)
	//	usleep(50000);

	ai->AddIncomingDataToQueue(s);

    (*ai->inBufferQueue)->Enqueue(ai->inBufferQueue, ai->inputBuffer[ai->currentBuff],bufferSize*sizeof(short));

    s_uint64 end = now_ns();
    s_double cb_time_ms = (end - now)/1000000.0;
    LOGI("AUDIOIN");//%llu (%f) - drift: %s%f ms (%s - %f ms)",ai->callbacksCount,cb_time_ms,sign.c_str(),error,caseIs.c_str(),calc_bufflen);

}

/*********************** TEST CALLBACK ****************************/

scdf::UDPSender testSender;
bool initDone = false;
s_char data = 'a';
std::string testIp("192.168.1.103");
s_uint64 startTime;
int lengthOccurences[20];
s_uint64 previousTime;
bool printcb = true;
s_int16 frames;
int testport = 55555;

void scdf::SensorAudioInputImpl::TestCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	if (!initDone) {
		LOGD("TestCallback - setup send - ip:%s, port:%d",testIp.c_str(),testport);
		testSender.InitEndpoints(testport,1,testIp);
		initDone = true;
		//startTime = now_ns();
	}

	SensorAudioInputImpl* ai = (SensorAudioInputImpl*)context;
	ai->callbacksCount++;
	//if (printcb)
	//	LOGD("Test audio callback %llu",ai->callbacksCount);
	s_uint64 now = now_ns();
	s_uint64 length = now - previousTime;
	s_uint64 nominal = (s_uint64)( ai->GetNumFramesPerCallback()*(1000000000.0/ai->GetRate()) );
	int i = (int)( ((double)length)/nominal + 0.5 );
	if (i>=20) i=19;
	lengthOccurences[i]++;

	if (0==ai->callbacksCount%6000) // modify module to get report sooner or later
	{
		printcb=false;
		float tot = 0;
		for (int k=0; k<20; k++)
			tot += lengthOccurences[k];
		for (int k=0; k<20; k++)
			LOGD("%d -> %f ",k,lengthOccurences[k]/tot*100);
	}

	previousTime = now;
	frames = (s_int16)ai->GetNumFramesPerCallback();
	testSender.SendData((s_char*)&frames,2,0);
	//testSender.SendData(&data,1,0);
	//data++;

	(*ai->inBufferQueue)->Enqueue(ai->inBufferQueue, ai->inputBuffer[ai->currentBuff],ai->GetBufferSize()*sizeof(short));
}


scdf::SensorAudioInputImpl::SensorAudioInputImpl()
{
	audioRecorderItf=NULL;
	recordItf=NULL;
	inBufferQueue=NULL;
	inputBuffer=NULL;

	// in case these values are asked before setup,
	// they act as "default" values:
	inputOpenSLFormat.samplesPerSec=44100000;
	inputOpenSLFormat.numChannels=1;
	framesPerBuffer = 512;

	theOpenSLEngine()->Initialize();
}

scdf::SensorAudioInputImpl::~SensorAudioInputImpl()
{
	Reset();
}

void scdf::SensorAudioInputImpl::Reset()
{
    // Destroy buffer queue audio recorder object, and invalidate all associated interfaces
    if(audioRecorderItf!= NULL) {
    	SLuint32 state;
    	(*audioRecorderItf)->GetState(audioRecorderItf,&state);
    	////LOGD("Recorder state: %d (1:unrealized,2:realized,3:suspended)",state);
    	if (state!=SL_OBJECT_STATE_UNREALIZED)
    		(*audioRecorderItf)->Destroy(audioRecorderItf);
        audioRecorderItf = NULL;
        recordItf = NULL;
        inBufferQueue = NULL;
    }

    if (inputBuffer==NULL)
    	return;
    for (int i=0; i<numBuffers; i++)
    	delete [] inputBuffer[i];
    delete [] inputBuffer;
    inputBuffer=NULL;
}


s_bool scdf::SensorAudioInputImpl::Setup(SensorSettings& settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false. but if setup succeeds with
	// modified values, return broken = false

	Reset();
	SLresult result;
	s_bool anySettingChanged = false;
	SensorAudioSettings &settingsAudio =  (SensorAudioSettings&)settings;

	LOGI("Setup Audio Input. rate %d, fpb: %d, chan %d",settingsAudio.rate,settingsAudio.bufferSize,settingsAudio.numChannels);

	numBuffers = 2;
    framesPerBuffer = settingsAudio.bufferSize;

    inputOpenSLFormat.numChannels = 1;
	if (settingsAudio.numChannels!=1) {
		LOGD("Num channels adjusted...");
		settingsAudio.numChannels=1;
		anySettingChanged = true;
	}

	inputOpenSLFormat.samplesPerSec = ConvertToOpenSLSampleRate(settingsAudio.rate);

	if (settingsAudio.rate!=((int)(inputOpenSLFormat.samplesPerSec/1000))) {
		LOGD("Rate adjusted...");
		settingsAudio.rate = inputOpenSLFormat.samplesPerSec/1000;
		anySettingChanged = true;
	}

	inputOpenSLFormat.formatType = SL_DATAFORMAT_PCM;
	inputOpenSLFormat.bitsPerSample = ConvertToOpenSLSampleFormat(16);
	inputOpenSLFormat.containerSize = inputOpenSLFormat.bitsPerSample;
	inputOpenSLFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
	inputOpenSLFormat.channelMask = SL_SPEAKER_FRONT_CENTER;

	// configure audio input source:
	SLDataLocator_IODevice ioDeviceLocator;
	ioDeviceLocator.locatorType = SL_DATALOCATOR_IODEVICE;
	ioDeviceLocator.deviceType = SL_IODEVICE_AUDIOINPUT;
	ioDeviceLocator.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
	ioDeviceLocator.device = NULL;

	SLDataSource inputSource;// = {&ioDeviceLocator, NULL};
	inputSource.pLocator = &ioDeviceLocator;
	inputSource.pFormat	= NULL; // no format for input

	// configure audio input sink:
	SLDataLocator_AndroidSimpleBufferQueue inBufferQueueLocator;
	inBufferQueueLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	inBufferQueueLocator.numBuffers = numBuffers;

	SLDataSink inputSink;
	inputSink.pLocator = &inBufferQueueLocator;
	inputSink.pFormat = &inputOpenSLFormat;

	// create audio recorder (requires the RECORD_AUDIO permission in manifest.xml):
	const SLuint32 numRecorderExplicitInterfaces = 2;
	const SLInterfaceID recorderInterfacesIDs[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,SL_IID_ANDROIDCONFIGURATION};
	const SLboolean recorderRequiredFlags[] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};

	result = (*theOpenSLEngine()->engineInterface)->CreateAudioRecorder( theOpenSLEngine()->engineInterface,
	                                                                      &audioRecorderItf,
	                                                                      &inputSource,
	                                                                      &inputSink,
	                                                                      numRecorderExplicitInterfaces,
	                                                                      recorderInterfacesIDs,
	                                                                      recorderRequiredFlags);
	if (result != SL_RESULT_SUCCESS) {
		LOGE("Error creating Audio recorder.");
		settingsAudio.broken = true;
		return false;
	}

	 // Get android specific configuration interface (can be retrieved before recorder realization):
	 SLAndroidConfigurationItf recConfig;
	 result = (*audioRecorderItf)->GetInterface(audioRecorderItf, SL_IID_ANDROIDCONFIGURATION, &recConfig);
	 // Choose a preset for the audio input:
	 SLint32 recPreset = SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION;
     if (result==SL_RESULT_SUCCESS) {
    	 result = (*recConfig)->SetConfiguration(recConfig, SL_ANDROID_KEY_RECORDING_PRESET, &recPreset, sizeof(SLint32));
	 }

     // realize the audio recorder:
	 result = (*audioRecorderItf)->Realize(audioRecorderItf, SL_BOOLEAN_FALSE);
	 if (result != SL_RESULT_SUCCESS) {
	 		LOGE("Error realizing Audio recorder.");
	 		settingsAudio.broken = true;
	 		return false;
	 }


	 // get the record interface:
	 result = (*audioRecorderItf)->GetInterface(audioRecorderItf, SL_IID_RECORD, &recordItf);
	 if (result != SL_RESULT_SUCCESS) {
	 		LOGE("Error getting record interface.");
	 		settingsAudio.broken = true;
	 		return false;
	 }

	 // get the buffer queue interface
	 result = (*audioRecorderItf)->GetInterface(audioRecorderItf, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
	                                                    &inBufferQueue);
	 if (result != SL_RESULT_SUCCESS) {
	 	LOGE("Error getting buffer queue interface.");
	 	settingsAudio.broken = true;
	 	return false;
	 }


	 // register callback on the buffer queue
#ifdef TEST_ANDROID_AUDIO_SEND_DIRECT
	 result = (*inBufferQueue)->RegisterCallback(inBufferQueue, TestCallback,this);
#else
	 result = (*inBufferQueue)->RegisterCallback(inBufferQueue, Callback,this);
#endif

	 if (result != SL_RESULT_SUCCESS) {
	 		LOGE("Error registering input audio callback.");
	 		settingsAudio.broken = true;
	 		return false;
	 }

	 inputBuffer = new short*[numBuffers];
	 for (int i=0; i<numBuffers; i++) {
		 inputBuffer[i] = new short[GetBufferSize()];
	 }

	 settingsAudio.broken = false;
	 return !anySettingChanged;
}

int scdf::SensorAudioInputImpl::GetBufferSize()
{
	return framesPerBuffer*inputOpenSLFormat.numChannels;
}

s_bool scdf::SensorAudioInputImpl::Start()
{
	SLresult result;
	SLuint32 recorderState;
	result = (*audioRecorderItf)->GetState(audioRecorderItf, &recorderState);
	if (SL_OBJECT_STATE_REALIZED != recorderState) {
		LOGE("Couldn't start input audio driver: AudioRecorder not realized!");
	    return false;
	}

	(*inBufferQueue)->Clear(inBufferQueue);
    //(*inBufferQueue)->GetState(inBufferQueue,&state);
	//LOGI("After clear, there are %d buffers in the INPUT queue",state.count);
    currentBuff=0;
    for (int i=0; i<numBuffers; i++)
	{
    	(*inBufferQueue)->Enqueue(inBufferQueue, inputBuffer[i],GetBufferSize()*sizeof(short));
	}
    currentBuff = 0;
    // set the recorder's state to rec:
	result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_RECORDING);

	startTimeReference = now_ns();
	lastCallbackTime = 	lastTimestamp = lastTimeId = startTimeReference;
	callbacksCount = 0;
	nGroupedCallbacks = 0;
	lastCallbackWasGrouped = false;
	//ReturnIfSLresultError(result,"Setting REC State");
	return true;
}

s_bool scdf::SensorAudioInputImpl::Stop()
{
	if (NULL==audioRecorderItf || NULL==recordItf || NULL == inBufferQueue)
		return false;
	SLresult result;
    result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);
    //ReturnIfSLresultError(result,"OpenSL Driver close: Input stop");
    result = (*inBufferQueue)->Clear(inBufferQueue);
    //ReturnIfSLresultError(result,"OpenSL Driver close: Input buffer queue clear");
    return true;
}

s_int32 scdf::SensorAudioInputImpl::GetRate()
{
	return (s_int32)(inputOpenSLFormat.samplesPerSec/1000);
}


s_int32 scdf::SensorAudioInputImpl::GetNumFramesPerCallback()
{
    return framesPerBuffer;
}


s_int32 scdf::SensorAudioInputImpl::GetNumChannels()
{
    return inputOpenSLFormat.numChannels;
}

/** OPENSL ENGINE METHODS */

// function called by both input and output drivers intialization methods:
// the engine might be already initialized, so we check it!

scdf::OpenSLEngine* scdf::theOpenSLEngine()
{
	static OpenSLEngine* theEngine = new OpenSLEngine();
	return theEngine;
}

s_bool scdf::OpenSLEngine::Initialize()
{
	//LOGD(__PRETTY_FUNCTION__);
	if (initialized)
		return true;

	SLresult result;
	if (NULL == objectInterface) {
        // create the engine and get its base interface:
		result = slCreateEngine(&objectInterface,
								numEngineOptions,
								engineOptions,
								numEngineExplicitInterfaces,
								engineInterfacesIDs,
								engineRequiredFlags);
		if (SL_RESULT_SUCCESS != result) {
            LOGE("Couldn't Create OpenSLES Engine.");
            return false;
        }
	}
	// realize the engine:
    SLuint32 state;
    result = (*objectInterface)->GetState(objectInterface, &state);
    if (SL_OBJECT_STATE_REALIZED != state) {
         result = (*objectInterface)->Realize(objectInterface,SL_BOOLEAN_FALSE);
         if (SL_RESULT_SUCCESS != result)
         {
             LOGE("Couldn't Realize OpenSLES Engine.");
             return false;
         }
    }
    if (NULL==engineInterface) {
		// get the SLEngineInterface of the engine:
        result = (*objectInterface)->GetInterface(objectInterface, SL_IID_ENGINE, &engineInterface);
		if (SL_RESULT_SUCCESS != result)
			LOGE("Couldn't obtain the engine's Engine Interface");
        return false;

    }
    return true;
}


void scdf::OpenSLEngine::Destroy()
{
    //LOGD(__PRETTY_FUNCTION__);
    if (initialized) {
        (*objectInterface)->Destroy(objectInterface);
        engineInterface = NULL;
        objectInterface = NULL;
        initialized=false;
    }
}


/****************************** UTILITY FUNCTIONS ****************************/


SLuint32 scdf::ConvertToOpenSLSampleRate(s_int32 insr)
{
    //LOGD(__PRETTY_FUNCTION__);

    SLuint32 sr;

    switch(insr){

        case 8000:
            sr = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            sr = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            sr = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            sr = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            sr = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            sr = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            sr = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            sr = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            sr = SL_SAMPLINGRATE_48;
            break;
        /* // following rates are not supported on android, see $NDK/docs/opensles/index.htm
        case 64000:
            sr = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            sr = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            sr = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            sr = SL_SAMPLINGRATE_192;
            break; */
        default:
            LOGE("Invalid sample rate, setting to SL_SAMPLERATE_44_1 (44.1kHz)");
            return SL_SAMPLINGRATE_44_1;
    }
    return sr;
}

SLuint16 scdf::ConvertToOpenSLSampleFormat(s_int32 sampleFormat)
{
    switch (sampleFormat) {
        case 8:
            return SL_PCMSAMPLEFORMAT_FIXED_8;
            break;
        case 16:
            return SL_PCMSAMPLEFORMAT_FIXED_16;
            break;
        default:
            LOGE("Invalid sample format, setting to SL_PCMSAMPLEFORMAT_FIXED_16 (16bits)");
            return SL_PCMSAMPLEFORMAT_FIXED_16;
    }
}


std::string scdf::StringizeSLresult(SLresult res)
{
    switch(res){

        case SL_RESULT_SUCCESS:
            return "OpenSL success";
            break;
        case SL_RESULT_PRECONDITIONS_VIOLATED:
            return "OpenSL preconditions violated";
            break;
        case SL_RESULT_PARAMETER_INVALID:
            return "OpenSL parameter Invalid";
            break;
        case SL_RESULT_MEMORY_FAILURE:
            return "OpenSL memory failure";
            break;
        case SL_RESULT_RESOURCE_ERROR:
            return "OpenSL resource error";
            break;
        case SL_RESULT_RESOURCE_LOST:
            return "OpenSL resource lost";
            break;
        case SL_RESULT_IO_ERROR:
            return "OpenSL IO error";
            break;
        case SL_RESULT_BUFFER_INSUFFICIENT:
            return "OpenSL buffer insufficient";
            break;
        case SL_RESULT_CONTENT_CORRUPTED:
            return "OpenSL result content corrupted";
            break;
        case SL_RESULT_CONTENT_UNSUPPORTED:
            return "OpenSL content unsupported";
            break;
        case SL_RESULT_CONTENT_NOT_FOUND:
            return "OpenSL content not found";
            break;
        case SL_RESULT_PERMISSION_DENIED:
            return "OpenSL permission denied";
            break;
        case SL_RESULT_FEATURE_UNSUPPORTED:
            return "OpenSL feature unsupported";
            break;
        case SL_RESULT_INTERNAL_ERROR:
            return "OpenSL internal error";
            break;
        case SL_RESULT_UNKNOWN_ERROR:
            return "OpenSL unknown error";
            break;
        case SL_RESULT_OPERATION_ABORTED:
            return "OpenSL operation aborted";
            break;
        case SL_RESULT_CONTROL_LOST:
            return "OpenSL control lost";
            break;

        default:
            return "Unknown OpenSL error";

    }
}

