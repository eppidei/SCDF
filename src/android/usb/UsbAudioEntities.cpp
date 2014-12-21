/*
 * UsbAudioEntities.cpp
 *
 *  Created on: 17/gen/2014
 *      Author: athos
 */


#include "UsbAudioEntities.h"
#include <bitset>
#include "Logging.h"

// The 12 standard channel configurations (used by several types of entity)
static const char * const channelConfigStrings_UAC1[12] = {
		"Left Front (L)", "Right Front (R)", "Center Front (C)", "Low Frequency Effects (LFE)",
        "Left Surround (LS)", "Right Surround (RS)", "Left of Center (LC)", "Right of Center (RC)",
        "Surround (S)", "Side Left (SL)", "Side Right (SR)", "Top (T)" };


static const char * const channelConfigStrings_UAC2[] = {
"Front Left (FL)", "Front Right (FR)", "Front Center (FC)", "Low Frequency Effects (LFE)",
"Back Left (BL)", "Back Right (BR)", "Front Left of Center (FLC)", "Front Right of Center (FRC)",
"Back Center (BC)","Side Left (SL)", "Side Right (SR)","Top Center (TC)", "Top Front Left (TFL)",
"Top Front Center (TFC)", "Top Front Right (TFR)", "Top Back Left (TBL)","Top Back Center (TBC)",
"Top Back Right (TBR)", "Top Front Left of Center (TFLC)", "Top Front Right of Center (TFRC)",
"Left Low Frequency Effects (LLFE)", "Right Low Frequency Effects (RLFE)", "Top Side Left (TSL)",
"Top Side Right (TSR)", "Bottom Center (BC)", "Back Left of Center (BLC)", "Back Right of Center (BRC)"
};

static uint32_t supportedSampleRates[] = { 44100, 48000, 88200, 96000, 176400, 192000 };
static const int numSupportedSampleRates = 6;


Usb::UnsupportedEntity::UnsupportedEntity(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t protoc)
{
	devh=dh;
	protocol=protoc;
	//  these offsets should be common to all entities
	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	id = rawDesc[3];
}

tstring Usb::UnsupportedEntity::DumpInfo()
{
	stringformatter os;
	os << "  UNSUPPORTED AudioControl ENTITY: " << AudioControlSubtypeToString(bDescriptorSubtype,protocol)
			<< " (" << (unsigned)bDescriptorSubtype
			<< ") [id: " << (unsigned)id<< "]\n";
	LOGD(os.str().c_str());
	return os.str();
}


/******************************** AUDIO CONTROL HEADER ***************************************/


Usb::AudioControlHeader::AudioControlHeader()
{
	bcdADC = 0;
	wTotalLength = 0;
	bInCollection = 0;
	baInterfaceNr = NULL;
}

Usb::AudioControlHeader::AudioControlHeader(uint8_t const* rawDesc, libusb_device_handle* dh,uint8_t protoc)
{
	devh=dh;
	protocol = protoc;

	baInterfaceNr = NULL;

	// attributes common to UAC1 and UAC2:
	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bcdADC = rawDesc[3] | (rawDesc[4]<<8) ;

	if (!IsUAC2()) { // UAC1 attributes (see audio10.pdf, page 37)
		wTotalLength = rawDesc[5] | (rawDesc[6]<<8);
		bInCollection = rawDesc[7];
		baInterfaceNr = new uint8_t[bInCollection];
		for (int i = 0; i < bInCollection; i++)
			baInterfaceNr[i] = rawDesc[8+i];
	} else { // UAC2
		bCategory = rawDesc[5];
		wTotalLength = wTotalLength = rawDesc[6] | (rawDesc[7]<<8);
		bmControls = rawDesc[8];
	}

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC_DESC_HEADER)
		isValid=false;
	else
		isValid=true;
}

Usb::AudioControlHeader::~AudioControlHeader()
{
	delete [] baInterfaceNr;
}

tstring Usb::AudioControlHeader::DumpInfo()
{
	stringformatter os;
	os << "  Audio Control Interface HEADER:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,protocol) << "\n"
		<< "    bcdADC: " << std::hex << bcdADC << std::dec << "\n"
		<< "    wTotalLength: " << (unsigned)wTotalLength << "\n";

	if (!IsUAC2()) {
		os << "    bInCollection: " << (unsigned)bInCollection << "\n";
		for (int i = 0; i < bInCollection; i++)
			os << "     baInterfaceNr[" << i << "]: " << (unsigned)baInterfaceNr[i] << "\n";
		if(bLength < 8+bInCollection)
			os << "   Warning: header descriptor too short! \n";
	} else { // UAC2
		os << "    bCategory: " << (unsigned)bCategory << "\n";
		std::bitset<8> controlsbitset(bmControls);
		os << "    bmControls: " << controlsbitset << "\n";
	}
	LOGD(os.str().c_str());
	return os.str();
}



/******************************** AUDIO CONTROL INPUT TERMINAL ***************************************/


Usb::InputTerminal::InputTerminal()
{
	bTerminalID = 0;
	wTerminalType = 0;
	bAssocTerminal = 0;
	bNrChannels	= 0;
	wChannelConfig	= 0;
	iChannelNames	= 0;
	iTerminal		= 0;
	bmChannelConfig = 0;
	bCSourceID = 0;
	bmControls = 0;

}

Usb::InputTerminal::InputTerminal(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t protoc)
{
	devh=dh;
	protocol=protoc;

	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bTerminalID = rawDesc[3];
	wTerminalType = rawDesc[4] | (rawDesc[5] << 8);
	bAssocTerminal = rawDesc[6];

	if (IsUAC2()) { // UAC2
		bCSourceID = rawDesc[7];
		bNrChannels = rawDesc[8];
		bmChannelConfig = rawDesc[9] | (rawDesc[10]<<8) | (rawDesc[11]<<16) | (rawDesc[12]<<24);
		iChannelNames = rawDesc[13];
		bmControls = rawDesc[14] | (rawDesc[15]<<8);
		iTerminal = rawDesc[16];
	} else { //  offsets according to usb specification (see audio10.pdf, page 39)
		bNrChannels	= rawDesc[7];
		wChannelConfig	= rawDesc[8] | (rawDesc[9] << 8);
		iChannelNames	= rawDesc[10];
		iTerminal		= rawDesc[11];
	}

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC_DESC_INPUT_TERMINAL)
		isValid=false;
	else
		isValid=true;
}

/* Each bit set in this bit map indicates there is a logical channel in the cluster that carries
 * audio information, destined for the indicated spatial location. The channel ordering in the
 * cluster must correspond to the ordering, imposed by the above list of predefined spatial locations.
 * If there are more channels in the cluster than there are bits set in the wChannelConfig field,
 * (i.e. bNrChannels > [Number_Of_Bits_Set]), then the first [Number_Of_Bits_Set] channels take the
 * spatial positions, indicated in wChannelConfig. The remaining channels have �non-predefined�
 * spatial positions (positions that do not appear in the predefined list). If none of the bits in
 * wChannelConfig are set, then all channels have non-predefined spatial positions. If one or more
 * channels have non-predefined spatial positions, their spatial location description can optionally
 * be derived from the iChannelNames field.
 *
 * iChannelNames: index to a string descriptor that describes the spatial location of the first
 * non-predefined logical channel in the cluster. The spatial locations of all remaining logical
 * channels must be described by string descriptors with indices that immediately follow the index
 * of the descriptor of the first non-predefined channel. Therefore, iChannelNames inherently
 * describes an array of string descriptor indices, ranging from iChannelNames
 * to (iChannelNames + (bNrChannels- [Number_Of_Bits_Set]) - 1)
 *
 * If there are no non-predefined logical channels, this index must be set to 0.
*/

bool Usb::InputTerminal::IsChannelPresent(uint8_t chan)
{
	return (chan<bNrChannels); //(wChannelConfig>>chan) & 1;
}

int Usb::InputTerminal::NumberPredefinedChannels()
{
	if (IsUAC2()) {
		std::bitset<32> set(bmChannelConfig);
		return set.count();
	} else {
		std::bitset<16> set(wChannelConfig);
		return set.count();
	}
}

tstring Usb::InputTerminal::GetChannelName(uint8_t chan)
{
	if (!IsChannelPresent(chan))
			return "Not present";

	int nPredef = NumberPredefinedChannels();

	if (chan >= nPredef) {
		if (iChannelNames==0)
			return "Unspecified";
		return Usb::GetString(devh, iChannelNames+(chan-nPredef) );
	}

	// find the chan-th bit set:
	if (IsUAC2()) {
		int count=0;
		chan++;
		for (int i=0; i<32; i++) {
			if (bmChannelConfig>>i & 1)
				count++;
			if (count==chan)
				return channelConfigStrings_UAC2[count-1];
		}
		return "Error";
	}
	else { // UAC 1
		int count=0;
		chan++;
		for (int i=0; i<16; i++) {
			if (wChannelConfig>>i & 1)
				count++;
			if (count==chan)
				return channelConfigStrings_UAC1[count-1];
		}
		return "Error";
	}
}

tstring Usb::InputTerminal::DumpInfo()
{
	stringformatter os;
	os << "  Audio Control INPUT TERMINAL:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,protocol) << "\n"
		<< "    bTerminalID: " <<  (unsigned)bTerminalID << "\n"
		<< "    wTerminalType: " << (unsigned)wTerminalType << " ("<< TerminalTypeToString(wTerminalType) << ")\n"
		<< "    bAssocTerminal: " << (unsigned)bAssocTerminal << "\n"
		<< "    bNrChannels: " <<  (unsigned)bNrChannels << "\n"
		<< "    iChannelNames: " <<  (unsigned)iChannelNames << "\n"
		<< "    iTerminal: " <<  (unsigned)iTerminal << " ("<< Usb::GetString(devh,iTerminal)<<")\n";

	if (IsUAC2()) {
		os << "    bCSourceID: " << (unsigned)bCSourceID << "\n";
		std::bitset<16> controlsbitset(bmControls);
		os << "    bmControls: " << controlsbitset << "\n";
		std::bitset<32> configbitset(bmChannelConfig);
		os << "    bmChannelConfig: " << configbitset << "\n";
		if (iChannelNames!=0 || configbitset.any()) // avoid lists of "Unspecified" channel names only
			for (uint8_t i=0; i<bNrChannels; i++)
				os << "      - Channel " << (unsigned)i << ": " << GetChannelName(i) << "\n";
		if(bLength < 17)
				os << "  Warning: header descriptor too short! \n";
	} else {
		std::bitset<16> configbitset(wChannelConfig);
		os << "    wChannelConfig: " << configbitset << "\n";
		if (iChannelNames!=0 || configbitset.any()) // avoid lists of "Unspecified" channel names only
			for (uint8_t i=0; i<bNrChannels; i++)
				os << "      - Channel " << (unsigned)i << ": " << GetChannelName(i) << "\n";
		if(bLength < 12)
			os << "  Warning: header descriptor too short! \n";
	}

	LOGD(os.str().c_str());
	return os.str();
}





/******************************** AUDIO CONTROL OUTPUT TERMINAL ***************************************/


Usb::OutputTerminal::OutputTerminal()
{
	bTerminalID = 0;
	wTerminalType = 0;
	bAssocTerminal = 0;
	bSourceID = 0;
	iTerminal = 0;
	bCSourceID = 0;
	bmControls = 0;
}

Usb::OutputTerminal::OutputTerminal(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t protoc)
{
	protocol = protoc;
	devh=dh;

	//  offsets according to usb specification (see audio10.pdf, page 39)
	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bTerminalID = rawDesc[3];
	wTerminalType = rawDesc[4] | (rawDesc[5] << 8);
	bAssocTerminal = rawDesc[6];
	bSourceID = rawDesc[7];

	if (IsUAC2()) {
		bCSourceID = rawDesc[8];
		bmControls = rawDesc[9] | (rawDesc[10]<<8);
		iTerminal = rawDesc[11];
	} else {
		iTerminal = rawDesc[8];
	}

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC_DESC_OUTPUT_TERMINAL)
		isValid=false;
	else
		isValid=true;
}

tstring Usb::OutputTerminal::DumpInfo()
{
	stringformatter os;
	os << "  Audio Control OUTPUT TERMINAL:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,protocol) << "\n"
		<< "    bTerminalID: " <<  (unsigned)bTerminalID << "\n"
		<< "    wTerminalType: " << wTerminalType << " ("<< TerminalTypeToString(wTerminalType) << ")\n"
		<< "    bAssocTerminal: " << (unsigned)bAssocTerminal << "\n"
		<< "    bSourceID (connected unit id): " << (unsigned) bSourceID << "\n"
		<< "    iTerminal: " <<  (unsigned)iTerminal << "("<< Usb::GetString(devh,iTerminal)<<")\n";

	if (IsUAC2()) {
		std::bitset<16> controlsset(bmControls);
		os << "    bCSourceID: " << (unsigned) bCSourceID << "\n"
			<< "    bmControls: " << controlsset << "\n";
	}

	LOGD(os.str().c_str());
	return os.str();
}




/******************************** AUDIO CONTROL FEATURE UNIT ***************************************/


Usb::FeatureUnit::FeatureUnit()
{
	bSourceID = 0;
}

Usb::FeatureUnit::FeatureUnit(uint8_t const* rawDesc, libusb_device_handle* dh, uint8_t itfIdx, uint8_t protoc)
{
	devh=dh;
	protocol = protoc;
	interfaceIndex=itfIdx;

	//  offsets according to usb specification (see audio10.pdf, page 43)
	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bUnitID = rawDesc[3];
	bSourceID = rawDesc[4];
	bControlSize  = IsUAC2() ? 4 : rawDesc[5];  // bmAcontrols is always 32 bit for UAC2!

	if (bControlSize==0)
		bControlSize=1;

	int bmaControlsStartPos = IsUAC2() ? 5 : 6;
	int fixedPartLen = bmaControlsStartPos+1;

	// we are inferring the number of channels from the size of the descriptor and the size of each channel's bmControl field
	numChannels = (bLength - fixedPartLen) / bControlSize;

	// NB: another (safer) way to know the channel number is getting it from the channel cluster of
	// the signal source for this unit. For this, we should have knowledge of the device's units graph, and
	// discover the first unit back in the chain (starting from this feature unit), that indicates
	// an output channel cluster descriptor (ie: a wChannelConfig or bmChannelConfig).

	bmaControls = new uint8_t[numChannels*bControlSize];
	for (int i=0; i<numChannels*bControlSize; i++) {
		bmaControls[i] = rawDesc[bmaControlsStartPos+i]; // 6 is the position of the first byte of the first bmControl
	}

	iFeature = rawDesc[bmaControlsStartPos+numChannels*bControlSize];

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC_DESC_FEATURE_UNIT)
		isValid=false;
	else
		isValid=true;
}

Usb::FeatureUnit::~FeatureUnit() {
	delete [] bmaControls;
}


uint32_t Usb::FeatureUnit::GetControlsBitmap(uint8_t channel)
{
	if (channel >= numChannels) {
		LOGE("USBAUDIO - FeatureUnit %d: channel %d not present!",bUnitID,channel);
		return 0;
	}

	int theBitmap = 0;

	for (int i=0; i<bControlSize; i++) {
		theBitmap = theBitmap | ( bmaControls[(channel*bControlSize)+i] << 8*i);
	}
	return theBitmap;
}


bool Usb::FeatureUnit::ChannelHasControl(uint8_t channel, uint8_t control) // for UAC2 we mean WRITE control
{
	if (control == FU_CTRL_UNDEFINED) {
		LOGE("USBAUDIO - FeatureUnit %d: can't have \"undefined\" control!",bUnitID);
		return false;
	}
	if (control > FU_CTRL_NUMBER_UAC_1 && !IsUAC2()) {
		LOGE("USBAUDIO - FeatureUnit %d: Unsupported control (max allowed:%d).",bUnitID,FU_CTRL_NUMBER_UAC_1);
		return false;
	}
	int uacFactor = IsUAC2() ? 2 : 1;  // for UAC2, we test the 2nd bit (write) associated to the control
	bool hasControl = ( GetControlsBitmap(channel) >> ((control*uacFactor)-1) ) & 1;
	//LOGD("USBAUDIO - Channel %u has control %u ? %s (uac2:%d)",channel,control,hasControl?"YES":"NO",IsUAC2());
	return hasControl;
	// NB: control is one of the FU_ macros (eg: FU_VOLUME), that start from 1
	// (0 means "undefined"), so we need subtract "1" for the bit shift
	// EG: FU_MUTE is "1", and in the bitmask is at position "0".
	// FOR UAC2, remember that if the second (write) bit for the control is set,
	// the first can be be supposed to be set as well (ie, allowed values for a
	// couple are 00,01 and 11, NOT 10)
}

tstring Usb::FeatureUnit::GetChannelName(uint8_t channel)
{
	if (channel==0) return "Master";
	if (channel>=numChannels) return "Not present";
	stringformatter os;
	os << "Channel " << (unsigned)channel;
	return os.str();
	// NB: yes, we need to get more significant names for the channels.
	// but for that we need to know the channels cluster descriptor
	// and thus the device's units graph. (see considerations in constructor)
}

tstring Usb::FeatureUnit::GetControlName(uint8_t control)
{
	if (control > FU_CTRL_NUMBER_UAC_1 && !IsUAC2()) // forcing "unsupported" for controls not available on UAC1
		return "Unsupported";
	return ControlToString(control);
}

bool Usb::FeatureUnit::SetControlRequest(uint8_t attribute, uint8_t channel, uint8_t control, uint8_t* data, uint16_t size)
{
	uint8_t bRequest = attribute;
	uint16_t wValue = channel | (control<<8);
	uint16_t wIndex = (bUnitID << 8) | interfaceIndex;

	//LOGD("USBAUDIO - SetControlRequest. wValue:%x (chan:%x | ctrl<<8:%x), wIndex:%x (unit<<8:%x | itf:%x)",
	//		wValue,channel,control<<8,wIndex,bUnitID<<8,interfaceIndex);

	return Usb::SetRequestForInterface(devh,bRequest,wValue,wIndex,data,size);
}


bool Usb::FeatureUnit::GetControlRequest(uint8_t attribute, uint8_t channel, uint8_t control, uint8_t* data, uint16_t size)
{
	uint8_t bRequest = attribute;
	uint16_t wValue = (control<<8) | channel ;
	uint16_t wIndex = (bUnitID << 8) | interfaceIndex;

	//LOGD("USBAUDIO - GetControlRequest. ctrl: %s, attr:%x, wValue:%x (chan:%x | ctrl<<8:%x), wIndex:%x (unit<<8:%x | itf:%x)",
	//		ControlToString(control).c_str(),attribute,wValue,channel,control<<8,wIndex,bUnitID<<8,interfaceIndex);

	return Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,data,size);
}



bool Usb::FeatureUnit::SetControlCurr(uint8_t chan, uint8_t control,uint32_t value)
{
	// NB: we only support setting of current value.

	if (!ChannelHasControl(chan,control)) {
		LOGE("USBAUDIO - Set Control: %s has no %s control",GetChannelName(chan).c_str(),GetControlName(control).c_str());
		return false;
	}

	uint8_t attribute = IsUAC2() ? UAC2_CUR : UAC1_SET_CUR;
	RequestData dataInfo = AllocateDataBuffer(control);
	if (dataInfo.valSize==0)
		return false;

	for (int i=0; i<dataInfo.valSize; i++)
		dataInfo.data[i] = (uint8_t)(value>>8*i);

	//LOGD("USBAUDIO - set control request. chan %u, control %u, value %u",chan,control,value);
	bool res = SetControlRequest(attribute,chan,control,dataInfo.data,dataInfo.valSize);
	// NB: set requests are for CUR attribute, so only the place for a single value is needed (both for UAC1 and UAC2)
	DeallocateDataBuffer(dataInfo.data);
	return res;
}

uint32_t Usb::FeatureUnit::GetControlCurr(uint8_t chan, uint8_t control)
{
	return GetControl( IsUAC2()? UAC2_CUR : UAC1_GET_CUR, chan, control);
}

uint32_t Usb::FeatureUnit::GetControlMin(uint8_t chan, uint8_t control)
{
	if (IsUAC2()){
		return GetControlRange(chan,control).Min(0);
	}
	return GetControl(UAC1_GET_MIN, chan, control);
}

uint32_t Usb::FeatureUnit::GetControlMax(uint8_t chan, uint8_t control)
{
	if (IsUAC2()){
		return GetControlRange(chan,control).Max(0);
	}
	return GetControl(UAC1_GET_MAX, chan, control);
}

uint32_t Usb::FeatureUnit::GetControlRes(uint8_t chan, uint8_t control)
{
	if (IsUAC2()){
		return GetControlRange(chan,control).Res(0);
	}
	return GetControl(UAC1_GET_RES, chan, control);
}

uint32_t Usb::FeatureUnit::GetControl(uint8_t attribute, uint8_t chan, uint8_t ctrl)
{
	uint8_t getReq = IsUAC2() ? UAC2_CUR : UAC1_GET_CUR;

	if (attribute!=getReq && IsControlBoolean(ctrl) ) {
		LOGE("USBAUDIO - GetControl: boolean attributes don't have range or resolution");
		return 0;
	}
	if (!ChannelHasControl(chan,ctrl)) {
		LOGE("USBAUDIO - Get Control: %s has no %s control",GetChannelName(chan).c_str(),GetControlName(ctrl).c_str());
		return 0;
	}
	if(IsUAC2() && attribute==UAC2_RANGE) {
		LOGE("USBAUDIO - Don't use GetControl() for UAC2 RANGE requests! Use GetControlRange()!");
		return 0;
	}

	RequestData dataInfo = AllocateDataBuffer(ctrl);
	if (dataInfo.valSize==0) {
		LOGE("USBAUDIO - Get Control error: zero length data buffer");
		return 0;
	}

	bool res = GetControlRequest(attribute,chan,ctrl,dataInfo.data,dataInfo.valSize); // CUR attribute or UAC1 -> one value
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: get request failed");
		return 0;
	}

	uint32_t retValue = 0;

	for (int i=0; i<dataInfo.valSize; i++)
		retValue = retValue | (dataInfo.data[i] << (8*i));
	DeallocateDataBuffer(dataInfo.data);
	return retValue;
}

Usb::ControlRange Usb::FeatureUnit::GetControlRange(uint8_t chan, uint8_t ctrl)
{
	ControlRange invalidRange;
	if (!IsUAC2()) {
		LOGE("USBAUDIO - Range not supported for UAC1!"); // we could actually build it with 3 different get requests...
		return invalidRange;
	}
	if (IsControlBoolean(ctrl) ) {
		LOGE("USBAUDIO - GetControl: boolean attributes don't have range or resolution");
		return invalidRange;
	}
	if (!ChannelHasControl(chan,ctrl)) {
		LOGE("USBAUDIO - Get Control: %s has no %s control",GetChannelName(chan).c_str(),GetControlName(ctrl).c_str());
		return invalidRange;
	}

	ControlRange range(1,ControlLayout(ctrl));

	bool res = GetControlRequest(UAC2_RANGE,chan,ctrl,range.rawData,range.rawSize);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: get request failed");
		return invalidRange;
	}
	return range;
}


Usb::RequestLayout Usb::FeatureUnit::ControlLayout(uint8_t control)
{
	switch(control) {
		case FU_CTRL_MUTE:		case FU_CTRL_BASS_BOOST:
		case FU_CTRL_AUTO_GAIN:	case FU_CTRL_LOUDNESS:
		case FU_CTRL_BASS:		case FU_CTRL_MID:
		case FU_CTRL_TREBLE:	case FU_CTRL_PHASE_INVERTER:
			return ReqLayout1;
		case FU_CTRL_VOLUME:	case FU_CTRL_INPUT_GAIN:
		case FU_CTRL_INPUT_GAIN_PAD:
			return ReqLayout2;
		default:
			LOGE("USBAUDIO - Feature Unit \"%s\" unsupported!",ControlToString(control).c_str());
			return ReqLayoutUnsupported; // null data buffer and 0 sizes.
		}
}


Usb::RequestData Usb::FeatureUnit::AllocateDataBuffer(uint8_t control)
{
	RequestData req;
	uint8_t valueSize = 0;
	// volume: len 2 value: 0x0000->0dB, 0x0001->0.0039dN, 0xFFFF->-0.0039, max 0x7FFFF->127.99, max 0x8000->-inf (v pag 77)
	// bass ctrl: len 1 value:number 0x80->-32db 0x00->0db 0x7f->31.75
	// mid 1 come bass
	// treble 1 come bass
	// eq: don't support for now
	// delay: 2 0x0000->0ms 0x0001->0.0156 0xFFFF->1023.9844

	switch(control) {
		case FU_CTRL_MUTE:
		case FU_CTRL_BASS_BOOST:
		case FU_CTRL_AUTO_GAIN:
		case FU_CTRL_LOUDNESS:
		case FU_CTRL_BASS:
		case FU_CTRL_MID:
		case FU_CTRL_TREBLE:
		case FU_CTRL_PHASE_INVERTER:
			req.valSize=1;
			break;
		case FU_CTRL_VOLUME:
		case FU_CTRL_INPUT_GAIN:
		case FU_CTRL_INPUT_GAIN_PAD:
			req.valSize=2;
			break;
		default:
			LOGE("USBAUDIO - Feature Unit \"%s\" unsupported! Won't allocate buffer.",ControlToString(control).c_str());
			return req; // null data buffer and 0 sizes.
		}

	req.rangeSize=2+req.valSize*3;
	// NB: 2 bytes for the wNumSubRanges field (that we'll ignore and support only one range)
	// 3 range data fields (dMIN, dMAX, dRES)
	req.data = new uint8_t[req.rangeSize];
	memset(req.data, 0, req.rangeSize*sizeof(uint8_t) );
	// NB: in UAC1 (or UAC2 CUR requests), we allocate a buffer bigger than needed
	return req;
}

void Usb::FeatureUnit::DeallocateDataBuffer(uint8_t* data)
{
	delete [] data;
}

tstring Usb::FeatureUnit::DumpInfo()
{
	stringformatter os;
	os << "  Audio Control FEATURE UNIT:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bUnitID: " << (unsigned)bUnitID << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,protocol) << "\n"
		<< "    bSourceID (connected unit id): " << (unsigned) bSourceID << "\n"
		<< "    iFeature: " <<  (unsigned)iFeature << " ("<< Usb::GetString(devh,iFeature)<<")\n";

	for (uint8_t i=0; i<numChannels; i++) {
		os << "    " << GetChannelName(i) << " controls: \n";
		int max = IsUAC2() ? FU_CTRL_NUMBER_UAC_2 : FU_CTRL_NUMBER_UAC_1;
		for (uint8_t k=FU_CTRL_MUTE; k<=max; k++) {
			if (ChannelHasControl(i,k)) {
				if (IsControlBoolean(k))
					os << "     - " << GetControlName(k) << " = " << (GetControlCurr(i,k)!=0?"on":"off") << "\n";
				else
					os << "     - " << GetControlName(k) << " = " << GetControlCurr(i,k)
						<< "  [min:" << GetControlMin(i,k) << " max:" << GetControlMax(i,k) << " res: " << GetControlRes(i,k) << "]\n";
			}
		}
	}

	//LOGI("! Master MUTE MIN: %d", GetControlRes(0,FU_CTRL_MUTE));

	/*for (uint8_t i=0; i<numChannels; i++) {
			os << "  " << GetChannelName(i) << " controls AGAIN!: \n";
			for (uint8_t k=FU_CTRL_MUTE; k<=FU_CTRL_NUMBER_USB_1; k++) {
				if (ChannelHasControl(i,k)) {
					if (IsControlBoolean(k))
						os << "   - " << GetControlName(k) << " = " << (GetControlCurr(i,k)!=0?"on":"off")<<"\n";
					else
						os << "   - " << GetControlName(k) << " = " << GetControlCurr(i,k)
							<< "  [min:" << GetControlMin(i,k) << " max:" << GetControlMax(i,k) << " res: " << GetControlRes(i,k) << "]\n";
			}
		}
	}*/

	LOGD(os.str().c_str());
	return os.str();
}


tstring Usb::FeatureUnit::ControlToString(uint8_t control_id)
{
	switch(control_id) {
		//Feature Unit Control Selectors
		case FU_CTRL_UNDEFINED: return "Undefined";
		case FU_CTRL_MUTE: 		return "Mute";
		case FU_CTRL_VOLUME: 	return "Volume";
		case FU_CTRL_BASS: 		return "Bass";
		case FU_CTRL_MID:		return "Mid";
		case FU_CTRL_TREBLE: 	return "Treble";
		case FU_CTRL_EQ: 		return "Graphic EQ";
		case FU_CTRL_AUTO_GAIN: return "Auto Gain";
		case FU_CTRL_DELAY:		return "Delay";
		case FU_CTRL_BASS_BOOST:return "Bass Boost";
		case FU_CTRL_LOUDNESS:	return "Loudness";
		// only for UAC2:
		case FU_CTRL_INPUT_GAIN:	 return "Input Gain";
		case FU_CTRL_INPUT_GAIN_PAD: return "Input Gain Pad";
		case FU_CTRL_PHASE_INVERTER: return "Phase Inverter";
		case FU_CTRL_UNDERFLOW: 	 return "Underflow";
		case FU_CTRL_OVERFLOW: 		 return "Overflow";
		case FU_CTRL_LATENCY: 		 return "Latency";
		default: return "Unsupported";
	}
}

bool Usb::FeatureUnit::IsControlBoolean(uint8_t control_id)
{
	switch(control_id) {
		case FU_CTRL_UNDEFINED:
		case FU_CTRL_MUTE:
		case FU_CTRL_AUTO_GAIN:
		case FU_CTRL_BASS_BOOST:
		case FU_CTRL_LOUDNESS:
		// recheck these:
		case FU_CTRL_INPUT_GAIN_PAD:
		case FU_CTRL_PHASE_INVERTER:
		case FU_CTRL_UNDERFLOW:
		case FU_CTRL_OVERFLOW:
			return true;
		default:
			 // FU_CTRL_VOLUME FU_CTRL_BASS FU_CTRL_MID FU_CTRL_TREBLE
			 // FU_CTRL_EQ FU_CTRL_DELAY FU_CTRL_INPUT_GAIN FU_CTRL_LATENCY
			return false;
	}
}




Usb::ClockSource::ClockSource()
{
	bClockID = 0;
}

Usb::ClockSource::ClockSource(uint8_t const* rawDesc, libusb_device_handle* dh,uint8_t itfIdx)
{
	devh=dh;
	interfaceIndex=itfIdx;

	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bClockID = rawDesc[3];
	bmAttributes = rawDesc[4];
	bmControls = rawDesc[5];
	bAssocTerminal = rawDesc[6];
	iClockSource = rawDesc[7];

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC2_DESC_CLOCK_SOURCE)
		isValid=false;
	else
		isValid=true;
}

Usb::ClockSource::~ClockSource() {

}

Usb::ClockSource::Type Usb::ClockSource::GetType()
{
	return (Type) ( bmAttributes & 0b11 );
}

bool Usb::ClockSource::IsSyncedToSOF()
{
	return ( (bmAttributes & 0b100) != 0);
}

bool Usb::ClockSource::CanSet(Control control)
{
	switch(control) {
		case Frequency: return ( (bmControls & 0b0010) != 0); // bit pos 1
		case Validity:  return ( (bmControls & 0b1000) != 0); // bit pos 3
	}
	return false;
}

bool Usb::ClockSource:: CanRead(Control control)
{
	switch(control) {
		case Frequency: return ( (bmControls & 0b0001) != 0); // bit pos 0
		case Validity:  return ( (bmControls & 0b0100) != 0); // bit pos 2
	}
	return false;
}

bool Usb::ClockSource::SetControlCur(Control control,uint32_t value) {

	if (!CanSet(control)) {
		LOGE("USBAUDIO - Clock Source: can't set control");
		return false;
	}

	RequestData reqData;
	reqData.valSize = control==Frequency ? 4 : 1; // frequency->layout 3, validity->layout 1
	reqData.data = new uint8_t[reqData.valSize];
	for (int i=0; i<reqData.valSize; i++)
		reqData.data[i] = (uint8_t)(value>>8*i);

	uint8_t bRequest = UAC2_CUR;
	uint16_t wValue = (control<<8); // for clock source, channel must be 0.
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;
	bool res = Usb::SetRequestForInterface(devh,bRequest,wValue,wIndex,reqData.data,reqData.valSize);
	delete [] reqData.data;
	return res;
}

uint32_t Usb::ClockSource::GetControlCur(Control control)
{
	if (!CanRead(control)) {
		LOGE("USBAUDIO - Clock Source: can't set control");
		return false;
	}

	RequestData reqData;
	reqData.valSize = control==Frequency ? 4 : 1; // frequency->layout 3, validity->layout 1
	reqData.data = new uint8_t[reqData.valSize];

	uint8_t bRequest = UAC2_CUR;
	uint16_t wValue = (control<<8); // for clock source, channel must be 0.
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;
	bool res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,reqData.data,reqData.valSize);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: get request failed");
		return 0;
	}
	uint32_t retValue = 0;
	for (int i=0; i<reqData.valSize; i++)
		retValue = retValue | (reqData.data[i] << (8*i));

	delete [] reqData.data;
	return retValue;
}

Usb::ControlRange Usb::ClockSource::GetControlRange(Control control)
{
	ControlRange invalidRange;
	if (!CanRead(control)) {
		LOGE("USBAUDIO - Clock Source: can't get control range!");
		return invalidRange;
	}

	// request parameters_
	uint8_t bRequest = UAC2_RANGE;
	uint16_t wValue = (control<<8); // for clock source, channel must be 0.
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;

	// we ask the number of subranges:
	uint8_t probedata[2];
	bool res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,probedata,2);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: couldn't get the number of subranges");
		return invalidRange;
	}
	uint16_t subranges = probedata[0] | (probedata[1]<<8);
	//LOGI("USBAUDIO - Get control range. Found %u subranges",subranges);

	ControlRange range(subranges,Frequency ? ReqLayout3 : ReqLayout1); // frequency->layout 3, validity->layout 1
	res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,range.rawData,range.rawSize);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: get request failed");
		return invalidRange;
	}
	//LOGI("USBAUDIO - Control range %u %u %u %u",range.min,range.max,r.res,r.subRanges);
	return range;
}


std::vector<uint32_t> Usb::ClockSource::GetAllowedSampleRates()
{
	std::vector<uint32_t> freqs;

	if (!CanRead(Frequency) || !CanSet(Frequency)) {
		LOGE("Can't write or read sample rate, return the current one!");
		freqs.push_back(GetSampleRate());
		return freqs;
	}

	ControlRange r = GetControlRange(Frequency);
	for (uint16_t i=0; i<r.Subranges(); i++)
	{
		for (int k=0; k<numSupportedSampleRates; k++)
		{
			if (supportedSampleRates[k]>=r.Min(i) && supportedSampleRates[k]<=r.Max(i)) {
				freqs.push_back(supportedSampleRates[k]);
			}
		}
	}
	//for (int i=0; i<freqs.size(); i++)
	//	LOGD("USBAUDIO - Allowed samplerate: %u",freqs[i]);
	return freqs;
}




tstring Usb::ClockSource::DumpInfo()
{
	std::bitset<8> attr(bmAttributes);
	std::bitset<8> ctrls(bmControls);

	stringformatter os;
	os << "  Audio Control CLOCK SOURCE:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bClockID: " << (unsigned)bClockID << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,USB_AUDIO_CLASS_2) << "\n"
		<< "    bmAttributes: " << attr << "\n"
		<< "    bmControls: " << ctrls << "\n"
		<< "    bAssocTerminal: " << (unsigned)bAssocTerminal << "\n"
		<< "    iClockSource: " << (unsigned)iClockSource << " (" << GetString(devh,iClockSource) << ")\n";

	os << "     Clock Type: " << TypeToString(GetType()) << "\n"
	   << "     Synced to SOF: " << (IsSyncedToSOF()?"Yes":"No")<<"\n";

	os << "     Frequency control. Can read: " << (CanRead(Frequency)?"Yes":"No") << ". Can set: "<<(CanSet(Frequency)?"Yes":"No")<<"\n";
	if (CanRead(Frequency)) {
		ControlRange r = GetControlRange(Frequency);
		os << "      - Current: "<<(unsigned)GetControlCur(Frequency) << " Subranges: "<<(unsigned)r.Subranges()<<"\n";
		for (uint16_t i=0; i<r.Subranges(); i++)
			os<< "      - Range[ " <<(unsigned)i<<"] - Min: "<< (unsigned)r.Min(i) << " Max: "<< (unsigned)r.Max(i)<<" Res: "<<(unsigned)r.Res(i)<<"\n";
	}
	os << "     Validity control. Can read: " << (CanRead(Validity)?"Yes":"No") << ". Can set: "<<(CanSet(Validity)?"Yes":"No")<<"\n";
	if (CanRead(Validity)) {
		os << "      - Current: "<<(unsigned)GetControlCur(Validity) << "\n";
	}

	LOGD(os.str().c_str());
	return os.str();
}

tstring Usb::ClockSource::TypeToString(Usb::ClockSource::Type type)
{
	switch(type) {
	case External: 			return _T("External");
	case InternalFixed: 	return _T("Internal Fixed");
	case InternalVariable: 	return _T("Internal Variable");
	case InternalProgrammable: return _T("Internal Programmable");
	}
	return _T("Invalid");
}

tstring Usb::ClockSource::ControlToString(Usb::ClockSource::Control control)
{
	switch(control) {
		case Frequency: return _T("Frequency");
		case Validity: 	return _T("Validity");
	}
	return _T("Invalid");
}






Usb::ClockSelector::ClockSelector(uint8_t const* rawDesc, libusb_device_handle* dh,uint8_t itfIdx)
{
	devh=dh;
	interfaceIndex=itfIdx;

	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bClockID = rawDesc[3];
	bNrInPins = rawDesc[4];

	for(int i=0; i<bNrInPins; i++)
		baCSourceID.push_back(rawDesc[5+i]);

	bmControls = rawDesc[ 5 + bNrInPins ];
	iClockSelector = rawDesc[ 6 + bNrInPins ];

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=UAC2_DESC_CLOCK_SELECTOR)
		isValid=false;
	else
		isValid=true;
}

bool Usb::ClockSelector::CanSet()
{
	return ( (bmControls & 0b0010) != 0); // bit pos 1
}

bool Usb::ClockSelector:: CanRead()
{
	return ( (bmControls & 0b0001) != 0); // bit pos 0
}

bool Usb::ClockSelector::SelectSource(uint8_t clockSourceID) // Layout I request
{
	if (!CanSet()) {
		LOGE("USBAUDIO - Clock Selector: unable to select a clock source");
		return false;
	}

	int whichIndex = 1;
	for (int i=0; i<baCSourceID.size(); i++) {
		if (baCSourceID[i]==clockSourceID) {
			whichIndex=i+1; // source index in the device is 1-based
			break;
		}
	}

	RequestData reqData;
	reqData.valSize = 1; // layout 1
	reqData.data = new uint8_t;
	*(reqData.data) = whichIndex;

	uint8_t bRequest = UAC2_CUR;
	uint16_t wValue = (1<<8); // for clock selector, channel must be 0.
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;
	bool res = Usb::SetRequestForInterface(devh,bRequest,wValue,wIndex,reqData.data,reqData.valSize);
	delete [] reqData.data;
	return res;
}

uint8_t Usb::ClockSelector::GetCurrentSource()
{
	if (!CanRead()) {
		LOGE("USBAUDIO - Clock Selector: unable to get current source");
		return false;
	}

	RequestData reqData;
	reqData.valSize = 1; // frequency->layout 3, validity->layout 1
	reqData.data = new uint8_t;

	uint8_t bRequest = UAC2_CUR;
	uint16_t wValue = (1<<8); // for clock source, channel must be 0. the CS (control selector) is 1
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;
	bool res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,reqData.data,reqData.valSize);
	if (false == res) {
		LOGE("USBAUDIO - Get Clock Selector Source error: get request failed");
		delete [] reqData.data;
		return 0;
	}
	uint8_t retValue = *(reqData.data);
	delete [] reqData.data;
	if (retValue > baCSourceID.size()) {
		LOGE("USBAUDIO - Get Clock Selector Source error: get request failed");
		return 0;
	}
	return baCSourceID[retValue-1];
	// the value returned from the device is actually the index in the baCSourceID array
	// 1-based, from which we can retrieve the clock source id.
}
/*
Usb::ControlRange Usb::ClockSelector::GetSourcesAsRange()
{
	ControlRange invalidRange;
	if (!CanRead(control)) {
		LOGE("USBAUDIO - Clock Source: can't get control range!");
		return invalidRange;
	}

	// request parameters_
	uint8_t bRequest = UAC2_RANGE;
	uint16_t wValue = (control<<8); // for clock source, channel must be 0.
	uint16_t wIndex = (bClockID << 8) | interfaceIndex;

	// we ask the number of subranges:
	uint8_t probedata[2];
	bool res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,probedata,2);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: couldn't get the number of subranges");
		return invalidRange;
	}
	uint16_t subranges = probedata[0] | (probedata[1]<<8);
	LOGI("USBAUDIO - Get control range. Found %u subranges",subranges);

	ControlRange range(subranges,Frequency ? ReqLayout3 : ReqLayout1); // frequency->layout 3, validity->layout 1
	res = Usb::GetRequestForInterface(devh,bRequest,wValue,wIndex,range.rawData,range.rawSize);
	if (false == res) {
		LOGE("USBAUDIO - Get Control error: get request failed");
		return invalidRange;
	}
	//LOGI("USBAUDIO - Control range %u %u %u %u",range.min,range.max,r.res,r.subRanges);
	return range;
}*/

tstring Usb::ClockSelector::DumpInfo()
{
	std::bitset<8> ctrls(bmControls);

	stringformatter os;
	os << "  Audio Control SOURCE SELECTOR:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bClockID: " << (unsigned)bClockID << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioControlSubtypeToString(bDescriptorSubtype,USB_AUDIO_CLASS_2) << "\n"
		<< "    bmControls: " << ctrls << "\n"
		<< "    iClockSelector: " << (unsigned)iClockSelector << " (" << GetString(devh,iClockSelector) << ")\n"
		<< "    bNrInPins: " << (unsigned) bNrInPins << "\n";

	for (int i=0; i<baCSourceID.size(); i++)
		os << "     - baCSourceID[" << i << "]: "<< (unsigned)baCSourceID[i] << "\n";

	os << "     Clock Selection control. Can read: " << (CanRead()?"Yes":"No") << ". Can set: "<<(CanSet()?"Yes":"No")<<"\n";
	if (CanRead()) {
		os << "      - Current source: "<<(unsigned)GetCurrentSource() << "\n";
	}

	LOGD(os.str().c_str());
	return os.str();
}







Usb::FormatType::FormatType(uint8_t const* rawDesc,libusb_device_handle* dh,uint8_t itfIdx, uint8_t protoc)
{
	devh=dh;
	interfaceIndex=itfIdx;
	protocol = protoc;

	bLength = rawDesc[0];
	bDescriptorType = rawDesc[1];
	bDescriptorSubtype = rawDesc[2];
	bFormatType = rawDesc[3];

	if (IsUAC2()) {
		bSubframeSize  = rawDesc[4];
		bBitResolution = rawDesc[5];
		bNrChannels=bSamFreqType=0;
		tLowerSamFreq=tUpperSamFreq=0;
		tSamFreq=NULL;

		if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=AS_DESC_FORMAT_TYPE)
			isValid=false;
		else
			isValid=true;
		return;
	}

	// UAC1:

	bNrChannels = rawDesc[4];
	bSubframeSize  = rawDesc[5];
	bBitResolution = rawDesc[6];
	bSamFreqType = rawDesc[7];
	//LOGD("Format Type len:%u, ty:%u, sub:%u, form:%u, chan:%u , subframe:%u, bit:%u, srtype:%u",
	//		bLength,bDescriptorType,bDescriptorSubtype,bFormatType,bNrChannels, bSubframeSize,bBitResolution,bSamFreqType);

	if (bSamFreqType==0) {
		// continuous sample rate:
		tLowerSamFreq = 0 | rawDesc[8] | (rawDesc[9] << 8) | (rawDesc[10] << 16);
		tUpperSamFreq = 0 | rawDesc[11] | (rawDesc[12] << 8) | (rawDesc[13] << 16);
		//LOGD("USBAUDIO - allowed sr LOW %u, HIGH %u",tLowerSamFreq,tUpperSamFreq);
		tSamFreq=NULL;
	} else {
		// discrete sample rate allowed values:
		tSamFreq = new uint32_t [bSamFreqType];
		for (uint8_t i=0; i<bSamFreqType; i++) {
			tSamFreq[i] = 0 | rawDesc[8+(3*i)] | (rawDesc[8+(3*i)+1] << 8) | (rawDesc[8+(3*i)+2] << 16);
			//LOGD("USBAUDIO - allowed sr: %u",tSamFreq[i]);
		}
		tLowerSamFreq = tSamFreq[0];
		tUpperSamFreq = tSamFreq[bSamFreqType-1];
	}

	if (bDescriptorType!=DESC_TYPE_INTERFACE || bDescriptorSubtype!=AS_DESC_FORMAT_TYPE)
		isValid=false;
	else
		isValid=true;
}

std::vector<uint32_t> Usb::FormatType::GetAllowedSampleRates()
{
	std::vector<uint32_t> freqs;

	if (bSamFreqType==0) {	// continuous sample rate:



		for (int i=0; i<numSupportedSampleRates; i++)
			if ( supportedSampleRates[i] >= tLowerSamFreq && supportedSampleRates[i] <= tUpperSamFreq ) {
				freqs.push_back( supportedSampleRates[i] );
				//LOGD("USBAUDIO - continuous sr. Allowed value: %u",freqs.back());
			}

	} else { // discrete sample rate allowed values:

		for (uint8_t i=0; i<bSamFreqType; i++)
		{
			for (int k=0; k<numSupportedSampleRates; k++)
			{
				if ( supportedSampleRates[k] == tSamFreq[i] ) {
					freqs.push_back( tSamFreq[i] );
					//LOGD("USBAUDIO - discrete sr. Allowed value: %u",freqs.back());
					break;
				}
			}
		}
	}
	//for (int i=0; i<freqs.size(); i++)
	//	LOGD("USBAUDIO - Allowed samplerate: %u",freqs[i]);
	return freqs;
}


Usb::FormatType::~FormatType()
{
	if (NULL!=tSamFreq)
		delete [] tSamFreq;
}

tstring Usb::FormatType::DumpInfo()
{
	stringformatter os;
	os << "  Audio Streaming FORMAT TYPE:\n"
		<< "    VALID: " << isValid << "\n"
		<< "    bLength: " << (unsigned)bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << AudioStreamingSubtypeToString(bDescriptorSubtype) << "\n"
		<< "    bFormatType: " << std::hex << (unsigned)bFormatType << std::dec << " (" << AudioFormatTypeToString(bFormatType) << ")\n"
		<< "    bSubFrameSize: " <<  (unsigned)bSubframeSize << "\n"
		<< "    bBitResolution: " << (unsigned)bBitResolution << "\n";

	if (!IsUAC2()) {
		os	<< "    bNrChannels: " << (unsigned)bNrChannels << "\n"
			<< "    bSamFreqType: " << (bSamFreqType==0? "Continuous" : "Discrete") << " [" << (unsigned)bSamFreqType << "] \n";
		if (bSamFreqType==0) {
			os << "    tLowerSamFreq: " << tLowerSamFreq << "\n"
			   << "    tUpperSamFreq: " << tUpperSamFreq << "\n";
		} else {
			for (int i=0; i<bSamFreqType; i++)
				os << "    tSamFreq[" << i << "]: " << tSamFreq[i] <<  "\n";
		}
	}

	LOGD(os.str().c_str());
	return os.str();
}


//tstring Usb







/* FOR USB 2:



static const struct bmcontrol uac2_input_term_bmcontrols[] = {
        { "Copy Protect",        0 },
        { "Connector",                1 },
        { "Overload",                2 },
        { "Cluster",                3 },
        { "Underflow",                4 },
        { "Overflow",                5 },
        { NULL }
};

static const struct bmcontrol uac2_output_term_bmcontrols[] = {
        { "Copy Protect",        0 },
        { "Connector",                1 },
        { "Overload",                2 },
        { "Underflow",                3 },
        { "Overflow",                4 },
        { NULL }
};

static const struct bmcontrol uac2_mixer_unit_bmcontrols[] = {
        { "Cluster",                0 },
        { "Underflow",                1 },
        { "Overflow",                2 },
        { NULL }
};

static const struct bmcontrol uac2_extension_unit_bmcontrols[] = {
        { "Enable",                0 },
        { "Cluster",                1 },
        { "Underflow",                2 },
        { "Overflow",                3 },
        { NULL }
};

static const struct bmcontrol uac2_clock_source_bmcontrols[] = {
        { "Clock Frequency",        0 },
        { "Clock Validity",        1 },
        { NULL }
};

static const struct bmcontrol uac2_clock_selector_bmcontrols[] = {
        { "Clock Selector",        0 },
        { NULL }
};

static const struct bmcontrol uac2_clock_multiplier_bmcontrols[] = {
        { "Clock Numerator",        0 },
        { "Clock Denominator",        1 },
        { NULL }
};

static const struct bmcontrol uac2_selector_bmcontrols[] = {
        { "Selector",        0 },
        { NULL }
};

*/

