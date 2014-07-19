#include "SensorStandardImplAndroid.h"
#include "CustomPipe.h"
#include "PipesManager.h"
#include <android/sensor.h>
#include <android/looper.h>


/*
about sensors' timestamps references on different devices,
and across different sensors on the same device:
https://code.google.com/p/android/issues/detail?id=56561
in short... they are not guaranteed to be consistent except for the same sensor!
*/

#include <time.h>
s_uint64 now_ns(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    s_uint64 now = (s_uint64)(1000000000 * (s_uint64)res.tv_sec) + (s_uint64) res.tv_nsec;
    //LOGD("GETTIME %llu",now);
    return now;
}

int scdf::SensorStandardImpl::Callback(int fd, int events, void* data)
{
	//LOGD("Sensor callback");
	scdf::SensorStandardImpl* s = (scdf::SensorStandardImpl*)data;
	ASensorEvent event;
    while (ASensorEventQueue_getEvents(s->sensorEventQueue, &event, 1) > 0)
    {
    	LOGD("   Sensor %d", s->GetType());
    	//LOGD("   Sensor %d : %f  %f  %f", s->GetType(),event.acceleration.x, event.acceleration.y, event.acceleration.z);
    	SensorData *sd=thePipesManager()->ReadFromReturnPipe(s->GetType());
   	    //LOGD("Return pipe size of %s: %d\n", SensorTypeString[sensorTypeRef].c_str(), (*(GetReturnPipes()))[sensorTypeRef]->GetSize());

   	    sd->type = s->GetType();
   	    sd->timeid = now_ns();
   	   	sd->timestamp = sd->timeid;
   	   	sd->rate = s->currentRate;
   	   	sd->numChannels = 1;

   	   	switch(event.type)
   	   	{
   	   		case ASENSOR_TYPE_ACCELEROMETER:
   	    	    sd->num_samples=3;
   	    	    sd->data[0]  = event.acceleration.x;
   	    	    sd->data[1]  = event.acceleration.y;
   	    	    sd->data[2]  = event.acceleration.z;
   	 			break;
   	 		case ASENSOR_TYPE_MAGNETIC_FIELD:
   	 			sd->num_samples=3;
   	 			sd->data[0]  = event.magnetic.x;
   	 		   	sd->data[1]  = event.magnetic.y;
   	 		   	sd->data[2]  = event.magnetic.z;
   	 		   	break;
   	 		case ASENSOR_TYPE_PROXIMITY:
   	 			sd->num_samples=1;
   	 		   	sd->data[0] = event.distance;
   	 		   	break;
   	 		case ASENSOR_TYPE_GYROSCOPE:
   	 			sd->num_samples=3;
   	 			sd->data[0]  = event.vector.x;
   	 		   	sd->data[1]  = event.vector.y;
   	 		   	sd->data[2]  = event.vector.z;
   	 		   	break;
   	 		case ASENSOR_TYPE_LIGHT:
   	 			sd->num_samples=1;
   	 	   	 	sd->data[0] = event.light;
   	 	   	 	break;
   	 		default:
   	 			LOGE("Sensor constructor error: invalid sensor type!");
   	 			break;
   	   	} if (NULL==sd){
   	    	sd = new scdf::SensorData();
   	    	sd->data=(s_sample*) new s_sample[3];
   	    }

   	   	/*static long count = 0;
   	   	LOGD("SENSOR callback %ld",count);
   	   	count++;*/

   	    s->AddIncomingDataToQueue(sd);
    }
    return 1;
}

int scdf::SensorStandardImpl::ScdfToAndroidType(SensorType scdfType)
{
	switch (scdfType) {
		case Accelerometer:	return ASENSOR_TYPE_ACCELEROMETER;
		case Magnetometer:	return ASENSOR_TYPE_MAGNETIC_FIELD;
		case Proximity:		return ASENSOR_TYPE_PROXIMITY;
		case Gyroscope:		return ASENSOR_TYPE_GYROSCOPE;
		case Light:			return ASENSOR_TYPE_LIGHT;
		default: LOGE("Invalid scdf sensor type!"); return -1;
	}
}

scdf::SensorType scdf::SensorStandardImpl::AndroidToScdfType(int andrType)
{
	switch (andrType) {
		case ASENSOR_TYPE_ACCELEROMETER:	return Accelerometer;
		case ASENSOR_TYPE_MAGNETIC_FIELD:	return Magnetometer;
		case ASENSOR_TYPE_PROXIMITY:		return Proximity;
		case ASENSOR_TYPE_GYROSCOPE:		return Gyroscope;
		case ASENSOR_TYPE_LIGHT:			return Light;
		default: LOGE("Invalid android sensor type!"); return Invalid;
	}
}

s_bool scdf::SensorStandardImpl::IsAvailable(SensorType type)
{
	int andrType = ScdfToAndroidType(type);
	if (-1==andrType)
		return false;
	ASensorManager* sm = ASensorManager_getInstance();
	return (NULL!=ASensorManager_getDefaultSensor(sm,andrType));
}

scdf::SensorStandardImpl::SensorStandardImpl(SensorType _type)
{
	SetType(_type);
	androidType = ScdfToAndroidType(_type);
	if (androidType<0)
		LOGE("Standard sensor creation error, invalid type (%d)",androidType);
	sensorEventQueue=NULL;
}

scdf::SensorStandardImpl::~SensorStandardImpl()
{
	ASensorManager* sm = ASensorManager_getInstance();
	ASensorManager_destroyEventQueue(sm, sensorEventQueue);
}

s_bool scdf::SensorStandardImpl::Setup(SensorSettings& settings)
{
	settings.broken = false;
	currentRate = -1;

	ASensorManager* sm = ASensorManager_getInstance();

	if (NULL!=sensorEventQueue) {
		ASensorManager_destroyEventQueue(sm, sensorEventQueue);
	}

	androidSensor = ASensorManager_getDefaultSensor(sm,androidType);

	if (NULL==androidSensor) {
		settings.broken = true;
		return false;
	}

	ALooper* looper = ALooper_forThread();
	if (NULL==looper) {
		settings.broken = true;
		return false;
	}

	sensorEventQueue = ASensorManager_createEventQueue(sm,looper,0, Callback,this);
	if (NULL==sensorEventQueue) {
		settings.broken = true;
		return false;
	}

	s_int32 minDelay =   ASensor_getMinDelay(androidSensor);
	if (minDelay==0) {
		currentRate = settings.rate = -1;
		settings.broken = false; // still ok, sensor will work...
		return false;
	}
	s_int32 maxRate  = (s_int32) (1000000.0 / minDelay); // we want it in hz

	if (maxRate<settings.rate || settings.rate<=0) {
		ASensorEventQueue_setEventRate(sensorEventQueue, androidSensor, minDelay);
		currentRate = settings.rate = maxRate;
		settings.broken = false;
		return false;
	}
	int desiredDelay = (int)(1000000.0/settings.rate);
	if (0 < ASensorEventQueue_setEventRate(sensorEventQueue, androidSensor,desiredDelay)) {
		currentRate=-1;
		return false;

	}
	currentRate = settings.rate;
	return true;
}

s_bool scdf::SensorStandardImpl::Start()
{
	if (NULL==androidSensor || NULL==sensorEventQueue)
		return false;
	return (0 <= ASensorEventQueue_enableSensor(sensorEventQueue,androidSensor) );
}

s_bool scdf::SensorStandardImpl::Stop()
{
	if (NULL==androidSensor || NULL == sensorEventQueue)
		return false;
	return (0 <= ASensorEventQueue_disableSensor(sensorEventQueue,androidSensor) );
}

s_int32 scdf::SensorStandardImpl::GetRate()
{
	return currentRate;
}

s_int32 scdf::SensorStandardImpl::GetNumSamples()
{
	switch(androidType)
   	{
   		case ASENSOR_TYPE_ACCELEROMETER:
   	    case ASENSOR_TYPE_MAGNETIC_FIELD:
   		case ASENSOR_TYPE_GYROSCOPE:
   	   	 	return 3;
   	    case ASENSOR_TYPE_PROXIMITY:
   	 	case ASENSOR_TYPE_LIGHT:
   	 		return 1;
   	 	default: LOGE("GetNumSamples(): invalid sensor type... returning 0"); return 0;
   	}
}

/*ASensorList list;
	int numSensors = ASensorManager_getSensorList(sm, &list);

	for (int i=0; i<numSensors; i++) {
		ASensorRef s = list[i];
		const char* name = ASensor_getName(s);
		const char* vendor = ASensor_getVendor(s);
		int type = ASensor_getType(s);
		float resolution = ASensor_getResolution(s);
		int minDelay = ASensor_getMinDelay(s);
		LOGD("%s (%s) - type: %d - resol. %f - min delay (us): %d", name,vendor,type,resolution,minDelay);
	}*/


