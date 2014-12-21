/*
 * UsbDevicesManager.cpp
 *
 *  Created on: 17/feb/2014
 *      Author: athos
 */
#include "UsbDevicesManager.h"


Usb::DevicesManager* theUsbDevicesManager()
{
	static Usb::DevicesManager* theManager = new Usb::DevicesManager();
	return theManager;
}

MIDIDevicesChanged Usb::DevicesManager::MIDIDevicesChangedProc = NULL;



bool Usb::DevicesManager::AddDevice(Usb::AudioDevice* dev, bool useAudio,bool useMidi)
{
	allDevicesForDump.insert(dev);

	if (!dev->IsSupported())
		return false;

	bool isComposite = dev->HasAudioStreamingInterfaces() && dev->HasMidiInterfaces();

	//LOGD("COMPOSITE is composite %d, useAudio %d, useMidi %d",isComposite,useAudio,useMidi);

	if (!isComposite || useAudio) {

		//LOGD("COMPOSITE add audio interface");

		std::vector<AudioStreamingInfo> devouts = dev->GetOutStreamingInterfacesInfo();
		for (int i=0; i<devouts.size(); i++) {
			InterfaceInfo ii;
			ii.device=dev;
			ii.info = devouts[i];
			ii.order = outInterfaces.empty() ? 0 : outInterfaces.back().order + 1;
			stringformatter ss;
			ss << dev->GetProductName() << " Out " << (unsigned)ii.order << " [" << (unsigned)ii.info.numChannels << " chan " << (unsigned)ii.info.numBits << "bit]";
			ii.description = ss.str();
			outInterfaces.push_back(ii);
			//LOGD("USBAUDIO Add device: %s (itf:%u, altset:%u)",ss.str().c_str(),ii.info.itf,ii.info.altSetting);
		}

		std::vector<AudioStreamingInfo> devins = dev->GetInStreamingInterfacesInfo();
		for (int i=0; i<devins.size(); i++) {
			InterfaceInfo ii;
			ii.device=dev;
			ii.info = devins[i];
			ii.order = inInterfaces.empty() ? 0 : inInterfaces.back().order + 1;
			stringformatter ss;
			ss << dev->GetProductName() << " In " << (unsigned)ii.order << " [" << (unsigned)ii.info.numChannels << " chan " << (unsigned)ii.info.numBits << "bit]";
			ii.description = ss.str();
			inInterfaces.push_back(ii);
			//LOGD("USBAUDIO Add device: %s",ss.str().c_str());
		}

	}

	if (!isComposite || useMidi) {

		//LOGD("COMPOSITE add midi interface");

		bool addedMidi = false;

		if (dev->HasMidiInput()) {
			midiInputInterfaces.insert( dev->GetMidiInterface(0) );
			addedMidi = true;
		}

		if (dev->HasMidiOutput()) {
			midiOutputInterfaces.insert( dev->GetMidiInterface(0) );
			addedMidi=true;
		}

		if (addedMidi && MIDIDevicesChangedProc)
			MIDIDevicesChangedProc(0);
		}

	return true;
}

Usb::MidiInterface* Usb::DevicesManager::GetMidiOutInterface(int index)
{
	if (index >= midiOutputInterfaces.size()) {
		LOGE("Usb devices manager: GetMidiOutInterface() error, invalid index");
		return NULL;
	}
	std::set<MidiInterface*>::iterator it = midiOutputInterfaces.begin();
	std::advance(it, index);
	return (*it);
}

Usb::MidiInterface* Usb::DevicesManager::GetMidiInInterface(int index)
{
	if (index >= midiInputInterfaces.size()) {
		LOGE("Usb devices manager: GetMidiInInterface() error, invalid index");
		return NULL;
	}

	std::set<MidiInterface*>::iterator it = midiInputInterfaces.begin();
	std::advance(it, index);
	return (*it);
}


bool Usb::DevicesManager::RemoveDevice(Usb::AudioDevice* dev)
{
	bool removedSomething=false;

	std::vector<Usb::InterfaceInfo>::iterator it = outInterfaces.begin();
	for ( ; it!=outInterfaces.end(); /*no inc*/)
	{
		if ((*it).device == dev) {
			it = outInterfaces.erase(it);
			removedSomething=true;
		} else {
			++it;
		}
	}
	it = inInterfaces.begin();
	for ( ; it!=inInterfaces.end(); /*no inc*/)
	{
		if ((*it).device == dev) {
			it = inInterfaces.erase(it);
			removedSomething=true;
		} else {
			++it;
		}
	}

	bool removedMidi = false;

	if (dev->HasMidiInput()) {
		midiInputInterfaces.erase(dev->GetMidiInterface(0));
		removedSomething=true;
		removedMidi = true;
	}


	if (dev->HasMidiOutput()) {
		midiOutputInterfaces.erase(dev->GetMidiInterface(0));
		removedSomething=true;
		removedMidi = true;
	}

	if (removedMidi && MIDIDevicesChangedProc)
		MIDIDevicesChangedProc(0);


	allDevicesForDump.erase(dev);

	return removedSomething;
}

Usb::AudioStreamingInterface* Usb::DevicesManager::GetOutInterfaceFromOrder(int order, bool claim)
{
	/* (int i=0; i<outInterfaces.size(); i++)
		LOGD("USBAUDIO === outitf. ord:%d, itf:%u, alts:%u, desc:%s",order,
		outInterfaces[i].info.itf,outInterfaces[i].info.altSetting, outInterfaces[i].description.c_str()); */

	for (int i=0; i<outInterfaces.size(); i++)
	{
		InterfaceInfo itfi = outInterfaces[i];
		if (itfi.order==order) {
			Usb::AudioStreamingInterface* as = itfi.device->GetStreamingInterface(itfi.info.itf);//,itfi.info.altSetting);
			if (claim) {
				as->Claim();
				as->SelectAlternateSetting(itfi.info.altSetting);
			}
			return as;
		}
	}
	return NULL;
}

Usb::AudioStreamingInterface* Usb::DevicesManager::GetInInterfaceFromOrder(int order, bool claim)
{
	for (int i=0; i<inInterfaces.size(); i++)
	{
		InterfaceInfo itfi = inInterfaces[i];
		if (itfi.order==order) {
			Usb::AudioStreamingInterface* as = itfi.device->GetStreamingInterface(itfi.info.itf);//,itfi.info.altSetting);
			if (claim) {
				as->Claim();
				as->SelectAlternateSetting(itfi.info.altSetting);
			}
			return as;
		}
	}
	return NULL;
}


Usb::AudioStreamingInfo Usb::DevicesManager::GetInputStreamingInfoFromOrder(int order)
{
	for (int i=0; i<inInterfaces.size(); i++)
	{
		InterfaceInfo itfi = inInterfaces[i];
		if (itfi.order==order) {
			Usb::AudioStreamingInterface* as = itfi.device->GetStreamingInterface(itfi.info.itf);//,itfi.info.altSetting);
			return as->GetStreamingInfo(itfi.info.altSetting);
			// NB: we may have returned itfi.info directly, but asking it to the streaming interface
			// we may get more up-to-date informations, such as the streaming broken actual state.
		}
	}
	LOGE("USBAUDIO - No USB IN interface with order %d: void info",order);
	AudioStreamingInfo invalidInfo;
	return invalidInfo;
}


Usb::AudioStreamingInfo Usb::DevicesManager::GetOutputStreamingInfoFromOrder(int order)
{
	for (int i=0; i<outInterfaces.size(); i++)
	{
		InterfaceInfo itfi = outInterfaces[i];
		if (itfi.order==order) {
			Usb::AudioStreamingInterface* as = itfi.device->GetStreamingInterface(itfi.info.itf);//,itfi.info.altSetting);
			return as->GetStreamingInfo(itfi.info.altSetting);
			// NB: we may have returned itfi.info directly, but asking it to the streaming interface
			// we may get more up-to-date informations, such as the streaming broken actual state.
		}
	}
	LOGE("USBAUDIO - No USB OUT interface with order %d: void info",order);
	AudioStreamingInfo invalidInfo;
	return invalidInfo;
}

Usb::InterfaceInfo Usb::DevicesManager::GetInterfaceInfoFromOrder(bool out, int order)
{
	if (out) {
		for (int i=0; i<outInterfaces.size(); i++)
		{
			InterfaceInfo itfi = outInterfaces[i];
			if (itfi.order==order) return itfi;
		}
	} else {
		for (int i=0; i<inInterfaces.size(); i++)
		{
			InterfaceInfo itfi = inInterfaces[i];
			if (itfi.order==order) return itfi;
		}
	}
	return InterfaceInfo();
}


std::vector<uint32_t> Usb::DevicesManager::GetAllowedSampleRates(int outOrder, int inOrder)
{
	// get out interface rates:
	std::vector<uint32_t> common;
	InterfaceInfo outInfo = GetInterfaceInfoFromOrder(true,outOrder);
	std::vector<uint32_t> outRates = outInfo.device->GetAllowedSampleRates(outInfo.info.itf,outInfo.info.altSetting);
	if (outRates.empty())
		return common;
	std::sort(outRates.begin(),outRates.end());
	// get in interface rates:
	InterfaceInfo inInfo = GetInterfaceInfoFromOrder(false,inOrder);
	std::vector<uint32_t> inRates = inInfo.device->GetAllowedSampleRates(inInfo.info.itf,inInfo.info.altSetting);
	if (inRates.empty())
			return common;
	std::sort(inRates.begin(),inRates.end());
	// return the values supported by both:
	std::set_intersection(outRates.begin(),outRates.end(),inRates.begin(),inRates.end(),std::back_inserter(common));
	return common;
}

std::vector<int> Usb::DevicesManager::GetAllowedFramesPerBuffer(int outOrder, int inOrder, uint32_t sr)
{
	// get out interface frames per buffer allowed values:
	std::vector<int> common;
	InterfaceInfo outInfo = GetInterfaceInfoFromOrder(true,outOrder);
	std::vector<int> outFPBs = outInfo.device->GetAllowedFramesPerBuffer(outInfo.info.itf,outInfo.info.altSetting,sr);
	if (outFPBs.empty())
		return common;
	std::sort(outFPBs.begin(),outFPBs.end());
	// same for in interface:
	InterfaceInfo inInfo = GetInterfaceInfoFromOrder(false,inOrder);
	std::vector<int> inFPBs = inInfo.device->GetAllowedFramesPerBuffer(inInfo.info.itf,inInfo.info.altSetting,sr);
	if (inFPBs.empty())
		return common;
	std::sort(inFPBs.begin(),inFPBs.end());
	// return the values supported by both:
	std::set_intersection(outFPBs.begin(),outFPBs.end(),inFPBs.begin(),inFPBs.end(),std::back_inserter(common));
	return common;
}

Usb::AudioDevice* Usb::DevicesManager::GetDeviceFromOutputOrder(int order)
{
	for (int i=0; i<outInterfaces.size(); i++)
	{
		InterfaceInfo itfi = outInterfaces[i];
		if (itfi.order==order)
			return	itfi.device;
	}
	return NULL;
}

Usb::AudioDevice* Usb::DevicesManager::GetDeviceFromInputOrder(int order)
{
	for (int i=0; i<inInterfaces.size(); i++)
	{
		InterfaceInfo itfi = inInterfaces[i];
		if (itfi.order==order)
			return	itfi.device;
	}
	return NULL;
}

tstring Usb::DevicesManager::DumpAllDevicesInfo()
{
	std::set<AudioDevice*>::iterator it;
	stringformatter info;

	for (it=allDevicesForDump.begin(); it!=allDevicesForDump.end(); it++)
	{
		info << (*it)->DumpInfo() << "\n\n";
	}
/*
	Usb::AudioDevice* d = NULL;
	stringformatter info;
	for (int i=0; i<outInterfaces.size(); i++)
	{
		Usb::AudioDevice* d2 = outInterfaces[i].device;
		if (d!=d2) {
			info << d->DumpInfo() << "\n\n";
			d=d2;
		}
	}
*/
	return info.str();
}



int Usb::DevicesManager::FindBestOutput()
{
	int optimal = -1;
	int suboptimal = -1;
	int whatever = -1;
	int blindLuck = -1;

	for (int i=0; i<outInterfaces.size(); i++)
	{
		if(outInterfaces[i].info.numBits==16 && outInterfaces[i].info.numChannels==2) {
			optimal = outInterfaces[i].order;
		} else if(outInterfaces[i].info.numChannels==2) {
			suboptimal = outInterfaces[i].order;
		} else if (outInterfaces[i].info.numBits==16) {
			whatever = outInterfaces[i].order;
		} else {
			blindLuck = outInterfaces[i].order;
		}
	}

	if (optimal!=-1) {
		return optimal;
	} else if (suboptimal!=-1) {
		return suboptimal;
	} else if (whatever!=-1) {
		return whatever;
	}
	return blindLuck;
}

int Usb::DevicesManager::FindBestInput()
{
	int optimal = -1;
	int suboptimal = -1;
	int whatever = -1;
	int blindLuck = -1;

	for (int i=0; i<inInterfaces.size(); i++)
	{
		if(inInterfaces[i].info.numBits==16 && inInterfaces[i].info.numChannels==2) {
			optimal = inInterfaces[i].order;
		} else if(inInterfaces[i].info.numChannels==2) {
			suboptimal = inInterfaces[i].order;
		} else if (inInterfaces[i].info.numBits==16){
			whatever = inInterfaces[i].order;
		} else {
			blindLuck = outInterfaces[i].order;
		}
	}

	if (optimal!=-1) {
			return optimal;
	} else if (suboptimal!=-1) {
		return suboptimal;
	} else if (whatever!=-1) {
		return whatever;
	}
	return blindLuck;
}
