/* SensorAudioInputImplAndroid.cpp
 *  Created on: Jun 22, 2014
 */

#include "SensorAudioInputImplAndroid.h"
#include "CustomPipe.h"
#include "PipesManager.h"
#include <time.h>
#include <climits>

s_uint64 now_ns(void); // definition in sensorstandardimplandroid.cpp
/*{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (s_uint64)(1000 * res.tv_sec) + (s_uint64) res.tv_nsec;
}*/

void scdf::SensorAudioInputImpl::Callback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	//LOGD("Opensl input callback");
	SensorAudioInputImpl* ai = (SensorAudioInputImpl*)context;

    SensorAudioData *s=(SensorAudioData*) thePipesManager()->ReadFromReturnPipe(AudioInput);
    // the size of the data buffer is decided in the sensor setup.

	#ifdef LOG_PIPES_STATUS
    LOGD("Return pipe size of %s: %d\n", SensorTypeString[AudioInput].c_str(), (*(GetReturnPipes()))[AudioInput]->GetSize());
	#endif


#ifdef LOG_TIMESTAMP
    s_uint64 timestampInterval=inTimeStamp->mHostTime-mach_absolute_time();
    printf("AUDIO CALLBACK TIMESTAMPS DIFF: %llu\n",timestampInterval);
    printf("AUDIO CALLBACK TIMESTAMPS DIFF MS: %llu\n",getUptimeInMilliseconds(timestampInterval));
#endif

    // fill with data info
    s->type = AudioInput;
    s->num_samples = ai->bufferSize;
    s->numChannels = ai->inputOpenSLFormat.numChannels;
    s->rate = (s_int32)ai->inputOpenSLFormat.samplesPerSec/1000; // opensl sr is in millihertz!
    s->timeid = now_ns();
    s_double samplinginterval_nanosec = ((s_double)1e9)/s->rate;
    s_uint64 num_frames = s->num_samples/s->numChannels;
    s->timestamp = s->timeid + (s_uint64)( num_frames * samplinginterval_nanosec );

    // convert to floating point and copy audio data:
    s_sample scale = ((s_sample)(1.0))/((s_sample)(-SHRT_MIN));
    for (int i=0; i<ai->bufferSize; i++)
       	s->data[i] = ai->inputBuffer[ai->currentBuff][i]*scale;

    ai->AddIncomingDataToQueue(s);

    // re-enqueue the buffer:
    (*ai->inBufferQueue)->Enqueue(ai->inBufferQueue, ai->inputBuffer[ai->currentBuff],ai->bufferSize*sizeof(short));
}

scdf::SensorAudioInputImpl::SensorAudioInputImpl()
{

	 audioRecorderItf=NULL;
	 recordItf=NULL;
	 inBufferQueue=NULL;
	 inputBuffer=NULL;
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
    // modify its value in settings and return false
    // try starting the sensor, return result

	Reset();
	// OBS: streamingEvents and numbuff not used.
	SLresult result;
	s_bool anySettingChanged = false;

	numBuffers = 2;
    bufferSize = 1024;
	inputOpenSLFormat.numChannels = 1;
	inputOpenSLFormat.samplesPerSec = ConvertToOpenSLSampleRate(44100);
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
	 // ThrowIfSLresultError(result,"Can't create android audio recorder");

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
	 //ThrowIfSLresultError(result,"Can't initialize android audio recorder");
	 // get the record interface:
	 result = (*audioRecorderItf)->GetInterface(audioRecorderItf, SL_IID_RECORD, &recordItf);
	 //ThrowIfSLresultError(result,"Can't get android recorder interface");

	 // get the buffer queue interface
	 result = (*audioRecorderItf)->GetInterface(audioRecorderItf, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
	                                                    &inBufferQueue);
	 //ThrowIfSLresultError(result,"Can't get android input buffer queue");

	 // register callback on the buffer queue
	 result = (*inBufferQueue)->RegisterCallback(inBufferQueue, Callback,this);
	 //ThrowIfSLresultError(result,"Can't setup android input callback");

	 inputBuffer = new short*[numBuffers];
	 for (int i=0; i<numBuffers; i++) {
		 inputBuffer[i] = new short[bufferSize/**sizeof(short)*/];
	 }
	 return true;
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
    	(*inBufferQueue)->Enqueue(inBufferQueue, inputBuffer[i],bufferSize*sizeof(short));
	}
    currentBuff = 0;
    // set the recorder's state to rec:
	result = (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_RECORDING);
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

s_int32 scdf::SensorAudioInputImpl::GetNumSamples()
{
	return (s_int32)(bufferSize);
}

/** OPENSL ENGINE METHODS */

// function called by both input and output drivers intialization methods:
// so  the engine might be already initialized: check it!

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
        /* // apparently, following rates are not supported on android
           // see $NDK/docs/opensles/index.htm
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
            LOGD("Invalid sample rate, setting to SL_SAMPLERATE_44_1 (44.1kHz)");
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


