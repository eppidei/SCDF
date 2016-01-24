
#pragma once

#include "RtpMidi_Defs.h"
#include "RtpMidiPack.h"
#include "RtpMidi_Clock.h"

//#define MAX_SESSIONS 4

class IRtpMidi
{
public:
	virtual bool PassesFilter(void* sender, DataByte, DataByte) = 0;

	virtual void OnNoteOn(void* sender, DataByte, DataByte, DataByte) = 0;
	virtual void OnNoteOff(void* sender, DataByte, DataByte, DataByte) = 0;
	virtual void OnPolyPressure(void* sender, DataByte, DataByte, DataByte) = 0;
	virtual void OnChannelPressure(void* sender, DataByte, DataByte) = 0;
	virtual void OnPitchBendChange(void* sender, DataByte, int) = 0;
	virtual void OnProgramChange(void* sender, DataByte, DataByte) = 0;
	virtual void OnControlChange(void* sender, DataByte, DataByte, DataByte) = 0;
	virtual void OnTimeCodeQuarterFrame(void* sender, DataByte) = 0;
	virtual void OnSongSelect(void* sender, DataByte) = 0;
	virtual void OnSongPosition(void* sender, int) = 0;
	virtual void OnTuneRequest(void* sender) = 0;
};

//class IAppleMidi : public IRtpMidi
//{
//public:
////	virtual void invite(IPAddress ip, uint16_t port = CONTROL_PORT) = 0;
//
//	virtual void OnInvitation(void* sender, AppleMIDI_Invitation&) = 0;
//	virtual void OnEndSession(void* sender, AppleMIDI_EndSession&) = 0;
//	virtual void OnReceiverFeedback(void* sender, AppleMIDI_ReceiverFeedback&) = 0;
//
//	virtual void OnInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&) = 0;
//	virtual void OnControlInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&) = 0;
//	virtual void OnContentInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&) = 0;
//
//	virtual void OnSyncronization(void* sender, AppleMIDI_Syncronization&) = 0;
//	virtual void OnBitrateReceiveLimit(void* sender, AppleMIDI_BitrateReceiveLimit&) = 0;
//	virtual void OnControlInvitation(void* sender, AppleMIDI_Invitation&) = 0;
//	virtual void OnContentInvitation(void* sender, AppleMIDI_Invitation&) = 0;
//};

/*! \brief The main class for AppleMidi_Class handling.\n
	See member descriptions to know how to use it,
	or check out the examples supplied with the library.
 */
//template<class UdpClass>
class RtpMidiPack_Class
{
protected:
	RtpMidiPack _rtpMidi;
	RtpMidi_Clock _rtpMidiClock;

	// SSRC, Synchronization source.
	// (RFC 1889) The source of a stream of RTP packets, identified by a 32-bit numeric SSRC identifier
	// carried in the RTP header so as not to be dependent upon the network address. All packets from a
	// synchronization source form part of the same timing and sequence number space, so a receiver groups
	// packets by synchronization source for playback. Examples of synchronization sources include the
	// sender of a stream of packets derived from a signal source such as a microphone or a camera, or an
	// RTP mixer. A synchronization source may change its data format, e.g., audio encoding, over time.
	// The SSRC identifier is a randomly chosen value meant to be globally unique within a particular RTP
	// session. A participant need not use the same SSRC identifier for all the RTP sessions in a
	// multimedia session; the binding of the SSRC identifiers is provided through RTCP. If a participant
	// generates multiple streams in one RTP session, for example from separate video cameras, each must
	// be identified as a different SSRC.
	uint32_t _ssrc;

//	Session_t	Sessions[MAX_SESSIONS];

//	char _sessionName[50];

	 uint32_t	createInitiatorToken();

public:
    char *GetPacket() {return _rtpMidi.GetPacket();}
    int GetPacketSize() {return _rtpMidi.GetPacketSize();}
	RtpMidiPack_Class(uint32_t ssrc, uint32_t initTimestamp);
	 ~RtpMidiPack_Class();

	void begin(const char*, uint16_t port = CONTROL_PORT);
//
//	 uint32_t	getSynchronizationSource() { return _ssrc; }
//	 char*	getSessionName() { return _sessionName; }

	 void run();

	// IAppleMidi

//	 void invite(IPAddress ip, uint16_t port = CONTROL_PORT);

//	 void OnInvitation(void* sender, AppleMIDI_Invitation&);
//	 void OnEndSession(void* sender, AppleMIDI_EndSession&);
//	 void OnReceiverFeedback(void* sender, AppleMIDI_ReceiverFeedback&);
//
//	 void OnInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);
//	 void OnControlInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);
//	 void OnContentInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted&);
//
//	 void OnSyncronization(void* sender, AppleMIDI_Syncronization&);
//	 void OnBitrateReceiveLimit(void* sender, AppleMIDI_BitrateReceiveLimit&);
//	 void OnControlInvitation(void* sender, AppleMIDI_Invitation&);
//	 void OnContentInvitation(void* sender, AppleMIDI_Invitation&);

	// IRtpMidi
	 bool PassesFilter (void* sender, DataByte, DataByte);

	void OnNoteOn (void* sender, DataByte, DataByte, DataByte);
	 void OnNoteOff(void* sender, DataByte, DataByte, DataByte);
	 void OnPolyPressure(void* sender, DataByte, DataByte, DataByte);
	 void OnChannelPressure(void* sender, DataByte, DataByte);
	 void OnPitchBendChange(void* sender, DataByte, int);
	 void OnProgramChange(void* sender, DataByte, DataByte);
	 void OnControlChange(void* sender, DataByte, DataByte, DataByte);
	 void OnTimeCodeQuarterFrame(void* sender, DataByte);
	 void OnSongSelect(void* sender, DataByte);
	 void OnSongPosition(void* sender, int);
	 void OnTuneRequest(void* sender);

private:
//	 void write(UdpClass&, AppleMIDI_InvitationRejected&, IPAddress ip, uint16_t port);
//	 void write(UdpClass&, AppleMIDI_InvitationAccepted&, IPAddress ip, uint16_t port);
//	 void write(UdpClass&, AppleMIDI_Syncronization&, IPAddress ip, uint16_t port);
//	 void write(UdpClass&, AppleMIDI_Invitation&, IPAddress ip, uint16_t port);


public:
    void noteOn(DataByte inNoteNumber, DataByte inVelocity, Channel inChannel);
    void noteOff(DataByte inNoteNumber, DataByte inVelocity, Channel inChannel);
     void programChange(DataByte inProgramNumber, Channel inChannel);
     void controlChange(DataByte inControlNumber, DataByte inControlValue, Channel inChannel);
     void pitchBend(int inPitchValue,    Channel inChannel);
     void pitchBend(double inPitchValue, Channel inChannel);
     void polyPressure(DataByte inNoteNumber, DataByte inPressure, Channel inChannel);
     void afterTouch(DataByte inPressure, Channel inChannel);
     void sysEx(unsigned int inLength, const byte* inArray, bool inArrayContainsBoundaries = true);
     void timeCodeQuarterFrame(DataByte inTypeNibble, DataByte inValuesNibble);
     void timeCodeQuarterFrame(DataByte inData);
     void songPosition(unsigned int inBeats);
     void songSelect(DataByte inSongNumber);
     void tuneRequest();
     void activeSensing();
     void start();
     void _continue();
     void stop();
     void systemReset();
     void clock();
     void tick();

protected:
     void send(MidiType inType, DataByte inData1, DataByte inData2, Channel inChannel);
     void send(MidiType inType, DataByte inData1, DataByte inData2);
     void send(MidiType inType, DataByte inData);
     void send(MidiType inType);

private:
     void internalSend(MidiType inType, DataByte inData1, DataByte inData2, Channel inChannel);
     void internalSend(MidiType inType, DataByte inData1, DataByte inData2);
     void internalSend(MidiType inType, DataByte inData);
     void internalSend(MidiType inType);

	StatusByte getStatus(MidiType inType, Channel inChannel) const;

public:
 //   void xxx(void (*fptr)(byte channel, byte note, byte velocity));
private:
    void (*mNoteOnSendingEvent)(byte channel, byte note, byte velocity);
    void (*mNoteOnSendEvent)(byte channel, byte note, byte velocity);
    void (*mNoteOffSendingEvent)(byte channel, byte note, byte velocity);
    void (*mNoteOffSendEvent)(byte channel, byte note, byte velocity);


//	 int	GetFreeSessionSlot();
//	 int	GetSessionSlotUsingSSrc(const uint32_t ssrc);
//	 int	GetSessionSlotUsingInitiatorToken(const uint32_t initiatorToken);
//	 void	CreateLocalSession(const int slot, const uint32_t ssrc);
//	 void	CreateRemoteSession(IPAddress ip, uint16_t port);
//	 void	CompleteLocalSessionControl(AppleMIDI_InvitationAccepted& invitationAccepted);
//	 void	CompleteLocalSessionContent(AppleMIDI_InvitationAccepted& invitationAccepted);
//	 void	DeleteSession(const uint32_t ssrc);
//	 void	DeleteSession(int slot);
//	 void	DeleteSessions();

	 void	DumpSession();

	 void ManageInvites();
	 void ManageTiming();


private:
    StatusByte mRunningStatus_RX;
    Channel    _inputChannel;

    // -------------------------------------------------------------------------
    // Input Callbacks


public:
	 void OnConnected(void(*fptr)(uint32_t, char*));
	 void OnDisconnected(void(*fptr)(uint32_t));

     void OnReceiveNoteOn(void (*fptr)(byte channel, byte note, byte velocity));
     void OnReceiveNoteOff(void (*fptr)(byte channel, byte note, byte velocity));
     void OnReceiveAfterTouchPoly(void (*fptr)(byte channel, byte note, byte pressure));
     void OnReceiveControlChange(void (*fptr)(byte channel, byte number, byte value));
     void OnReceiveProgramChange(void (*fptr)(byte channel, byte number));
     void OnReceiveAfterTouchChannel(void (*fptr)(byte channel, byte pressure));
     void OnReceivePitchBend(void (*fptr)(byte channel, int bend));
     void OnReceiveSystemExclusive(void (*fptr)(byte * array, byte size));
     void OnReceiveTimeCodeQuarterFrame(void (*fptr)(byte data));
     void OnReceiveSongPosition(void (*fptr)(unsigned int beats));
     void OnReceiveSongSelect(void (*fptr)(byte songnumber));
     void OnReceiveTuneRequest(void (*fptr)(void));
     void OnReceiveClock(void (*fptr)(void));
     void OnReceiveStart(void (*fptr)(void));
     void OnReceiveContinue(void (*fptr)(void));
     void OnReceiveStop(void (*fptr)(void));
     void OnReceiveActiveSensing(void (*fptr)(void));
     void OnReceiveSystemReset(void (*fptr)(void));

private:

     void launchCallback();

	void(*mConnectedCallback)(uint32_t, char*);
	void(*mDisconnectedCallback)(uint32_t);

    void (*mNoteOffCallback)(byte channel, byte note, byte velocity);
    void (*mNoteOnCallback)(byte channel, byte note, byte velocity);
    void (*mAfterTouchPolyCallback)(byte channel, byte note, byte velocity);
    void (*mControlChangeCallback)(byte channel, byte, byte);
    void (*mProgramChangeCallback)(byte channel, byte);
    void (*mAfterTouchChannelCallback)(byte channel, byte);
    void (*mPitchBendCallback)(byte channel, int);
    void (*mSongPositionCallback)(unsigned int beats);
    void (*mSongSelectCallback)(byte songnumber);
    void (*mTuneRequestCallback)(void);
    void (*mTimeCodeQuarterFrameCallback)(byte data);

    void (*mSystemExclusiveCallback)(byte * array, byte size);
    void (*mClockCallback)(void);
    void (*mStartCallback)(void);
    void (*mContinueCallback)(void);
    void (*mStopCallback)(void);
    void (*mActiveSensingCallback)(void);
    void (*mSystemResetCallback)(void);


};
