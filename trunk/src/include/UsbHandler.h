#ifndef NATIVE_USB_HANDLER_H
#define NATIVE_USB_HANDLER_H

#include "JniGlue.h"
#include <string>

namespace Usb {
	class AudioDevice;
	class MidiInterface;
}

class UsbHandler : public JavaClass {

public:

	static bool TryOpeningFirstUsbDevice();
	static int GetNumMidiOutInterfaces();
	static std::string GetMidiInterfaceName(int index);
	static Usb::MidiInterface* GetMidiInterface(int index);

private:

	static Usb::AudioDevice* GetDevice(int index);
	static int GetNumOpenDevices();

	static bool InitClass();
	static jclass GetClass();
	static jmethodID tryOpeningFirstUsbDevice;
	static jmethodID getNumOpenDevices;
	static jmethodID getDevice;
};


#endif
