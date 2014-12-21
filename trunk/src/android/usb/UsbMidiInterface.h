/*
 * UsbMidiInterface.h
 *
 *  Created on: 01/ago/2014
 *      Author: athos
 */

#ifndef USBMIDIINTERFACE_H_
#define USBMIDIINTERFACE_H_

#include "UsbAudioInterfaces.h"
#include <string>
//#include "Sync.h"



namespace Usb {

class AudioDevice;

class MidiInterface : public AudioInterface
{

public:

	MidiInterface(AudioDevice* audioDev, libusb_interface itf);
	~MidiInterface();

	std::string DumpInfo();

	bool HasOutput();
	bool HasInput();

	#ifdef ENABLE_MIDI_INPUT
	bool StartInput();
	bool StopInput();
	static void MidiInTransferCallback(libusb_transfer* xfer);
	#endif

	bool SendMIDIMessage(uint32_t message);
	bool SendSysexMessage(unsigned char* data, int length, uint32_t* completionFlags);

	static void MidiOutTransferCallback(libusb_transfer* xfer);

	std::string GetOutName();
	std::string GetInName();

private:

	#ifdef ENABLE_MIDI_INPUT
	Sync::CCritSec inputTransfersDeletionLock;
	Endpoint* inEp;
	std::vector<Endpoint*> additionalIns;
	pthread_t inputThread;
	bool stopInputThread;
	static void* InputPolling(void* thiz); // this may one day call a user-provided callback

	bool* destroyed;
	bool inputThreadAlive;

	static const int numInputTransfers = 1;
	std::vector<libusb_transfer*> inputTransfers;
	void FreeInputTransfer(libusb_transfer* xfr);

	bool StartInputSynchronous();

	#endif


	Endpoint* outEp;
	std::vector<Endpoint*> additionalOuts;
	int numUnsupportedEndpoints;
	bool SendMIDIMessageSynchronous(uint32_t message);

};




}




#endif /* USBMIDIINTERFACE_H_ */
