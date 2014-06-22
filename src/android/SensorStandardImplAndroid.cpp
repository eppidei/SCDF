#include "SensorStandardImplAndroid.h"

#include <android/sensor.h>
#include <android/looper.h>

#include <android/log.h>
#define  LOG_TAG    "Scdf test"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//TODO: define mutiplatform logging functions and remove these

int SensorEventCallback(int fd, int events, void* data)
{
	scdf::SensorStandardImpl* s = (scdf::SensorStandardImpl*)data;
	ASensorEvent event;
    while (ASensorEventQueue_getEvents(s->sensorEventQueue, &event, 1) > 0)
    {
    	//LOGD("X = %f y = %f z=%f ", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    	scdf::SensorData *sData = new scdf::SensorData();
   	    sData->type = scdf::Accelerometer;
   	    sData->timestamp = event.timestamp; // scdf timestamp is float, android's one is int64!!
   	    sData->num_samples=3;
   	    sData->data = new s_sample[3];
   	    sData->data[0]  = event.acceleration.x;
   	    sData->data[1]  = event.acceleration.y;
   	    sData->data[2]  = event.acceleration.z;

   	    s->AddIncomingDataToQueue(sData);
    }
    return 1;
}

scdf::SensorStandardImpl::SensorStandardImpl(SensorType _type)
{
	sensorEventQueue=NULL;
}

scdf::SensorStandardImpl::~SensorStandardImpl()
{

}

s_bool scdf::SensorStandardImpl::Setup(SensorSettings& settings)
{
	ASensorManager* sm = ASensorManager_getInstance();
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

	//ASENSOR_TYPE_ACCELEROMETER, ASENSOR_TYPE_MAGNETIC_FIELD, ASENSOR_TYPE_GYROSCOPE,
	//ASENSOR_TYPE_LIGHT,	ASENSOR_TYPE_PROXIMITY

	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	ALooper* looper = ALooper_forThread();
	if (NULL==looper)
		return false;
	sensorEventQueue = ASensorManager_createEventQueue(sm,looper,0, SensorEventCallback,this);
	if (NULL==sensorEventQueue)
		return false;
	return true;
}

s_bool scdf::SensorStandardImpl::Start()
{
	ASensorManager* sm = ASensorManager_getInstance();
	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	return (0 <= ASensorEventQueue_enableSensor(sensorEventQueue,s) );
    return true;
}

s_bool scdf::SensorStandardImpl::Stop()
{
	ASensorManager* sm = ASensorManager_getInstance();
	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	return (0 <= ASensorEventQueue_disableSensor(sensorEventQueue,s) );
    return true;
}





