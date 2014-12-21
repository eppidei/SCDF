#include "UsbHandler.h"
#include "libusb.h"
#include "usb/UsbAudioDevice.h"

jmethodID UsbHandler::tryOpeningFirstUsbDevice = 0;
jmethodID UsbHandler::getNumOpenDevices = 0;
jmethodID UsbHandler::getDevice = 0;

bool UsbHandler::InitClass()
{
	static bool initDone = false;
	if (initDone) return true;
	JNIEnv* env = Jni::Env();
	jclass clazz = GetClass();
	if (!clazz) return false;
	tryOpeningFirstUsbDevice = Jni::FindStaticMethod("TryOpeningFirstUsbDevice","()Z",clazz,env);
	if (!tryOpeningFirstUsbDevice) return false;
	getDevice = Jni::FindStaticMethod("GetDevice","(I)J",clazz,env);
	if (!getDevice) return false;
	getNumOpenDevices = Jni::FindStaticMethod("GetNumOpenDevices","()I",clazz,env);
	if (!getNumOpenDevices) return false;

	initDone=true;
	return initDone;
}

bool UsbHandler::TryOpeningFirstUsbDevice()
{
	LOGD("Try opening first usb device");
	if (!InitClass()) return false; // methodID and class validity check inside here
	JNIEnv* env = Jni::Env();
	jboolean res = env->CallStaticBooleanMethod(GetClass(),tryOpeningFirstUsbDevice);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	return (exc ? false : res);
}

Usb::AudioDevice* UsbHandler::GetDevice(int index)
{
	if (!InitClass()) return NULL;
	JNIEnv* env = Jni::Env();
	jlong res = env->CallStaticLongMethod(GetClass(),getDevice,index);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	return exc ? NULL : (Usb::AudioDevice*)res;
}

int UsbHandler::GetNumOpenDevices()
{
	if (!InitClass()) return NULL;
	JNIEnv* env = Jni::Env();
	jint res = env->CallStaticIntMethod(GetClass(),getDevice);
	jboolean exc = Jni::CheckForException(__PRETTY_FUNCTION__,env);
	return exc ? 0 : res;
}


jclass UsbHandler::GetClass()
{
	return Jni::FindClass("it/scdf/framework/UsbHandler");
}

int UsbHandler::GetNumMidiOutInterfaces()
{
	int numMidiItf = 0;
	int nDev = GetNumOpenDevices();
	for (int i=0; i<nDev; i++) {
		Usb::AudioDevice* dev = GetDevice(i);
		if (dev->HasMidiOutput())
			numMidiItf++;
	}
	return numMidiItf;
}

std::string UsbHandler::GetMidiInterfaceName(int index)
{
	if (index>=GetNumOpenDevices())
		return "[invalid MIDI out]";

	int k = -1;
	int nDev = GetNumOpenDevices();
	for (int i=0; i<nDev; i++)
	{
		Usb::AudioDevice* dev = GetDevice(i);
		if (dev->HasMidiOutput()) k++;
		if (index==k) {
			return dev->GetProductName();
		}
	}
	return "[Invalid MIDI out]";
}

Usb::MidiInterface* UsbHandler::GetMidiInterface(int index)
{
	if (index>=GetNumOpenDevices())
		return NULL;

	int k = -1;
	int nDev = GetNumOpenDevices();
	for (int i=0; i<nDev; i++)
	{
		Usb::AudioDevice* dev = GetDevice(i);
		if (dev->HasMidiOutput()) k++;
		if (index==k) {
			return dev->GetMidiInterface(0);
		}
	}
	return NULL;

}


extern "C" {


JNIEXPORT jboolean JNICALL
	Java_android_usb_test_MainActivity_UsbDiscoverTest(JNIEnv* e, jclass c)
{
	LOGD("USB discover test - native");
	return UsbHandler::TryOpeningFirstUsbDevice();
}


JNIEXPORT jlong JNICALL
	Java_it_scdf_framework_UsbHandler_NativeOpenDevice(JNIEnv* e, jclass c, jint fd, jint vid, jint pid)
{
	LOGD("USB - Open usb device, native side");
	Usb::AudioDevice* theDevice = Usb::AudioDevice::Create(vid,pid,fd);
	if (!theDevice)	return (jlong)0;
	theDevice->DumpInfo();
	//Usb::MidiInterface* mItf = theDevice->GetMidiInterface(0);
	//LOGD("TEST Midi interface name: %s", mItf->GetOutName().c_str());
	//bool res = mItf->SendMIDIMessage(0);
	//LOGD("SEND MIDI MESSAGE TEST RESULT: %d",res);
	return (jlong) theDevice ; // return a handle to the native usb audio device
}



JNIEXPORT void JNICALL
	Java_it_scdf_framework_UsbHandler_NativeCloseDevice(JNIEnv* e, jclass c, jlong devicePtr)
{
	if (!devicePtr)	return;
	Usb::AudioDevice::Destroy((Usb::AudioDevice*)devicePtr);
}

JNIEXPORT void JNICALL
	Java_it_scdf_framework_UsbHandler_CallNativeListeners(JNIEnv* e, jclass c,jint deviceId, jint evCode)
{
	LOGD("Notify native listeners for USB event : %d (device id: %d)",evCode,deviceId);
}

} // end of export c

