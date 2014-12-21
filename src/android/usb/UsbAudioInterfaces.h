/*
 * UsbAudioInterfaces.h
 *
 *  Created on: 14/gen/2014
 *      Author: athos
 */

#ifndef USBAUDIOINTERFACES_H_
#define USBAUDIOINTERFACES_H_

#ifdef ENABLE_AUDIO_INTERFACES
#include "UsbAudioEntities.h"
#endif

#include "UsbAudioEndpoint.h"
#include <vector>
#include <string>
#include <list>

namespace Usb {

class AudioDevice;

struct AudioStreamingInfo {
	uint8_t itf;
	uint8_t altSetting;
	uint32_t sampleRate;
	uint8_t  numChannels;
	uint8_t  numBits;
	uint8_t	 bytesPerSample;
	int numActiveChannels;
	int  framesPerBuffer;
	int	 numBuffers;
	bool  streamingBroken;
};

class AlternateSetting {

protected:
	AudioDevice* aDev;
	libusb_device_handle* Devh();
	libusb_interface_descriptor descriptor;
	bool isValid;
	bool isZeroBandwidth;
public:
	AlternateSetting(libusb_interface_descriptor desc, AudioDevice* aDev);
	virtual ~AlternateSetting() {};
	uint8_t GetIndex();
	uint8_t GetInterfaceIndex();
	virtual std::string DumpInfo() = 0;
	bool IsValid() { return isValid; }
	bool IsZeroBandwidth() { return isZeroBandwidth;}
	bool IsUAC2();
private:
	AlternateSetting() : aDev(NULL) {};

};


class AudioInterface {

public:

	class Listener {
	public:
		virtual void OnUsbInterfaceDestroyed(AudioInterface* destroyedItf) = 0;
	};

	AudioInterface(AudioDevice* audioDev, libusb_interface itf);
	int GetIndex();
	bool DetachAndClaim();
	bool Claim();
	bool Release();
	bool SelectAlternateSetting(int altSetNum);
	virtual std::string DumpInfo() = 0;
	virtual ~AudioInterface();
	// could use a "virtual GetInterfaceType()=0;" to know, at runtime, the derived type when using base pointer
	bool IsUAC2() { return interface.altsetting[0].bInterfaceProtocol==USB_AUDIO_CLASS_2; }

	// NB: we are basing the UAC2 result on one of the alternate settings for the interface
	// This may not be true, but I don't know if it's even possible to have some alternate
	// settings belonging to UAC1 and some to UAC2 in the same interface!!!! I guess, and HOPE, not!

	void AddListener(Listener* list) { if (list) listeners.push_back(list); }
	void RemoveListener(Listener* list) { listeners.remove(list); }
	void OnInterfaceDestroyed()
	{
		for(auto it=listeners.begin(); it!=listeners.end(); it++)
			(*it)->OnUsbInterfaceDestroyed(this);
	}

protected:

	AudioDevice* aDev;
	libusb_device_handle* Devh();// { return aDev->GetDeviceHandle() };
	libusb_interface interface;
	std::vector<AlternateSetting*> settings;
	int currSetting;

	std::list<Listener*> listeners;
};


#ifdef ENABLE_AUDIO_INTERFACES

/*
 * The AudioControl interface of an audio function may support multiple alternate settings.
 * Alternate settings of the AudioControl interface could for instance be used to implement
 * audio functions that support multiple topologies by presenting different class-specific
 * AudioControl interface descriptors for each alternate setting.
 */

class AudioControlInterface;

class AC_AlternateSetting : public AlternateSetting {

	friend class AudioControlInterface;

public:
	AC_AlternateSetting(libusb_interface_descriptor desc, AudioDevice* aDev);
	~AC_AlternateSetting();
	bool UnMuteAll();
	tstring DumpInfo();
private:
	//AC_AlternateSetting() : header(NULL) {};
	AudioControlHeader* header;
	std::vector<InputTerminal*> inputTerminals;
	std::vector<OutputTerminal*> outputTerminals;
	std::vector<FeatureUnit*> featureUnits;
	std::vector<UnsupportedEntity*> unsupportedEntities;
	std::vector<ClockSource*> clockSources;
	std::vector<ClockSelector*> clockSelectors;

	bool CreateEntities();
	void CleanupEntities();
};


class AS_AlternateSetting : public AlternateSetting {

public:
	AS_AlternateSetting(libusb_interface_descriptor desc, AudioDevice* aDev);
	~AS_AlternateSetting();
	bool SetSampleRate(uint32_t sr);
	std::vector<uint32_t> GetAllowedSampleRates(bool cached = false);// { return GetFormat()->GetAllowedSampleRates();}
	tstring DumpInfo();
	AudioEndpoint* GetAudioEndpoint() { return audioEp; }
	Endpoint* GetFeedbackEndpoint() { return synchEp; }
	AudioStreamingInfo GetStreamingInfo();
	uint32_t GetSampleRate();
	uint8_t GetTerminalLink() { return bTerminalLink; }

	uint8_t GetNumChannels();
	uint8_t GetNumBits();
	uint8_t GetBytesPerSample();
	bool IsPCM();
	bool HasExplicitFeedback() { return (synchEp!=NULL && synchEp->IsSynchEndpoint()); }

private:

	FormatType* GetFormat() { return formatType; }

	bool Initialize();
	bool SetupGeneralInfo(uint8_t const* rawDesc);
	bool SetupEndpoints();
	bool InitAllowedSampleRates();
	void Cleanup();

	uint8_t bLength;			// size of descriptor (7 for this one)
	uint8_t bDescriptorType;	// type (must be CS_INTERFACE)
	uint8_t bDescriptorSubtype;  // subtype https://mail.google.com/mail/u/0/?ui=2&ik=4b0445967a&view=fimg&th=14478f45fd2aeca8&attid=0.1&disp=inline&realattid=1461294257658658816-local0&safe=1&attbid=ANGjdJ9qZ0PYKQ3GmdGKHwf7JtmRQdBABqP6sH10TFpQSBGhiUWQ6L7iT9F73Zm5IA78kp42lDvbwhY6-90721CiFJnGU7CWOEvhj9O1okKw-WHtsQkj2rWz00cePj4&ats=1393598700377&rm=14478f45fd2aeca8&zw&sz=w1125-h728(must be AS_GENERAL)
	uint8_t bTerminalLink;		// ID of the terminal to which the endpoint of this interface is connected

	// UAC1 only attributes:
	uint8_t bDelay;				// Delay introduced by the data path, in number of frames
	uint16_t wFormatTag;		// Audio Data Format that must be used to communicate with this interface

	// UAC2 only attributes:
	uint8_t bmControls;			// 0/1:active alt setting, 2/3: valid alt setting (actually, only read is allowed)
	uint8_t bFormatType;		// format type the interface is using
	uint32_t bmFormats;			// audio data formats that can be used to communicate with the interface
	uint8_t  bNrChannels;		// number of physical channels in the AS interface audio channel cluster
	uint32_t bmChannelConfig;	// spatial location of the physical channels
	uint8_t  iChannelNames;		// string descriptor of the first physical channel

	FormatType* formatType;
	AudioEndpoint* audioEp;
	Endpoint* synchEp;

	// these are just to keep track of unsupported formats we find enumerating the AS alt setting:
	bool decoderDesc;
	bool encoderDesc;
	bool formatSpecificDesc;

	std::vector<uint32_t> cachedAllowedSampleRates;
};




class AudioControlInterface : public AudioInterface {
	//std::vector<AC_AlternateSetting*> settings;
public:
	AudioControlInterface(AudioDevice* audioDev, libusb_interface itf);
	//~AudioControlInterface();

	bool UnMuteAll();

	AC_AlternateSetting* GetCurrentSetting();
	AC_AlternateSetting* GetSetting(int i);

	ClockSource* FindClockSourceFromTerminalID(uint8_t termID);

	tstring DumpInfo();
	static tstring SubtypeToString(uint8_t subtype);
};

/* TODO: STATUS INTERRUPT ENDPOINT (in audio control Interface)
 * A USB AudioControl interface can support an optional interrupt endpoint to inform the Host
 * about the status the status of the different addressable Entities (Terminals, Units, interfaces
 * and endpoints) inside the audio function. In fact, the interrupt endpoint is used by the
 * entire Audio Interface Collection to convey status information to the Host. It is considered
 * part of the AudioControl interface because this is the anchor interface for the Collection.
 */

typedef void (*ErrorCallback)(void*,int);  // arguments: user-defined context, error code

class AudioStreamingInterface : public AudioInterface {

public:

	AudioStreamingInterface(AudioDevice* audioDev, libusb_interface itf);
	~AudioStreamingInterface();

	bool IsOutput();
	bool IsInput();

	int GetFramesPerBufferBaseValue();
	std::vector<int> GetAllowedFramesPerBuffer();
	std::vector<int> GetAllowedFramesPerBuffer(uint8_t altSetting, uint32_t sampRate);

	bool IsFramesPerBufferAllowed(int fpb);

	std::vector<uint32_t> GetAllowedSampleRates();
	std::vector<uint32_t> GetAllowedSampleRates(uint8_t altSetting);
	uint32_t GetSampleRate();
	bool SetSampleRate(uint32_t sr);
	bool SetFramesPerBuffer(int fpb);
	int GetFramesPerBuffer();
	bool SetNumBuffers(int numbuf);
	bool SetCallback(AudioDriverCallback newCallback, void* aCtx);
	bool SetErrorCallback(ErrorCallback newErrCallback, void* errCtx); // NB: error callback will be called from audio thread!
	AudioStreamingInfo GetStreamingInfo();
	AudioStreamingInfo GetStreamingInfo(int altSetting);
	std::vector<AudioStreamingInfo> GetInfos();
	bool IsStreamingBroken();

	bool SetupStreaming(AudioStreamingInfo& settings);
	bool StartStreaming();
	bool StopStreaming();
	bool IsStreamOpen();

	static int numOpenStreams;
	static bool IsAnyStreamOpen();
	static void DecreaseNumOpenStreams();
	static void IncreaseNumOpenStreams();

	uint8_t* GetAudioBuffer();

	static void IsochronousCallback(libusb_transfer* xfr);
	static void FeedbackCallback(libusb_transfer* xfr);

	int GetCurrentAudioBufferSizeInBytes();
	int GetFramesInCurrentAudioBuffer();
	__int64 GetTransferredFrames() { return transferredFramesCount; }

	//static void IsochronousInputCallback(libusb_transfer* xfr);

	tstring DumpInfo();
	static tstring DumpIsoPacketsInfo(libusb_transfer* xfr);
	static tstring DumpTransferInfo(libusb_transfer* xfr);
	static tstring SubtypeToString(uint8_t subtype);

private:
	AS_AlternateSetting* GetCurrentSetting();
	AS_AlternateSetting* GetSetting(int i);
	AS_AlternateSetting* FindSettingById(uint8_t id);

	int framesPerBuffer;
	int numBuffers;

	int numActiveChannels; // used only for input, allows us skipping the copy of unneeded channels samples

	bool PrepareBuffersAndTransfers();
	//bool PrepareBuffersAndTransfers_USB2();

	void ClearBuffersAndTransfers();
	void DismissStreaming();
	bool CheckAndHandleStopRequest();

	int GetAudioBufferNominalSizeInBytes();
	double GetIdealAverageFramesPerPacket(bool fromCallback);
	double GetIdealAverageFramesPerPacket(uint8_t altSetting, uint32_t sampRate);
	double CalculateIdealAverageFramesPerPacket(uint32_t sr, int bInterval);
	std::vector<int> PopulateAllowedFramesPerBufferList(double idealFramesPerPacket);
	//nTrack::Sync::Semaphore stopStreamSemaphore;
	//nTrack::Sync::Event dismissStreamEvent;
	nTrack::Sync::CCritSec streamActiveLock;
	bool streamActive;
	//int stopStreamCounter;
	bool askedToStopStreaming;
	int numEnqueuedTransfers;
	bool HandleStreamingError(int state); // called by the audio tread in the callback!

	uint8_t** audioBuffer;
	libusb_transfer** transfers; // length of the transfers queue is numBuffers
	int currentTransfer;
	uint8_t** inputIntermediateBuffer;
	uint32_t cachedSampleRate;
	//std::vector<uint32_t> cachedAllowedSampleRates;


	AudioDriverCallback audioCallback;
	void* audioContext;

	ErrorCallback errorCallback;
	void* errorContext;

	libusb_transfer* feedbackTransfer;
	nTrack::Sync::CCritSec feedbackLock;
	bool feedbackTransferGoingOn;
	bool askedToFreeFeedbackTransfer;
	uint8_t* feedbackData;
	double adjustedFramesPerPacket;
	double framesAccumulator;
	int adjustedFramesPerBuffer; // valid for the current buffer
	__int64 transferredFramesCount;

	bool PrepareFeedbackTransfer();
	void ClearFeedbackTransfer();
	bool StartFeedbackTransfer();


};


#endif


} // usb namespace end




#endif /* USBAUDIOINTERFACES_H_ */
