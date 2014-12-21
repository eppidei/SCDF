#include <pthread.h>
#include "UsbHelper.h"
#include <unistd.h>
#include <errno.h>
#include "Logging.h"
#include <sstream>


/*
 * 	LIBRARY INITIALIZATION AND EVENT HANDLING
 */


int Usb::InitUsb()
{
	int rc;
	rc = libusb_init(NULL);
	if (rc < 0) {
		LOGE("Error initializing Libusb: %s\n", libusb_error_name(rc));
	}
	return rc;
}

void Usb::FinishUsb(libusb_device_handle* devh)
{
	libusb_close(devh);
	libusb_exit(NULL);
}

static bool usbEventHandlingActive = false;

int Usb::StartUsbEventHandling()
{
	if (usbEventHandlingActive)
		return 0;
	LOGI("USBAUDIO - Start USB event handling");
	pthread_t usbEventsThread;
	usbEventHandlingActive=true;
    return pthread_create(&usbEventsThread,NULL,Usb::HandleUsbEvents,NULL);
}

void Usb::StopUsbEventHandling()
{
	usbEventHandlingActive=false;
}

void* Usb::HandleUsbEvents(void*) // function called by the usb events thread
{
	//LOGD("Usb event");
	int rc;
	while (usbEventHandlingActive) {
		//LOGI(">>>> USBAUDIO Calling libusb_handle_events. <<<<<<<");
		rc = libusb_handle_events(NULL);
		//LOGD("  >>>> CALLED libusb_handle_events. result=%d <<<<<<",rc);
		if (rc != LIBUSB_SUCCESS) {
			LOGE("USBAUDIO Error handling usb event");
			break;
		}
	}
	//LOGD(">>>>>>>>>>> Handle usb events returned!");
	return NULL;
}



/*
 * 	USB DEVICE ENUMERATION, PROPERTIES AND CREATION
 */


libusb_device_handle* Usb::OpenDevice(int vid, int pid, int fd)
{
	libusb_device_handle* devh = libusb_open_device_with_vid_pid_and_fd(NULL, vid, pid, fd);
	if (!devh) {
		LOGE("USBAUDIO - Error opening USB device.");
	}
	return devh;
}


libusb_device_descriptor Usb::GetDeviceDescriptor(libusb_device_handle* devh)
{
	libusb_device* dev = libusb_get_device(devh);
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r<0)
		LOGE("USBAUDIO - error retrieving device descriptor!");
	return desc;
}


std::string Usb::GetDeviceName(libusb_device_handle* devh)
{
	// device vendor and product info:
	libusb_device* dev = libusb_get_device(devh);
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	unsigned char prod[20];
	int ret2 = libusb_get_string_descriptor_ascii(devh,desc.iProduct,prod,20);
	//LOGD("USBAUDIO: asked device name, it's: %s",prod);
	return std::string((char*)prod);
}

Usb::Version Usb::GetVersion(libusb_device_handle* devh)
{
	libusb_device* dev = libusb_get_device(devh);
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r<0)
		return Usb::Unknown;
	switch (desc.bcdUSB) {
		case 0x0100: return Usb::v10;
		case 0x0110: return Usb::v11;
		case 0x0200: return Usb::v20;
		case 0x0300: return Usb::v30;
		default: return Usb::Unknown;
	}
}

Usb::Speed Usb::GetSpeed(libusb_device_handle* devh)
{
	libusb_device* dev = libusb_get_device(devh);
	int speed = libusb_get_device_speed(dev);
	switch (speed) {
		case LIBUSB_SPEED_UNKNOWN:	return UnknownSpeed;
		case LIBUSB_SPEED_LOW: 		return LowSpeed;
		case LIBUSB_SPEED_FULL: 	return FullSpeed;
		case LIBUSB_SPEED_HIGH:	 	return HighSpeed;
		case LIBUSB_SPEED_SUPER: 	return SuperSpeed;
		default: 	return UnknownSpeed;
	}
}





/*
 * 	USB REQUESTS
 */


bool Usb::SetRequestForInterface(libusb_device_handle* devh,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size)
{
	uint8_t bmRequestType = 0b00100001; // set request => msb==0
	//LOGD("   SET request for interface. bRequest:%x, wValue:%x, wIndex:%x, data:%x, size:%d",bRequest,wValue,wIndex,data,size);
	int ret = libusb_control_transfer(devh,bmRequestType,bRequest,wValue,wIndex,data,size,USB_REQUEST_TIMEOUT);
	if (ret<0) {
		PrintError("Set Request For Interface",ret);
		return false;
	}
	return true;
}

bool Usb::GetRequestForInterface(libusb_device_handle* devh,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size)
{
	uint8_t bmRequestType = 0b10100001;	// NB: get request => msb==1
	//LOGD("   GET request for interface. bRequest:%x, wValue:%x, wIndex:%x, data:%x, size:%d",bRequest,wValue,wIndex,data,size);
	int ret = libusb_control_transfer(devh,bmRequestType,bRequest,wValue,wIndex,data,size,USB_REQUEST_TIMEOUT);
	if (ret<0) {
		PrintError("Get Request For Interface",ret);
		//libusb_clear_halt(devh,0);
		return false;
	}
	return true;
}

bool Usb::SetRequestForEndpoint(libusb_device_handle* devh,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size)
{
	uint8_t bmRequestType = 0b00100010; // set request for endpoint

	int ret = libusb_control_transfer(devh,bmRequestType,bRequest,wValue,wIndex,data,size,USB_REQUEST_TIMEOUT);
	if (ret<0) {
		PrintError("Set Request For Enpoint",ret);
		return false;
	}
	return true;
}

bool Usb::GetRequestForEndpoint(libusb_device_handle* devh,
		uint8_t bRequest, uint16_t wValue, uint16_t wIndex,uint8_t* data,uint16_t size)
{
	uint8_t bmRequestType = 0b10100010;	// NB: get request => msb==1
	//LOGD("   get request for endpoint. bRequest:%x, wValue:%x, wIndex:%x, data:%x, size:%d",bRequest,wValue,wIndex,data,size);
	int ret = libusb_control_transfer(devh,bmRequestType,bRequest,wValue,wIndex,data,size,USB_REQUEST_TIMEOUT);
	if (ret<0) {
		PrintError("Get Request For Endpoint",ret);
		//libusb_clear_halt(devh,0);
		return false;
	}
	return true;
}



/*
 * 	PRINTING MESSAGES AND CONSTANTS
 */

std::string Usb::GetString(libusb_device_handle* devh, uint8_t stringIndex)
{
	if (stringIndex==0)
		return std::string("[...]");
	uint8_t data[42];
	if ( 0 > libusb_get_string_descriptor_ascii(devh,stringIndex,data,42))
		return std::string("(...)");
	return std::string((char*)data);
}


void Usb::PrintError(char* message, int errorCode)
{
	LOGE("Native USB - Error: %s ( %s ) ERRNO:%s", message, libusb_error_name(errorCode), strerror(errno));
}

std::string Usb::GetErrorString(int errorCode)
{
	std::stringstream os;
	if (errorCode<=0)
		os << LibusbErrorToString(errorCode);
	else
		os << TransferErrorToString(errorCode);
	os << " [" << errorCode << "] (" << strerror(errno) << ")";
	return os.str();
}

std::string Usb::LibusbErrorToString(int errorCode)
{
	switch(errorCode) {
		case LIBUSB_ERROR_IO: return "I/O Error";
		case LIBUSB_ERROR_INVALID_PARAM: return "Invalid Parameter";
		case LIBUSB_ERROR_ACCESS: return "Access Denied";
		case LIBUSB_ERROR_NO_DEVICE: return "No such device";
		case LIBUSB_ERROR_NOT_FOUND: return "Entity not found";
		case LIBUSB_ERROR_BUSY: return "Resource busy";
		case LIBUSB_ERROR_TIMEOUT: return "Operation timed out";
		case LIBUSB_ERROR_OVERFLOW: return "Overflow";
		case LIBUSB_ERROR_PIPE: return "Pipe error";
		case LIBUSB_ERROR_INTERRUPTED: return "System call interrupted";
		case LIBUSB_ERROR_NO_MEM: return "Insufficient memory";
		case LIBUSB_ERROR_NOT_SUPPORTED: return "Operation not supported";
		case LIBUSB_ERROR_OTHER: return "Other error";
		default: return "Unknown USB error";
	}
}

std::string Usb::TransferErrorToString(int errorCode)
{
	switch(errorCode) {
		case LIBUSB_TRANSFER_COMPLETED: return "USB Transfer Completed (not an error)";
		case LIBUSB_TRANSFER_CANCELLED:	return "USB Transfer Cancelled";
		case LIBUSB_TRANSFER_ERROR: return "USB Transfer Error";
		case LIBUSB_TRANSFER_TIMED_OUT: return "USB Transfer Timed Out";
		case LIBUSB_TRANSFER_STALL: return "USB Transfer Stall";
		case LIBUSB_TRANSFER_NO_DEVICE: return "USB Device not found";
		case LIBUSB_TRANSFER_OVERFLOW: return "USB Transfer overflow";
		default: return "Unknown USB transfer error";
	}
}


std::string Usb::DescriptorTypeToString(uint8_t type)
{
	switch (type) {
		case DESC_TYPE_UNDEFINED: 		return "Undefined";
		case DESC_TYPE_DEVICE:	  		return "Device";
		case DESC_TYPE_CONFIGURATION: 	return "Configuration";
		case DESC_TYPE_STRING:			return "String";
		case DESC_TYPE_INTERFACE: 		return "Interface";
		case DESC_TYPE_ENDPOINT:		return "Endpoint";
		default:						return "Invalid type";
	}
}


std::string Usb::AudioControlSubtypeToString(uint8_t subtype, uint8_t protocol)
{
	switch (subtype) {
		case UAC_DESC_UNDEFINED:		return "Undefined";
		case UAC_DESC_HEADER:  			return "Header";
		case UAC_DESC_INPUT_TERMINAL:	return "Input Terminal";
		case UAC_DESC_OUTPUT_TERMINAL:	return "Output Terminal";
		case UAC_DESC_FEATURE_UNIT:		return "Feature Unit";
		case UAC_DESC_MIXER_UNIT:		return "Mixer Unit";
		case UAC_DESC_SELECTOR_UNIT:	return "Selector Unit";

		default:

		if (protocol==USB_AUDIO_CLASS_2) {
			switch (subtype) {
				case UAC2_DESC_CLOCK_SOURCE:			return "Clock Source";
				case UAC2_DESC_CLOCK_SELECTOR:			return "Clock Selector";
				case UAC2_DESC_CLOCK_MULTIPLIER:		return "Clock Multiplier";
				case UAC2_DESC_SAMPLE_RATE_CONVERTER:	return "Sample Rate Converter";
				case UAC2_DESC_EFFECT_UNIT:				return "Effect Unit";
				case UAC2_DESC_PROCESSING_UNIT: 		return "Processing Unit";
				case UAC2_DESC_EXTENSION_UNIT:			return "Extension Unit";
			}
		} else { // UAC 1
			switch (subtype) {
				case UAC1_DESC_PROCESSING_UNIT:	return "Processing Unit";
				case UAC1_DESC_EXTENSION_UNIT:	return "Extension Unit";
			}
		}
		return "Invalid Subtype";
	}
}


std::string Usb::AudioStreamingSubtypeToString(uint8_t subtype)
{
	switch (subtype) {
		case AS_DESC_UNDEFINED:			return "Undefined";
		case AS_DESC_GENERAL:  			return "General Info";
		case AS_DESC_FORMAT_TYPE:		return "Format Type";
		case AS_DESC_FORMAT_SPECIFIC:	return "Specific/Encoder";
		case AS_DESC_DECODER:			return "Decoder";
		default:						return "Invalid Subtype";
	}
}

std::string Usb::AudioFormatTypeToString(uint8_t type)
{
	switch (type) {
		case FORMAT_TYPE_UNDEFINED:	return "Undefined";
		case FORMAT_TYPE_I:  		return "Type I";
		case FORMAT_TYPE_II:		return "Type II";
		case FORMAT_TYPE_III:		return "Type III";
		default:					return "Invalid Format Type";
	}
}


std::string Usb::AudioFormatTagToString(uint16_t tag)
{
	switch (tag) {
		case FORMAT_TYPE_I_UNDEFINED:	return "Undefined";
		case FORMAT_TYPE_I_PCM:  		return "PCM";
		case FORMAT_TYPE_I_PCM8:		return "PCM 8 bit";
		case FORMAT_TYPE_I_IEEE_FLOAT:	return "IEEE Float";
		case FORMAT_TYPE_I_ALAW:		return "Type aLAW";
		case FORMAT_TYPE_I_MULAW:		return "Type uLAW";
		default:					return "Unsupported/Invalid Format Type";
	}
}

std::string Usb::TerminalTypeToString(uint16_t type)
{
	switch(type) {
		case TT_USB_UNDEFINED:		return "USB (undefined)";
		case TT_USB_STREAMING: 		return "USB Streaming";
		case TT_USB_VENDOR_SPEC: 	return "USB (Vendor specific)";
		case TT_INPUT_UNDEFINED: 	return "Input (undefined)";
		case TT_MICROPHONE: 		return "Microphone";
		case TT_DESKTOP_MIC: 		return "Desktop Microphone";
		case TT_PERSONAL_MIC: 		return "Personal Microphone";
		case TT_OMNI_DIR_MIC: 		return "Omni-directional Microphone";
		case TT_MIC_ARRAY: 			return "Microphone Array";
		case TT_PROC_MIC_ARRAY: 	return "Processing Microphone Array";
		case TT_OUTPUT_UNDEFINED: 	return "Output (undefined)";
		case TT_SPEAKER: 			return "Speaker";
		case TT_HEADPHONES: 		return "Headphones";
		case TT_HEAD_DISPLAY: 		return "Head-mounted Display Audio";
		case TT_DESKTOP_SPEAKER: 	return "Desktop Speaker";
		case TT_ROOM_SPEAKER: 		return "Room Speaker";
		case TT_COMM_SPEAKER: 		return "Communication Speaker";
		case TT_LFE_SPEAKER: 		return "Low-frequency Effects Speaker";
		case TT_BIDIR_UNDEFINED: 	return "Bidirectional (undefined)";
		case TT_HANDSET:		 	return "Handset";
		case TT_HEADSET: 			return "Headset";
		case TT_SPEAKERPHONE: 		return "Speakerphone";
		case TT_ECHO_SUPPRESSING: 	return "Echo Suppressing Speakerphone";
		case TT_ECHO_CANCELING: 	return "Echo Canceling SpeakerPhone";
		case TT_MULTITRACK_REC: 	return "Multitrack Recorder";
		case TT_SYNTHESIZER:		return "Synthesizer";
	default:
		if (TT_TELEPHONY_UNDEF==type&TT_TELEPHONY_UNDEF) return "Telephony Category";
		else if (TT_EXTERNAL_UNDEF==type&TT_EXTERNAL_UNDEF) return "External Category";
		else if (TT_EMBEDDED_UNDEF==type&TT_EMBEDDED_UNDEF) return "Embedded Category";
		else return "Unknown terminal type";
	}
}

// utils:

char* TransferStatusStr(libusb_transfer_status st)
{
	switch(st) {
	case LIBUSB_TRANSFER_COMPLETED:	return "Transfer Completed";
	case LIBUSB_TRANSFER_ERROR:		return "Transfer Error";
	case LIBUSB_TRANSFER_TIMED_OUT: return "Transfer Timed Out";
	case LIBUSB_TRANSFER_CANCELLED: return "Transfer Cancelled";
	case LIBUSB_TRANSFER_STALL: 	return "Transfer Stall";
	case LIBUSB_TRANSFER_NO_DEVICE: return "Transfer No device";
	case LIBUSB_TRANSFER_OVERFLOW: 	return "Transfer Overflow";
	}
	return "status unknown";
}


/*




void Usb::PrintDeviceName(libusb_device_handle* devh)
{
	// device vendor and product info:
	libusb_device* dev = libusb_get_device(devh);
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	unsigned char ven[20];
	unsigned char prod[20];
	int ret = libusb_get_string_descriptor_ascii(devh,desc.iManufacturer,ven,20);
	int ret2 = libusb_get_string_descriptor_ascii(devh,desc.iProduct,prod,20);
	LOGI("DEVICE INFO: manuf: %s, prod: %s",ven,prod);
	LOGI("  Id:%04x:%04x (bus: %d, device address: %d)",
			desc.idVendor, desc.idProduct,	libusb_get_bus_number(dev), libusb_get_device_address(dev));
}

int Usb::ObtainInterface(libusb_device_handle* devh,int itfNum, int altSetNum)
{
	int rc;
	if (1==libusb_kernel_driver_active(devh,itfNum)) {
		LOGD("USBAUDIO There's a Kernel driver active on interface %d... detaching",itfNum);
		rc = libusb_detach_kernel_driver(devh,itfNum);
		if(0!=rc) {
			LOGE("USBAUDIO Error detaching kernel driver");
			return rc;
			// TODO: driver detaching error are of the same tipe than the ones for claim_interface and alt_settings?
		}
	}

	LOGD("USBAUDIO Claim interface %d",itfNum);
	rc = libusb_claim_interface(devh, itfNum);
	if (rc < 0) {
		LOGE("USBAUDIO Error claiming interface %d: %s\n", itfNum,libusb_error_name(rc));
		return rc;
	}

	if (-1==altSetNum) return rc;

	LOGD("    USBAUDIO Set alternate setting %d",altSetNum);

	rc = libusb_set_interface_alt_setting(devh,itfNum,altSetNum);
	if (rc < 0) {
		LOGE("USBAUDIO Error setting alternate setting %d on interface %d: %s\n",altSetNum,itfNum,libusb_error_name(rc));
		return rc;
	}
	return rc;
}





int UsbHelper::packets=0;
int UsbHelper::iso_pack_size=0;
libusb_transfer** UsbHelper::isoOutTransfers=NULL;
int UsbHelper::lastOutIsoTransfer=-1;
int UsbHelper::numBuf=0;
void (*UsbHelper::externalOutputCallback)(uint8_t*,int,void*) = NULL;

int UsbHelper::SetupOutput(libusb_device_handle* devh, uint8_t ep,int bufLen,int _numBuf, void(*callback)(uint8_t*,int,void*))
{
	externalOutputCallback=callback;
	numBuf=_numBuf;
	isoOutTransfers=new libusb_transfer*[numBuf];
	lastOutIsoTransfer=0;

	int MAX_iso_pack_size = libusb_get_max_iso_packet_size(libusb_get_device(devh),ep);
	//  given the buffer length,calculate how many packets you need to enqueue,and how long must they be:

	int test=0;
	int i=0;
	while(test!=bufLen || iso_pack_size>MAX_iso_pack_size) {
		packets = bufLen / ( MAX_iso_pack_size - i );
		iso_pack_size = bufLen / packets;
		test = iso_pack_size * packets;
		//LOGD("USBAUDIO bufLen:%d, max iso packet size: %d, packets:%d, packet size:%d, test:%d",
		//		bufLen,MAX_iso_pack_size,packets,iso_pack_size,test);
		i++;
	}
	return 0;
}

int UsbHelper::SetupOutputWithOptimalBuffer(libusb_device_handle* devh, uint8_t ep,int suggBufLen,int _numBuf, void(*callback)(uint8_t*,int,void*))
{
	externalOutputCallback=callback;
	numBuf=_numBuf;
	isoOutTransfers=new libusb_transfer*[numBuf];
	lastOutIsoTransfer=0;

	packets = 20; //suggBufLen/MAX_iso_pack_size;
	iso_pack_size = 192;
	int actualBufLen = packets*iso_pack_size;
	//LOGD("USBAUDIO. Output max iso pack size: %d packets: %d Actual buffer length: %d",MAX_iso_pack_size,packets,actualBufLen);
	return actualBufLen;
}

int UsbHelper::CloseOutput()
{
	LOGE("UsbHelper::CloseOutput() not implemented");
	return 0;
	// dispose the transfers if they are still going on, and then delete the array of transfers
}

int UsbHelper::SubmitNewIsoOutputTransfer(libusb_device_handle* devh, uint8_t ep,uint8_t* buf,int bufLen, void* userData)
{
	libusb_transfer *xfr = isoOutTransfers[lastOutIsoTransfer];
	lastOutIsoTransfer = (lastOutIsoTransfer+1)%numBuf;

	xfr = libusb_alloc_transfer(packets);
	if (!xfr) {
		LOGE("USBAUDIO Error allocating OUT isochronous transfer");
		return -1;
	}

	libusb_fill_iso_transfer(xfr, devh, ep, buf, bufLen, packets, UsbHelper::IsochronousOutputCallback,userData,5000);
	libusb_set_iso_packet_lengths(xfr, iso_pack_size);
	libusb_submit_transfer(xfr);
	return 0;
}

// TODO: stop transfers on error!

void UsbHelper::IsochronousOutputCallback(struct libusb_transfer *xfr)
{
	unsigned int i;
	//static int cnt=0; if(cnt==0) LOGD("Callback"); cnt=(cnt+1)%50;

	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		LOGE("USBAUDIO. Transfer incomplete. Transfer status %s", TransferStatusStr(xfr->status));
		libusb_free_transfer(xfr);
		return;
	}

	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {

		for (i = 0; i < xfr->num_iso_packets; i++) {
			libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			//LOGD("   > USBAUDIO OUT iso packet len:%d actual len:%d",pack->length, pack->actual_length);
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				LOGE("USBAUDIO. Pack transfer incomplete: pack %u status %d\n", i, pack->status);
				return;
			}
			//printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
		}
	}

	if (NULL!=externalOutputCallback)
	{
		externalOutputCallback(xfr->buffer,xfr->length,xfr->user_data);
	}

	//for (i = 0; i < xfr->actual_length; i++) { printf("%02x", xfr->buffer[i]); }
	//GimmeSomeNoise(xfr->buffer,1000);

	if (libusb_submit_transfer(xfr) < 0) {
		LOGE("USBAUDIO error re-submitting URB\n");
	}
}


static int packets_in=0;
static int iso_pack_size_in=0;
static libusb_transfer** isoInTransfers=NULL;
static int lastInIsoTransfer=-1;
static int numBuf_in=0;
void (*externalInputCallback)(uint8_t*,int,void*) = NULL;

int UsbHelper::SetupInput(libusb_device_handle* devh, uint8_t ep,int bufLen,int _numBuf, void(*callback)(uint8_t*,int,void*))
{
	/ * externalInputCallback=callback;
	numBuf_in=_numBuf;
	isoInTransfers=new libusb_transfer*[numBuf_in];
	lastOutIsoTransfer=0;

	int MAX_iso_pack_size = libusb_get_max_iso_packet_size(libusb_get_device(devh),ep);
	//  given the buffer length,calculate how many packets you need to enqueue,and how long must they be:

	int test=0;
	int i=0;
	while(test!=bufLen || iso_pack_size>MAX_iso_pack_size) {
		packets = bufLen / ( MAX_iso_pack_size - i );
		iso_pack_size = bufLen / packets;
		test = iso_pack_size * packets;
		LOGD("bufLen:%d, max iso packet size: %d, packets:%d, packet size:%d, test:%d",
				bufLen,MAX_iso_pack_size,packets,iso_pack_size,test);
		i++;
	} * /
	return 0;
}

unsigned char** test_buf;

int UsbHelper::SetupInputWithOptimalBuffer(libusb_device_handle* devh, uint8_t ep,int suggBufLen,int _numBuf, void(*callback)(uint8_t*,int,void*))
{
	externalInputCallback=callback;
	numBuf_in=_numBuf;
	isoInTransfers=new libusb_transfer*[numBuf_in];
	lastInIsoTransfer=0;

	iso_pack_size_in = 100;//libusb_get_max_iso_packet_size(libusb_get_device(devh),ep);
	packets_in = 20;
	int actualBufLen = packets_in*iso_pack_size_in;
	//LOGD("USBAUDIO INPUT. max iso pack size: %d packets: %d Actual buffer length: %d",iso_pack_size_in,packets_in,actualBufLen);

	test_buf = new unsigned char*[numBuf_in];
	for (int j=0; j<numBuf_in; j++) test_buf[j] = new unsigned char[actualBufLen];

	return actualBufLen;

}

int UsbHelper::SubmitNewIsoInputTransfer(libusb_device_handle* devh, uint8_t ep,uint8_t* buf,int bufLen,void* userData)
{
	LOGI("USBAUDIO Submit new iso input transfer");
	libusb_transfer *xfr = isoInTransfers[lastInIsoTransfer];
	unsigned char* the_buffer = test_buf[lastInIsoTransfer];
	lastInIsoTransfer = (lastInIsoTransfer+1) % numBuf_in;

	xfr = libusb_alloc_transfer(packets_in);
	if (!xfr) {
		LOGE("USBAUDIO Error allocating isochronous INPUT transfer");
		return -1;
	}

	libusb_fill_iso_transfer(xfr, devh, ep, the_buffer, packets_in*iso_pack_size_in, packets_in, IsochronousInputCallback,userData,5000);
	libusb_set_iso_packet_lengths(xfr, iso_pack_size_in);

	int result=libusb_submit_transfer(xfr);
	if(0>result) {
		LOGE("USBAUDIO Error %d submitting iso transfer for input",result);
	}
	return 0;
}


int UsbHelper::CloseInput()
{
	LOGE("UsbHelper::CloseInput() not implemented");
	// dispose the transfers if they are still going on, and then delete the array of transfers
	return 0;
}


// TODO: stop transfers on error!

void UsbHelper::IsochronousInputCallback(struct libusb_transfer *xfr)
{
	//unsigned int i;
	//static int cnt=0; if(cnt==0) LOGD("In Callback"); cnt=(cnt+1)%50;
	//LOGD("> USBAUDIO. ISO IN callback.");

	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		LOGE("USBAUDIO Transfer incomplete. Transfer status %d", xfr->status);
		libusb_free_transfer(xfr);
		return;
	}

	int test_len=0;

	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		for (int i = 0; i < xfr->num_iso_packets; i++) {
			libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			//LOGD("   > USBAUDIO iso packet len:%d actual len:%d",pack->length, pack->actual_length);
			test_len+=pack->actual_length;
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				LOGE("USBAUDIO Pack transfer incomplete: pack %u status %d\n", i, pack->status);
				return;
			}
		}
	}
}
*/

