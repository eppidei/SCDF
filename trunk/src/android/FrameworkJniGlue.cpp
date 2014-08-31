/* FrameworkJniGlue.cpp
 *  Created on: Jul 17, 2014
 *      Author: athos */

#include "it_scdf_framework_Scdf.h" // use GenerateJniHeader.sh script in src/android dir
#include "Logging.h"
#include "UDPSendersManager.h"
#include "SensorsManager.h"
#include "Harvester.h"
#include "Sensor.h"

using namespace scdf;


SensorType ItoType(int itype)
{
	return (SensorType)itype;
	// in case we need more complex or safer way to convert from int to sensortype,
	// we'll just need to change this
}

JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_IsSensorAvailable
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->IsSensorAvailable(ItoType(type));
}

JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_StartSensor
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->StartSensor(ItoType(type));
}


JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_StopSensor
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->StopSensor(ItoType(type));
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_SetupSensor
  (JNIEnv *e, jclass c, jint type, jint rate)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	if (type==AudioInput) {
		LOGE("Don't use SetupSensor for audio input!");
		return false;
	}
	SensorSettings settings;
	settings.rate = rate;
	s_bool ret = theSensorManager()->InitSensor(ItoType(type),settings);
	if (ret==true)
		return it_scdf_framework_Scdf_SENSOR_SETUP_OK;
	else if (settings.broken==false)
		return it_scdf_framework_Scdf_SENSOR_SETUP_OK_CHANGED;
	else
		return it_scdf_framework_Scdf_SENSOR_SETUP_BROKEN;
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_SetupAudioInput
  (JNIEnv *e, jclass c, jint rate, jint bufferSize, jint channels)
{
	LOGD("Java called setupaudioinput. rate %d, fpb %d, ch %d",rate,bufferSize,channels);
	SensorAudioSettings settings;

	settings.rate = rate==-1 ? theSensorManager()->GetRate(AudioInput) : rate;
	settings.bufferSize = bufferSize==-1 ? theSensorManager()->GetNumFramesPerCallback(AudioInput) : bufferSize;
	settings.numChannels = channels==-1 ? theSensorManager()->GetNumChannels(AudioInput) : channels;

	s_bool ret = theSensorManager()->InitSensor(AudioInput,settings);

	if (ret==true)
		return it_scdf_framework_Scdf_SENSOR_SETUP_OK;
	else if (settings.broken==false)
		return it_scdf_framework_Scdf_SENSOR_SETUP_OK_CHANGED;
	else
		return it_scdf_framework_Scdf_SENSOR_SETUP_BROKEN;
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_GetSensorRate
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->GetRate(ItoType(type));
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_GetAudioInputFramesPerBuffer
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->GetNumFramesPerCallback(AudioInput);
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_GetAudioInputChannels
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	// TODO: return actual number of channels;
	return 1;
}


JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_IsSensorActive
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);

	return theSensorManager()->SensorActivated(ItoType(type));
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_StartAllSensors
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->StartAllSensors();
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_StopAllSensors
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->StopAllSensors();
}


JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_StartPreviouslyActiveSensors
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return theSensorManager()->StartPrecActiveSensors();
}


JNIEXPORT void JNICALL Java_it_scdf_framework_Scdf_SetUdpOSCmode
  (JNIEnv *e, jclass c, jboolean active)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	UDPSendersManager::Instance()->SetUseOSCPackaging(active);
}


JNIEXPORT void JNICALL Java_it_scdf_framework_Scdf_SetUdpMultiportMode
  (JNIEnv *e, jclass c, jboolean active)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	UDPSendersManager::Instance()->SetMultiOutput(active);
}

static std::string theIp("127.0.0.1");
static int thePort = 55556;

JNIEXPORT void JNICALL Java_it_scdf_framework_Scdf_SetUdpDestinationIp
  (JNIEnv* e, jclass c, jstring jip)
{
	char const* cstr = e->GetStringUTFChars(jip, NULL);
	std::string ip(cstr);

	theIp = ip;

	// TODO: implement, and find a way not to start the manager if you don't want!
	//UDPSendersManager::Instance()->SetOutputAddress("");
}

JNIEXPORT void JNICALL Java_it_scdf_framework_Scdf_SetUdpDestinationPort
  (JNIEnv * e, jclass c, jint port)
{
	thePort = port;

	// TODO: implement, and find a way not to start the manager if you don't want!
	//UDPSendersManager::Instance()->SetDestinationPort(0);
}

JNIEXPORT jstring JNICALL Java_it_scdf_framework_Scdf_GetUdpDestinationIp
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	//std::string ip = UDPSendersManager::Instance()->GetOutputAddress();
	return e->NewStringUTF(theIp.c_str());
}

JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_GetUdpDestinationPort
  (JNIEnv *, jclass)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return thePort;//UDPSendersManager::Instance()->GetOutputPort();
}

JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_IsUdpOSCmodeActive
  (JNIEnv *, jclass)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return UDPSendersManager::Instance()->UseOSCPackaging();
}


JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_IsUdpMultiportModeActive
  (JNIEnv *, jclass)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return UDPSendersManager::Instance()->GetMultiOutput();
}


JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_OpenUdpConnection
  (JNIEnv *e, jclass c, jstring jip, jint port)
{
	//LOGD("%s",__PRETTY_FUNCTION__);

	//char const* cstr = e->GetStringUTFChars(jip, NULL);
	//std::string ip(cstr);

	UDPSendersManager::Instance()->InitSender(thePort,theIp);

	// TODO: make init report success or failure
	//e->ReleaseStringUTFChars(jip,cstr);
	return true;
}


JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_CloseUdpConnection
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
    //UDPSendersManager::Instance()->ReleaseSender();
	// tODO: make release report success or failure
	return true;
}





/*JNIEXPORT jboolean JNICALL Java_it_scdf_framework_Scdf_SetMasterSensor
  (JNIEnv *e, jclass c, jint type)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	Harvester::Instance()->SetType(ItoType(type));
	// TODO: make set type return success or failure;
	return true;
}

JNIEXPORT jint JNICALL Java_it_scdf_framework_Scdf_GetMasterSensor
  (JNIEnv *e, jclass c)
{
	//LOGD("%s",__PRETTY_FUNCTION__);
	return Harvester::Instance()->GetType();
}*/
