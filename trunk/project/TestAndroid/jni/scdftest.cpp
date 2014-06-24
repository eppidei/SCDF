#include <jni.h>
#include "Sensor.h"
//#include <android/sensor.h>
//#include <android/looper.h>

#include <android/log.h>
#define  LOG_TAG    "Scdf test"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

/*
ASensorEventQueue* sensorEventQueue;

int SensorEventCallback(int fd, int events, void* data)
{
	ASensorEvent event;
    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0)
    {
    	LOGD("X = %f y = %f z=%f ", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }
    return 1;
}*/

void InitFramework();

extern "C" {

scdf::Sensor* acc;


JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeInit(JNIEnv* env, jobject thiz)
{
    InitFramework();
	scdf::SensorSettings s_settings;
    s_settings.rate=1;
    acc = scdf::Sensor::Create(scdf::Accelerometer);
    acc->Setup(s_settings);
    acc->Start();
}

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnCreate(JNIEnv* env, jobject thiz)
{
	return true;
	InitFramework();
	scdf::SensorSettings s_settings;
    s_settings.rate=1;
    acc = scdf::Sensor::Create(scdf::Accelerometer);
    return acc->Setup(s_settings);

	/*ASensorManager* sm = ASensorManager_getInstance();
	ASensorList list;
	int numSensors = ASensorManager_getSensorList(sm, &list);

	for (int i=0; i<numSensors; i++) {
		ASensorRef s = list[i];
		const char* name = ASensor_getName(s);
		const char* vendor = ASensor_getVendor(s);
		int type = ASensor_getType(s);
		float resolution = ASensor_getResolution(s);
		int minDelay = ASensor_getMinDelay(s);
		LOGD("%s (%s) - type: %d - resol. %f - min delay (us): %d", name,vendor,type,resolution,minDelay);
	}

	//ASENSOR_TYPE_ACCELEROMETER, ASENSOR_TYPE_MAGNETIC_FIELD, ASENSOR_TYPE_GYROSCOPE,
	//ASENSOR_TYPE_LIGHT,	ASENSOR_TYPE_PROXIMITY
	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	ALooper* looper = ALooper_forThread();
	if (NULL==looper)
		return false;
	sensorEventQueue = ASensorManager_createEventQueue(sm,looper,0, SensorEventCallback,NULL);
	return true;*/
}

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnResume(JNIEnv* env, jobject thiz)
{
	return true;
	acc->Start();
	/*ASensorManager* sm = ASensorManager_getInstance();
	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	return (0 <= ASensorEventQueue_enableSensor(sensorEventQueue,s) );*/

}

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnPause(JNIEnv* env, jobject thiz)
{
	return true;
	acc->Stop();
	/*ASensorManager* sm = ASensorManager_getInstance();
	ASensorRef s = ASensorManager_getDefaultSensor(sm,ASENSOR_TYPE_ACCELEROMETER);
	return (0 <= ASensorEventQueue_disableSensor(sensorEventQueue,s) );*/
}





} // extern "c" block
