/*
 * UsbAudioDevice.h
 *
 *  Created on: 14/gen/2014
 *      Author: athos
 */

#ifndef USBAUDIODEVICE_H_
#define USBAUDIODEVICE_H_

#include "UsbHelper.h"
#include "UsbAudioInterfaces.h"
#include "UsbMidiInterface.h"

//#define USB_COMPOSITE_MODE_AUDIO_AND_MIDI 0
//#define USB_COMPOSITE_MODE_MIDI_ONLY 1
//#define USB_COMPOSITE_MODE_AUDIO_ONLY 2
// these macros must mirror the COMPOSITE_DEVICE_* constants in AudioDevice.java

namespace Usb {


class AudioDevice {

public:

	static AudioDevice* Create(int vid, int pid, int fd);
	static void Destroy(AudioDevice *device);

	int GetMaxCurrent(); // in mA
	bool HasAudioCapabilities();
	std::string GetProductName();
	std::string GetVendorName();
	void ReleaseAllInterfaces();

#ifdef ENABLE_AUDIO_INTERFACES

	bool ClaimControlInterface(uint8_t altSetNum=0);
	bool ClaimStreamingInterface(uint8_t interface, uint8_t altsetting);
	//bool SetSampleRate(uint32_t sr);
	AudioStreamingInterface* GetStreamingInterface(uint8_t itf);//, uint8_t altset);
	std::vector<uint32_t> GetAllowedSampleRates(uint8_t interface, uint8_t altSetting);
	std::vector<int> GetAllowedFramesPerBuffer(uint8_t interface, uint8_t altSetting, uint32_t sr);
	bool HasAudioStreamingInterfaces();

	AudioControlInterface* GetControlInterface();
	AudioStreamingInterface* GetFirstOutInterface();
	AudioStreamingInterface* GetFirstInInterface();

	std::vector<AudioStreamingInfo> GetOutStreamingInterfacesInfo();
	std::vector<AudioStreamingInfo> GetInStreamingInterfacesInfo();

#endif

	bool IsAudioSupported();

	bool HasMidiInterfaces();
	bool HasMidiInput();
	bool HasMidiOutput();
	MidiInterface* GetMidiInterface(int i);

	bool IsSupported() { return IsAudioSupported()||HasMidiInterfaces(); }

	bool IsStreamingActive();
	std::string DumpInfo();
	libusb_device_handle* GetDeviceHandle() { return devh;}

private:

	libusb_device_handle* devh;
	libusb_config_descriptor* activeConfigDescriptor;

#ifdef ENABLE_AUDIO_INTERFACES
	AudioControlInterface* audioControlInterface;
	std::vector<AudioStreamingInterface*> audioStreamingInterfaces;
	AudioStreamingInterface* FindASInterfaceByIndex(uint8_t index);

	bool InitAudioControlInterface();
#endif

	std::vector<MidiInterface*> midiInterfaces;

	std::vector<std::string> unsupportedInterfaces;

	AudioDevice();
	AudioDevice(libusb_device_handle* devHandler);
	~AudioDevice();

	libusb_device* GetDevice();
	Version GetUsbVersion();
	libusb_device_descriptor GetDescriptor();
	libusb_config_descriptor* GetActiveConfigDescriptor();
	void FreeActiveConfigDescriptor();

	bool InitAudioAndMidiStreamingInterfaces();
	int GetInterfacesNumber();

};





} // usb namespace end




#endif /* USBAUDIODEVICE_H_ */
