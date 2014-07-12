#include <jni.h>
#include "Sensor.h"
#include "SensorsManager.h"
#include "Harvester.h"
#include "UDPSendersManager.h"

void InitFramework();

extern "C" {

using namespace scdf;

/* ACTIVITY LIFECYCLE - NATIVE SIDE */

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnCreate(JNIEnv* env, jobject thiz)
{
	return true;
}

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnResume(JNIEnv* env, jobject thiz)
{
	return theSensorManager()->StartPrecActiveSensors();
}

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeOnPause(JNIEnv* env, jobject thiz)
{
	return theSensorManager()->StopAllSensors();
}



/* SCDF FRAMEWORK AND SENSORS MANAGEMENT */

JNIEXPORT jboolean JNICALL Java_it_scdf_test_TestActivity_NativeInit(JNIEnv* env, jobject thiz)
{
    InitFramework();

    theSensorManager()->CreateSensor(AudioInput);
    theSensorManager()->CreateSensor(Accelerometer);

    scdf::SensorSettings accSettings;
    accSettings.rate=1;
    theSensorManager()->InitSensor(Accelerometer,accSettings);

    scdf::SensorAudioSettings audioSettings;
    audioSettings.rate=44100; // settings are ignored as of now
    audioSettings.bufferSize=1024;
    theSensorManager()->InitSensor(AudioInput,audioSettings);

    theSensorManager()->StartAllSensors();

    UDPSendersManager::Instance()->InitSender(9000,"10.12.209.49");
    UDPSendersManager::Instance()->SetMultiOutput(true);

}



} // extern "c" block
