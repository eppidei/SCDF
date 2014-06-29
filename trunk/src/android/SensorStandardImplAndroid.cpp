#include "SensorStandardImplAndroid.h"
#include "CustomPipe.h"
#include <android/sensor.h>
#include <android/looper.h>


#include <time.h>
s_uint64 now_ns(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (s_uint64)(1000 * res.tv_sec) + (s_uint64) res.tv_nsec;
}


int scdf::SensorStandardImpl::Callback(int fd, int events, void* data)
{
	LOGD("Sensor callback");
	scdf::SensorStandardImpl* s = (scdf::SensorStandardImpl*)data;
	ASensorEvent event;
    while (ASensorEventQueue_getEvents(s->sensorEventQueue, &event, 1) > 0)
    {
    	LOGD("   Sensor %d : %f  %f  %f", s->GetType(),event.acceleration.x, event.acceleration.y, event.acceleration.z);
    	SensorData *sd=(*(GetReturnPipes()))[s->GetType()]->ReadMessage<SensorData*>();

   		#ifdef LOG_PIPES_STATUS
   	    LOGD("Return pipe size of %s: %d\n", SensorTypeString[sensorTypeRef].c_str(), (*(GetReturnPipes()))[sensorTypeRef]->GetSize());
   		#endif

   	    if (NULL==sd){
   	        sd = new scdf::SensorData();
   	        sd->data=(s_sample*) new s_sample[3];
   	    }

   	    sd->type = s->GetType();
   	    sd->timeid = now_ns();
   	   	sd->timestamp = sd->timeid;
   	   	sd->rate = 1;

   	    sd->num_samples=3;
   	    sd->data[0]  = event.acceleration.x;
   	    sd->data[1]  = event.acceleration.y;
   	    sd->data[2]  = event.acceleration.z;

   	    s->AddIncomingDataToQueue(sd);
    }
    return 1;
}

scdf::SensorStandardImpl::SensorStandardImpl(SensorType _type)
{
	SetType(_type);
	sensorEventQueue=NULL;
}

scdf::SensorStandardImpl::~SensorStandardImpl()
{

}

s_bool scdf::SensorStandardImpl::Setup(SensorSettings& settings)
{
	ASensorManager* sm = ASensorManager_getInstance();
	//ASENSOR_TYPE_ACCELEROMETER, ASENSOR_TYPE_MAGNETIC_FIELD, ASENSOR_TYPE_GYROSCOPE,
	//ASENSOR_TYPE_LIGHT,	ASENSOR_TYPE_PROXIMITY

	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	ALooper* looper = ALooper_forThread();
	if (NULL==looper)
		return false;

	sensorEventQueue = ASensorManager_createEventQueue(sm,looper,0, Callback,this);
	if (NULL==sensorEventQueue)
		return false;

	// ASensor_getMinDelay(s) // se è maggiore del rate desiderato o è 0, usa questo valore per seteventrate
	// ASensorEventQueue_setEventRate(sensorEventQueue, s, 1000); // test, 1 sec
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


