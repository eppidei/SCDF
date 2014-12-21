/*
 * UsbAudioInterfaces.cpp
 *
 *  Created on: 14/gen/2014
 *      Author: athos
 */

#include "UsbAudioInterfaces.h"
#include "Logging.h"
#include "UsbAudioDevice.h"
//#include "stdafx.h"
//#include "UIServices.h"
#include <bitset>
#include <cmath>

Usb::AudioInterface::AudioInterface(AudioDevice* audioDev,libusb_interface itf)
{
	interface=itf;
	aDev=audioDev;
	currSetting=0;
}


Usb::AudioInterface::~AudioInterface()
{
	for (int i=0; i<settings.size(); i++)
		delete settings[i];
}

libusb_device_handle* Usb::AudioInterface::Devh()
{
	return aDev->GetDeviceHandle();
}


int Usb::AudioInterface::GetIndex()
{
	if (interface.num_altsetting==0) return -1; // invalid
	return interface.altsetting[0].bInterfaceNumber;
	// return settings[0]->GetInterfaceIndex();
}

bool Usb::AudioInterface::DetachAndClaim()
{
	int itfNum = GetIndex();
	LOGI("USBAUDIO - DETACH and CLAIM Audio Interface (index %d)",itfNum);
	if (-1==itfNum)
		return false;
	int rc;
	if (1==libusb_kernel_driver_active(Devh(),itfNum)) {
		LOGI("USBAUDIO - There's a Kernel driver active on interface %d... detaching",itfNum);
		rc = libusb_detach_kernel_driver(Devh(),itfNum);
		if(0!=rc) {
			LOGE("USBAUDIO - Error detaching kernel driver");
			return false;
			// TODO: driver detaching error are of the same tipe than the ones for claim_interface and alt_settings?
		}
	}
	rc = libusb_claim_interface(Devh(), itfNum);
	if (rc < 0) {
		LOGE("USBAUDIO - Error claiming interface %d: %s\n", itfNum,libusb_error_name(rc));
		return false;
	}
	return true;
}

bool Usb::AudioInterface::Claim()
{
	int itfNum = GetIndex();
	//LOGI("USBAUDIO - CLAIM Audio Interface (index %d)",itfNum);
	if (-1==itfNum) return false;
	int rc;
	rc = libusb_claim_interface(Devh(), itfNum);
	if (rc < 0) {
		LOGE("USBAUDIO - Error claiming interface %d: %s\n", itfNum,libusb_error_name(rc));
		return false;
	}
	return true;
}

bool Usb::AudioInterface::Release()
{
	int itfNum = GetIndex();
	//LOGD("USBAUDIO - RELEASE Audio Interface (index %d)",itfNum);
	if (-1==itfNum) return false;

	int rc = libusb_release_interface(Devh(),itfNum);
	if (0!=rc) {
		LOGE("USBAUDIO - Error releasing interface %d",itfNum,libusb_error_name(rc));
		return false;
	}
	return true;
}

bool Usb::AudioInterface::SelectAlternateSetting(int altSetNum)
{
	//LOGI("USBAUDIO - Select alternate setting %d",altSetNum);
	if (altSetNum>=settings.size()) {
		LOGE("USBAUDIO Requested %d setting not present.",altSetNum);
		return false;
	}
	int rc = libusb_set_interface_alt_setting(Devh(),GetIndex(),altSetNum);
	if (rc < 0) {
		LOGE("USBAUDIO Error selecting alternate setting %d: %s\n",altSetNum,libusb_error_name(rc));
		return false;
	}
	currSetting=altSetNum;
	return true;
}


Usb::AlternateSetting::AlternateSetting(libusb_interface_descriptor desc, AudioDevice* _aDev)
{
	aDev=_aDev;
	descriptor=desc;
	isValid=false;
	isZeroBandwidth=false; // we assume it's not zerobw, and set this to true when we understand it is
}

libusb_device_handle* Usb::AlternateSetting::Devh()
{
	return aDev->GetDeviceHandle();
}

uint8_t Usb::AlternateSetting::GetInterfaceIndex()
{
	return descriptor.bInterfaceNumber;
}


uint8_t Usb::AlternateSetting::GetIndex()
{
	return descriptor.bAlternateSetting;
}

bool Usb::AlternateSetting::IsUAC2()
{
	return (descriptor.bInterfaceProtocol == USB_AUDIO_CLASS_2);
}


#ifdef ENABLE_AUDIO_INTERFACES


/*

AUDIO CONTROL INTERFACE:

control endpoint: 0
una sola, ma possibili pi� alternate settings

status interrupt endpoint (optional)
- stato delle unit
- pacchetti di 2 byte (bStatusType, bOriginator: 0 per Audio Control)
- se lo si riceve, mandare una Get Status Request (o una Get Memory Request, vedi bit 6 di bStatusType)

*/


Usb::AudioControlInterface::AudioControlInterface(AudioDevice* audioDev,libusb_interface itf)
	: AudioInterface(audioDev,itf)
{
	//LOGD("USBAUDIO - Audio CONTROL Interface constructor");

	for (int i=0; i<interface.num_altsetting; i++)
	{
		settings.push_back(new AC_AlternateSetting(interface.altsetting[i],aDev) );
	}
	DetachAndClaim();
	SelectAlternateSetting(0);
}

Usb::AC_AlternateSetting* Usb::AudioControlInterface::GetCurrentSetting()
{
	return (AC_AlternateSetting*)settings[currSetting];
}

Usb::AC_AlternateSetting* Usb::AudioControlInterface::GetSetting(int i)
{
	return (AC_AlternateSetting*)settings[i];
}

bool Usb::AudioControlInterface::UnMuteAll()
{
	//LOGD("USBAUDIO - audio control interface UnMuteAll");

	return GetCurrentSetting()->UnMuteAll();
}

Usb::ClockSource* Usb::AudioControlInterface::FindClockSourceFromTerminalID(uint8_t termID)
{
	// THIS function makes sense only for UAC2, otherwise there's no ClockSource entity!
	//LOGD("USBAUDIO - Find clock source associated to terminal with id %d",termID);
	AC_AlternateSetting* sett = GetCurrentSetting();
	for (int i=0; i<sett->inputTerminals.size(); i++)
	{
		if (sett->inputTerminals[i]->GetID() == termID)
		{
			//LOGD("USBAUDIO - Find clock source... found suitable input terminal!");
			uint8_t csID = sett->inputTerminals[i]->GetClockSourceID(); // may be of a source or a selector
			for (int k=0; k<sett->clockSources.size(); k++)
			{
				if (sett->clockSources[k]->GetID()==csID) {
					//LOGD("USBAUDIO - Find clock source... found clock source!");
					return sett->clockSources[k];
				}
			}
			for (int k=0; k<sett->clockSelectors.size(); k++)
			{
				if (sett->clockSelectors[k]->GetID()==csID)
				{
					uint8_t currentCsID = sett->clockSelectors[k]->GetCurrentSource();
					//LOGD("USBAUDIO - Found clock selector! Current source: %u",currentCsID);
					for (int j=0; j<sett->clockSources.size(); j++)
					{
						if (sett->clockSources[j]->GetID()==currentCsID) {
							//LOGD("USBAUDIO - Find clock source... found clock source!");
							return sett->clockSources[j];
						}
					}
				}
			}
		}
	}

	for (int i=0; i<sett->outputTerminals.size(); i++)
	{
		if (sett->outputTerminals[i]->GetID() == termID)
		{
			//LOGD("USBAUDIO - Find clock source... found suitable output terminal!");
			uint8_t csID = sett->outputTerminals[i]->GetClockSourceID();
			for (int k=0; k<sett->clockSources.size(); k++)
			{
				if (sett->clockSources[k]->GetID()==csID) {
					//LOGD("USBAUDIO - Find clock source... found clock source!");
					return sett->clockSources[k];
				}
			}
			for (int k=0; k<sett->clockSelectors.size(); k++)
			{
				if (sett->clockSelectors[k]->GetID()==csID)
				{
					uint8_t currentCsID = sett->clockSelectors[k]->GetCurrentSource();
					//LOGD("USBAUDIO - Found clock selector! Current source: %u",currentCsID);
					for (int j=0; j<sett->clockSources.size(); j++)
					{
						if (sett->clockSources[j]->GetID()==currentCsID) {
							//LOGD("USBAUDIO - Find clock source... found clock source!");
							return sett->clockSources[j];
						}
					}
				}
			}
		}
	}

	// at this point, if we didn't find anything, the only remaining option is that the
	// clock indicated in the terminal is

	//LOGD("USBAUDIO - didn't find any clock source");
	return NULL;
}




tstring Usb::AudioControlInterface::DumpInfo()
{
	stringformatter os;
	os << "AUDIO CONTROL interface (index " << GetIndex() << "):\n";
	LOGD(os.str().c_str());
	for (int i=0; i<settings.size(); i++)
		os << settings[i]->DumpInfo();
	return os.str();
}


Usb::AC_AlternateSetting::AC_AlternateSetting(libusb_interface_descriptor desc, AudioDevice* _aDev)
	: AlternateSetting(desc,_aDev)
{
	header=NULL;
	isValid = CreateEntities();
}


Usb::AC_AlternateSetting::~AC_AlternateSetting()
{
	CleanupEntities();
}


bool Usb::AC_AlternateSetting::CreateEntities()
{
	int totSize = descriptor.extra_length;
	uint8_t const* acDescBuf = descriptor.extra;
	uint8_t protocol = descriptor.bInterfaceProtocol;

	if (totSize==0) {
		LOGW("This AC alternate setting has zero bandwidth");
		isZeroBandwidth = true;
		return true;
	}

	while (totSize >= 2*sizeof(u_int8_t) )
	{
		uint8_t len = acDescBuf[0];		// 0: offset for length in a descriptor chunk
		uint8_t type = acDescBuf[1];	// 1: offset for the type
		uint8_t subtype = acDescBuf[2];	// 2: offset for the SUB-type

		if (len < 2) {  // too short chunk is sign of corruption
			LOGE("USBAUDIO - UAC descriptors setup: too short audio control entity descriptor chunk.");
			totSize-=len;
			acDescBuf+=len;
			continue;
		}

		if ( DESC_TYPE_INTERFACE != type) {
			LOGE("USBAUDIO - UAC descriptors setup: this is not an interface related descriptor!");
			return false;
		}

		switch (subtype) {
			case UAC_DESC_UNDEFINED:
				LOGE("USBAUDIO - AC descriptors setup: UNDEFINED descriptor");
				break;
			case UAC_DESC_HEADER:
				//LOGI("USBAUDIO - Found Audio Control HEADER descriptor. protocol: %x",protocol);
				header = new AudioControlHeader(acDescBuf,Devh(),protocol);
				break;
			case UAC_DESC_INPUT_TERMINAL:
				//LOGI("USBAUDIO - Found Audio Control INPUT TERMINAL descriptor. protocol: %x",protocol);
				inputTerminals.push_back( new InputTerminal(acDescBuf,Devh(),protocol) );
				break;
			case UAC_DESC_OUTPUT_TERMINAL:
				//LOGI("USBAUDIO - Found Audio Control OUTPUT TERMINAL descriptor. protocol: %x",protocol);
				outputTerminals.push_back(new OutputTerminal(acDescBuf,Devh(),protocol));
				break;
			case UAC_DESC_FEATURE_UNIT:
				LOGI("USBAUDIO - Found Audio Control FEATURE UNIT descriptor. protocol: %x",protocol);
				featureUnits.push_back(new FeatureUnit(acDescBuf,Devh(),GetInterfaceIndex(),protocol));
				break;
			case UAC2_DESC_CLOCK_SOURCE:
				//LOGI("USBAUDIO - Found Audio Control CLOCK SOURCE descriptor. protocol: %x",protocol);
				if (protocol==USB_AUDIO_CLASS_2)
					clockSources.push_back(new ClockSource(acDescBuf,Devh(),GetInterfaceIndex()));
				break;
			case UAC2_DESC_CLOCK_SELECTOR:
				//LOGI("USBAUDIO - Found Audio Control CLOCK SELECTOR descriptor. protocol: %x",protocol);
				if (protocol==USB_AUDIO_CLASS_2)
					clockSelectors.push_back(new ClockSelector(acDescBuf,Devh(),GetInterfaceIndex()));
				break;
			default:
				LOGE("USBAUDIO - UNSUPPORTED Audio Control Interface  entity found.");
				unsupportedEntities.push_back(new UnsupportedEntity(acDescBuf,Devh(),protocol));
				break;
				// NB: if one day you add processing, effect or extension unit,
				// be careful about the mismatch between UAC1 and UAC2
		}
		totSize-=len;
		acDescBuf+=len;
	}
	return true;
}


void Usb::AC_AlternateSetting::CleanupEntities()
{
	delete header;
	header = NULL;
	for (int i=0; i<inputTerminals.size(); i++)
		delete inputTerminals[i];
	for (int i=0; i<outputTerminals.size(); i++)
		delete outputTerminals[i];
	for (int i=0; i<featureUnits.size(); i++)
			delete featureUnits[i];
	for (int i=0; i<unsupportedEntities.size(); i++)
			delete unsupportedEntities[i];
	for (int i=0; i<clockSources.size(); i++)
		delete clockSources[i];
	for (int i=0; i<clockSelectors.size(); i++)
			delete clockSelectors[i];

	inputTerminals.clear();
	outputTerminals.clear();
	featureUnits.clear();
	clockSources.clear();
	clockSelectors.clear();
	unsupportedEntities.clear();

	isValid=false;
}


bool Usb::AC_AlternateSetting::UnMuteAll()
{
	if (featureUnits.size()==0) {
		return false;
	}
	//LOGD("USBAUDIO - AC_AlternateSetting unmute all");
	for (int i=0; i<featureUnits.size(); i++) {
		for(int k=0; k<featureUnits[i]->GetNumChannels(); k++) {
			if (featureUnits[i]->ChannelHasControl(k,FU_CTRL_MUTE)) {
				featureUnits[i]->SetControlCurr(k,FU_CTRL_MUTE,0);
			}
		}
	}
	return true;
}

tstring Usb::AC_AlternateSetting::DumpInfo()
{
	stringformatter os;
	os << "  Audio Control ALTERNATE SETTING " << (unsigned)GetIndex() << ":\n";
	LOGD(os.str().c_str());
	for (int i=0; i<inputTerminals.size(); i++)
		os << inputTerminals[i]->DumpInfo();
	for (int i=0; i<outputTerminals.size(); i++)
		os << outputTerminals[i]->DumpInfo();
	for (int i=0; i<featureUnits.size(); i++)
		os << featureUnits[i]->DumpInfo();
	for (int i=0; i<clockSources.size(); i++)
			os << clockSources[i]->DumpInfo();
	for (int i=0; i<clockSelectors.size(); i++)
		os << clockSelectors[i]->DumpInfo();
	for (int i=0; i<unsupportedEntities.size(); i++)
		os << unsupportedEntities[i]->DumpInfo();
	return os.str();
}


int Usb::AudioStreamingInterface::numOpenStreams=0;
void Usb::AudioStreamingInterface::DecreaseNumOpenStreams() { numOpenStreams--; }
void Usb::AudioStreamingInterface::IncreaseNumOpenStreams() { numOpenStreams++; }
bool Usb::AudioStreamingInterface::IsAnyStreamOpen() { return numOpenStreams>0; }

Usb::AudioStreamingInterface::AudioStreamingInterface(AudioDevice* audioDev,libusb_interface itf)
	: AudioInterface(audioDev,itf), framesPerBuffer(0), numBuffers(0), audioContext(NULL),
	  currentTransfer(-1),audioBuffer(NULL),transfers(NULL),inputIntermediateBuffer(NULL),
	  streamActive(false), errorContext(NULL), numEnqueuedTransfers(0), askedToStopStreaming(false),
	  transferredFramesCount(0),numActiveChannels(-1)
{
	audioCallback=NULL;
	errorCallback=NULL;

	for (int i=0; i<interface.num_altsetting; i++)
	{
		settings.push_back(new AS_AlternateSetting(interface.altsetting[i],aDev) );
	}
	currSetting=0; // by default, the first alternate setting is selected
	DetachAndClaim();
}


Usb::AudioStreamingInterface::~AudioStreamingInterface()
{
	//dismissStreamEvent.Close();
}

bool Usb::AudioStreamingInterface::SetSampleRate(uint32_t sr)
{
	if (IsAnyStreamOpen()) {
		LOGE("USBAUDIO - There's a stream open! Can't set Sample Rate");
		return false;
	}
	return GetCurrentSetting()->SetSampleRate(sr);
}

std::vector<uint32_t> Usb::AudioStreamingInterface::GetAllowedSampleRates() // allowed SRs for current setting
{
	bool getCached = IsAnyStreamOpen();
	return GetCurrentSetting()->GetAllowedSampleRates(getCached);
}

std::vector<uint32_t> Usb::AudioStreamingInterface::GetAllowedSampleRates(uint8_t altSetting)
// allowed SRs for ANY setting
{
	bool getCached = IsAnyStreamOpen();
	AS_AlternateSetting* set = FindSettingById(altSetting);
	if (set==NULL) {
		return std::vector<uint32_t>();
	}
	return set->GetAllowedSampleRates(getCached);
}

uint32_t Usb::AudioStreamingInterface::GetSampleRate()
{
	if (IsAnyStreamOpen()) {
		LOGW("USBAUDIO - GetSampleRate: stream active, returning cached value");
		return cachedSampleRate;
	}
	cachedSampleRate = GetCurrentSetting()->GetSampleRate();
	//LOGD("Cached SR for %s interface: %u",IsOutput()?"OUT":"IN",cachedSampleRate);
	return cachedSampleRate;
}

Usb::AS_AlternateSetting* Usb::AudioStreamingInterface::GetCurrentSetting()
{
	return (AS_AlternateSetting*)settings[currSetting];
}

Usb::AS_AlternateSetting* Usb::AudioStreamingInterface::GetSetting(int i)
{
	return (AS_AlternateSetting*)settings[i];
}

Usb::AS_AlternateSetting* Usb::AudioStreamingInterface::FindSettingById(uint8_t id)
{
	for (int i=0; i<settings.size(); i++) {
		if (GetSetting(i)->GetIndex()==id)
			return GetSetting(i);
	}
	LOGE("USBAUDIO - Find setting error: no such id!");
	return NULL;
}

bool Usb::AudioStreamingInterface::IsOutput()
{
	for (int i=0; i<settings.size(); i++) {
		if (!GetSetting(i)->IsZeroBandwidth() && GetSetting(i)->GetAudioEndpoint()) {
			return GetSetting(i)->GetAudioEndpoint()->IsOutput();
		}
	}
	return false;
}

bool Usb::AudioStreamingInterface::IsInput()
{
	for (int i=0; i<settings.size(); i++) {
		if (!GetSetting(i)->IsZeroBandwidth() && GetSetting(i)->GetAudioEndpoint()) {
			return GetSetting(i)->GetAudioEndpoint()->IsInput();
		}
	}
	return false;
}

bool Usb::AudioStreamingInterface::SetCallback(AudioDriverCallback newCallback, void* aCtx)
{
	audioCallback=newCallback;
	audioContext=aCtx;
	return true;
}

bool Usb::AudioStreamingInterface::SetErrorCallback(ErrorCallback newErrCallback, void* errCtx)
{
	errorCallback=newErrCallback; // NBB: will be called from the audio thread!!!
	errorContext=errCtx;
	return true;
}


std::vector<Usb::AudioStreamingInfo> Usb::AudioStreamingInterface::GetInfos()
{
	std::vector<Usb::AudioStreamingInfo> infos;

	for (int i=0; i<settings.size(); i++) {

		if (!GetSetting(i)->IsZeroBandwidth() && GetSetting(i)->GetAudioEndpoint()) {

			AudioStreamingInfo asi = GetSetting(i)->GetStreamingInfo();

			if (asi.numBits<16 || (IsOutput() && asi.numChannels<2) ) {
				LOGW("USBAUDIO - 8-bit or mono out interface... won't show it!");
				continue; // DON'T INCLUDE 8 BITS or MONO-OUT INTERFACES!!!
			}

			if (IsUAC2()) {
				asi.sampleRate = GetSampleRate();
			}

			if(GetSetting(i)==GetCurrentSetting()) {
				asi.framesPerBuffer = framesPerBuffer;
				asi.numBuffers = numBuffers;
				asi.streamingBroken = IsStreamingBroken();
			}

			infos.push_back( asi );
		}
	}
	return infos;
}


Usb::AudioStreamingInfo Usb::AudioStreamingInterface::GetStreamingInfo()
{
	AudioStreamingInfo asi;
	if (GetCurrentSetting()->IsZeroBandwidth() || !GetCurrentSetting()->GetAudioEndpoint()) {
		LOGE("Asked streaming settings for a non audio alternate setting");
		asi.streamingBroken=true;
		asi.framesPerBuffer=asi.bytesPerSample=asi.numBits=0;
		asi.numChannels=0; asi.numActiveChannels=-1;
		return asi;
	}

	asi = GetCurrentSetting()->GetStreamingInfo();
	if (IsUAC2())
		asi.sampleRate = GetSampleRate();
	asi.framesPerBuffer = GetFramesPerBuffer();
	asi.numBuffers = numBuffers;
	asi.numActiveChannels = numActiveChannels;
	asi.streamingBroken = IsStreamingBroken();
	return asi;
}


Usb::AudioStreamingInfo Usb::AudioStreamingInterface::GetStreamingInfo(int setting)
{
	AudioStreamingInfo asi;
	if (GetSetting(setting)->IsZeroBandwidth() || !GetSetting(setting)->GetAudioEndpoint()) {
		LOGE("Alternate Setting is non-audio: void streaming info");
		asi.streamingBroken=true;
		asi.framesPerBuffer=asi.bytesPerSample=asi.numBits=0;
		asi.numChannels=0; asi.numActiveChannels=-1;
		return asi;
	}

	asi = GetSetting(setting)->GetStreamingInfo();

	if (GetSetting(setting)!=GetCurrentSetting()) {
		return asi;
	}

	// if this is the current setting, add streaming state informations:
	if (IsUAC2())
		asi.sampleRate = GetSampleRate();
	asi.framesPerBuffer = GetFramesPerBuffer();
	asi.numBuffers = numBuffers;
	asi.numActiveChannels = numActiveChannels;
	asi.streamingBroken = IsStreamingBroken();
	return asi;
}


/*
int Usb::AudioStreamingInterface::GetFramesPerBufferBaseValue()
// ONLY MEANINGFUL FOR USB 1.1 FULL SPEED ENDPOINTS!
{
	// for the input this shouldn't be necessary, since we are allocating a max size buffer anyway.
	// but we setup this like this as well, just to keep things equal in out and in.

	uint32_t sr = GetSampleRate();
	uint32_t chans = GetCurrentSetting()->GetNumChannels();
	//uint32_t bits = GetCurrentSetting()->GetBitsResolution();
	uint32_t samplesize = GetCurrentSetting()->GetBytesPerSample();
	//uint8_t interval = GetCurrentSetting()->GetAudioEndpoint()->GetPollingInterval();

	uint16_t packetsize = (sr/1000)*chans*samplesize;

	if (packetsize > GetCurrentSetting()->GetAudioEndpoint()->GetMaxPacketSize()) {
		LOGE("USBAUDIO - Error, base value for framesPerBuffer is too big for the MaxPacketSize!");
		return -1;
	}

	int fpbBase = packetsize/(chans*samplesize);

	//LOGD("USBAUDIO - FramesPerBufferBaseValue:%d (packsize: %u, sr:%u, chans:%u, samplesize:%u)",
	//		fpbBase,packetsize,sr,chans,samplesize);

	return fpbBase;
}*/

double Usb::AudioStreamingInterface::CalculateIdealAverageFramesPerPacket(uint32_t sr, int bInterval)
{
	int interval = pow(2,bInterval-1);
	double res=0.0;
	if (Usb::GetSpeed(Devh())>=HighSpeed)
		res = ( ((double)sr)/8000.0)*interval; // interval is in subframes
	else
		res = (((double)sr)/1000.0)*interval; // interval is in frames
	//LOGD("USBAUDIO - Calc ideal average fpp. binterval: %d, interval: %d, idealfpp:%f",bInterval,interval,res);
	return res;
}

double Usb::AudioStreamingInterface::GetIdealAverageFramesPerPacket(bool fromCallback)
{
	uint32_t sr = fromCallback ? cachedSampleRate : GetSampleRate();
	//LOGD("USBAUDIO - get %s ideal average frames per packet. sr: %u",IsOutput()?"OUT":"IN",sr);
	int bInterval = GetCurrentSetting()->GetAudioEndpoint()->GetPollingInterval();
	return CalculateIdealAverageFramesPerPacket(sr,bInterval);
}

double Usb::AudioStreamingInterface::GetIdealAverageFramesPerPacket(uint8_t altSetting, uint32_t sr)
{
	AS_AlternateSetting* aset = FindSettingById(altSetting);
	if (!aset) {
		LOGE("USBAUDIO - GetIdealAverageFramesPerPacket error: no such alternate setting!");
		return 0.0;
	}
	int bInterval = aset->GetAudioEndpoint()->GetPollingInterval();
	return CalculateIdealAverageFramesPerPacket(sr,bInterval);
}

std::vector<int> Usb::AudioStreamingInterface::PopulateAllowedFramesPerBufferList(double idealFramesPerPacket)
{
	std::vector<int> fpbs;
	int base;
	// the smaller buffer will be of 1 ms:
	if (GetSpeed(Devh())>=HighSpeed)
		idealFramesPerPacket *= 8;
	base = (int)(idealFramesPerPacket+0.5);
	for (int i=1; i<=16; i++) {
		fpbs.push_back(base*i);
		//LOGD("USBAUDIO - Allowed fpb: %d (fpp:%f - base:%d)",fpbs.back(),idealFramesPerPacket,base);
	}
	return fpbs;
}

std::vector<int> Usb::AudioStreamingInterface::GetAllowedFramesPerBuffer()
// get list of allowed fpb for current setting (and the sr it is currently set to)
{
	//LOGD("USBAUDIO - get allowed frames per buffer...");
	//uint32_t sr = GetSampleRate();
	double fBase = GetIdealAverageFramesPerPacket(false);
	return PopulateAllowedFramesPerBufferList(fBase);
}

std::vector<int> Usb::AudioStreamingInterface::GetAllowedFramesPerBuffer(uint8_t altSetting, uint32_t sampRate)
// get list of allowed fpb for any setting, any sample rate
{
	double fBase = GetIdealAverageFramesPerPacket(altSetting,sampRate);
	return PopulateAllowedFramesPerBufferList(fBase);
}

bool Usb::AudioStreamingInterface::IsFramesPerBufferAllowed(int fpb)
{
	std::vector<int> fpbs = GetAllowedFramesPerBuffer();
	for (int i=0; i<fpbs.size(); i++)
		if (fpbs[i]==fpb)
			return true;
	return false;
}

bool Usb::AudioStreamingInterface::SetFramesPerBuffer(int fpb)
{
	if (GetAllowedFramesPerBuffer().empty()) {
		LOGE("USBAUDIO - SetFramesPerBuffer error... no allowed values!");
		framesPerBuffer=0;
		return false;
	}

	if (!IsFramesPerBufferAllowed(fpb)) {
		LOGW("USBAUDIO - Error! FramesPerBuffer %d not allowed. Setting a close one",fpb);
		std::vector<int> fpbs = GetAllowedFramesPerBuffer();
		int closest=0;
		int minDist = (fpbs[0]-fpb)>=0 ? (fpbs[0]-fpb) : -1*(fpbs[0]-fpb);
		for (int i=1; i<fpbs.size(); i++)
		{
			int currDist = (fpbs[i]-fpb)>=0 ? (fpbs[i]-fpb) : -1*(fpbs[i]-fpb); // too lazy to include abs
			if (currDist<minDist) {
				closest = i;
				minDist = currDist;
			}
		}
		framesPerBuffer=fpbs[closest];
		return false;
	}

	framesPerBuffer=fpb;
	return true;
}

int Usb::AudioStreamingInterface::GetFramesPerBuffer()
{
	return framesPerBuffer;
}

bool Usb::AudioStreamingInterface::SetNumBuffers(int numbuf)
{
	numBuffers=numbuf;
	return true;
}


bool Usb::AudioStreamingInterface::SetupStreaming(AudioStreamingInfo& settings)
{
	//LOGD("USBAUDIO - setup streaming");
	//dismissStreamEvent.Wait(INFINITE);
	//LOGD("USBADIO - actually setup streaming");

	bool noneChanged = true;

	if(!SetSampleRate(settings.sampleRate)) {
		settings.sampleRate=GetSampleRate();
		noneChanged = false;
	}

	if(!SetFramesPerBuffer(settings.framesPerBuffer)) {
		settings.framesPerBuffer=GetFramesPerBuffer();
		noneChanged = false;
		if (framesPerBuffer==0) {
			settings.streamingBroken=true;
			return noneChanged;
		}
	}

	SetNumBuffers(settings.numBuffers);

	if (settings.numBits!=GetCurrentSetting()->GetNumBits()) {
		settings.numBits=GetCurrentSetting()->GetNumBits();
		noneChanged=false;
	}

	if (settings.numChannels!=GetCurrentSetting()->GetNumChannels()) {
		settings.numChannels=GetCurrentSetting()->GetNumChannels();
		noneChanged=false;
	}

	if (IsInput()) { // active channels only influencing input behaviour now.

		numActiveChannels = GetCurrentSetting()->GetNumChannels() < settings.numActiveChannels ?
				GetCurrentSetting()->GetNumChannels() : settings.numActiveChannels;

		if (settings.numActiveChannels!=numActiveChannels) {
			settings.numActiveChannels=numActiveChannels;
			noneChanged=false;
		}
	}

	settings.bytesPerSample=GetCurrentSetting()->GetBytesPerSample();

	if (!PrepareBuffersAndTransfers()) {
		noneChanged=false;
		settings.streamingBroken=true;
	} else {
		settings.streamingBroken=false;
	}

	if (GetCurrentSetting()->HasExplicitFeedback())
		PrepareFeedbackTransfer();

	GetAllowedSampleRates(); // just to cache the allowed sample rates before streaming starts

	return noneChanged;
}

bool Usb::AudioStreamingInterface::IsStreamingBroken()
{
	return (transfers==NULL || audioBuffer==NULL || framesPerBuffer==0); // find a better condition!
}

int Usb::AudioStreamingInterface::GetAudioBufferNominalSizeInBytes()
{
	uint32_t chans = GetCurrentSetting()->GetNumChannels();
	uint32_t bytesPerSample = GetCurrentSetting()->GetBytesPerSample();
	return framesPerBuffer*chans*bytesPerSample;
}

int Usb::AudioStreamingInterface::GetCurrentAudioBufferSizeInBytes()
{
	uint32_t chans = GetCurrentSetting()->GetNumChannels();
	uint32_t bytesPerSample = GetCurrentSetting()->GetBytesPerSample();
	return adjustedFramesPerBuffer * chans * bytesPerSample;
}

int Usb::AudioStreamingInterface::GetFramesInCurrentAudioBuffer()
{
	return adjustedFramesPerBuffer;
}

/*
USB 2.0:
Interval for polling endpoint for data transfers.
Expressed in frames or microframes depending on the device operating
speed (i.e., either 1 millisecond or 125 us units).
For full-/high-speed isochronous endpoints, this value must be in the
range from 1 to 16.
The bInterval value is used as the exponent for a 2^(bInterval-1) value;
e.g., a bInterval of 4 means a period of 8 (2^(4-1)).

USB 1.1:
Interval for polling endpoint for data transfers. Expressed in milliseconds.
This field is ignored for bulk and control endpoints. For isochronous
endpoints this field must be set to 1. For interrupt endpoints, this
field may range from 1 to 255.

*/
/*
bool Usb::AudioStreamingInterface::PrepareBuffersAndTransfers() // old, for 1.1 only
{
	//if (GetVersion(Devh())>=v20) {
		return PrepareBuffersAndTransfers_USB2();
	//}

	/*LOGD("USBAUDIO - PrepareBuffersAndTransfers for %s",IsOutput()?"OUTPUT":"INPUT");

	uint32_t chans = GetCurrentSetting()->GetNumChannels();
	uint32_t bytesPerSample = GetCurrentSetting()->GetBytesPerSample();
	int bytesize = GetAudioBufferNominalSizeInBytes();

	transfers=new libusb_transfer*[numBuffers];
	currentTransfer=0;

	audioBuffer = new uint8_t*[numBuffers];
	for (int i=0; i<numBuffers; i++) {
		audioBuffer[i] = new uint8_t[bytesize];
		memset(audioBuffer[i],0,bytesize);
		LOGI("USBAUDIO - Allocated a %d bytes buffer",bytesize);
	}

	int iso_packet_size = GetFramesPerBufferBaseValue()*chans*bytesPerSample;
	int num_packets = (framesPerBuffer*chans*bytesPerSample)/iso_packet_size;

	if (IsInput()) {
		LOGD("USBAUDIO - Input interface, preparing an intermediate buffer with max packets sizes");
		iso_packet_size = GetCurrentSetting()->GetAudioEndpoint()->GetMaxPacketSize();
		inputIntermediateBuffer = new uint8_t*[numBuffers];
		for (int i=0; i<numBuffers; i++)
			inputIntermediateBuffer[i] = new uint8_t[iso_packet_size*num_packets];
	}

	for(int i=0; i<numBuffers; i++) {
		transfers[i] = libusb_alloc_transfer(num_packets);
		if (!transfers[i]) {
			LOGE("USBAUDIO - Error allocating isochronous transfer");
			return false;
		}

		if (IsOutput()) {

			//LOGD(" USBAUDIO - Allocating OUT iso transfer. pack_size: %d, num_packs:%d",iso_packet_size,num_packets);

			libusb_fill_iso_transfer(transfers[i],
									Devh(),
									GetCurrentSetting()->GetAudioEndpoint()->GetAddress(),
									audioBuffer[i],
									iso_packet_size*num_packets,
									num_packets,
									Usb::AudioStreamingInterface::IsochronousCallback,this,1000);
		} else {

			//LOGD(" USBAUDIO - Allocating IN iso transfer. pack_size: %d, num_packs:%d",iso_packet_size,num_packets);

			libusb_fill_iso_transfer(transfers[i],
										Devh(),
										GetCurrentSetting()->GetAudioEndpoint()->GetAddress(),
										inputIntermediateBuffer[i],
										iso_packet_size*num_packets,
										num_packets,
										Usb::AudioStreamingInterface::IsochronousCallback,this,1000);
		}
		libusb_set_iso_packet_lengths(transfers[i], iso_packet_size);
	}
	return true;
}*/

/*int Lcm(int a, int b)
{
	int aa = a;	int bb = b;	int t=0;
	if (a<b) {	t=a; a=b; b=t; t=0;	}
	while (b!=0) {
       t = b;   b = a % b;	   a = t;
	} // "a" is now the GCD
	return (aa/a)*b;
}*/

// #include <limits>

bool Usb::AudioStreamingInterface::PrepareBuffersAndTransfers()
{
	//LOGD("USBAUDIO - PrepareBuffersAndTransfers for %s",IsOutput()?"OUTPUT":"INPUT");
	uint32_t chans = GetCurrentSetting()->GetNumChannels();
	uint32_t bytesPerSample = GetCurrentSetting()->GetBytesPerSample();

	double avgFramesInPacket = GetIdealAverageFramesPerPacket(false);
	adjustedFramesPerPacket=0; // set to zero, so it will be invalid until the first feedback rate is retrieved
	int wholeFramesInPacket = (int) avgFramesInPacket; // used only for out
	int numberOfPackets = (int) ( framesPerBuffer / avgFramesInPacket + 0.5); // round

	//LOGD("USBAUDIO - Prepare - chans: %u, bytesPerSample: %u, avgFramesInPacket: %f, numberOfPackets: %d",
	//		chans,bytesPerSample,avgFramesInPacket,numberOfPackets);

	//int bytesize = GetAudioBufferSizeInBytes();
	int fullTransferSize = numberOfPackets * GetCurrentSetting()->GetAudioEndpoint()->GetMaxPacketSize();
	int safeTransferSize = (int) ( numberOfPackets * avgFramesInPacket * chans * bytesPerSample * 2); // "2" is a safety factor

	// We stay SUPERSAFE. Even though we will probably never use the full buffer, we allocate
	// space as if we expect to receive/send all packets of maximum size.
	// Then, each time we submit the transfer, we will just use a part of the underlying data buffer

	transfers=new libusb_transfer*[numBuffers];
	currentTransfer=0;

	audioBuffer = new uint8_t*[numBuffers];
	for (int i=0; i<numBuffers; i++) {
		audioBuffer[i] = new uint8_t[safeTransferSize];
		memset(audioBuffer[i],0,safeTransferSize);
	}

	//LOGI("USBAUDIO - Allocated %d buffers of %d bytes",numBuffers,safeTransferSize);
	uint8_t** theBuffer;

	if (IsInput()) {
		//LOGD("USBAUDIO - INPUT => preparing %d intermediate buffers of max size (%d bytes)",numBuffers,fullTransferSize);
		inputIntermediateBuffer = new uint8_t*[numBuffers];
		for (int i=0; i<numBuffers; i++) {
			inputIntermediateBuffer[i] = new uint8_t[ fullTransferSize ];
			memset(inputIntermediateBuffer[i],0,fullTransferSize);
		}
		theBuffer = inputIntermediateBuffer;
	} else {
		// output:
		theBuffer = audioBuffer;
	}

	framesAccumulator = 0.0;

	for(int i=0; i<numBuffers; i++)
	{
		transfers[i] = libusb_alloc_transfer(numberOfPackets);
		if (!transfers[i]) {
			LOGE("USBAUDIO - Error allocating isochronous transfer");
			return false;
		}

		int theTransferActualSize;

		if (IsInput()) {

			int thePacketSize = GetCurrentSetting()->GetAudioEndpoint()->GetMaxPacketSize();
			//libusb_set_iso_packet_lengths(transfers[i], thePacketSize); // won't work before fill_iso_transfer
			for (int k=0; k<numberOfPackets; k++) {// transfers[i]->num_iso_packets is ZERO now
				transfers[i]->iso_packet_desc[k].length = thePacketSize;
				//transfers[i]->iso_packet_desc[k].actual_length = thePacketSize;//libusb_set_iso_packet_lengths(transfers[i], thePacketSize);
			}
			theTransferActualSize = fullTransferSize;
			adjustedFramesPerBuffer = framesPerBuffer;

			////LOGD("USBAUDIO - INPUT - adj fpb: %d, transf size:%d, nPacks:%d, packSize:%d, test (nPacks*packSize):%d",
			//		adjustedFramesPerBuffer,fullTransferSize,numberOfPackets,thePacketSize,numberOfPackets*thePacketSize);
		}
		else {

			adjustedFramesPerBuffer = 0;
			//double epsilon = std::numeric_limits<double>::epsilon();
			////LOGD("USBAUDIO - Out transfer %d setup. num packets:%d, avg frames: %f, whole frames: %d",
			//		i,numberOfPackets, avgFramesInPacket,wholeFramesInPacket);

			for (int k=0; k<numberOfPackets; k++) { // transfers[i]->num_iso_packets is ZERO now

				framesAccumulator += avgFramesInPacket - (double)wholeFramesInPacket;

				if (framesAccumulator < (1.0/*-epsilon*/)) {
					transfers[i]->iso_packet_desc[k].length = wholeFramesInPacket * chans * bytesPerSample;
					//transfers[i]->iso_packet_desc[k].actual_length = wholeFramesInPacket * chans * bytesPerSample;
					adjustedFramesPerBuffer+=wholeFramesInPacket;
				} else {
					transfers[i]->iso_packet_desc[k].length = (wholeFramesInPacket+1) * chans * bytesPerSample;
					//transfers[i]->iso_packet_desc[k].actual_length = (wholeFramesInPacket+1) * chans * bytesPerSample;
					adjustedFramesPerBuffer+=wholeFramesInPacket+1;
					framesAccumulator -= 1;
				}
				////LOGD("    USBAUDIO - frames per buffer after packet %d: %d. In-progress transfer size: %d",
				//		k,adjustedFramesPerBuffer,adjustedFramesPerBuffer * chans * bytesPerSample);
			}
			theTransferActualSize = adjustedFramesPerBuffer * chans * bytesPerSample;
		}

		////LOGD(" USBAUDIO - Allocating %s iso transfer. transferSize: %d, numberOfPackets:%d",
		//				IsOutput()?"OUTPUT":"INPUT",theTransferActualSize,numberOfPackets);

		libusb_fill_iso_transfer(transfers[i],
									Devh(),
									GetCurrentSetting()->GetAudioEndpoint()->GetAddress(),
									theBuffer[i],
									theTransferActualSize,
									numberOfPackets,
									IsochronousCallback,this,2000);
	} // end of transfers preparation loop

	return true;
}



void Usb::AudioStreamingInterface::ClearBuffersAndTransfers()
{
	for (int i=0; i<numBuffers; i++)
		delete [] audioBuffer[i];
	delete [] audioBuffer;
	audioBuffer=NULL;

	if (IsInput()) {
		for (int i=0; i<numBuffers; i++)
			delete [] inputIntermediateBuffer[i];
		delete [] inputIntermediateBuffer;
		inputIntermediateBuffer=NULL;
	}

	for(int i=0; i<numBuffers; i++) {
		////LOGD("USBAUDIO - free transfer[%d] for %s interface",i,IsOutput()?"OUT":"IN");
		libusb_free_transfer(transfers[i]);
	}
	delete [] transfers; //=new libusb_transfer*[numBuffers];
	transfers=NULL;

	if (GetCurrentSetting()->HasExplicitFeedback()) {
		feedbackLock.Lock();
		if (feedbackTransferGoingOn)
			askedToFreeFeedbackTransfer=true;
		else
			ClearFeedbackTransfer();
		feedbackLock.Unlock();
	}

}


bool Usb::AudioStreamingInterface::StartStreaming()
{
	//return true;
	streamActiveLock.Lock();
	//stopStreamCounter=0;
	if (streamActive) {
		LOGE("USBAUDIO - Stream already active! Won't start it again");
		streamActiveLock.Unlock();
		return false;
	}

	if (transfers==NULL) {
		LOGE("USBAUDIO - Cannot start streaming: transfers structure not prepared!");
		streamActiveLock.Unlock();
		return false;
	}

	streamActive=true;
	askedToStopStreaming=false;
	IncreaseNumOpenStreams();
	transferredFramesCount=0;

	////LOGD("USBAUDIO - StartStreaming for %s",IsOutput()?"OUTPUT":"INPUT");
	for(int i=0; i<numBuffers; i++) {
		numEnqueuedTransfers++;
		int ret = libusb_submit_transfer(transfers[i]);
		if (ret<0) {
			LOGE("USBAUDIO - Error enqueuing starting transfer %d. error %d, str:%s",i,ret,strerror(errno));
			// We need to stop the transfers that have been successfully enqueued:
			askedToStopStreaming=true;
			//streamActiveLock.Lock();
			//streamActive=false;
			streamActiveLock.Unlock();
			CheckAndHandleStopRequest();
			if (errorCallback)
				errorCallback(errorContext,ret);
			return false;
		}
		//LOGD("USBAUDIO - start streaming: enqueued transfer %d",i);
		if (IsOutput())
			transferredFramesCount+=GetFramesPerBuffer();

	}
	streamActiveLock.Unlock();
	//if (GetCurrentSetting()->HasExplicitFeedback())
	//	StartFeedbackTransfer();
	return true;
}


bool Usb::AudioStreamingInterface::IsStreamOpen()
{
	streamActiveLock.Lock();
	bool res = streamActive;
	streamActiveLock.Unlock();
	return res;
}


bool Usb::AudioStreamingInterface::CheckAndHandleStopRequest() {

	streamActiveLock.Lock();

	if (streamActive==false) { // error case!
		LOGE("USBAUDIO - Stream is not active, but transfer is still going on!");
		streamActiveLock.Unlock();
		return true;
	}

	if (askedToStopStreaming) {

		//LOGD("USBAUDIO - Stop request going on. transfers in queue: %d",numEnqueuedTransfers);
		//stopStreamCounter--;
		numEnqueuedTransfers--;
		if (numEnqueuedTransfers==0) {
			//LOGD("USBAUDIO - %s stream stop COMPLETED, destroy transfers and buffers",IsOutput()?"OUT":"IN");
			DismissStreaming();
			transferredFramesCount=0;
			streamActive=false;
			DecreaseNumOpenStreams();
		}
		streamActiveLock.Unlock();
		return true;
	}
	streamActiveLock.Unlock();
	return false;
}

bool Usb::AudioStreamingInterface::StopStreaming()
// return value: true="actually triggered stop, wait before trying to open it again!"
// 				 false="stream already inactive, you can safely restart stream if you want"
{
	//LOGD("USBAUDIO - Stop %s streaming.",IsOutput()?"OUTPUT":"INPUT");
	streamActiveLock.Lock();

	////LOGD("   USBAUDIO - Passed the lock, stop streaming");

	if(!streamActive) { // stream has been stopped by someone else (eg: error transfer)
		LOGW("USBAUDIO - Stream already stopped by someone else, doing nothing");
		streamActiveLock.Unlock();
		return false;
	}

	if(!askedToStopStreaming) {
		// counter won't be 0 if a transfer error occurred but some transfers are still going on
		// in that case we don't touch the counter and let the process of "dismantling" the stream
		// be carried on by the audio thread. Otherwise we initialize the stopStreamCounter:
		//LOGD("USBAUDIO - Initializing stream stop procedure");
		askedToStopStreaming=true;
		//stopStreamCounter=numBuffers;
		// in this way the audiothread will start the process of dismantling of the stream
		// NB: we DON'T set the streaming as inactive here, this will happen when the
		// audio thread understands it's time (no more ongoing transfers);
	}
	streamActiveLock.Unlock();
	//LOGD("   USBAUDIO - Stop streaming over");
	return true;
}

void Usb::AudioStreamingInterface::DismissStreaming()
{
	ClearBuffersAndTransfers();
	//dismissStreamEvent.Set();
}

bool Usb::AudioStreamingInterface::HandleStreamingError(int status)
{
	switch (status) {
	//case LIBUSB_TRANSFER_COMPLETED 	// NB: this doesn't mean that all the requested data was transferred.
	case LIBUSB_TRANSFER_CANCELLED:
		// Not an error (user requested). Actually we should never see this
		// (we never cancel a transfer)
		LOGW("USBAUDIO. Transfer CANCELLED. Who did this?");
		return true;
	case LIBUSB_TRANSFER_ERROR:
	case LIBUSB_TRANSFER_TIMED_OUT:
	case LIBUSB_TRANSFER_STALL:
	case LIBUSB_TRANSFER_NO_DEVICE:
	//case LIBUSB_TRANSFER_OVERFLOW: // apparently a "fake" error for isochronous streams
		if(CheckAndHandleStopRequest()==true) { // if it's the first error this will be false
			LOGE("USBAUDIO - %s Transfer error (not the first): %d",IsOutput()?"OUT":"IN",status);
			return true;
		}
		else {
			LOGE("USBAUDIO - %s Transfer error: %d. Stop the stream!",IsOutput()?"OUT":"IN",status);
			StopStreaming();
			CheckAndHandleStopRequest(); // mmm... why i need this again for first error?!?
			if (errorCallback)
				errorCallback(errorContext,status);
		}
		return true;
	}

	return false;
}


void Usb::AudioStreamingInterface::IsochronousCallback(struct libusb_transfer *xfr)
{
	unsigned int i;
	AudioStreamingInterface* as = (AudioStreamingInterface*)xfr->user_data;

	//static int kk = 0; if (kk<5) {LOGD("-> USBAUDIO - OUT iso callback"); kk++; }
	//LOGD("-> USBAUDIO - %s callback",as->IsOutput()?"OUTPUT":" INPUT");
	// DumpTransferInfo(xfr); DumpIsoPacketsInfo(xfr);

	if (as->HandleStreamingError(xfr->status) ) {
		LOGE("USBAUDIO - %s Transfer Status error.",as->IsInput()?"IN":"OUT");
		return;
	}

	//LOGD("USBAUDIO - Packets check");

	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		for (i = 0; i < xfr->num_iso_packets; i++) {
			libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			//LOGD("   > USBAUDIO OUT iso packet len:%d actual len:%d",pack->length, pack->actual_length);
			if (as->HandleStreamingError(pack->status) ) {
				LOGE("USBAUDIO - Transfer packet error: pack %u status %d\n", i, pack->status);
				return;
			}
			//printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
		}
	}

	//LOGD("USBAUDIO - current transfer check");

	for (int i=0; i<as->numBuffers; i++) {
		if (as->transfers[i] == xfr) {
			//LOGD("USBAUDIO - found current out transfer: %d (%x = %x)",i,xfr,as->transfers[i]);
			as->currentTransfer=i;
			break;
		}
	}

	//LOGD("USBAUDIO - Stop request check");

	//LOGD("USBAUDIO - out count : %d (thread:%x)",as->stopStreamCounter,pthread_self());
	if (as->CheckAndHandleStopRequest()) {
		//LOGD("USBAUDIO - handled stop step in %s callback",as->IsInput()?"IN":"OUT");
		return;
	}

	// input uses an intermediate buffer for the coming packets.
	// let's copy the samples to the audio buffer:

	//LOGD("USBAUDIO - pre-callback preparation");

	if (as->IsInput()) {

		as->adjustedFramesPerBuffer=0;
		int totalBytes = 0;
		int chans = as->GetCurrentSetting()->GetNumChannels();
		int bytesPerSample = as->GetCurrentSetting()->GetBytesPerSample();
		int channelsToCopy = as->numActiveChannels < 0 ? chans : as->numActiveChannels;
		int bytesToCopy = channelsToCopy*bytesPerSample;

#ifndef NTRACK_NEW_INPUT_SAMPLES_COPY
		for (int i=0; i<xfr->num_iso_packets; i++)
		{
			int pack_len = xfr->iso_packet_desc[i].actual_length;
			unsigned char* pack_buffer = libusb_get_iso_packet_buffer_simple(xfr,i);
			//LOGD("USBAUDIO - In Packet %d ACTUAL length: %d. buffer ptr: %d",i,pack_len,pack_buffer);
			for (int k=0; k<pack_len; k++)
			{
				//LOGD("USBAUDIO - copy byte %d, to buffer %d, position %d",k,as->currentTransfer,totalBytes);
				as->audioBuffer[as->currentTransfer][totalBytes] = pack_buffer[k];
				totalBytes++;
			}
		}
		// the transfer size must not be modified for input
#else
		for (int i=0; i<xfr->num_iso_packets; i++)
		{
			int pack_len = xfr->iso_packet_desc[i].actual_length;
			unsigned char* pack_buffer = libusb_get_iso_packet_buffer_simple(xfr,i);
			//LOGD("USBAUDIO - Packet %d ACTUAL length: %d. buffer ptr: %d",i,pack_len,pack_buffer);

			for (int k=0; k<pack_len; k+=(chans*bytesPerSample) ) // for each frame in the packet
			{
				for (int j=0; j<bytesToCopy; j++) // for each active channel...
				{
					as->audioBuffer[as->currentTransfer][totalBytes+k+j] = pack_buffer[k+j];
					// NB: we are assuming that audio frames cannot be split among packets...
				}
			}
			totalBytes = totalBytes + pack_len;
		}
		//LOGD("INPUT - Adjusted frames per buffer: %d", as->adjustedFramesPerBuffer);
		// the transfer size must not be modified for input

#endif
		as->adjustedFramesPerBuffer = totalBytes / (chans*bytesPerSample);
		//LOGI("USBAUDIO - INPUT - Adjusted frames per buffer: %d", as->adjustedFramesPerBuffer);

	} else {

		as->adjustedFramesPerBuffer = 0;
		//double epsilon = std::numeric_limits<double>::epsilon();

		double floatFramesPerPacket = as->adjustedFramesPerPacket!=0 ?
				as->adjustedFramesPerPacket : as->GetIdealAverageFramesPerPacket(true);

		int wholeFramesPerPacket = ((int)floatFramesPerPacket);
		int chans = as->GetCurrentSetting()->GetNumChannels();
		int bytesPerSample = as->GetCurrentSetting()->GetBytesPerSample();

		for (int k=0; k<xfr->num_iso_packets; k++) {

			as->framesAccumulator += floatFramesPerPacket - (double)wholeFramesPerPacket;

			if (as->framesAccumulator < (1.0/*-epsilon*/)) {
				xfr->iso_packet_desc[k].length = wholeFramesPerPacket * chans * bytesPerSample;
				//xfr->iso_packet_desc[k].actual_length = wholeFramesPerPacket * chans * bytesPerSample;
				as->adjustedFramesPerBuffer+=wholeFramesPerPacket;
			} else {
				xfr->iso_packet_desc[k].length = (wholeFramesPerPacket+1) * chans * bytesPerSample;
				//xfr->iso_packet_desc[k].actual_length = (wholeFramesPerPacket+1) * chans * bytesPerSample;
				as->adjustedFramesPerBuffer+=wholeFramesPerPacket+1;
				as->framesAccumulator -= 1;
			}
		}
		xfr->length =  as->adjustedFramesPerBuffer * chans * bytesPerSample;
		//xfr->actual_length = as->adjustedFramesPerBuffer * chans * bytesPerSample;
		//LOGD("USBAUDIO OUTPUT - Adjusted frames per buffer: %d", as->adjustedFramesPerBuffer);
		/*LOGD("USBAUDIO - Out %s frames per packet: %f    Per buffer: %d",
						as->adjustedFramesPerPacket!=0?"ADJUSTED":"NOMINAL",floatFramesPerPacket,
						as->adjustedFramesPerBuffer);*/

		// ask for an update of the frames per packet (usb samplerate) value:
		if (as->GetCurrentSetting()->HasExplicitFeedback())
			as->StartFeedbackTransfer();
	}

	//LOGD("USBAUDIO - %s adjusted frames per buffer: %d",as->IsOutput()?"OUT":"IN",as->adjustedFramesPerBuffer);
	if (NULL!=as->audioCallback) {
		as->audioCallback(as->audioContext);
	}

	//LOGD("USBAUDIO - resubmit out transfer (status: %d)",xfr->status);
	if (libusb_submit_transfer(xfr) < 0) {
		as->HandleStreamingError(LIBUSB_TRANSFER_ERROR);
		LOGE("USBAUDIO - error re-submitting %s transfer\n",as->IsInput()?"IN":"OUT");
	}

	as->transferredFramesCount += as->GetFramesInCurrentAudioBuffer();
	//LOGD("    USBAUDIO - callback over");
}


uint8_t* Usb::AudioStreamingInterface::GetAudioBuffer()
{
	if (!audioBuffer) {
		LOGE("USBAUDIO - Asked audio buffer for %s, but it's null",IsOutput()?"OUT":"IN");
		return NULL;
	}
	return audioBuffer[currentTransfer];
}



/**
 *
 *  SAMPLE RATE FEEDBACK STUFF:
 *
 */

bool Usb::AudioStreamingInterface::PrepareFeedbackTransfer()
{
	// ATTENZIONE: se il data endpoint � ASINCRONO, il feedback serve per l'output e quindi il
	// feedback endpoint � di INPUT.
	// se il data endpoint � ADATTIVO, il feedback serve per l'input e quindi il feedback endpoint
	// � di output.
	// PER ORA STIAMO GESTENDO SOLO IL CASO ASINCRONO!!!

	//LOGD("USBAUDIO - Input interface, preparing an intermediate buffer with max packets sizes");
	Endpoint* fbEp = GetCurrentSetting()->GetFeedbackEndpoint();
	if (NULL==fbEp || !fbEp->IsSynchEndpoint()) {
		LOGE("USBAUDIO - Can't setup feedback transfer, not a feedback endpoint! (ptr: %x)",fbEp);
		return false;
	}

	int packetSize = fbEp->GetMaxPacketSize();
	feedbackData = new uint8_t[packetSize]; // TODO: if output fb ep, fill this with meaningful sr data
	int numberOfPackets = 1;
	int transferSize = packetSize*numberOfPackets;

	feedbackTransfer = libusb_alloc_transfer(numberOfPackets);
	if (!feedbackTransfer) {
		LOGE("USBAUDIO - Error allocating isochronous FEEDBACK transfer");
		return false;
	}

	libusb_fill_iso_transfer(feedbackTransfer,
								Devh(),
								fbEp->GetAddress(),
								feedbackData,
								transferSize,
								numberOfPackets,
								FeedbackCallback,
								this,
								1000);

	libusb_set_iso_packet_lengths(feedbackTransfer,packetSize);

	feedbackTransferGoingOn=false;

	return true;
}

void Usb::AudioStreamingInterface::ClearFeedbackTransfer()
{
	//LOGD("USBAUDIO - free feedback transfer for %s interface",IsOutput()?"OUT":"IN");
	libusb_free_transfer(feedbackTransfer);
	delete [] feedbackData;
	feedbackData = NULL;
	feedbackTransfer = NULL;
}

bool Usb::AudioStreamingInterface::StartFeedbackTransfer()
{
	feedbackLock.Lock();
	if (feedbackTransferGoingOn) {
		//LOGE("USBAUDIO - Feedback transfer already enqueued!");
		feedbackLock.Unlock();
		return false;
	}
	feedbackTransferGoingOn=true;
	askedToFreeFeedbackTransfer=false;
	int ret = libusb_submit_transfer(feedbackTransfer);
	if (ret<0) {
		LOGE("USBAUDIO - Error enqueuing FEEDBACK transfer. Error %d (%s)",ret,strerror(errno));
		feedbackLock.Unlock();
		return false;
	}
	feedbackLock.Unlock();
	return true;
}

void Usb::AudioStreamingInterface::FeedbackCallback(libusb_transfer* xfr)
{
	AudioStreamingInterface* as = (AudioStreamingInterface*)xfr->user_data;
	//LOGD("[ USBAUDIO - Feedback callback ] ");

	if (LIBUSB_TRANSFER_COMPLETED != xfr->status ) {
		LOGE("USBAUDIO -  Feedback transfer packet status error.");
		return;
	}

	if (xfr->type != LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		LOGE("USBAUDIO - Not an isochronous feedback transfer... how's that even possible!?!");
		return;
	}

	if (xfr->num_iso_packets != 1)
		LOGW("USBAUDIO - FEEDBACK transfer should have only one packet");

	if (LIBUSB_TRANSFER_COMPLETED != xfr->iso_packet_desc[0].status ) {
		LOGE("USBAUDIO - Isochronous FEEDBACK transfer error (status: %d)",xfr->iso_packet_desc[0].status);
		return;
	}

/*	if (as->stopStreamCounter > 0) {
		LOGI("USBAUDIO - stopping FEEDBACK transfer");
		as->ClearFeedbackTransfer();
		return;
	} */

	// In summary, for full-speed endpoints, the Ff value shall be encoded in an unsigned 10.10 (K=10)
	// format which fits into three bytes. Because the maximum integer value is fixed to 1,023, the
	// 10.10 number will be left-justified in the 24 bits, so that it has a 10.14 format. Only the
	// first ten bits behind the binary point are required. The lower four bits may be optionally used
	// to extend the precision of Ff, otherwise, they shall be	reported as zero. For high-speed
	// endpoints, the Ff value shall be encoded in an unsigned 12.13 (K=13) format which fits into
	// four bytes. The value shall be aligned into these four bytes so that the binary point is
	// located between the second and the third byte so that it has a 16.16 format. The most
	// significant four bits shall be reported zero. Only the first 13 bits behind the binary point
	// are required. The lower three bits may be optionally used to extend the precision of Ff,
	// otherwise, they shall be reported as zero.

	int len = xfr->iso_packet_desc[0].actual_length;
	double fractional = (((uint16_t)xfr->buffer[0]) | (xfr->buffer[1]<<8) ) / ((double)(pow(2,16)-1));
	double entire = (double)(((uint16_t)xfr->buffer[2]) | (xfr->buffer[3]<<8) );
	as->adjustedFramesPerPacket = entire+fractional;
	//LOGD("USBAUDIO - New frames per packet value: %f",as->adjustedFramesPerPacket);

	/*if (libusb_submit_transfer(xfr) < 0) {
		LOGE("USBAUDIO - error re-submitting FEEDBACK transfer\n");
	}*/
	as->feedbackLock.Lock();
	as->feedbackTransferGoingOn = false;
	if (as->askedToFreeFeedbackTransfer)
		as->ClearFeedbackTransfer();
	as->feedbackLock.Unlock();
}





tstring Usb::AudioStreamingInterface::DumpTransferInfo(libusb_transfer* xfr)
{
	stringformatter os;
	os << "* Transfer Info: \n";
	os << " - Status: " << xfr->status << ". Type: " << xfr->type << std::endl;
	os << " - Num Iso Packets: " << xfr->num_iso_packets << std::endl;
	os << " - Length: " << xfr->length << ". ACTUAL length: " << xfr->actual_length << std::endl;
	os << " - buffer ptr: " << xfr->buffer << std::endl;
	os << " - flags: " << (unsigned)xfr->flags << std::endl;
	os << " - endpoint: " << (unsigned)xfr->endpoint << std::endl;
	os << " - timeout: " << xfr->timeout << std::endl;
	os << " - user data ptr: " << xfr->user_data << std::endl;
	os << " - devh: " << xfr->dev_handle << std::endl;
	os << " - callback: " << xfr->callback << std::endl;
	LOGD(os.str().c_str());
	return os.str();
}

tstring Usb::AudioStreamingInterface::DumpIsoPacketsInfo(libusb_transfer* xfr)
{
	stringformatter os;
	for (int i=0; i<xfr->num_iso_packets; i++) {
		os << "  > ISO packet " << i
			<< " - Status: " << xfr->iso_packet_desc[i].status
			<< ", Length: " << (unsigned)xfr->iso_packet_desc[i].length
			<< ", ACTUAL length: " << (unsigned)xfr->iso_packet_desc[i].actual_length << std::endl;
	}
	LOGD(os.str().c_str());
	return os.str();
}


tstring Usb::AudioStreamingInterface::DumpInfo()
{
	stringformatter os;
	os << "AUDIO STREAMING interface (index " << GetIndex() << "):\n";
	LOGD(os.str().c_str());
	for (int i=0; i<settings.size(); i++)
		os << settings[i]->DumpInfo();
	return os.str();
}



Usb::AS_AlternateSetting::AS_AlternateSetting(libusb_interface_descriptor desc, AudioDevice* _aDev)
	: AlternateSetting(desc,_aDev) , audioEp(NULL), synchEp(NULL), decoderDesc(false), encoderDesc(false),
	  formatSpecificDesc(false)
{
	// LOGD("AS alt setting constructor");
	formatType=NULL;
	isValid = Initialize();
	SetupEndpoints();
	//CacheInformations();
	InitAllowedSampleRates();
}


Usb::AS_AlternateSetting::~AS_AlternateSetting()
{
	// LOGD("AS alt setting destructor");
	Cleanup();
}


bool Usb::AS_AlternateSetting::Initialize()
{
	int totSize = descriptor.extra_length;
	uint8_t const* asDescBuf = descriptor.extra;
	uint8_t protocol = descriptor.bInterfaceProtocol;

	if (totSize==0) {
		LOGI("[%u,%u] AS alternate setting is zero bandwidth (no endpoints)",GetInterfaceIndex(),GetIndex());
		isZeroBandwidth = true;
		return true; // the setting can still be valid.
	}

	while (totSize >= 2*sizeof(u_int8_t) )
	{
		uint8_t len = asDescBuf[0];		// 0: offset for length in a descriptor chunk
		uint8_t type = asDescBuf[1];	// 1: offset for the type
		uint8_t subtype = asDescBuf[2];	// 2: offset for the SUB-type

		if (len < 2) {  // too short chunk is sign of corruption
			LOGE("USBAUDIO - AS descriptors setup: too short audio streaming format descriptor chunk.");
			totSize-=len;
			asDescBuf+=len;
			continue;
		}

		if ( DESC_TYPE_INTERFACE != type) {
			LOGE("USBAUDIO - AS descriptors setup: this is not an interface related descriptor!");
			return false;
		}

		switch (subtype) {
			case AS_DESC_UNDEFINED:
				LOGE("USBAUDIO - AC descriptors setup: UNDEFINED descriptor");
				break;
			case AS_DESC_GENERAL:
				LOGI("USBAUDIO - Found Audio Streaming GENERAL chunk descriptor");
				if (!SetupGeneralInfo(asDescBuf))
					return false;
				break;
			case AS_DESC_FORMAT_TYPE:
				LOGI("USBAUDIO - Found Audio Streaming FORMAT TYPE");
				formatType = new FormatType(asDescBuf,Devh(),GetIndex(),protocol);
				break;
			case AS_DESC_FORMAT_SPECIFIC: // the same value is assigned to ENCODER in UAC2
				if (IsUAC2()) {
					LOGW("USBAUDIO - Found Audio Streaming ENCODER Descriptor");
					encoderDesc=true;
				} else {
					LOGW("USBAUDIO - Found Audio Streaming FORMAT SPECIFIC Descriptor");
					formatSpecificDesc=true;
				}
				break;
			case AS_DESC_DECODER:
				LOGW("USBAUDIO - Found Audio Streaming DECODER Descriptor");
				decoderDesc=true;
			default:
				LOGE("USBAUDIO - UNSUPPORTED Audio Streaming Interface entity found.");
		}
		totSize-=len;
		asDescBuf+=len;
	}
	return true;
}


bool Usb::AS_AlternateSetting::SetupGeneralInfo(uint8_t const* rawDesc)
{
	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bTerminalLink = rawDesc[3];

	if (IsUAC2()) {
		bmControls = rawDesc[4];
		bFormatType = rawDesc[5];
		bmFormats = rawDesc[6] | (rawDesc[7]<<8) | (rawDesc[8]<<16) | (rawDesc[9]<<24);
		bNrChannels = rawDesc[10];
		bmChannelConfig = rawDesc[11] | (rawDesc[12]<<8) | (rawDesc[13]<<16) | (rawDesc[14]<<24);;
		iChannelNames = rawDesc[15];
		// uac1 attributes, set empty:
		bDelay = 0;
		wFormatTag = 0;
	}
	else {
		bDelay = rawDesc[4];
		wFormatTag  = rawDesc[5] || (rawDesc[6]<<8);
		// uac2 attributes, set empty:
		bmControls = bFormatType = 	bNrChannels = iChannelNames = 0;
		bmChannelConfig = bmFormats = 0;
	}

	// LOGD("AS general. blen:%u, type:%u, sub:%du, term:%u, del:%u, form:%u",bLength,bDescriptorType,
	//		bDescriptorSubtype,bTerminalLink,bDelay,wFormatTag);
	return (bDescriptorType==DESC_TYPE_INTERFACE && bDescriptorSubtype==AS_DESC_GENERAL);
}

bool Usb::AS_AlternateSetting::SetupEndpoints()
{
	if (descriptor.bNumEndpoints==0) {
		LOGI("[%u,%u] AS alternate setting is zero bandwidth (no endpoints)",GetInterfaceIndex(),GetIndex());
		isZeroBandwidth=true;
		return false;
	}

	for (int i = 0; i<descriptor.bNumEndpoints; i++) {
		libusb_endpoint_descriptor epd = descriptor.endpoint[i];
		if (Endpoint::IsIsochronous(epd) && !Endpoint::IsSynchEndpoint(epd)) {
			audioEp = new AudioEndpoint(epd,Devh());
		} else if (Endpoint::IsIsochronous(epd) && Endpoint::IsSynchEndpoint(epd)) {
			synchEp = new Endpoint(epd,Devh());
		}
	}

	if (audioEp || synchEp)
		return true;
	else
		return false;
}

void Usb::AS_AlternateSetting::Cleanup()
{
	// LOGD("AS alternate setting cleanup");
	isValid = false;
	delete formatType;
	delete audioEp;
	delete synchEp;
	formatType=NULL;
	audioEp=NULL;
	synchEp=NULL;
}

bool Usb::AS_AlternateSetting::InitAllowedSampleRates()
{
	if (IsZeroBandwidth()) {
		cachedAllowedSampleRates.clear();
		return false;
	}

	if (!IsUAC2()) {
		if (!GetFormat()) { // actually, this should happen only with zero bandwidth
			LOGE("USBAUDIO - Cannot retrieve alternate setting's format. No allowed rates");
			cachedAllowedSampleRates.clear();
			return false;
		}

		cachedAllowedSampleRates = GetFormat()->GetAllowedSampleRates();
		return true;
	}
	// UAC2:
	uint8_t terminalID = GetTerminalLink();
	ClockSource* cs = aDev->GetControlInterface()->FindClockSourceFromTerminalID(terminalID);
	if (NULL==cs) {
		LOGE("USBAUDIO - AltSetting GetAllowedSampleRates: no clock source: not UAC2? too early?");
		//ASSERT(false);
		return false;
	}
	cachedAllowedSampleRates = cs->GetAllowedSampleRates();
	return true;
}

std::vector<uint32_t> Usb::AS_AlternateSetting::GetAllowedSampleRates(bool cached)
{
	if (cached)
		return cachedAllowedSampleRates;
	// if a stream is active, it will want to avoid interfering with it sending a request to the
	// audio control interface.

	InitAllowedSampleRates();
	return cachedAllowedSampleRates;
}

bool Usb::AS_AlternateSetting::SetSampleRate(uint32_t sr)
{
	if (IsZeroBandwidth()) {
		LOGW("Tried setting SR on zero-bandwidth interface alt setting. Returning false");
		return false;
	}

	if (IsUAC2()) { // discover the clock source entity and set the sr:
		uint8_t terminalID = GetTerminalLink();
		ClockSource* cs = aDev->GetControlInterface()->FindClockSourceFromTerminalID(terminalID);
		if (NULL==cs) {
			LOGE("USBAUDIO - SetSampleRate: no clock source: not UAC? too early?");
			return false;
		}
		return cs->SetSampleRate(sr);
	}

	// UAC1:

	if (NULL==audioEp) {
		LOGE("USBAUDIO - no audio endpoint for interface %u altsetting %u",GetInterfaceIndex(),GetIndex());
		return false;
	}

	std::vector<uint32_t> freqs = formatType->GetAllowedSampleRates();
	if (freqs.size()==0) {
		LOGE("USBAUDIO - NO allowed sample rate frequencies for this alt setting!!!");
		return false;
	} else if (freqs.size()==1) {
		if (sr==freqs[0]) {
			//LOGD("USBAUDIO - The only allowed sample rate matches the requested one");
			//bool singleres = audioEp->SetSampleRate(sr); // this fails
			//LOGD("USBAUDIO - Tried set with single sr allowed. res: %d",singleres);
			return true;
		}
		else {
			LOGE("USBAUDIO - The only allowed sample rate DOESN'T match the requested one");
			return false;
		}
	}
	// more than 1 allowed sr:
	//uint32_t oldsr = audioEp->GetCurrentSampleRate();
	//LOGD("USBAUDIO - Set Sample Rate (itf:%u,alt:%u). Old SR: %u. new:%u",GetInterfaceIndex(),GetIndex(),oldsr,sr);
	bool res = audioEp->SetSampleRate(sr);
	uint32_t newsr = audioEp->GetCurrentSampleRate();
	//LOGD("USBAUDIO -   Sample rate after setting attempt on %x: %u (res:%d)",audioEp->GetAddress(),newsr,res);
	/*
	if (newsr==0) {
		uint8_t twinEpAddress = audioEp->GetAddress() ^ 0b10000000;
		bool hackRet = AudioEndpoint::SetSampleRate(Devh(),twinEpAddress,sr);
		uint32_t newnewSr = audioEp->GetCurrentSampleRate();
		LOGD("USBAUDIO -       Sample rate after attempt on %x: %u",twinEpAddress,newnewSr);
		return hackRet;
	}*/

	return res;
}

uint32_t Usb::AS_AlternateSetting::GetSampleRate()
{
	if (IsUAC2()) {
		uint8_t terminalID = GetTerminalLink();
		ClockSource* cs = aDev->GetControlInterface()->FindClockSourceFromTerminalID(terminalID);
		if (NULL==cs) {
			LOGE("USBAUDIO - GetSampleRate: no clock source: no UAC2? too early?");
			return 0;
		}
		return cs->GetSampleRate();
	}

	// UAC1 :

	if (NULL==audioEp) {
		LOGE("USBAUDIO - Interface [%u,%u] get SR error. No audio endpoint",GetInterfaceIndex(),GetIndex());
		return 0;
	}

	//aDev->
	//audioEp->HasSampleRateControl();

	uint32_t srFromEp = audioEp->GetCurrentSampleRate();
	//LOGD("USBAUDIO - Get SR for %s interface ep. sr:%u (numallowed:%d)",audioEp->IsInput()?"IN":"OUT",srFromEp,formatType->GetAllowedSampleRates().size());
	if (srFromEp==0 && formatType->GetAllowedSampleRates().size()==1) {
	//		LOGD("USBAUDIO -    returning sample rate: %u",formatType->GetAllowedSampleRates()[0]);
			return formatType->GetAllowedSampleRates()[0];
	}
	return srFromEp;
}

Usb::AudioStreamingInfo Usb::AS_AlternateSetting::GetStreamingInfo()
{
	AudioStreamingInfo asi;
	asi.itf = GetInterfaceIndex();
	asi.altSetting = GetIndex();
	asi.numChannels = GetNumChannels();
	asi.bytesPerSample = GetBytesPerSample();
	asi.numBits = GetNumBits();
	asi.sampleRate = 0; // IsUAC2() ? 0 : GetSampleRate(); // SR from AC interface, in UAC2

	// only the parent audiostreaming interface can tell about following fields:
	asi.framesPerBuffer = 0;
	asi.numBuffers = 0;
	asi.streamingBroken=true;

	//LOGD("USBAUDIO - Get Streaming info. itf:%u, alts:%u, bits:%u, Bps:%u, ch:%u, sr:%u",
	//		asi.itf,asi.altSetting,asi.numBits,asi.bytesPerSample,asi.numChannels,asi.sampleRate);

	return asi;
}

uint8_t Usb::AS_AlternateSetting::GetNumChannels()
{
	return IsUAC2() ? bNrChannels : GetFormat()->GetNumChannels();
}

uint8_t Usb::AS_AlternateSetting::GetNumBits()
{
	return GetFormat()->GetBitResolution();
}

uint8_t Usb::AS_AlternateSetting::GetBytesPerSample()
{
	return GetFormat()->GetBytesPerSample();
}

bool Usb::AS_AlternateSetting::IsPCM()
{
	if (IsUAC2()) {
		return GetFormat()->IsType1() && ((bmFormats&1) == 1); // bit 0 in the mask is for PCM
	}
	return GetFormat()->IsType1() && ((bmFormats&1) == 1);
}


tstring Usb::AS_AlternateSetting::DumpInfo()
{
	stringformatter os;
	os << "Audio Streaming "<< (unsigned)GetInterfaceIndex() <<" - ALT. SETTING: " << (unsigned)GetIndex() << ":\n";

	if (IsZeroBandwidth()) {
		os << "  Zero bandwidth alternate setting.\n";
		LOGD(os.str().c_str());
		return os.str();
	}

	os << "  GENERAL INFO: \n"
		<< "   VALID: " << isValid << "\n"
		<< "   bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "   bDescriptorSubtype: " << AudioStreamingSubtypeToString(bDescriptorSubtype) << "\n"
		<< "   bTerminalLink: " << (unsigned) bTerminalLink << "\n";

	if (IsUAC2()){
		os << "   bmControls: " << (std::bitset<8>(bmControls)) << "\n"
		   << "   bFormatType: " << (unsigned)bFormatType  << "\n"
		   << "   bmFormats: " << (std::bitset<32>(bmFormats)) << "\n"
		   << "   bNrChannels: " << (unsigned) bNrChannels  << "\n"
		   << "   bmChannelConfig: " << (std::bitset<32>(bmChannelConfig)) << "\n"
		   << "   iChannelNames: " << (unsigned)iChannelNames << GetString(Devh(),iChannelNames) << "\n";
	} else {
		os << "   bDelay: " << (unsigned) bDelay << "\n"
		   << "   wFormatTag: " << std::hex << wFormatTag << std::dec << " (" << AudioFormatTagToString(wFormatTag) << ")\n";
	}
	if (formatSpecificDesc) os << "   - Found unsupported FORMAT SPECIFIC DESCRIPTOR";
	if (encoderDesc) os << "   - Found unsupported ENCODER DESCRIPTOR";
	if (decoderDesc) os << "   - Found unsupported DECODER DESCRIPTOR";

	LOGD(os.str().c_str());
	os << formatType->DumpInfo();
	if (NULL!=audioEp)
		os << audioEp->DumpInfo();
	if (NULL!=synchEp)
		os << synchEp->DumpInfo();
	return os.str();
}

#endif
