/*
 * UsbAudioEndpoint.h
 *
 *  Created on: 27/gen/2014
 *      Author: athos
 */

#ifndef USBAUDIOENDPOINT_H_
#define USBAUDIOENDPOINT_H_

#include "UsbHelper.h"

#define LOCK_DELAY_UNDEF 		0
#define LOCK_DELAY_IN_MILLISECS	1
#define LOCK_DELAY_IN_SAMPLES	2

namespace Usb {


class Endpoint
{

protected:
	libusb_device_handle* devh;
	libusb_endpoint_descriptor desc;
	Endpoint() {};

public:

	Endpoint(libusb_endpoint_descriptor dsc, libusb_device_handle* dh) : desc(dsc), devh(dh) {}
	virtual std::string DumpInfo();
	virtual ~Endpoint() {}

	bool IsBulk();
	bool IsIsochronous();

	bool IsSynchronous();
	bool IsAsynchronous();
	bool IsAdaptive();

	bool IsOutput();
	bool IsInput();

	bool IsSynchEndpoint();
	bool IsDataEndpoint();
	bool HasImplicitFeedback();

	//bool IsUsageImplicit();
	//bool IsUsageData();
	//bool IsUsageFeedback();

	uint8_t  GetAddress();
	uint16_t GetMaxPacketSize();
	uint8_t  GetPollingInterval(); 	// its meaning varies for full or highspeed devices (see AudioStreamingInterface::GetAllowedFramesPerBuffer)
	uint8_t  GetRefresh(); 			// Must be a power of 2, so only the power is reported (from 1=>2ms to 9=>512ms)
	bool HasSynchEndpoint();
	uint8_t GetSynchEndpointAddress();

	static bool IsOutput(libusb_endpoint_descriptor ep);
	static bool IsInput(libusb_endpoint_descriptor ep);

	static bool IsBulk(libusb_endpoint_descriptor ep);
	static bool IsIsochronous(libusb_endpoint_descriptor ep);

	static bool IsSynchEndpoint(libusb_endpoint_descriptor ep);
	static bool IsDataEndpoint(libusb_endpoint_descriptor ep);
	static bool HasImplicitFeedback(libusb_endpoint_descriptor ep);

	static libusb_endpoint_direction Direction(libusb_endpoint_descriptor ep);
	static int8_t TransferType(libusb_endpoint_descriptor ep);
	static int8_t IsoSyncType(libusb_endpoint_descriptor ep);  // makes sense only for isochronous
	static int8_t IsoUsageType(libusb_endpoint_descriptor ep); // makes sense for isochronous, 2.0 only  ( set to 0 otherwise)

	static char* DirectionStr(libusb_endpoint_direction dir);
	static char* TransferTypeStr(uint8_t type);
	static char* IsoSyncTypeStr(uint8_t synctype);
	static char* IsoUsageTypeStr(uint8_t usagetype);

	static bool IsSynchronous(libusb_endpoint_descriptor ep);
	static bool IsAsynchronous(libusb_endpoint_descriptor ep);
	static bool IsAdaptive(libusb_endpoint_descriptor ep);

};


class AudioEndpoint : public Endpoint {

	// bRefresh must be ZERO!
	// bInterval must be 1!

public:

	AudioEndpoint(libusb_endpoint_descriptor dsc, libusb_device_handle* dh);

	bool HasSampleRateControl();
	bool SetSampleRate(int sr);
	//static bool SetSampleRate(libusb_device_handle* devhnd, uint8_t epAddress, int sr);
	uint32_t GetCurrentSampleRate();
	bool HasPitchControl();

	bool OnlyAcceptsMaxSizePackets();
	uint8_t GetLockDelayUnits();
	uint16_t GetLockDelay();
	std::string DumpInfo();

private:

	AudioEndpoint() {}
	uint32_t desiredSampleRateForAdaptive;
	bool isValid;
	// class-specific part of the endpoint descriptor:
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bmAttributes;		// supported controls. 0:sr, 1:pitch, 2-6:reserved, 7:max size packets only
	uint8_t  bLockDelayUnits;	// units used for the wLockDelay (1:ms, 2:samples)
	uint16_t wLockDelay;		// time for the (synchronous or adaptive) endpoint to lock to clock
};


class SynchEndpoint : public Endpoint {

	// bRefresh must be NOT ZERO!
	// bInterval must be 1!

};

}

#endif /* USBAUDIOENDPOINT_H_ */
