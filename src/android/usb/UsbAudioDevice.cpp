#include "UsbAudioDevice.h"
#include "Logging.h"
#include <sstream>

Usb::AudioDevice* Usb::AudioDevice::Create(int vid, int pid, int fd)
{
	// Init libusb framework
	// (it won't harm calling this multiple times, but it may be better to put it elsewere before?)
	if (0 > Usb::InitUsb()) {
		LOGE("Cannot initialize USB stack! Device won't be created");
		return NULL;
	}

	Usb::StartUsbEventHandling();

	libusb_device_handle* devHandler = Usb::OpenDevice(vid,pid,fd);
	if (NULL==devHandler) {
		return NULL;
	}

	Usb::AudioDevice* device = new Usb::AudioDevice(devHandler);
	return device;
}


void Usb::AudioDevice::Destroy(Usb::AudioDevice* device)
{
	delete device;
}


bool Usb::AudioDevice::IsAudioSupported()
{
#ifdef ENABLE_AUDIO_INTERFACES

	bool inSupported = false;
	bool outSupported = false;

	/*Usb::Version v = GetUsbVersion();
	if (v!=Usb::v10 && v!=Usb::v11) {
		LOGE("USBAUDIO - 2.0 and 3.0 usb protocols not supported yet!");
		return false;
	}*/

	std::vector<AudioStreamingInfo> outstreams = GetOutStreamingInterfacesInfo();
	for (int i=0; i<outstreams.size(); i++)
	{
		if (outstreams[i].numChannels >= 2 && outstreams[i].numBits >= 16) {
			outSupported=true;
			break;
		}
	}

	std::vector<AudioStreamingInfo> instreams = GetInStreamingInterfacesInfo();
	for (int i=0; i<instreams.size(); i++)
	{
		if (instreams[i].numChannels>0 /*&& instreams[i].numChannels<3*/ && instreams[i].numBits >= 16) {
			inSupported=true;
			break;
		}
	}
	return (inSupported && outSupported);
#else
	return false;
#endif
}


bool Usb::AudioDevice::HasMidiInterfaces()
{
	return (midiInterfaces.size()!=0);
}

bool Usb::AudioDevice::HasMidiInput()
{
	if (!HasMidiInterfaces())
		return false;
	return midiInterfaces[0]->HasInput();
}

bool Usb::AudioDevice::HasMidiOutput()
{
	if (!HasMidiInterfaces())
			return false;
	return midiInterfaces[0]->HasOutput();
}

Usb::MidiInterface* Usb::AudioDevice::GetMidiInterface(int i)
{
	if (i>=midiInterfaces.size())
		return NULL;
	return midiInterfaces[i];
}

Usb::AudioDevice::~AudioDevice()
{
#ifdef ENABLE_AUDIO_INTERFACES
	delete audioControlInterface;

	for (int i=0; i < audioStreamingInterfaces.size(); i++)
		delete audioStreamingInterfaces[i];
	audioStreamingInterfaces.clear();
#endif

	for (int i=0; i < midiInterfaces.size(); i++)
		delete midiInterfaces[i];
	midiInterfaces.clear();

	if (activeConfigDescriptor != NULL)
		FreeActiveConfigDescriptor();

	libusb_close(devh);
}


Usb::AudioDevice::AudioDevice()
{
	devh = NULL;
	activeConfigDescriptor=NULL;
}


Usb::AudioDevice::AudioDevice(libusb_device_handle* deviceHandler)
{
	devh = deviceHandler;
	activeConfigDescriptor = NULL;
	#ifdef ENABLE_AUDIO_INTERFACES
	audioControlInterface = NULL;
	#endif

	int config = 0;
	int ret = libusb_get_configuration(devh,&config);
	if (0==ret) {
		if (0!=libusb_reset_device(devh))
			LOGE("USBAUDIO - error resetting the USB device!");
		if(0!=libusb_set_configuration(devh,config))
			LOGE("USBAUDIO - error setting USB device's configuration!");
	}
	else
		LOGE("USBAUDIO - Get config error: %d", ret);

	#ifdef ENABLE_AUDIO_INTERFACES
	bool hasAudioControl = InitAudioControlInterface();
	#endif
	// NB: we need the audio control interface created BEFORE any streaming one
	InitAudioAndMidiStreamingInterfaces();
}


libusb_device* Usb::AudioDevice::GetDevice()
{
	return libusb_get_device(devh);
}

Usb::Version Usb::AudioDevice::GetUsbVersion()
{
	return Usb::GetVersion(devh);
}

libusb_device_descriptor Usb::AudioDevice::GetDescriptor()
{
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(GetDevice(), &desc);
	if (r<0)
		LOGE("USBAUDIO - Error retrieving usb device descriptor");

	return desc;
}


std::string Usb::AudioDevice::GetProductName()
{
	uint8_t prodStrId = GetDescriptor().iProduct;
	if (0==prodStrId)
		return "USB Device";
	return Usb::GetString(devh,prodStrId);
}

std::string Usb::AudioDevice::GetVendorName()
{
	uint8_t vendStrId = GetDescriptor().iManufacturer;
	if (0==vendStrId)
		return "Unknown";
	return Usb::GetString(devh,vendStrId);
}



libusb_config_descriptor* Usb::AudioDevice::GetActiveConfigDescriptor()
{
	// the descriptor is got from libusb the first time someone asks for it.
	// it must be released, and this will be done on device destruction

	if (NULL==activeConfigDescriptor) {
		int r = libusb_get_active_config_descriptor(GetDevice(),&activeConfigDescriptor);
		if(0 != r) {
			Usb::PrintError("active configuration descriptor request failed",r);
			activeConfigDescriptor=NULL;
			return NULL;
		}
	}
	return activeConfigDescriptor;
}

void Usb::AudioDevice::FreeActiveConfigDescriptor()
{
	if (NULL!=activeConfigDescriptor)
		libusb_free_config_descriptor(activeConfigDescriptor);
}

int Usb::AudioDevice::GetMaxCurrent() // in mA
{
	libusb_config_descriptor* config = GetActiveConfigDescriptor();
	if (NULL == config) return -1;
	return config->MaxPower*2; // as for the usb specs
}

int Usb::AudioDevice::GetInterfacesNumber()
{
	libusb_config_descriptor* config = GetActiveConfigDescriptor();
	if (NULL == config) return 0;
	return config->bNumInterfaces;
}

void Usb::AudioDevice::ReleaseAllInterfaces()
{
#ifdef ENABLE_AUDIO_INTERFACES
	if (audioControlInterface)
		audioControlInterface->Release();
	for (int i=0; i<audioStreamingInterfaces.size(); i++)
		audioStreamingInterfaces[i]->Release();
#endif
	for (int i=0; i<midiInterfaces.size(); i++)
		midiInterfaces[i]->Release();
}


bool Usb::AudioDevice::InitAudioAndMidiStreamingInterfaces()
{
	if (NULL == GetActiveConfigDescriptor()) {
	   	LOGE("USBAUDIO - Error initiating audiostreaming interfaces.");
	   	//ASSERT(false);
		return false;
	}

    for (int k=0 ; k < GetInterfacesNumber() ; k++)
    {
    	libusb_interface interface = GetActiveConfigDescriptor()->interface[k];
    	libusb_interface_descriptor interface_desc = interface.altsetting[0];

		if (USB_CLASS_AUDIO != interface_desc.bInterfaceClass) {
			std::stringstream ss;
			ss << "Non-Audio interface. Class: " << (unsigned)interface_desc.bInterfaceClass
			   << " Subclass: " << (unsigned)interface_desc.bInterfaceSubClass;
			unsupportedInterfaces.push_back(ss.str());
			continue;
		}

		switch (interface_desc.bInterfaceSubClass) {
			case USB_SUBCLASS_AUDIOCONTROL:
				// we already initialized it.
				break;
			case USB_SUBCLASS_AUDIOSTREAMING:
				LOGI("USBAUDIO - Init device interfaces: AUDIO STREAMING interface found");
				#ifdef ENABLE_AUDIO_INTERFACES
				audioStreamingInterfaces.push_back(new AudioStreamingInterface(this,interface));
				#endif
				break;
			case USB_SUBCLASS_MIDISTREAMING:
				midiInterfaces.push_back(new MidiInterface(this,interface));
				break;
			case USB_SUBCLASS_UNDEFINED:
				LOGE("USBAUDIO - Init device interfaces: UNDEFINED interface found");
				unsupportedInterfaces.push_back("Undefined audio interface");
				break;
			default:
				LOGE("USBAUDIO - Init device interfaces: ERROR - not an audio standard interface");
				unsupportedInterfaces.push_back("Non-standard audio interface");
				break;
		}
	} // end of interfaces for loop
	return true;
}





#ifdef ENABLE_AUDIO_INTERFACES


bool Usb::AudioDevice::HasAudioStreamingInterfaces()
{
	return ( 0 != audioStreamingInterfaces.size() );
}

bool Usb::AudioDevice::ClaimStreamingInterface(uint8_t index, uint8_t altsetting)
{
	AudioStreamingInterface* asi = FindASInterfaceByIndex(index);
	if (NULL==asi) {
		LOGE("USBAUDIO - No interface with %u index to be claimed", index);
		return false;
	}
	if (!asi->Claim())
		return false;
	return asi->SelectAlternateSetting(altsetting);
}

std::vector<uint32_t> Usb::AudioDevice::GetAllowedSampleRates(uint8_t interface, uint8_t altSetting)
{
	std::vector<uint32_t> empty;
	AudioStreamingInterface* asi = FindASInterfaceByIndex(interface);
	if (NULL==asi) {
		LOGE("USB device get allowed sample rates error. no such interface");
		return empty;
	}
	return asi->GetAllowedSampleRates(altSetting);
}

std::vector<int> Usb::AudioDevice::GetAllowedFramesPerBuffer(uint8_t interface, uint8_t altSetting, uint32_t sr)
{
	std::vector<int> empty;
	AudioStreamingInterface* asi = FindASInterfaceByIndex(interface);
	if (NULL==asi) {
		LOGE("USB device get allowed FPB error. no such interface");
		return empty;
	}
	return asi->GetAllowedFramesPerBuffer(altSetting,sr);
}



bool Usb::AudioDevice::ClaimControlInterface(uint8_t altSetNum)
{
	if (!audioControlInterface || !audioControlInterface->Claim())
		return false;
	return true; //audioControlInterface->SelectAlternateSetting(altSetNum);
}




Usb::AudioControlInterface* Usb::AudioDevice::GetControlInterface()
{
	if (!audioControlInterface)
		return NULL;
	ClaimControlInterface();
	return audioControlInterface; // consider returning null if claim fails
}


Usb::AudioStreamingInterface* Usb::AudioDevice::GetFirstOutInterface()
{
	for (int i=0; i<audioStreamingInterfaces.size(); i++)
	{
		if (audioStreamingInterfaces[i]->IsOutput()) {
			return audioStreamingInterfaces[i];
		}
	}
	return NULL;
}

Usb::AudioStreamingInterface* Usb::AudioDevice::GetStreamingInterface(uint8_t itf)//, uint8_t altset)
{
	//if (ClaimStreamingInterface(itf,altset))
		return FindASInterfaceByIndex(itf);
	//else
	//	return NULL;
}


Usb::AudioStreamingInterface* Usb::AudioDevice::GetFirstInInterface()
{
	for (int i=0; i<audioStreamingInterfaces.size(); i++)
	{
		if (audioStreamingInterfaces[i]->IsInput()) {
			return audioStreamingInterfaces[i];
		}
	}
	return NULL;
}


Usb::AudioStreamingInterface* Usb::AudioDevice::FindASInterfaceByIndex(uint8_t index)
{
	for (int i=0; i<audioStreamingInterfaces.size(); i++) {
		if (audioStreamingInterfaces[i]->GetIndex() == index)
			return audioStreamingInterfaces[i];
	}
	return NULL;
}

std::vector<Usb::AudioStreamingInfo> Usb::AudioDevice::GetOutStreamingInterfacesInfo()
{
	//LOGD("USBAUDIO - Get out streaming interfaces info");
	std::vector<AudioStreamingInfo> infos;

	for(int i=0; i<audioStreamingInterfaces.size(); i++) {
		if (audioStreamingInterfaces[i]->IsInput())
			continue;
		std::vector<AudioStreamingInfo> altsetinfos = audioStreamingInterfaces[i]->GetInfos();
		infos.insert(infos.end(),altsetinfos.begin(),altsetinfos.end());
	}
	return infos;
}

std::vector<Usb::AudioStreamingInfo> Usb::AudioDevice::GetInStreamingInterfacesInfo()
{
	//LOGD("USBAUDIO - Get in streaming interfaces info");
	std::vector<AudioStreamingInfo> infos;

	for(int i=0; i<audioStreamingInterfaces.size(); i++) {
		if (audioStreamingInterfaces[i]->IsOutput())
			continue;
		std::vector<AudioStreamingInfo> altsetinfos = audioStreamingInterfaces[i]->GetInfos();
		infos.insert(infos.end(),altsetinfos.begin(),altsetinfos.end());
	}
	return infos;
}


bool Usb::AudioDevice::HasAudioCapabilities()
{
	libusb_config_descriptor* config = GetActiveConfigDescriptor();
	if (NULL==config) return false;

	for (int k=0 ; k < config->bNumInterfaces ; k++)
	{
		libusb_interface interface = config->interface[k];
	    for (int j=0; j<interface.num_altsetting; j++)
	    {
	    	libusb_interface_descriptor interface_desc = interface.altsetting[j];
	    	if (USB_CLASS_AUDIO==interface_desc.bInterfaceClass) {
	    		return true;
	    	}
	    }
	}
	return false;
}



bool Usb::AudioDevice::InitAudioControlInterface()
{
	if (NULL == GetActiveConfigDescriptor()) {
	   	LOGE("USBAUDIO - Error initiating audio control interface.");
	   	//ASSERT(false);
		return false;
	}

    for (int k=0 ; k < GetInterfacesNumber() ; k++)
    {
    	libusb_interface interface = GetActiveConfigDescriptor()->interface[k];
    	libusb_interface_descriptor interface_desc = interface.altsetting[0];

		if (USB_CLASS_AUDIO != interface_desc.bInterfaceClass) {
			LOGE("USBAUDIO - in InitAudioControlInterface: not an audio interface");
		}

		if (interface_desc.bInterfaceSubClass == USB_SUBCLASS_AUDIOCONTROL) {
			LOGI("USBAUDIO - Init device interfaces: AUDIO CONTROL interface found");
			if (NULL == audioControlInterface) {
				audioControlInterface = new AudioControlInterface(this,interface);
			}
			else {
				LOGD("USBAUDIO - already found an audiocontrol interface, discarding this one");
				unsupportedInterfaces.push_back("Additional Audio Control Interface");
			}
		}
	} // end of interfaces for loop
    //ASSERT(audioControlInterface!=NULL);
	return audioControlInterface!=NULL;
}




bool Usb::AudioDevice::IsStreamingActive()
{
	for (int i=0; i<audioStreamingInterfaces.size(); i++) {
		if (audioStreamingInterfaces[i]->IsStreamOpen())
			return true;
	}
	return false;
}

#endif

std::string Usb::AudioDevice::DumpInfo()
{
	libusb_device_descriptor desc = Usb::GetDeviceDescriptor(devh);

	std::stringstream os;
	os << "USB AUDIO DEVICE : \n";
	os << "Manufacturer: " << GetVendorName() << std::endl;
	os << "Product: " << GetProductName() << std::endl;
	os << "ID: " << std::hex << (unsigned)desc.idVendor << ":" << (unsigned)desc.idProduct << std::dec << std::endl;
	os << "Usb version: " << GetUsbVersion() << std::endl;
	os << "Max Current: " << GetMaxCurrent() << std::endl;
	LOGD("%s",os.str().c_str()); // to logcat


#ifdef ENABLE_AUDIO_INTERFACES
	if (audioControlInterface)
		os << audioControlInterface->DumpInfo();

	for (int i=0; i < audioStreamingInterfaces.size(); i++)
		os << audioStreamingInterfaces[i]->DumpInfo();
#endif
	for (int i=0; i < midiInterfaces.size(); i++)
			os << midiInterfaces[i]->DumpInfo();
	for (int i=0; i < unsupportedInterfaces.size(); i++)
		os << "[!] " << unsupportedInterfaces[i] << std::endl;
	return os.str();
}

