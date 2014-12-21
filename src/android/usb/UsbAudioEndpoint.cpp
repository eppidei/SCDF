#include "UsbAudioEndpoint.h"
#include <bitset>
#include "Logging.h"
#include <sstream>

Usb::AudioEndpoint::AudioEndpoint(libusb_endpoint_descriptor dsc, libusb_device_handle* dh) : Endpoint(dsc,dh)
{
	isValid=false;
	int len = desc.extra_length	;
	uint8_t const* buf = desc.extra;
	if (len!=0) {
		bLength = buf[0];
		bDescriptorType = buf[1];
		bDescriptorSubtype = buf[2];
		bmAttributes = buf[3];
		bLockDelayUnits = buf[4];
		wLockDelay = buf[5] | (buf[6]<<8);
	}
	if (bDescriptorSubtype!=EP_GENERAL || bDescriptorType!=DESC_TYPE_ENDPOINT
		 || GetRefresh()!=0 || GetPollingInterval()!=1) // for the validity conditions see usb 1.0 audio spec at page 61
	{
		isValid = false;
	} else {
		isValid = true;
	}
	desiredSampleRateForAdaptive=0;
}

bool Usb::AudioEndpoint::SetSampleRate(int sr)
{
	if (!HasSampleRateControl()) {
		LOGD("USBAUDIO - Set sample rate: endpoint %u has no sr control",GetAddress());
		if (IsAdaptive()) {
			desiredSampleRateForAdaptive=sr;
			return true;
		}
		else
			return false;
	}

	uint16_t wValue = EP_CONTROL_SAMPLERATE << 8; // low byte is zero (see page 95 in audio10.pdf)
	uint16_t wIndex = GetAddress();
	uint16_t size = 3;

	uint8_t param[3];
	param[0] = (unsigned char) (sr & 0xff);
	param[1] = (unsigned char) (sr >> 8);
	param[2] = (unsigned char) (sr >> 16);

	return SetRequestForEndpoint(devh,UAC1_SET_CUR,wValue,wIndex,param,size);
}

/*bool Usb::AudioEndpoint::SetSampleRate(libusb_device_handle* devhnd, uint8_t epAddress, int sr)
// static version!
{
	// we should check (or know already) if samplerate control is available for the endpoint...
	uint16_t wValue = EP_CONTROL_SAMPLERATE << 8; // low byte is zero (see page 95 in audio10.pdf)
	uint16_t wIndex = epAddress;
	uint16_t size = 3;

	uint8_t param[3];
	param[0] = (unsigned char) (sr & 0xff);
	param[1] = (unsigned char) (sr >> 8);
	param[2] = (unsigned char) (sr >> 16);

	return SetRequestForEndpoint(devhnd,UAC1_SET_CUR,wValue,wIndex,param,size);
}*/

uint32_t Usb::AudioEndpoint::GetCurrentSampleRate()
{
	if (!HasSampleRateControl()) {
		LOGD("USBAUDIO - GetCurrentSampleRate: endpoint %x has no sample rate control",GetAddress());
		if (IsAdaptive()) {
			return desiredSampleRateForAdaptive;
		}
		return 0; // in this way the alternate setting will try to figure out the sr (if it's only one!)
	}

	uint16_t wValue = EP_CONTROL_SAMPLERATE << 8; // low byte is zero (see page 95 in audio10.pdf)
	uint16_t wIndex = GetAddress();
	uint16_t size = 3;
	uint8_t param[3];

	bool ret = GetRequestForEndpoint(devh,UAC1_GET_CUR,wValue,wIndex,param,size);

	if (!ret) {
		LOGE("USBAUDIO - Error getting sample rate on endpoint %u",GetAddress());
		return 0;
	}

	uint32_t rate = param[0] | (param[1] << 8) | (param[2] << 16);
	//LOGD("USBAUDIO - GetCurrentSampleRate: %u",rate);
	return rate;

}





/* TODO:
SEt and Get Pitch Control
The Pitch Control enables or disables the ability of an adaptive endpoint to dynamically track its
sampling frequency. The Control is necessary because the clock recovery circuitry must be informed
whether it should allow for relatively large swings in the sampling frequency. A Pitch Control can
have only the current setting attribute (CUR). The position of a Pitch Control CUR attribute can be
either TRUE or FALSE.*/

bool Usb::AudioEndpoint::HasSampleRateControl()
{
	return (bmAttributes & 0b00000001); // bit D0 indicates presence of SR control
}

bool Usb::AudioEndpoint::HasPitchControl()
{
	return (bmAttributes & 0b00000010); // bit D1 indicates presence of pitch control
}

bool Usb::AudioEndpoint::OnlyAcceptsMaxSizePackets()
{
	return (bmAttributes & 0b10000000); // bit D7 indicates the requirement for wMaxPacketSize packets (pag 62)
}

uint8_t Usb::AudioEndpoint::GetLockDelayUnits()
{
	return bLockDelayUnits;
}

uint16_t Usb::AudioEndpoint::GetLockDelay()
{
	return wLockDelay;
}



std::string Usb::AudioEndpoint::DumpInfo()
{
	std::stringstream os, ostemp;
	std::bitset<8> attributesbitset(bmAttributes);

	ostemp << "  AUDIO ENDPOINT class specific info:\n"
		<< "   VALID: " << isValid << "\n"
		<< "    bLength: " << bLength << "\n"
		<< "    bDescriptorType: " << DescriptorTypeToString(bDescriptorType) << "\n"
		<< "    bDescriptorSubtype: " << (bDescriptorSubtype==EP_GENERAL?"EP_GENERAL":"Invalid") << "\n"
		<< "    bmAttributes: " << attributesbitset << " [";

	if (HasSampleRateControl())
		ostemp << "SampleRate CTL, ";
	if (HasPitchControl())
		ostemp << "Pitch CTL";

	ostemp << "]\n" << "    Max size packets only? " << (OnlyAcceptsMaxSizePackets()?"Yes":"No") << "\n"
		<< "   Lock Delay: " << (unsigned) GetLockDelay() << " ";

	if (GetLockDelayUnits()==1) // ms
		ostemp << "ms\n";
	else if (GetLockDelayUnits()==2) // samples
		ostemp << "PCM samples\n";
	else
		ostemp << "(invalid unit)\n";

	os << Endpoint::DumpInfo();
	LOGD("%s",ostemp.str().c_str());
	os << ostemp.str();

	return os.str();
}




/* ----------------------------- GENERIC ENDPOINT --------------------------------  */

bool Usb::Endpoint::IsBulk(libusb_endpoint_descriptor ep) // static
{
	return (TransferType(ep) == LIBUSB_TRANSFER_TYPE_BULK);
}

bool Usb::Endpoint::IsIsochronous(libusb_endpoint_descriptor ep) // static
{
	return (TransferType(ep) == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS);
}

bool Usb::Endpoint::IsSynchronous(libusb_endpoint_descriptor ep) // static
{
	if (!IsIsochronous(ep)) return false;
	return (IsoSyncType(ep) == LIBUSB_ISO_SYNC_TYPE_SYNC);
}

bool Usb::Endpoint::IsAdaptive(libusb_endpoint_descriptor ep) // static
{
	if (!IsIsochronous(ep)) return false;
	return (IsoSyncType(ep) ==  LIBUSB_ISO_SYNC_TYPE_ADAPTIVE);
}

bool Usb::Endpoint::IsAsynchronous(libusb_endpoint_descriptor ep) // static
{
	if (!IsIsochronous(ep)) return false;
	return (TransferType(ep) ==  LIBUSB_ISO_SYNC_TYPE_ASYNC);
}

bool Usb::Endpoint::IsBulk() { return IsBulk(desc); }
bool Usb::Endpoint::IsIsochronous() { return IsIsochronous(desc); }
bool Usb::Endpoint::IsSynchronous() { return IsSynchronous(desc); }
bool Usb::Endpoint::IsAdaptive() { return IsAdaptive(desc); }
bool Usb::Endpoint::IsAsynchronous() { return IsAsynchronous(desc); }


/* BM ATTRIBUTES FIELD BITS:
For isochronous endpoints, they are defined as:
Bits 3..2: Synchronization Type
      00 = No Synchronization
      01 = Asynchronous
      10 = Adaptive
      11 = Synchronous
Bits 5..4: Usage Type
      00 = Data
      01 = Feedback
      10 = Implicit feedback
      11 = Reserved
 */

bool Usb::Endpoint::IsSynchEndpoint(libusb_endpoint_descriptor ep)
{
	return (((ep.bmAttributes >> 4) & 0b00000011) == 01);
}

bool Usb::Endpoint::HasImplicitFeedback(libusb_endpoint_descriptor ep)
{
	return (((ep.bmAttributes >> 4) & 0b00000011) == 10);
}

bool Usb::Endpoint::IsDataEndpoint(libusb_endpoint_descriptor ep)
{
	return (((ep.bmAttributes >> 4) & 0b00000011) == 00);
}

bool Usb::Endpoint::IsSynchEndpoint() { return IsSynchEndpoint(desc);}
bool Usb::Endpoint::HasImplicitFeedback() { return IsSynchEndpoint(desc);}
bool Usb::Endpoint::IsDataEndpoint() { return IsSynchEndpoint(desc);}


bool Usb::Endpoint::IsOutput(libusb_endpoint_descriptor ep)
{
	return (Direction(ep) == LIBUSB_ENDPOINT_OUT);
}

bool Usb::Endpoint::IsInput(libusb_endpoint_descriptor ep)
{
	return (Direction(ep)== LIBUSB_ENDPOINT_IN);
}

bool Usb::Endpoint::IsOutput() { return IsOutput(desc); }
bool Usb::Endpoint::IsInput() { return IsInput(desc); }

uint16_t Usb::Endpoint::GetMaxPacketSize()
{
	return desc.wMaxPacketSize;
}

uint8_t Usb::Endpoint::GetAddress()
{
	return desc.bEndpointAddress;
}

uint8_t Usb::Endpoint::GetPollingInterval()
{
	return desc.bInterval;
}

uint8_t Usb::Endpoint::GetRefresh()
{
	return desc.bRefresh;
}

bool Usb::Endpoint::HasSynchEndpoint()
{
	return (desc.bSynchAddress != 0);
}

uint8_t Usb::Endpoint::GetSynchEndpointAddress()
{
	return desc.bSynchAddress;
}

libusb_endpoint_direction Usb::Endpoint::Direction(libusb_endpoint_descriptor ep)
{
	int8_t val = (ep.bEndpointAddress&LIBUSB_ENDPOINT_DIR_MASK);
	return (val==(int8_t)0x80) ? LIBUSB_ENDPOINT_IN : LIBUSB_ENDPOINT_OUT;
}

int8_t Usb::Endpoint::TransferType(libusb_endpoint_descriptor ep)
{
	return ep.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK; // 0000 0011
}

int8_t Usb::Endpoint::IsoSyncType(libusb_endpoint_descriptor ep)
{
	return (ep.bmAttributes & LIBUSB_ISO_SYNC_TYPE_MASK) >> 2; // mask: 0000 1100 => sync type is in bit 2:3
}

int8_t Usb::Endpoint::IsoUsageType(libusb_endpoint_descriptor ep)
{
	return (ep.bmAttributes & LIBUSB_ISO_USAGE_TYPE_MASK) >> 4; // mask: 0011 0000 => usage type is in bit 4:5
}

char* Usb::Endpoint::DirectionStr(libusb_endpoint_direction dir)
{
	if 		(dir == LIBUSB_ENDPOINT_IN)  return "Input";
	else if (dir == LIBUSB_ENDPOINT_OUT) return "Output";
	else return "Error";
}

char* Usb::Endpoint::TransferTypeStr(uint8_t type)
{
	switch(type)
	{
		case LIBUSB_TRANSFER_TYPE_CONTROL: 		return "Control";
		case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:	return "Isochronous";
		case LIBUSB_TRANSFER_TYPE_BULK:			return "Bulk";
		case LIBUSB_TRANSFER_TYPE_INTERRUPT:	return "Interrupt";
	}
	return "Error";
}

char* Usb::Endpoint::IsoSyncTypeStr(uint8_t synchtype)
{
	switch(synchtype) {
		case LIBUSB_ISO_SYNC_TYPE_NONE: 		return "None";
		case LIBUSB_ISO_SYNC_TYPE_ASYNC:		return "Async";
		case LIBUSB_ISO_SYNC_TYPE_ADAPTIVE:		return "Adaptive";
		case LIBUSB_ISO_SYNC_TYPE_SYNC:			return "Sync";
	}
	return "Error";
}

char* Usb::Endpoint::IsoUsageTypeStr(uint8_t usagetype)
{
	switch(usagetype) {
		case LIBUSB_ISO_USAGE_TYPE_DATA: 		return "Data";
		case LIBUSB_ISO_USAGE_TYPE_FEEDBACK:	return "Feedback";
		case LIBUSB_ISO_USAGE_TYPE_IMPLICIT:	return "Implicit";
	}
	return "Error";
}


std::string Usb::Endpoint::DumpInfo()
{
	std::stringstream os;
	os << "  ENDPOINT:\n"
		<< "    Address: 0x" << std::hex << (unsigned)GetAddress() << std::dec << "\n"
		<< "    Direction: " << DirectionStr(Direction(desc)) << "\n"
		<< "    Transfer Type: " << TransferTypeStr(TransferType(desc)) << "\n"
		<< "    Isoc. Synch type: " << IsoSyncTypeStr(IsoSyncType(desc)) << "\n"
		<< "    Isoc. Usage type: " << IsoUsageTypeStr(IsoUsageType(desc)) << "\n"
		<< "    MAX packet size: " << (unsigned)GetMaxPacketSize() << "\n"
		<< "    Polling Interval: " << (unsigned)GetPollingInterval() << "\n";

	if (IsSynchEndpoint()) {
		os	<< "    It's a Synch Endpoint.\n     Refresh: 2^" << (unsigned)GetRefresh() << "\n";
	} else {
		os << "    Refresh: " << (unsigned)GetRefresh()<< "\n"
			<< "    Synch endpoint address: " << (unsigned)GetSynchEndpointAddress() << "\n";
	}
	LOGD("%",os.str().c_str());
	return os.str();
}

