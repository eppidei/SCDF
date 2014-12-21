/*
 * UsbDevicesManager.h
 *
 *  Created on: 17/feb/2014
 *      Author: athos
 */

#ifndef USBDEVICESMANAGER_H_
#define USBDEVICESMANAGER_H_

#include "UsbAudioDevice.h"

typedef void (*MIDIDevicesChanged)(int code);

namespace Usb {

struct InterfaceInfo {
	AudioDevice* device;
	tstring description;
	AudioStreamingInfo info;
	int order;
	InterfaceInfo() : device(NULL),description(""),order(-1) {}
};



class DevicesManager {
	//friend DevicesManager* ::theDevicesManager();
private:
	//DevicesManager() {}
public:
	std::vector<InterfaceInfo> outInterfaces;
	std::vector<InterfaceInfo> inInterfaces;

	std::set<MidiInterface*> midiOutputInterfaces;
	std::set<MidiInterface*> midiInputInterfaces;

	int GetNumMidiOutInterfaces() { return midiOutputInterfaces.size(); }
	int GetNumMidiInInterfaces() { return midiInputInterfaces.size(); }
	MidiInterface* GetMidiOutInterface(int index);
	MidiInterface* GetMidiInInterface(int index);
	static MIDIDevicesChanged MIDIDevicesChangedProc;


	bool AddDevice(Usb::AudioDevice* dev,bool useAudio, bool useMidi);
	bool RemoveDevice(Usb::AudioDevice* dev);

	AudioStreamingInterface* GetOutInterfaceFromOrder(int order, bool claim = true);
	AudioStreamingInterface* GetInInterfaceFromOrder(int order, bool claim = true);

	AudioDevice* GetDeviceFromOutputOrder(int order);
	AudioDevice* GetDeviceFromInputOrder(int order);

	AudioStreamingInfo GetInputStreamingInfoFromOrder(int order);
	AudioStreamingInfo GetOutputStreamingInfoFromOrder(int order);

	InterfaceInfo GetInterfaceInfoFromOrder(bool out, int order);
	std::vector<uint32_t> GetAllowedSampleRates(int outOrder, int inOrder);
	std::vector<int> GetAllowedFramesPerBuffer(int outOrder, int inOrder, uint32_t sr);

	int FindBestOutput();
	int FindBestInput();

	// Utilities for enumeration:
	int NumOuts() { return outInterfaces.size(); }
	int NumIns()  { return inInterfaces.size(); }
	int GetOrderForOutAt(int position) { return outInterfaces[position].order;}
	int GetOrderForInAt(int position) { return inInterfaces[position].order;}
	tstring GetNameForOutAt(int position) { return outInterfaces[position].description;}
	tstring GetNameForInAt(int position) { return inInterfaces[position].description;}

	tstring DumpAllDevicesInfo();
	std::set<Usb::AudioDevice*> allDevicesForDump;
};


}

Usb::DevicesManager* theUsbDevicesManager(); // SINGLETON retrieval function




#endif /* USBDEVICESMANAGER_H_ */
