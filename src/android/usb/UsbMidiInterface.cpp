#include "UsbMidiInterface.h"
#include "Logging.h"
#include "UsbAudioDevice.h"
#include <sstream>
//extern FILETIME midirecsystime_start_device;


Usb::MidiInterface::MidiInterface(AudioDevice* audioDev,libusb_interface itf)
	: AudioInterface(audioDev,itf), outEp(NULL)
{
	//LOGD("USBAUDIO - Audio CONTROL Interface constructor");
	#ifdef ENABLE_MIDI_INPUT
	inEp(NULL), numUnsupportedEndpoints(0),
	stopInputThread(true),destroyed(NULL), inputThreadAlive(false)
	destroyed = new bool;
	*destroyed = false;
	#endif

	if (interface.num_altsetting == 0) {
		LOGE("No alternate setting for this MIDI streaming interface!");
		return;
	}

	libusb_interface_descriptor descriptor = interface.altsetting[0];

	if (descriptor.bNumEndpoints==0) {
		LOGI("This midi streaming interface (%s) has no endpoints",GetIndex());
		return;
	}

	bool foundIn = 0;
	bool foundOut = 0;

	for (int i = 0; i<descriptor.bNumEndpoints; i++)
	{

		libusb_endpoint_descriptor epd = descriptor.endpoint[i];

		if (Endpoint::IsBulk(epd) && Endpoint::IsInput(epd)) {

			#ifdef ENABLE_MIDI_INPUT
			if (!foundIn) {
				inEp = new Endpoint(epd,Devh());
				foundIn=true;
			} else {
				additionalIns.push_back( new Endpoint(epd,Devh()) );
			}
			#endif
		}
		else if (Endpoint::IsBulk(epd) && Endpoint::IsOutput(epd)) {

			if (!foundOut) {
				outEp = new Endpoint(epd,Devh());
				foundOut=true;
			} else {
				additionalOuts.push_back( new Endpoint(epd,Devh()) );
			}

		} else {
			numUnsupportedEndpoints++;
		}
	}

	if (!HasInput() && !HasOutput()) {
		LOGE("Midi streaming interface setup error: no suitable endpoint");
		return;
	}

	if (!DetachAndClaim()){
		LOGE("Could not claim MIDI streaming interface!");
		return;
	}

	SelectAlternateSetting(0);

	//StartInput(); // test;
	//DumpInfo();
}

std::string Usb::MidiInterface::GetOutName()
{
	if (!HasOutput())
		return std::string("(none)");
	std::stringstream os;
	os << aDev->GetProductName();// << " MIDI Out ";// << GetIndex();
	return os.str();
}

std::string Usb::MidiInterface::GetInName()
{
	if (!HasInput())
			return std::string("(none)");
	std::stringstream os;
	os << aDev->GetProductName();// << " MIDI In";// << GetIndex();
	return os.str();
}

bool Usb::MidiInterface::HasInput()
{
	#ifdef ENABLE_MIDI_INPUT
	return NULL!=inEp;
	#else
	return false;
	#endif
}

bool Usb::MidiInterface::HasOutput()
{
	return NULL!=outEp;
}

Usb::MidiInterface::~MidiInterface()
{
	#ifdef ENABLE_MIDI_INPUT

	StopInput();
	int count = 0;
	while (count < 10)
	{
		{
			Sync::CAutoLock alock(&inputTransfersDeletionLock);
			if (inputTransfers.size()==0) break;
		}
		usleep(30000);
		count++;
	}

	delete inEp;

	for (int i=0; i<additionalIns.size();i++)
		delete additionalIns[i];

	*destroyed = true;
	if (!inputThreadAlive)
		delete destroyed;

	#endif

	delete outEp;
	for (int i=0; i<additionalOuts.size();i++)
		delete additionalOuts[i];

}


std::string Usb::MidiInterface::DumpInfo()
{
	std::stringstream os;
	os << "MIDI STREAMING interface (index " << GetIndex() << "):\n";
	if (outEp)
		os << " ** Has midi OUTPUT endpoint:\n" << outEp->DumpInfo();

	if (additionalOuts.size()!=0)
		for (int i=0; i<additionalOuts.size(); i++)
			os << "  Additional midi out:\n" << additionalOuts[i]->DumpInfo();

	#ifdef ENABLE_MIDI_INPUT
		if (inEp)
				os << " ** Has midi INPUT endpoint:\n" << inEp->DumpInfo();

		if (additionalIns.size()!=0)
			for (int i=0; i<additionalIns.size(); i++)
				os << "  Additional midi in:\n" << additionalIns[i]->DumpInfo();

	#endif

	if (numUnsupportedEndpoints!=0)
		os << "[!] There are " << numUnsupportedEndpoints << " unsupported midi endpoints!\n";

	LOGD("%s",os.str().c_str());
	return os.str();
}


// for midi specs, system messages and usb matching, see:
// https://ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html
// http://www.gweep.net/~prefect/eng/reference/protocol/midispec.html
// http://www.usb.org/developers/devclass_docs/midi10.pdf

bool Usb::MidiInterface::SendMIDIMessage(uint32_t message)
{
	//return true;
	if ( !HasOutput() || outEp->GetMaxPacketSize() < 4 )
			return false;

		int packlen = 4;//outEp->GetMaxPacketSize();
		unsigned char* data = new unsigned char[packlen];

		unsigned char hiNibble = (message & 0xff) >> 4; // contains midi message
		unsigned char loNibble = message & 0xf;         // if system message, will contain type

		unsigned char cable = 0;
		unsigned char cin = 0;

		if (hiNibble==0xf) { // a system message!

			switch (loNibble) { // see usb midi specs, pag 16
			case 0x0: // start sysex
				cin = 0x4;
				break;
			case 0x7: // end of sysex... how many bytes?
				cin = 0x5;// 1 byte. 0x6->2byte 0x7->3byte
				break;
			case 0x1: // mtc quarter frame
			case 0x3: // song select
				cin = 0x2; // two bytes system common message
				break;
			case 0x2: // song pos pointer
				cin = 0x3; // three bytes system common message
				break;
			case 0x6: // tune request
			case 0x8: // midi clock
			case 0xFA: // staRT
			case 0xFB: // continue
			case 0xFC: // stop
			case 0xFE: // "Active Sensing"
			case 0xFF: // "Reset"
				cin = 0x4; // single byte system common message
				break;
			case 0xF9: // "Undefined"
			case 0xFD: // "Undefined"
			case 0x4: // undef
			case 0x5: // undef
			default:
				return false; // since they are not supported yet!
			}

		} else {
			cin = hiNibble; // note on, off, control etc have the cin equal to the hiNibble!
		}

		data[0] = cin | (cable << 4);
		data[1] = message;
		data[2] = message >> 8;
		data[3] = message >> 16;

		//LOGI("USB MIDI OUT - message: %x => to usb: %x %x %x %x", message,data[0],data[1],data[2],data[3]);

		libusb_transfer* tran = libusb_alloc_transfer(0);
		libusb_fill_bulk_transfer(tran,Devh(),outEp->GetAddress(),data,packlen,MidiOutTransferCallback,NULL,100);
		tran->flags = LIBUSB_TRANSFER_FREE_TRANSFER | LIBUSB_TRANSFER_FREE_BUFFER;

		int ret = libusb_submit_transfer(tran);
		if (ret!=0)
			LOGE("USB MIDI OUT submission ERROR %s",LibusbErrorToString(ret).c_str());
		return (ret==0);
}

void Usb::MidiInterface::MidiOutTransferCallback(libusb_transfer* xfer)
{
	if(xfer->status!=LIBUSB_TRANSFER_COMPLETED)
		LOGE("MIDI OUT USB transfer error: %s",TransferErrorToString(xfer->status).c_str() );

	if (xfer->user_data != NULL) {
		//LOGD("Final part of a sysex message, reporting completion.");
		uint32_t* flags = (uint32_t*) xfer->user_data;
		//(*flags) |= MHDR_DONE;
	}

}

// TODO: substitute the completion flags pointer with a listener interface to call for completion!

bool Usb::MidiInterface::SendSysexMessage(unsigned char* sysexData, int length, uint32_t* completionFlags)
{
	if ( !HasOutput() || outEp->GetMaxPacketSize() < 4 || length<=0 )
		return false;

	int packlen = 4;// outEp->GetMaxPacketSize();
	int bytesLeft = length;

	while (bytesLeft > 0)
	{
		int pos = length - bytesLeft;
		unsigned char* data = new unsigned char[packlen];
		unsigned char cable = 0;
		unsigned char cin = 0;
		void* userData = NULL;
		int bytesInPacket = 0;

		if (bytesLeft > 3) {
			// OSS: if the sysex is less than 4 bytes long, the "starting" packet (cin=0x4)
			// will never be sent. it may still be ok, didn't find a reference about this
			cin = 0x4;
			bytesInPacket = 3;
		}
		else if (bytesLeft == 3) {
			cin = 0x7;
			bytesInPacket = 3;
			userData = (void*) completionFlags;
		}
		else if (bytesLeft == 2) {
			cin = 0x6;
			bytesInPacket = 2;
			userData = (void*) completionFlags;
		}
		else if (bytesLeft == 1) {
			cin = 0x5;
			bytesInPacket = 1;
			userData = (void*) completionFlags;
		}
		else {
			LOGE("Error sending sysex, invalid packet length.");
			// you should never be here actually!
			return true;
			delete [] data;
		}

		data[0] = cin | (cable << 4);
		data[1] = sysexData[pos];
		data[2] = bytesInPacket < 2 ? 0 : sysexData[pos+1];
		data[3] = bytesInPacket < 3 ? 0 : sysexData[pos+2];

		//LOGI("USB MIDI OUT - sysex part [%d bytes]: %x %x %x %x",bytesInPacket,data[0],data[1],data[2],data[3]);

		libusb_transfer* tran = libusb_alloc_transfer(0);
		libusb_fill_bulk_transfer(tran,Devh(),outEp->GetAddress(),data,packlen,MidiOutTransferCallback,userData,100);
		tran->flags = LIBUSB_TRANSFER_FREE_TRANSFER | LIBUSB_TRANSFER_FREE_BUFFER;

		int ret = libusb_submit_transfer(tran);
		if (ret!=0) {
			LOGE("USB MIDI OUT submission ERROR %s",LibusbErrorToString(ret).c_str());
			return false;
		}
		bytesLeft -= bytesInPacket;
	}
	return true;
}



bool Usb::MidiInterface::SendMIDIMessageSynchronous(uint32_t message)
{
	if ( !HasOutput() || outEp->GetMaxPacketSize() < 4 )
		return false;

	unsigned char data[4];
	int transferred = 0;

	unsigned char hiNibble = (message & 0xff) >> 4;
	unsigned char loNibble = message & 0xf;

	unsigned char cable = 0;
	unsigned char cin = 0;

	if (hiNibble==0xf) { // a system message!

		switch (loNibble) { // see usb midi specs, pag 16
		case 0x0: // start sysex
			cin = 0x4;
			break;
		case 0x7: // end of sysex... how many bytes?
			cin = 0x5;// 1 byte. 0x6->2byte 0x7->3byte
			break;
		case 0x1: // mtc quarter frame
		case 0x3: // song select
			cin = 0x2; // two bytes system common message
			break;
		case 0x2: // song pos pointer
			cin = 0x3; // three bytes system common message
			break;
		case 0x6: // tune request
		case 0x8: // midi clock
		case 0xFA: // staRT
		case 0xFB: // continue
		case 0xFC: // stop
		case 0xFE: // "Active Sensing"
		case 0xFF: // "Reset"
			cin = 0x4; // single byte system common message
			break;
		case 0xF9: // "Undefined"
		case 0xFD: // "Undefined"
		case 0x4: // undef
		case 0x5: // undef
		default:
			return false; // since they are not supported yet!
		}

	} else {
		cin = hiNibble; // note on, off, control etc have the cin equal to the hiNibble!
	}

	data[0] = cin | (cable << 4);
	data[1] = message;
	data[2] = message >> 8;
	data[3] = message >> 16;

	//LOGI("USB MIDI OUT - message: %x => to usb: %x %x %x %x",
		//	message,data[0],data[1],data[2],data[3]);

	int ret = libusb_bulk_transfer(Devh(),outEp->GetAddress(),data,4,&transferred,50); // blocking!
	if (ret!=0)
		LOGE("USB MIDI OUT ERROR %s, transferred %d",LibusbErrorToString(ret).c_str(),transferred);
	return (ret==0) && (transferred==4);
}

#ifdef ENABLE_MIDI_INPUT

bool Usb::MidiInterface::StartInputSynchronous()
{
	stopInputThread = false;
	int res = pthread_create(&inputThread,NULL,MidiInterface::InputPolling,(void*)this);
    return true;
}

bool Usb::MidiInterface::StartInput()
{
	//LOGE("----> MIDI input START");
	if (inputTransfers.size()!=0) {
		LOGE("USB MIDI IN start error. transfers queue not empty!");
		return false;
	}

	bool somethingFailed = false;
	for (int i=0; i<numInputTransfers; i++)
	{
		int packlen = inEp->GetMaxPacketSize();
		unsigned char* data = new unsigned char[packlen];
		libusb_transfer* xfr = libusb_alloc_transfer(0);
		libusb_fill_bulk_transfer(xfr,Devh(),inEp->GetAddress(),data,packlen,MidiInTransferCallback,this,0);

		//LOGD("TRANSFER SUBM %x devh %x, ep %x, flags %d, len %d, actual_len %d, buf: %d, stat: %d, timeout %d",
		//	xfr,xfr->dev_handle,xfr->endpoint,xfr->flags,	xfr->length,xfr->actual_length,	xfr->buffer,xfr->status,xfr->timeout);

		int ret = libusb_submit_transfer(xfr);
		if (ret!=0) {
				LOGE("USB MIDI in transfer submission ERROR %s",LibusbErrorToString(ret).c_str());
				somethingFailed=true;
				libusb_free_transfer(xfr);
		} else {
			inputTransfers.push_back(xfr);
		}
	}
	//LOGE(" ---> MIDI input STARTED");
	return !somethingFailed;
}


void Usb::MidiInterface::MidiInTransferCallback(libusb_transfer* xfr)
{
	//LOGE("----> MIDI callback input");
	//libusb_transfer* other_xfr = (libusb_transfer*) xfr->user_data;
	MidiInterface* mi = (MidiInterface*) xfr->user_data;

	bool error = false;
	bool cancelled = false;

	switch (xfr->status) {
		case LIBUSB_TRANSFER_CANCELLED:
		case LIBUSB_TRANSFER_NO_DEVICE:
			LOGI("USB MIDI IN - Transfer cancelled: %s",TransferErrorToString(xfr->status).c_str());
			cancelled = true;
			error = true;
			break;
		case LIBUSB_TRANSFER_ERROR:
		case LIBUSB_TRANSFER_TIMED_OUT:
		case LIBUSB_TRANSFER_STALL:
		case LIBUSB_TRANSFER_OVERFLOW:
			LOGE("USB MIDI IN - Transfer error: %s",TransferErrorToString(xfr->status).c_str());
			error = true;
			break;
	}

	if (!error) {

		uint32_t midimsg=0;
		unsigned char* data = xfr->buffer;
		int transferred = xfr->actual_length;

		for(int bytesCount=0; bytesCount<transferred; bytesCount+=4)
		{
			if (data[bytesCount]==0 && data[bytesCount+1]==0) // filter empty messages
				continue;
			if ((data[bytesCount+2] &0b10000000) || (data[bytesCount+3]&0b10000000))
				continue; // filter malformed messages ([2] e [3] must be DATA bytes)

			int cin = data[bytesCount] & 0xf;
			int cable = (data[bytesCount] >> 4) & 0xf;

			//LOGV("USB MIDI IN - cable: %x, CIN: %x, MIDI msg: %x %x %x",
			//		cable,cin,data[bytesCount+1],data[bytesCount+2],data[bytesCount+3]);

			midimsg = 0;
			midimsg|=((unsigned int)data[bytesCount+3])<<16;
			midimsg|=((unsigned int)data[bytesCount+2])<<8;
			midimsg|=data[bytesCount+1];

			FILETIME timestamp;
			GetSystemTimeAsFileTime(&timestamp);
			MTC::MidiInProcInternal(NULL, MIM_DATA, NULL, midimsg, ((long long)timestamp-(long long)midirecsystime_start_device)/10000, -1,NULL);
		}
	}

	if (cancelled) {
		//LOGD("MIDI IN - transfer cancelled, freeing memory");
		mi->FreeInputTransfer(xfr);
		return;
	}

	// LOGD("TRANSFER devh %x, ep %x, flags %d, len %d, actual_len %d, buf: %d, stat: %d, timeout %d",
	// xfr->dev_handle,xfr->endpoint,xfr->flags,xfr->length,xfr->actual_length,xfr->buffer,xfr->status,xfr->timeout);

	if (libusb_submit_transfer(xfr) < 0) {
		// LOGE("USB MIDI INPUT error re-submitting transfer\n");
		mi->FreeInputTransfer(xfr);
	}
}

bool Usb::MidiInterface::StopInput()
{
	Sync::CAutoLock alock(&inputTransfersDeletionLock);
	bool anyCancelFailed = false;
	for (int i=0; i<inputTransfers.size(); i++) {
		int ret = libusb_cancel_transfer(inputTransfers[i]);
		if (ret!=0) {
			LOGE("MIDI IN stop error with transfer %d: %s",i,LibusbErrorToString(ret).c_str());
			anyCancelFailed = true;
		}
	}
	inputTransfers.clear(); // don't delete transfers, will be done in callback!
	return !anyCancelFailed;
}

void Usb::MidiInterface::FreeInputTransfer(libusb_transfer* xfr)
{
	Sync::CAutoLock alock(&inputTransfersDeletionLock);
	std::vector<libusb_transfer*>::iterator it;
	for (it=inputTransfers.begin(); it!=inputTransfers.end(); it++)
	{
		if ((*it) == xfr) {
			//LOGD("Found transfer, removing");
			inputTransfers.erase(it);
			break;
		}
	}
	libusb_free_transfer(xfr);
}

void* Usb::MidiInterface::InputPolling(void* thiz) // double check thread safety!
{
	MidiInterface* mi = (MidiInterface*) thiz; // may be destroyed... very unlikely but possible :/
	int ret;
	bool* interfaceDestroyed = mi->destroyed;
	int maxLen = 4; //mi->inEp->GetMaxPacketSize();
	unsigned char addr = mi->inEp->GetAddress();
	libusb_device_handle* devHandle = mi->Devh(); // still very NON thread safe
	unsigned char* data = new unsigned char[maxLen];
	int transferred = 0;
	int errors = 0;
	uint32_t midimsg=0;

	bool stop = mi->stopInputThread;

	while (!stop) {

		ret = libusb_bulk_transfer(devHandle,addr,data,maxLen,&transferred,0); // blocking!



		if (0!=ret) {
			++errors;
			if (errors >= 2) break; // TODO: report error to user
			else continue; // skip this loop iteration but give other opportunities...
		}
		else
			errors = 0;

		for(int bytesCount=0; bytesCount<transferred; bytesCount+=4)
		{
			int cin = data[bytesCount] & 0xf; // midi is big-endian
			int cable = (data[bytesCount] >> 4) & 0xf;

			//LOGV("USB MIDI IN - cable: %x, CIN: %x, MIDI msg: %x %x %x",
			//		cable,cin,data[bytesCount+1],data[bytesCount+2],data[bytesCount+3]);

			//int payloadBytes = 1;

			/*switch (cin) {
				case 8:   // note off
				case 9:   // note on
				case 0xb: // modulation wheel, amplitude slider, sustain pedal
				case 0xe: // pitch bend
					payloadBytes = 3;
					break;
				case 0xc: // program change
					payloadBytes = 2;
					break;
			}*/

			midimsg = 0;
			midimsg|=((unsigned int)data[bytesCount+3])<<16;
			midimsg|=((unsigned int)data[bytesCount+2])<<8;
			midimsg|=data[bytesCount+1];

			FILETIME timestamp;
			GetSystemTimeAsFileTime(&timestamp);
			MTC::MidiInProcInternal(NULL, MIM_DATA, NULL, midimsg, ((long long)timestamp-(long long)midirecsystime_start_device)/10000, -1,NULL);
		}

		if (*interfaceDestroyed) {
			delete [] data;
			return NULL;
		}
		stop = mi->stopInputThread; //  lock
	}
	mi->inputThreadAlive = false;
	delete [] data;
	return NULL;
}

#endif




