/*
 * UsbAudioEntities.h
 *
 *  Created on: 17/gen/2014
 *      Author: athos
 */

#ifndef USBAUDIOENTITIES_H_
#define USBAUDIOENTITIES_H_

#include "UsbHelper.h"
#include <vector>

namespace Usb {

/* per sapere il cluster descriptor (quindi configurazione dei canali)
	bisogna percorrere a ritroso l'albero delle entity finch non se ne trova
	una che definisce il cluster in uscita, come per esempio l'input terminal,
	la mixer unit, la processing unit
*/

enum RequestLayout { ReqLayoutUnsupported, ReqLayout1=1, ReqLayout2=2, ReqLayout3=4 };
// NB: values are the byte occupied by a single value

struct RequestData {
	uint16_t  valSize; // used for all request in UAC1, for CUR requests in UAC2
	uint8_t* data;
	uint16_t  rangeSize; // only used in UAC2, depending on the layout type: I->1byte, II->2byte, III->4byte
	RequestData():valSize(0),rangeSize(0),data(0){}
};


class ControlRange {

public:

	uint8_t* rawData;	// access these only when you need to pass the usb request
	uint8_t rawSize;	// functions a buffer and a buffersize for the transfer
	uint8_t valueSize;

	ControlRange() : rawSize(0), rawData(NULL), valueSize(0) {} // creates an invalid range data object

	ControlRange(uint16_t sub_ranges, RequestLayout layout)
	{
		valueSize = (uint8_t) layout;
		rawSize =  2 + (sub_ranges * valueSize * 3);
		rawData = new uint8_t[rawSize];
		memset(rawData,0,rawSize*sizeof(uint8_t));
	}

	ControlRange(const ControlRange& src)
	{
	  valueSize = src.valueSize;
	  rawSize = src.rawSize;
	  rawData = new uint8_t[rawSize];
	  memcpy(rawData, src.rawData, rawSize*sizeof(uint8_t) );
	}

	ControlRange& operator=( const ControlRange& rhs )
	{
		valueSize = rhs.valueSize;
		rawSize = rhs.rawSize;
		rawData = new uint8_t[rawSize];
		memcpy(rawData, rhs.rawData, rawSize*sizeof(uint8_t) );
	    return *this;
	}

	~ControlRange() { delete [] rawData; }

	uint16_t Subranges() { return rawData!=NULL ? ( rawData[0] | (rawData[1]<<8) ) : 0; }

	uint32_t Min(uint16_t whichSubrange)
	{
		if (whichSubrange>=Subranges()) return 0;
		uint32_t min = 0;
		for (uint8_t i=0; i<valueSize; i++)
			min = min | (rawData[ 2 + (whichSubrange*3*valueSize) + i ] << (8*i) );
		return min;
	}

	uint32_t Max(uint16_t whichSubrange)
	{
		if (whichSubrange>=Subranges()) return 0;
		uint32_t max = 0;
		for (uint8_t i=0; i<valueSize; i++)
			max = max | (rawData[ 2 + (whichSubrange*3*valueSize) + valueSize + i ] << (8*i));
		return max;
	}

	uint32_t Res(uint16_t whichSubrange)
	{
		if (whichSubrange>=Subranges()) return 0;
		uint32_t res = 0;
		for (uint8_t i=0; i<valueSize; i++)
			res = res | (rawData[ 2 + (whichSubrange*3*valueSize) + valueSize*2 + i ] << (8*i));
		return res;
	}

};




class AudioControlEntity {

protected:
	libusb_device_handle* devh;
	uint8_t  interfaceIndex;
	uint8_t  bLength;			// size of descriptor
	uint8_t	 bDescriptorType;	// will contain: CS_INTERFACE descriptor type
	uint8_t  bDescriptorSubtype;// will contain: HEADER descriptor subtype
	//	uint8_t  bSourceID; // the input terminal doesn't have it // the processing unit has an array of it

	uint8_t protocol;
	bool isValid;

public:
	bool IsValid()		{ return isValid;}
	uint8_t  GetLength(){ return bLength; }
	uint8_t  GetType()	{ return bDescriptorType; }
	uint8_t  GetSubtype() { return bDescriptorSubtype; }
	bool IsUAC2() { return (protocol==USB_AUDIO_CLASS_2); }
	virtual uint8_t GetID() = 0;
	virtual tstring DumpInfo() = 0;

	AudioControlEntity() : devh(NULL),bLength(0),bDescriptorType(0),bDescriptorSubtype(0),isValid(false),interfaceIndex(0),protocol(USB_AUDIO_CLASS_1) {}
	virtual ~AudioControlEntity() {}
};


class UnsupportedEntity : public AudioControlEntity {
	uint8_t id;
public:
	UnsupportedEntity() : id(0) {}
	UnsupportedEntity(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t protoc);
	uint8_t GetID() { return id; }
	tstring DumpInfo();
};



class AudioControlHeader : public AudioControlEntity {

public:

	AudioControlHeader();
	AudioControlHeader(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t protoc);
	~AudioControlHeader();

	uint16_t GetTotalLength() { return wTotalLength; }
	tstring DumpInfo();
	uint8_t GetID() { return 0; }  // the HEADER has NO ID!

private:

	uint16_t bcdADC;			// Audio Device Class Specification Release number in binary coded decimal
	uint16_t wTotalLength;		// length of this descriptor header + all units and terminal descriptors

	// UAC1 only descriptor attributes:
	uint8_t  bInCollection; 	// The number of AudioStreaming (or midi) interfaces in the collection
	uint8_t* baInterfaceNr;		// array of AudioStreaming or MidiStreaming interfaces in the collection

	// UAC2 only descriptor attributes:
	uint8_t bCategory;			// primary use of this audio function (as defined by the manufacturer)
	uint8_t bmControls;			// available function controls, bit couples (actually only latency is defined)
};


class InputTerminal : public AudioControlEntity {

public:

	InputTerminal();
	InputTerminal(uint8_t const* rawDesc,libusb_device_handle* dh, uint8_t protoc);

	int NumberPredefinedChannels();
	bool IsChannelPresent(uint8_t chan);
	std::string GetChannelName(uint8_t chan);
	uint8_t GetID() { return bTerminalID; }
	uint8_t GetClockSourceID() { return bCSourceID;}
	tstring DumpInfo();

private:
	// common attributes:
	uint8_t  bTerminalID;		// unique identifier for the terminal. use this for requests
	uint16_t wTerminalType;		// type of terminal (see TT_ macros in UsbSpecCodes.h. eg: TT_USB_STREAMING)
	uint8_t  bAssocTerminal;    // id of the output terminal associated to this one
	uint8_t  bNrChannels;		// number of channels in the channel cluster
	uint8_t  iChannelNames;		// index of a string descriptor with the name of the first logical channel
	uint8_t  iTerminal;			// index of a string descriptor with the name of the input terminal
	// UAC1 only attributes:
	uint16_t wChannelConfig;	// bit-map of the spatial location of the logical channels
	// UAC2 only attributes:
	uint8_t bCSourceID;			// clock entity to which this input terminal is connected
	uint32_t bmChannelConfig;	// spatial location of the logical channels for UAC2 (32bit instead of 16bit!!!)
	uint16_t bmControls;		// 1/0: copy protect, 2/3: connector, 4/5: overload, 6/7: cluster, 8/9: underflow, 10/11: overflow, 12...15: reserved

	// NB: bNrChannels, wChannelConfig and iChannelNames constitute the CLUSTER DESCRIPTOR
};


class OutputTerminal : public AudioControlEntity {

public:

	OutputTerminal();
	OutputTerminal(uint8_t const* rawDesc,libusb_device_handle* dh, uint8_t protocol);
	uint8_t GetID() { return bTerminalID; }
	tstring DumpInfo();
	uint8_t GetClockSourceID() { return bCSourceID;}

private:

	// uint8_t  bLength;				// these are in the AudioControlEntity class,
	// uint8_t	bDescriptorType;		// since they are common to all entities
	// uint8_t  bDescriptorSubtype;
	uint8_t  bTerminalID;		// unique identifier for the terminal. use this for requests
	uint16_t wTerminalType;		// type of terminal (see TT_ macros in UsbSpecCodes.h. eg: TT_USB_STREAMING)
	uint8_t  bAssocTerminal;    // id of the output terminal associated to this one
	uint8_t  bSourceID;			// ID of the unit or terminal connected to the input of this terminal
	uint8_t  iTerminal;			// index of a string descriptor with the name of the input terminal

	// UAC2 only attributes:
	uint8_t bCSourceID;			// clock entity to which this output terminal is connected
	uint16_t bmControls;		// available controls (like input: copy protect, connector, overload, underflow, overflow)

	// NB: The cluster descriptor (channel config) is not repeated here. It is up to us to trace
	// the connection "upstream" to locate the cluster descriptor pertaining to this audio channel cluster.

};



class FeatureUnit : public AudioControlEntity {

public:

	FeatureUnit();
	FeatureUnit(uint8_t const* rawDesc,libusb_device_handle* dh,uint8_t itfIdx, uint8_t protocol);
	~FeatureUnit();
	uint8_t GetID() { return bUnitID; }
	tstring DumpInfo();

	uint32_t GetControlsBitmap(uint8_t channel);
	bool ChannelHasControl(uint8_t channel,uint8_t control);
	tstring GetControlName(uint8_t control);
	tstring GetChannelName(uint8_t channel);

	bool SetControlCurr(uint8_t chan, uint8_t control,uint32_t value);
	uint32_t GetControl(uint8_t attribute, uint8_t chan, uint8_t control);
	uint32_t GetControlCurr(uint8_t chan, uint8_t control);
	uint32_t GetControlMin(uint8_t chan, uint8_t control);
	uint32_t GetControlMax(uint8_t chan, uint8_t control);
	uint32_t GetControlRes(uint8_t chan, uint8_t control);
	ControlRange GetControlRange(uint8_t chan,uint8_t control);

	//bool SetMute(uint8_t chan,bool mute);
	uint8_t GetNumChannels() { return numChannels; }

	static RequestLayout ControlLayout(uint8_t control);
	static tstring ControlToString(uint8_t control_id);
	static bool IsControlBoolean(uint8_t control_id);

private:

	// bLength, bDescriptorType, bDescriptorSubtype  are in the AudioControlEntity class,
	uint8_t  bUnitID;		// unique identifier for the unit in the audio function
	uint8_t  bSourceID;		// ID of the unit or terminal connected to the input of this terminal
	uint8_t  bControlSize;	// size (in bytes) of each element in bmaControls array
	uint8_t* bmaControls; 	// array of the active controls for each channel (0 is master). Its size is equal to the channels number + 1
	uint8_t  iFeature;		// index of a string descriptor with the name of the unit

	// not part of the descriptor
	uint8_t numChannels;  // actually, number of channels + master channel ( index 0)

	// NB: for UAC 2, bControlSize is not given, the specification assumes
	// it's 4 (32 bit for each bmaControls bit-map)
	// ALSO, for UAC 2 each control has 2 bits associated to it, one for read capability, one for write

	bool SetControlRequest(uint8_t attribute, uint8_t channel, uint8_t control, uint8_t* data, uint16_t size);
	bool GetControlRequest(uint8_t attribute, uint8_t channel, uint8_t control, uint8_t* data,uint16_t size);
	RequestData AllocateDataBuffer(uint8_t control);
	void DeallocateDataBuffer(uint8_t* databuffer);
};


class ClockSource : public AudioControlEntity {

public:

	enum Type { External=0b00, InternalFixed=0b01, InternalVariable=0b10, InternalProgrammable=0b11 };
	enum Control {Frequency=0x01, Validity=0x02};
	static tstring TypeToString(Type type);
	static tstring ControlToString(Control control);

	ClockSource();
	ClockSource(uint8_t const* rawDesc,libusb_device_handle* dh,uint8_t itfIdx); // no need for protocol, clocksource is only for UAC2
	~ClockSource();
	uint8_t GetID() { return bClockID; }
	tstring DumpInfo();

	Type GetType();
	bool IsSyncedToSOF();
	bool CanSet(Control control); // can set control
	bool CanRead(Control control);	  // can write control
	bool SetControlCur(Control control,uint32_t value);
	uint32_t GetControlCur(Control control);
	ControlRange GetControlRange(Control control);

	uint32_t GetSampleRate() { return GetControlCur(Frequency); }
	bool SetSampleRate(uint32_t newsr) { return SetControlCur(Frequency,newsr); }
	std::vector<uint32_t> GetAllowedSampleRates();

private:
	uint8_t bClockID;		// unique identifier for the unit in the audio function
	uint8_t bmAttributes;   // 0/1: type (00:ext, 01:int-fix, 10:int-var, 11:int-prog), 2:synced to SOF, 3/7: reserved
	uint8_t bmControls;		// 0/1: freq control, 2/3: validity control, 4/7: reserved
	uint8_t bAssocTerminal;	// id of the terminal associated with this clock source
	uint8_t iClockSource;	// index of the clock source entity string descriptor

};



class ClockSelector : public AudioControlEntity {

public:

	ClockSelector(uint8_t const* rawDesc,libusb_device_handle* dh,uint8_t itfIdx); // no need for protocol, clocksource is only for UAC2
	uint8_t GetID() { return bClockID; }
	tstring DumpInfo();

	bool CanSet();
	bool CanRead();
	bool SelectSource(uint8_t clockSourceID);
	uint8_t GetCurrentSource();
	ControlRange GetSourcesAsRange(); // probably unuseful

private:
	uint8_t bClockID;		// unique identifier for the unit in the audio function
	uint8_t bNrInPins;		// Number of input pins for this selector (clock sources connected to it)
	std::vector<uint8_t> baCSourceID; // array for the IDs of the clock sources connected to this selector
	uint8_t bmControls;	 	// bits 0/1 : clock selection control
	uint8_t iClockSelector;	// index of the clock selector entity string descriptor

	ClockSelector() {} // we don't want anyone to call this
};


class FormatType {  // used by the audio streaming interface

	libusb_device_handle* devh;
	uint8_t interfaceIndex;
	uint8_t protocol;
	bool IsUAC2() { return (protocol==USB_AUDIO_CLASS_2); }
	bool isValid;

	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bFormatType;
	uint8_t bBitResolution;
	uint8_t bSubframeSize; // bytes occupied by a subframe (sample relative to one channel). will be <= than bits/8
	// NB: in UAC2 specification, instead of bSubframeSize you have "bSubslotSize",
	// so we are using bSubframeSize for both versions,

	// UAC1 only attributes:
	uint8_t bNrChannels;
	uint8_t bSamFreqType;   	// 0: continuous 1-255: number of supported frequencies
	uint32_t tLowerSamFreq;
	uint32_t tUpperSamFreq;
	uint32_t* tSamFreq;


public:
	FormatType(uint8_t const* rawDesc,libusb_device_handle* dh,uint8_t itfIdx, uint8_t protoc);
	~FormatType();
	tstring DumpInfo();
	std::vector<uint32_t> GetAllowedSampleRates();
	uint8_t GetNumChannels() { return bNrChannels; }
	uint8_t GetBitResolution() { return bBitResolution; }
	uint8_t GetBytesPerSample() { return bSubframeSize; }
	uint8_t IsType1() { return bFormatType==FORMAT_TYPE_I;}
	// NB: bBitResolution can be less than 8*bSubframeSize !!!
	// (in that case, you should shift the sample value by 8 bits to the left. see frmts10.pdf pag 9)
};


}

#endif /* USBAUDIOENTITIES_H_ */
