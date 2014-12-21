#ifndef ANDROID_USB_HELPER_H_
#define ANDROID_USB_HELPER_H_

#include "libusb.h"
#include "UsbSpecCodes.h"
#include <string>

#define USB_REQUEST_TIMEOUT 2500

char* TransferStatusStr(libusb_transfer_status st);

typedef void (*AudioDriverCallback)(void*);  // argument: user-defined context

namespace Usb {

	enum Version { Unknown=0, v10=10, v11=11, v20=20, v30=30 };
	enum Speed { UnknownSpeed=0, LowSpeed=1, FullSpeed=2, HighSpeed=3,SuperSpeed=4};
	int InitUsb();
	void FinishUsb(libusb_device_handle* devh);
	int StartUsbEventHandling();
	void StopUsbEventHandling();
	void* HandleUsbEvents(void*);

	libusb_device_handle* OpenDevice(int vid, int pid, int fd);
	std::string GetDeviceName(libusb_device_handle* devh);
	libusb_device_descriptor GetDeviceDescriptor(libusb_device_handle* devh);
	Version GetVersion(libusb_device_handle* devh);
	Speed GetSpeed(libusb_device_handle* devh);

	bool SetRequestForInterface(libusb_device_handle* devh,uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size);
	bool GetRequestForInterface(libusb_device_handle* devh,uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size);

	bool SetRequestForEndpoint(libusb_device_handle* devh,uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size);
	bool GetRequestForEndpoint(libusb_device_handle* devh,uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size);

	std::string GetString(libusb_device_handle* devh, uint8_t stringId);
	void PrintError(char* message, int errorCode);
	std::string GetErrorString(int errorCode);
	std::string TransferErrorToString(int errorCode);
	std::string LibusbErrorToString(int errorCode);

	std::string DescriptorTypeToString(uint8_t type);
	std::string TerminalTypeToString(uint16_t type);
	std::string AudioControlSubtypeToString(uint8_t subtype, uint8_t protocol);
	std::string AudioStreamingSubtypeToString(uint8_t subtype);
	std::string AudioFormatTypeToString(uint8_t type);
	std::string AudioFormatTagToString(uint16_t tag);

}

/*
class UsbHelper
{

public:

	static void PrintDeviceName(libusb_device_handle* devh);
	static int ObtainInterface(libusb_device_handle* devh,int itfNum, int altSetNum=-1);

	static bool usbEventHandlingActive;
	static int StartUsbEventHandling();
	static void StopUsbEventHandling();
	static void* HandleUsbEvents(void*);

	static void IsochronousOutputCallback(struct libusb_transfer *xfr);
	static void (*externalOutputCallback)(uint8_t*,int,void*);
	static int packets;
	static int iso_pack_size;
	static libusb_transfer** isoOutTransfers;
	static int lastOutIsoTransfer;
	static int numBuf;
	static int SetupOutput(libusb_device_handle* devh, uint8_t ep,int bufLen,int numBuf, void(*callback)(uint8_t*,int,void*));
	static int SetupOutputWithOptimalBuffer(libusb_device_handle* devh, uint8_t ep,int suggBufLen,int numBuf, void(*callback)(uint8_t*,int,void*));

	static int CloseOutput();
	static int SubmitNewIsoOutputTransfer(libusb_device_handle* devh, uint8_t ep,uint8_t* buf,int bufLen,void* userData);

	static void IsochronousInputCallback(struct libusb_transfer *xfr);
	static int SetupInput(libusb_device_handle* devh, uint8_t ep,int bufLen,int numBuf, void(*callback)(uint8_t*,int,void*));
	static int SetupInputWithOptimalBuffer(libusb_device_handle* devh, uint8_t ep,int suggBufLen,int numBuf, void(*callback)(uint8_t*,int,void*));
	static int CloseInput();
	static int SubmitNewIsoInputTransfer(libusb_device_handle* devh, uint8_t ep,uint8_t* buf,int bufLen,void* userData);

	static int UnMuteMaster(libusb_device_handle* devh, int itfNum, int featureUnitId);

};
*/

#endif /* USBHELPER_H_ */
