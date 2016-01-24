#include "stdlib.h"
#include "RtpMidi.h"
#include "RtpMidi_Defs.h"

 RtpMidiPack_Class::RtpMidiPack_Class(uint32_t ssrc, uint32_t initTimestamp)
{
    // Initialise callbacks to NULL pointer
    mConnectedCallback				= NULL;
    mDisconnectedCallback			= NULL;

    mNoteOffCallback				= NULL;
    mNoteOnCallback					= NULL;
    mAfterTouchPolyCallback			= NULL;
    mControlChangeCallback			= NULL;
    mProgramChangeCallback			= NULL;
    mAfterTouchChannelCallback		= NULL;
    mPitchBendCallback				= NULL;
    mSystemExclusiveCallback		= NULL;
    mTimeCodeQuarterFrameCallback	= NULL;
    mSongPositionCallback			= NULL;
    mSongSelectCallback				= NULL;
    mTuneRequestCallback			= NULL;
    mClockCallback					= NULL;
    mStartCallback					= NULL;
    mContinueCallback				= NULL;
    mStopCallback					= NULL;
    mActiveSensingCallback			= NULL;
    mSystemResetCallback			= NULL;

    mNoteOnSendingEvent				= NULL;
    mNoteOnSendEvent				= NULL;
    mNoteOffSendingEvent			= NULL;
    mNoteOffSendEvent				= NULL;

    _ssrc=ssrc;
    _rtpMidi.ssrc=ssrc;
    _rtpMidiClock.Init(initTimestamp, MIDI_SAMPLING_RATE_DEFAULT);
}

 RtpMidiPack_Class::~RtpMidiPack_Class()
{
}

//---------------------------------------------------------------------------------------

/*! \brief Call the begin method in the setup() function of the Arduino.

 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */

void RtpMidiPack_Class::begin(const char* sessionName, uint16_t port)
{
    //
    //	strcpy(_sessionName, sessionName);
    //
    //	Port = port;
    //
    //	_inputChannel = MIDI_CHANNEL_OMNI;
    //
    //	// Generate Synchronization Source
    //	// Unique 32 bit number (see definition)
    //	byte buffer[4];
    //	for (int i = 0; i < 4; i++)
    //		buffer[i] = 17 + (rand() % 255);
    //	_ssrc = *((uint32_t*) &buffer[0]);
    //
    //	// Initialize Sessions
    //	DeleteSessions();
    //
    //	// open UDP socket for control messages
    ////	_controlUDP.begin(Port);
    //	// open UDP socket for rtp messages
    ////	_contentUDP.begin(Port + 1);
    //
    //	_controlDissector.init(Port, this);
    //	_controlDissector.addPacketDissector(&PacketAppleMidi::dissect_apple_midi);	// Add parser
    //
    //	_contentDissector.init(Port + 1, this);
    //	_contentDissector.addPacketDissector(&PacketRtpMidi::dissect_rtp_midi);		// Add parser
    //	_contentDissector.addPacketDissector(&PacketAppleMidi::dissect_apple_midi);	// Add parser
    //
    //	_rtpMidi.ssrc = _ssrc;
    //	_rtpMidi.sequenceNr = 1;
    //
    //#if (APPLEMIDI_DEBUG)
    //	LOGD("Starting");
    //#if (APPLEMIDI_DEBUG_VERBOSE)
    //	LOGD(" Verbose");
    //#endif
    //	LOGD(" logging");
    //#endif
}

/*! \brief Evaluates incoming Rtp messages.
 */

 void RtpMidiPack_Class::run()
{
    //	// resend invitations
    //	ManageInvites();
    //
    //	// do syncronization here
    //	ManageTiming();
    //
    //	byte _packetBuffer[UDP_TX_PACKET_MAX_SIZE];
    //
    //	// Get first packet of CONTROL logic, if any
    //	int packetSize = _controlUDP.parsePacket();
    //	int bytesRead = 0;
    //
    //	// While we still have packets to process
    //	while (packetSize > 0) {
    //		// While we still have bytes to process in the packet
    //		while (packetSize > 0) {
    //			bytesRead = _controlUDP.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //			packetSize = packetSize - bytesRead;
    //			_controlDissector.addPacket(_packetBuffer, bytesRead);
    //		}
    //
    //		// Dissect packet only after all bytes have been added to the buffer
    //		_controlDissector.dissect();
    //
    //		// Get next packet
    //		packetSize = _controlUDP.parsePacket();
    //	}
    //
    //	// Get first packet of CONTENT logic, if any
    //	packetSize = _contentUDP.parsePacket();
    //	bytesRead = 0;
    //
    //	// While we still have packets to process
    //	while (packetSize > 0) {
    //		// While we still have bytes to process in the packet
    //		while (packetSize > 0) {
    //			bytesRead = _contentUDP.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //			packetSize = packetSize - bytesRead;
    //			_contentDissector.addPacket(_packetBuffer, bytesRead);
    //		}
    //
    //		// Dissect packet only after all bytes have been added to the buffer
    //		_contentDissector.dissect();
    //
    //		// Get next packet
    //		packetSize = _contentUDP.parsePacket();
    //	}
}


/*! \brief The Arduino initiates the session.
 */

// void RtpMidiPack_Class::invite(IPAddress ip, uint16_t port)
//{
//	CreateRemoteSession(ip, port);
//
//#if (APPLEMIDI_DEBUG)
//	LOGD("Queued invite");
//#endif
//}

/*! \brief The Arduino is being invited to a session.
 */

// void RtpMidiPack_Class::OnInvitation(void* sender, AppleMIDI_Invitation& invitation)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////	if (dissector->_identifier == Port)
////		OnControlInvitation(sender, invitation);
////	if (dissector->_identifier == Port + 1)
////		OnContentInvitation(sender, invitation);
//}
//
///*! \brief The session has been ended by the remote source.
//*/
//
// void RtpMidiPack_Class::OnEndSession(void* sender, AppleMIDI_EndSession& sessionEnd)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> End Session for ssrc 0x");
////	LOGD(sessionEnd.ssrc);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	LOGD(", initiatorToken = 0x");
////	LOGD(sessionEnd.initiatorToken);
////#endif
////	LOGD("");
////#endif
////
////	DeleteSession(sessionEnd.ssrc);
////
////	if (mDisconnectedCallback != 0)
////		mDisconnectedCallback(sessionEnd.ssrc);
//}
//
///* \brief With the receiver feedback packet, the recipient can tell the sender up to what sequence
//* number in the RTP-stream the packets have been received; this can be used to shorten the
//* recovery-journal-section in the RTP-session */
//
// void RtpMidiPack_Class::OnReceiverFeedback(void* sender, AppleMIDI_ReceiverFeedback& receiverFeedback)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> Receiver Feedback: seq = ");
////	LOGD(receiverFeedback.sequenceNr);
////#endif
//}
//
//
///*! \brief The invitation that we have send, has been accepted.
//*/
//
//void RtpMidiPack_Class::OnInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted& invitationAccepted)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////	if (dissector->_identifier == Port)
////		OnControlInvitationAccepted(sender, invitationAccepted);
////	if (dissector->_identifier == Port + 1)
////		OnContentInvitationAccepted(sender, invitationAccepted);
//}
//
///*! \brief .
//*/
//
//void RtpMidiPack_Class::OnControlInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted& invitationAccepted)
//{
//#if (APPLEMIDI_DEBUG)
//	LOGD("> Control InvitationAccepted: peer = \"");
//	LOGD(invitationAccepted.name);
//	LOGD("\"");
//	LOGD(" ,ssrc 0x");
//	LOGD(invitationAccepted.ssrc);
//	LOGD(" ,initiatorToken = 0x");
//	LOGD(invitationAccepted.initiatorToken);
//	LOGD();
//#endif
//
//	CompleteLocalSessionControl(invitationAccepted);
//}
//
///*! \brief .
//*/
//
//void RtpMidiPack_Class::OnContentInvitationAccepted(void* sender, AppleMIDI_InvitationAccepted& invitationAccepted)
//{
//#if (APPLEMIDI_DEBUG)
//	LOGD("> Content InvitationAccepted: peer = \"");
//	LOGD(invitationAccepted.name);
//	LOGD("\"");
//	LOGD(" ,ssrc 0x");
//	LOGD(invitationAccepted.ssrc);
//#if (APPLEMIDI_DEBUG_VERBOSE)
//	LOGD(" ,initiatorToken = 0x");
//	LOGD(invitationAccepted.initiatorToken);
//#endif
//	LOGD();
//#endif
//
//	CompleteLocalSessionContent(invitationAccepted);
//}

/*! \brief Part 1 of being invited into a session, the Control Invitation.
 */

//void RtpMidiPack_Class::OnControlInvitation(void* sender, AppleMIDI_Invitation& invitation)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> Control Invitation: peer = \"");
////	LOGD(invitation.sessionName);
////	LOGD("\"");
////	LOGD(" ,ssrc 0x");
////	LOGD(invitation.ssrc, HEX);
////	LOGD(" ,initiatorToken = 0x");
////	LOGD(invitation.initiatorToken, HEX);
////	LOGD();
////#endif
////
////	// Do we know this ssrc already?
////	// In case initiator reconnects (after a crash of some sort)
////	int index = GetSessionSlotUsingSSrc(invitation.ssrc);
////	if (index < 0)
////	{
////		// No, not existing; must be a new initiator
////		// Find a free slot to remember this session in
////		index = GetFreeSessionSlot();
////		if (index < 0)
////		{
////			// no free slots, we cant accept invite
////			AppleMIDI_InvitationRejected invitationRejected(invitation.ssrc, invitation.initiatorToken, invitation.sessionName);
////			write(_controlUDP, invitationRejected, _controlUDP.remoteIP(), _controlUDP.remotePort());
////
////			return;
////		}
////	}
////
////	// Initiate a session got this ssrc
////	CreateLocalSession(index, invitation.ssrc);
////
////	AppleMIDI_InvitationAccepted acceptInvitation(_ssrc, invitation.initiatorToken, getSessionName());
////	write(_controlUDP, acceptInvitation, _controlUDP.remoteIP(), _controlUDP.remotePort());
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("< Control InvitationAccepted: peer = \"");
////	LOGD(getSessionName());
////	LOGD("\"");
////	LOGD(" ,ssrc 0x");
////	LOGD(_ssrc, HEX);
////	LOGD(" ,initiatorToken = 0x");
////	LOGD(invitation.initiatorToken, HEX);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	LOGD(" ,in slot = ");
////	LOGD(index);
////#endif
////	LOGD();
////#endif
//}
//
///*! \brief .
//*/
//
//void RtpMidiPack_Class::OnContentInvitation(void* sender, AppleMIDI_Invitation& invitation)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> Content Invitation: peer = \"");
////	LOGD(invitation.sessionName);
////	LOGD("\"");
////	LOGD(" ,ssrc 0x");
////	LOGD(invitation.ssrc, HEX);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	LOGD(" ,initiatorToken = 0x");
////	LOGD(invitation.initiatorToken, HEX);
////#endif
////	LOGD();
////#endif
////
////	// Find the slot, it should be there because created by control session
////	int i = GetSessionSlotUsingSSrc(invitation.ssrc);
////	if (i < 0)
////	{
////#if (APPLEMIDI_DEBUG)
////		LOGD("Error - control session does not exists for ");
////		LOGD(invitation.ssrc, HEX);
////		LOGD(". Rejecting invitation.");
////#endif
////		AppleMIDI_InvitationRejected invitationRejected(invitation.ssrc, invitation.initiatorToken, invitation.sessionName);
////		write(_contentUDP, invitationRejected, _contentUDP.remoteIP(), _contentUDP.remotePort());
////
////		return;
////	}
////
////	AppleMIDI_InvitationAccepted acceptInvitation(_ssrc, invitation.initiatorToken, getSessionName());
////	write(_contentUDP, acceptInvitation, _contentUDP.remoteIP(), _contentUDP.remotePort());
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("< Content InvitationAccepted: peer = \"");
////	LOGD(getSessionName());
////	LOGD("\"");
////	LOGD(" ,ssrc 0x");
////	LOGD(_ssrc, HEX);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	LOGD(" ,initiatorToken = 0x");
////	LOGD(invitation.initiatorToken, HEX);
////	LOGD(" ,in slot = ");
////	LOGD(index);
////#endif
////	LOGD();
////#endif
////
////	Sessions[i].contentIP = _contentUDP.remoteIP();
////	Sessions[i].contentPort = _contentUDP.remotePort();
////	Sessions[i].invite.status = None;
////	Sessions[i].syncronization.enabled = true; // synchronisation can start
////
////
////	if (mConnectedCallback != 0)
////		mConnectedCallback(invitation.ssrc, invitation.sessionName);
//}
//
///*! \brief .
//
//From: http://en.wikipedia.org/wiki/RTP_MIDI
//
//The session initiator sends a first message (named CK0) to the remote partner, giving its local time on
//64 bits (Note that this is not an absolute time, but a time related to a local reference, generally given
//in microseconds since the startup of operating system kernel). This time is expressed on 10 kHz sampling
//clock basis (100 microseconds per increment) The remote partner must answer to this message with a CK1 message,
//containing its own local time on 64 bits. Both partners then know the difference between their respective clocks
//and can determine the offset to apply to Timestamp and Deltatime fields in RTP-MIDI protocol. The session
//initiator finishes this sequence by sending a last message called CK2, containing the local time when it
//received the CK1 message. This technique allows to compute the average latency of the network, and also to
//compensate a potential delay introduced by a slow starting thread (this situation can occur with non-realtime
//operating systems like Linux, Windows or OS X)
//
//Apple recommends to repeat this sequence a few times just after opening the session, in order to get better
//synchronization accuracy (in case of one of the sequence has been delayed accidentally because of a temporary
//network overload or a latency peak in a thread activation)
//
//This sequence must repeat cyclically (between 2 and 6 times per minute typically), and always by the session
//initiator, in order to maintain long term synchronization accuracy by compensation of local clock drift, and also
//to detect a loss of communication partner. A partner not answering to multiple CK0 messages shall consider that
//the remote partner is disconnected. In most cases, session initiators switch their state machine into "Invitation"
//state in order to re-establish communication automatically as soon as the distant partner reconnects to the
//network. Some implementations (especially on personal computers) display also an alert message and offer to the
//user to choose between a new connection attempt or closing the session.
//*/
//
//void RtpMidiPack_Class::OnSyncronization(void* sender, AppleMIDI_Syncronization& synchronization)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> Syncronization for ssrc 0x");
////	LOGD(synchronization.ssrc, HEX);
////	LOGD(", count = ");
////	LOGD(synchronization.count);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	//LOGD  (" Timestamps = ");
////	//LOGD  (synchronization.timestamps[0], HEX);
////	//LOGD  (" ");
////	//LOGD  (synchronization.timestamps[1], HEX);
////	//LOGD  (" ");
////	//LOGD  (synchronization.timestamps[2], HEX);
////#endif
////	LOGD("");
////#endif
////
////	// If we know this session already, ignore it.
////
////	int index = GetSessionSlotUsingSSrc(synchronization.ssrc);
////	if (index < 0)
////	{
////#if (APPLEMIDI_DEBUG)
////		LOGD("hmmm - Syncronization for a session that has never started.");
////#endif
////		return;
////	}
////
////	uint32_t now = _rtpMidiClock.Now();
////
////	if (synchronization.count == 0) /* From session initiator */
////	{
////		synchronization.count = 1;
////
////		synchronization.timestamps[synchronization.count] = now;
////	}
////	else if (synchronization.count == 1) /* From session responder */
////	{
////		/* compute media delay */
////		//uint64_t diff = (now - synchronization.timestamps[0]) / 2;
////		/* approximate time difference between peer and self */
////		//diff = synchronization.timestamps[2] + diff - now;
////
////		// Send CK2
////		synchronization.count = 2;
////		synchronization.timestamps[synchronization.count] = now;
////
////		/* getting this message means that the responder is still alive! */
////		/* remember the time, if it takes to long to respond, we can assume the responder is dead */
////		/* not implemented at this stage*/
////		Sessions[index].syncronization.lastTime = millis();
////		Sessions[index].syncronization.count++;
////		Sessions[index].syncronization.busy = false;
////	}
////	else if (synchronization.count >= 2) /* From session initiator */
////	{
////		/* compute media delay */
////		//uint64_t diff = (synchronization.timestamps[2] - synchronization.timestamps[0]) / 2;
////		/* approximate time difference between peer and self */
////		//diff = synchronization.timestamps[2] + diff - now;
////
////		synchronization.count = 0;
////		synchronization.timestamps[synchronization.count] = now;
////	}
////
////	AppleMIDI_Syncronization synchronizationResponse(_ssrc, synchronization.count, synchronization.timestamps);
////	write(_contentUDP, synchronizationResponse, _contentUDP.remoteIP(), _contentUDP.remotePort());
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("< Syncronization for ssrc 0x");
////	LOGD(_ssrc, HEX);
////	LOGD(", count = ");
////	LOGD(synchronizationResponse.count);
////#if (APPLEMIDI_DEBUG_VERBOSE)
////	//LOGD  (" Timestamps = ");
////	//LOGD  (synchronization.timestamps[0], HEX);
////	//LOGD  (" ");
////	//LOGD  (synchronization.timestamps[1], HEX);
////	//LOGD  (" ");
////	//LOGD  (synchronization.timestamps[2], HEX);
////#endif
////	LOGD("");
////#endif
//
//}
//
///* With the bitrate receive limit packet, the recipient can tell the sender to limit
//the transmission to a certain bitrate. This is important if the peer is a gateway
//to a hardware-device that only supports a certain speed. Like the MIDI 1.0 DIN-cable
//MIDI-implementation which is limited to 31250. */
//
//void RtpMidiPack_Class::OnBitrateReceiveLimit(void* sender, AppleMIDI_BitrateReceiveLimit& bitrateReceiveLimit)
//{
////	Dissector* dissector = (Dissector*) sender;
////
////#if (APPLEMIDI_DEBUG)
////	LOGD("> BitrateReceiveLimit: Limit = ");
////	LOGD(bitrateReceiveLimit.bitratelimit);
////#endif
//
//}

//------------------------------------------------------------------------------------

/*! \brief .
 */

bool RtpMidiPack_Class::PassesFilter(void* sender, DataByte type, DataByte channel)
{
    // This method handles recognition of channel
    // (to know if the message is destinated to the Arduino)

    //if (mMessage.type == InvalidType)
    //    return false;

    // First, check if the received message is Channel
    if (type >= NoteOff && type <= PitchBend)
    {
        // Then we need to know if we listen to it
        if ((channel == _inputChannel)
            || (_inputChannel == MIDI_CHANNEL_OMNI))
        {
            return true;
        }
        else
        {
            // We don't listen to this channel
            return false;
        }
    }
    else
    {
        // System messages are always received
        return true;
    }
}

/*! \brief .
 */

void RtpMidiPack_Class::OnNoteOn(void* sender, DataByte channel, DataByte note, DataByte velocity)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Note On (c=");
    LOGD(channel);
    LOGD(", n=");
    LOGD(note);
    LOGD(", v=");
    LOGD(velocity);
    LOGD(")");
#endif

    if (mNoteOnCallback)
        mNoteOnCallback(channel, note, velocity);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnNoteOff(void* sender, DataByte channel, DataByte note, DataByte velocity)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Note Off (c=");
    LOGD(channel);
    LOGD(", n=");
    LOGD(note);
    LOGD(", v=");
    LOGD(velocity);
    LOGD(")");
#endif

    if (mNoteOffCallback)
        mNoteOffCallback(channel, note, velocity);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnPolyPressure(void* sender, DataByte channel, DataByte note, DataByte pressure)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Poly Pressure (c=");
    LOGD(channel);
    LOGD(", n=");
    LOGD(note);
    LOGD(", p=");
    LOGD(pressure);
    LOGD(")");
#endif

    if (mAfterTouchPolyCallback)
        mAfterTouchPolyCallback(channel, note, pressure);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnChannelPressure(void* sender, DataByte channel, DataByte pressure)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Channel Pressure (c=");
    LOGD(channel);
    LOGD(", p=");
    LOGD(pressure);
    LOGD(")");
#endif

    if (mAfterTouchChannelCallback)
        mAfterTouchChannelCallback(channel, pressure);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnPitchBendChange(void* sender, DataByte channel, int pitch)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Pitch Bend (c=");
    LOGD(channel);
    LOGD(", p=");
    LOGD(pitch);
    LOGD(")");
#endif

    if (mPitchBendCallback)
        mPitchBendCallback(channel, pitch);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnProgramChange(void* sender, DataByte channel, DataByte program)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Program Change (c=");
    LOGD(channel);
    LOGD(", p=");
    LOGD(program);
    LOGD(") ");

    switch (program)
    {
        default:
            LOGD("Other");
            break;
    }

#endif

    if (mProgramChangeCallback)
        mProgramChangeCallback(channel, program);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnControlChange(void* sender, DataByte channel, DataByte controller, DataByte value)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> Control Change (c=");
    LOGD(channel);
    LOGD(", e=");
    LOGD(controller);
    LOGD(", v=");
    LOGD(value);
    LOGD(") ");

    switch (controller)
    {
        case BankSelect:
            LOGD("BankSelect");
            break;
        case ModulationWheel:
            LOGD("ModulationWheel");
            break;
        case BreathController:
            LOGD("BreathController");
            break;
        case FootController:
            LOGD("FootController");
            break;
        case PortamentoTime:
            LOGD("PortamentoTime");
            break;
        case DataEntry:
            LOGD("DataEntry");
            break;
        case ChannelVolume:
            LOGD("ChannelVolume");
            break;
        case Balance:
            LOGD("Balance");
            break;
        case Pan:
            LOGD("Pan");
            break;
        case ExpressionController:
            LOGD("ExpressionController");
            break;
        case EffectControl1:
            LOGD("EffectControl1");
            break;
        case EffectControl2:
            LOGD("EffectControl2");
            break;
        case GeneralPurposeController1:
            LOGD("GeneralPurposeController1");
            break;
        case GeneralPurposeController2:
            LOGD("GeneralPurposeController2");
            break;
        case GeneralPurposeController3:
            LOGD("GeneralPurposeController3");
            break;
        case GeneralPurposeController4:
            LOGD("GeneralPurposeController4");
            break;
        case Sustain:
            LOGD("Sustain");
            break;
        case Portamento:
            LOGD("Portamento");
            break;
        case Sostenuto:
            LOGD("Sostenuto");
            break;
        case SoftPedal:
            LOGD("SoftPedal");
            break;
        case Legato:
            LOGD("Legato");
            break;
        case Hold:
            LOGD("Hold");
            break;
        case SoundController1:
            LOGD("SoundController1");
            break;
        case SoundController2:
            LOGD("SoundController2");
            break;
        case SoundController3:
            LOGD("SoundController3");
            break;
        case SoundController4:
            LOGD("SoundController4");
            break;
        case SoundController5:
            LOGD("SoundController5");
            break;
        case SoundController6:
            LOGD("SoundController6");
            break;
        case SoundController7:
            LOGD("SoundController7");
            break;
        case SoundController8:
            LOGD("SoundController8");
            break;
        case SoundController9:
            LOGD("SoundController9");
            break;
        case SoundController10:
            LOGD("SoundController10");
            break;
        case GeneralPurposeController5:
            LOGD("GeneralPurposeController5");
            break;
        case GeneralPurposeController6:
            LOGD("GeneralPurposeController6");
            break;
        case GeneralPurposeController7:
            LOGD("GeneralPurposeController7");
            break;
        case GeneralPurposeController8:
            LOGD("GeneralPurposeController8");
            break;
        case PortamentoControl:
            LOGD("PortamentoControl");
            break;
        case Effects1:
            LOGD("Effects1");
            break;
        case Effects2:
            LOGD("Effects2");
            break;
        case Effects3:
            LOGD("Effects3");
            break;
        case Effects4:
            LOGD("Effects4");
            break;
        case Effects5:
            LOGD("Effects5");
            break;
        case AllSoundOff:
            LOGD("AllSoundOff");
            break;
        case ResetAllControllers:
            LOGD("ResetAllControllers");
            break;
        case LocalControl:
            LOGD("LocalControl");
            break;
        case AllNotesOff:
            LOGD("AllNotesOff");
            break;
        case OmniModeOff:
            LOGD("OmniModeOff");
            break;
        case OmniModeOn:
            LOGD("OmniModeOn");
            break;
        case MonoModeOn:
            LOGD("MonoModeOn");
            break;
        case PolyModeOn:
            LOGD("PolyModeOn");
            break;
        default:
            LOGD("Other");
            break;
    }
#endif

    if (mControlChangeCallback)
        mControlChangeCallback(channel, controller, value);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnSongSelect(void* sender, DataByte songNr)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> SongSelect (s=");
    LOGD(songNr);
    LOGD(")");
#endif

    if (mSongSelectCallback)
        mSongSelectCallback(songNr);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnSongPosition(void* sender, int value)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> SongPosition (c=");
    LOGD(value);
    LOGD(")");
#endif

    if (mSongPositionCallback)
        mSongPositionCallback(value);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnTimeCodeQuarterFrame(void* sender, DataByte value)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> TimeCodeQuarterFrame (c=");
    LOGD(value);
    LOGD(")");
#endif

    if (mTimeCodeQuarterFrameCallback)
        mTimeCodeQuarterFrameCallback(value);
}

/*! \brief .
 */

void RtpMidiPack_Class::OnTuneRequest(void* sender)
{
#if (APPLEMIDI_DEBUG)
    LOGD("> TuneRequest ()");
#endif

    if (mTuneRequestCallback)
        mTuneRequestCallback();
}

//------------------------------------------------------------------------------

///*! \brief Find a free session slot.
// */
//
//int RtpMidiPack_Class::GetFreeSessionSlot()
//{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//        if (0 == Sessions[i].ssrc)
//            return i;
//    return -1;
//}
//
///*! \brief Find the slot of a session, based on the ssrc.
// */
//
//int RtpMidiPack_Class::GetSessionSlotUsingSSrc(const uint32_t ssrc)
//{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//        if (ssrc == Sessions[i].ssrc)
//            return i;
//    return -1;
//}
//
///*! \brief Find the slot of a session, based on the ssrc.
// */
//
//int RtpMidiPack_Class::GetSessionSlotUsingInitiatorToken(const uint32_t initiatorToken)
//{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//        if (initiatorToken == Sessions[i].invite.initiatorToken)
//            return i;
//    return -1;
//}

/*! \brief .
 */

//void RtpMidiPack_Class::CompleteLocalSessionControl(AppleMIDI_InvitationAccepted& invitationAccepted)
//{
//	// Find slot, based on initiator token
//	int i = GetSessionSlotUsingInitiatorToken(invitationAccepted.initiatorToken);
//	if (i < 0)
//	{
//#if (APPLEMIDI_DEBUG)
//		LOGD("hmm, initiatorToken not found");
//#endif
//		return;
//	}
//
//	//
//	if (Sessions[i].invite.status != WaitingForControlInvitationAccepted)
//	{
//#if (APPLEMIDI_DEBUG) // issue warning
//		LOGD("status not what expected");
//#endif
//	}
//
//	// Initiate next step in the invitation process
//	Sessions[i].invite.lastSend = 0;
//	Sessions[i].invite.attempts = 0;
//	Sessions[i].invite.ssrc = invitationAccepted.ssrc;
//	Sessions[i].invite.status = SendContentInvite;
//}
//
///*! \brief .
//*/
//
//void RtpMidiPack_Class::CompleteLocalSessionContent(AppleMIDI_InvitationAccepted& invitationAccepted)
//{
//	// Find slot, based on initiator token
//	int i = GetSessionSlotUsingInitiatorToken(invitationAccepted.initiatorToken);
//	if (i < 0)
//	{
//#if (APPLEMIDI_DEBUG)
//		LOGD("hmm, initiatorToken not found");
//#endif
//		return;
//	}
//
//	//
//	if (Sessions[i].invite.status != WaitingForContentInvitationAccepted)
//	{
//#if (APPLEMIDI_DEBUG) // issue warning
//		LOGD("status not what expected");
//#endif
//	}
//
//	// Finalize invitation process
//	Sessions[i].ssrc = invitationAccepted.ssrc;
////	strcpy(Sessions[i].name, invitationAccepted.name);
//	Sessions[i].invite.status = None;
//	Sessions[i].syncronization.enabled = true; // synchronisation can start
//
//	if (mConnectedCallback != 0)
//		mConnectedCallback(Sessions[i].ssrc, invitationAccepted.name);
//}

/*! \brief Initialize session at slot 'index'.
 */

//void RtpMidiPack_Class::CreateLocalSession(const int i, const uint32_t ssrc)
//{
//#if (APPLEMIDI_DEBUG)
//    LOGD  ("New Local Session in slot ");
//    LOGD  (i);
//    LOGD  (" with SSRC ");
//    LOGD(ssrc);
//#endif
//
//    Sessions[i].ssrc = ssrc;
//    Sessions[i].seqNum = 1;
//    Sessions[i].initiator = Remote;
//    Sessions[i].syncronization.lastTime = 0;
//    Sessions[i].syncronization.count = 0;
//    Sessions[i].syncronization.busy = false;
//    Sessions[i].syncronization.enabled = false;
//    Sessions[i].invite.status = ReceiveControlInvitation;
//}

/*! \brief Initialize session at slot 'index'.
 */
//
//void RtpMidiPack_Class::CreateRemoteSession(IPAddress ip, uint16_t port)
//{
//	int i = GetFreeSessionSlot();
//	if (i < 0)
//	{
//#if (APPLEMIDI_DEBUG)
//		LOGD("Invite: No free slot availble, invitation cancelled.");
//#endif
//		return;
//	}
//
//#if (APPLEMIDI_DEBUG)
//	LOGD("New Remote Session in slot ");
//	LOGD(i);
//#endif
//
//	Sessions[i].ssrc = -1;
//	Sessions[i].seqNum = 0;
//	Sessions[i].initiator = Local;
//	Sessions[i].contentIP = ip;
//	Sessions[i].contentPort = port + 1;
//	Sessions[i].syncronization.lastTime = 0;
//	Sessions[i].syncronization.count = 0;
//	Sessions[i].syncronization.busy = false;
//	Sessions[i].syncronization.enabled = false;
//
//	Sessions[i].invite.remoteHost = ip;
//	Sessions[i].invite.remotePort = port;
//	Sessions[i].invite.lastSend = 0;
//	Sessions[i].invite.attempts = 0;
//	Sessions[i].invite.status = SendControlInvite;
//}

/*! \brief .
 */

//void RtpMidiPack_Class::DeleteSession(const uint32_t ssrc)
//{
//    // Find the slot first
//    int slot = GetSessionSlotUsingSSrc(ssrc);
//    if (slot < 0)
//        return;
//
//    DeleteSession(slot);
//}

/*! \brief .
 */

//void RtpMidiPack_Class::DeleteSession(int slot)
//{
//    // Then zero-ize it
//    Sessions[slot].ssrc = 0;
//    Sessions[slot].seqNum = 0;
//    Sessions[slot].initiator = Undefined;
//    Sessions[slot].invite.status = None;
//    Sessions[slot].syncronization.enabled = false;
//
//#if (APPLEMIDI_DEBUG)
//    LOGD("Freeing Session slot ");
//    LOGD(slot);
//#endif
//}
//
///*! \brief .
// */
//
//void RtpMidiPack_Class::DeleteSessions()
//{
//    for (int slot = 0; slot < MAX_SESSIONS; slot++)
//    {
//        Sessions[slot].ssrc = 0;
//        Sessions[slot].initiator = Undefined;
//        Sessions[slot].invite.status = None;
//        Sessions[slot].invite.status = None;
//        Sessions[slot].invite.attempts = 0;
//        Sessions[slot].invite.lastSend = 0;
//        Sessions[slot].syncronization.enabled = false;
//    }
//}
//
///*! \brief .
// */
//
//void RtpMidiPack_Class::DumpSession()
//{
//#if (APPLEMIDI_DEBUG)
//    for (int i = 0; i < MAX_SESSIONS; i++)
//    {
//        LOGD("Slot ");
//        LOGD(i);
//        LOGD(" ssrc = 0x");
//        LOGD(Sessions[i].ssrc);
//    }
//#endif
//}


/*! \brief .
 */

 uint32_t RtpMidiPack_Class::createInitiatorToken()
{
    static int counter = 0;
    return 0x12345000 + ++counter;
}

/*! \brief .
 */

 void RtpMidiPack_Class::ManageInvites()
{
    //	for (int i = 0; i < MAX_SESSIONS; i++)
    //	{
    //		Session_t* session = &Sessions[i];
    //
    //		if (session->invite.status == None)
    //		{
    //			// No invitation pending
    //		}
    //		else if (session->invite.status == SendControlInvite)
    //		{
    //			// Send invitation
    //			AppleMIDI_Invitation invitation;
    //			invitation.initiatorToken = createInitiatorToken();
    //			invitation.ssrc = _ssrc;
    //			strcpy(invitation.sessionName, getSessionName());
    //			write(_controlUDP, invitation, session->invite.remoteHost, session->invite.remotePort);
    //
    //			session->invite.initiatorToken = invitation.initiatorToken;
    //			session->invite.lastSend = millis();
    //			session->invite.attempts = 1;
    //			session->invite.status = WaitingForControlInvitationAccepted;
    //
    //#if (APPLEMIDI_DEBUG)
    //			LOGD("< Control Invitation: peer = \"");
    //			LOGD(invitation.sessionName);
    //			LOGD("\"");
    //			LOGD(" ,ssrc 0x");
    //			LOGD(invitation.ssrc, HEX);
    //			LOGD(" ,Attempt = ");
    //			LOGD(session->invite.attempts);
    //			LOGD(" ,initiatorToken = 0x");
    //			LOGD(invitation.initiatorToken, HEX);
    //			LOGD();
    //#endif
    //		}
    //		else if (session->invite.status == WaitingForControlInvitationAccepted)
    //		{
    //			if (session->invite.lastSend + 1000 < millis())
    //			{
    //				// If no response received after 1 second, send invitation again
    //				// with a maximum of 10 times.
    //
    //				if (session->invite.attempts >= 10) // Max attempts
    //				{
    //					DeleteSession(i); // give up
    //					return;
    //				}
    //
    //				// Send invitation
    //				AppleMIDI_Invitation invitation;
    //				invitation.initiatorToken = session->invite.initiatorToken;
    //				invitation.ssrc = _ssrc;
    //				strcpy(invitation.sessionName, getSessionName());
    //				write(_controlUDP, invitation, session->invite.remoteHost, session->invite.remotePort);
    //
    //				session->invite.lastSend = millis();
    //				session->invite.attempts++;
    //
    //#if (APPLEMIDI_DEBUG)
    //				LOGD("< Control Invitation: peer = \"");
    //				LOGD(invitation.sessionName);
    //				LOGD("\"");
    //				LOGD(" ,ssrc 0x");
    //				LOGD(invitation.ssrc, HEX);
    //				LOGD(" ,Attempt = ");
    //				LOGD(session->invite.attempts);
    //				LOGD(" ,initiatorToken = 0x");
    //				LOGD(invitation.initiatorToken, HEX);
    //				LOGD();
    //#endif
    //			}
    //		}
    //		else if (session->invite.status == SendContentInvite)
    //		{
    //			AppleMIDI_Invitation invitation;
    //			invitation.initiatorToken = session->invite.initiatorToken;
    //			invitation.ssrc = _ssrc;
    //			strcpy(invitation.sessionName, getSessionName());
    //			write(_contentUDP, invitation, session->invite.remoteHost, session->invite.remotePort + 1);
    //
    //			session->invite.lastSend = millis();
    //			session->invite.attempts = 1;
    //			session->invite.status = WaitingForContentInvitationAccepted;
    //
    //#if (APPLEMIDI_DEBUG)
    //			LOGD("< Content Invitation: peer = \"");
    //			LOGD(invitation.sessionName);
    //			LOGD("\"");
    //			LOGD(" ,ssrc 0x");
    //			LOGD(invitation.ssrc, HEX);
    //			LOGD(" ,Attempt = ");
    //			LOGD(session->invite.attempts);
    //			LOGD(" ,initiatorToken = 0x");
    //			LOGD(invitation.initiatorToken, HEX);
    //			LOGD();
    //#endif
    //		}
    //		else if (session->invite.status == WaitingForContentInvitationAccepted)
    //		{
    //			if (session->invite.lastSend + 1000 < millis())
    //			{
    //				// If no response received after 1 second, send invitation again
    //				// with a maximum of 10 times.
    //
    //				if (session->invite.attempts >= 10) // Max attempts
    //				{
    //					DeleteSession(session->invite.ssrc); // Give up
    //					return;
    //				}
    //
    //				AppleMIDI_Invitation invitation;
    //				invitation.initiatorToken = session->invite.initiatorToken;
    //				invitation.ssrc = _ssrc;
    //				strcpy(invitation.sessionName, getSessionName());
    //				write(_contentUDP, invitation, session->invite.remoteHost, session->invite.remotePort + 1);
    //
    //				session->invite.lastSend = millis();
    //				session->invite.attempts++;
    //
    //#if (APPLEMIDI_DEBUG)
    //				LOGD("< Content Invitation: peer = \"");
    //				LOGD(invitation.sessionName);
    //				LOGD("\"");
    //				LOGD(" ,ssrc 0x");
    //				LOGD(invitation.ssrc, HEX);
    //				LOGD(" ,Attempt = ");
    //				LOGD(session->invite.attempts);
    //				LOGD(" ,initiatorToken = 0x");
    //				LOGD(invitation.initiatorToken, HEX);
    //				LOGD();
    //#endif
    //			}
    //		}
    //	}
}

/*! \brief The initiator of the session polls if remote station is still alive.
 */

 void RtpMidiPack_Class::ManageTiming()
{
    //	for (int i = 0; i < MAX_SESSIONS; i++)
    //	{
    //		if (Sessions[i].initiator == Local && Sessions[i].syncronization.enabled)
    //		{
    //			if (!Sessions[i].syncronization.busy)
    //			{
    //				bool doSyncronize = false;
    //
    //				if (Sessions[i].syncronization.count < 2)
    //				{
    //					// immediately after last CK2
    //					doSyncronize = true;
    //				}
    //				else if (Sessions[i].syncronization.count < 10)
    //				{
    //					// every second after last CK2
    //					if (Sessions[i].syncronization.lastTime + 1000 < millis())
    //					{
    //						doSyncronize = true;
    //					}
    //				}
    //				else
    //				{
    //					// every 20 seconds after last CK2
    //					if (Sessions[i].syncronization.lastTime + 20000 < millis())
    //					{
    //						doSyncronize = true;
    //					}
    //				}
    //
    //				if (doSyncronize)
    //				{
    //					AppleMIDI_Syncronization synchronization;
    //					synchronization.timestamps[0] = _rtpMidiClock.Now();
    //					synchronization.timestamps[1] = 0;
    //					synchronization.timestamps[2] = 0;
    //					synchronization.count = 0;
    //
    //					AppleMIDI_Syncronization synchronizationResponse(_ssrc, synchronization.count, synchronization.timestamps);
    //					write(_contentUDP, synchronizationResponse, Sessions[i].contentIP, Sessions[i].contentPort);
    //
    //					Sessions[i].syncronization.busy = true;
    //
    //#if (APPLEMIDI_DEBUG)
    //					LOGD("< Syncronization for ssrc 0x");
    //					LOGD(synchronizationResponse.ssrc, HEX);
    //					LOGD(", count = ");
    //					LOGD(synchronizationResponse.count);
    //					LOGD(", to = ");
    //					LOGD(Sessions[i].contentIP);
    //					LOGD(" ");
    //					LOGD(Sessions[i].contentPort);
    //#if (APPLEMIDI_DEBUG_VERBOSE)
    //					//LOGD  (" Timestamps = ");
    //					//LOGD  (synchronizationResponse.timestamps[0], HEX);
    //					//LOGD  (" ");
    //					//LOGD  (synchronizationResponse.timestamps[1], HEX);
    //					//LOGD  (" ");
    //					//LOGD  (synchronizationResponse.timestamps[2], HEX);
    //#endif
    //					LOGD("");
    //#endif
    //				}
    //			}
    //			else
    //			{
    //			}
    //		}
    //	}
    //
    //	//if (_lastTimeSessionSyncronized + 30000 < millis())
    //	//{
    //	//	// Send synchronization
    //	//	AppleMIDI_Syncronization synchronization;
    //	//	synchronization.timestamps[0] = _rtpMidiClock.Now();
    //	//	synchronization.timestamps[1] = 0;
    //	//	synchronization.timestamps[2] = 0;
    //	//	synchronization.count = 0;
    //
    //	//	AppleMIDI_Syncronization synchronizationResponse(_ssrc, synchronization.count, synchronization.timestamps);
    //	//	synchronizationResponse.write(&_contentUDP);
    //	//}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------


// void RtpMidiPack_Class::write(UdpClass& udp, AppleMIDI_InvitationRejected& ir, IPAddress ip, uint16_t port)
//{
//	udp.beginPacket(ip, port);
//
//		udp.write(ir.signature, sizeof(ir.signature));
//		udp.write(ir.command, sizeof(ir.command));
//
//		// To appropriate endian conversion
//		uint32_t _version = AppleMIDI_Util::toEndian(ir.version);
//		uint32_t _initiatorToken = AppleMIDI_Util::toEndian(ir.initiatorToken);
//		uint32_t _ssrc = AppleMIDI_Util::toEndian(ir.ssrc);
//
//		// write then out
//		udp.write((uint8_t*) ((void*) (&_version)), sizeof(_version));
//		udp.write((uint8_t*) ((void*) (&_initiatorToken)), sizeof(_initiatorToken));
//		udp.write((uint8_t*) ((void*) (&_ssrc)), sizeof(_ssrc));
//
//		udp.write((uint8_t*) ir.sessionName, strlen(ir.sessionName) + 1);
//
//	udp.endPacket();
//	udp.flush();
//}
//
//
// void RtpMidiPack_Class::write(UdpClass& udp, AppleMIDI_InvitationAccepted& ia, IPAddress ip, uint16_t port)
//{
//	udp.beginPacket(ip, port);
//
//		udp.write(ia.signature, sizeof(ia.signature));
//		udp.write(ia.command, sizeof(ia.command));
//
//		// To appropriate endian conversion
//		uint32_t _version = AppleMIDI_Util::toEndian(ia.version);
//		uint32_t _initiatorToken = AppleMIDI_Util::toEndian(ia.initiatorToken);
//		uint32_t _ssrc = AppleMIDI_Util::toEndian(ia.ssrc);
//
//		// write then out
//		udp.write((uint8_t*) ((void*) (&_version)), sizeof(_version));
//		udp.write((uint8_t*) ((void*) (&_initiatorToken)), sizeof(_initiatorToken));
//		udp.write((uint8_t*) ((void*) (&_ssrc)), sizeof(_ssrc));
//
//		udp.write((uint8_t*) ia.name, strlen(ia.name) + 1);
//
//	udp.endPacket();
//	udp.flush();
//}
//
//
// void RtpMidiPack_Class::write(UdpClass& udp, AppleMIDI_Syncronization& sy, IPAddress ip, uint16_t port)
//{
//	udp.beginPacket(ip, port);
//
//	udp.write(sy.signature, sizeof(sy.signature));
//		udp.write(sy.command, sizeof(sy.command));
//
//		// To appropriate endian conversion
//		uint32_t _ssrc = AppleMIDI_Util::toEndian(sy.ssrc);
//		uint8_t _count = AppleMIDI_Util::toEndian(sy.count);
//		uint8_t _zero = 0;
//		int64_t _ts0 = AppleMIDI_Util::toEndian(sy.timestamps[0]);
//		int64_t _ts1 = AppleMIDI_Util::toEndian(sy.timestamps[1]);
//		int64_t _ts2 = AppleMIDI_Util::toEndian(sy.timestamps[2]);
//
//		// write then out
//		udp.write((uint8_t*) ((void*) (&_ssrc)), sizeof(_ssrc));
//		udp.write((uint8_t*) ((void*) (&_count)), sizeof(_count));
//		udp.write((uint8_t*) ((void*) (&_zero)), sizeof(_zero));
//		udp.write((uint8_t*) ((void*) (&_zero)), sizeof(_zero));
//		udp.write((uint8_t*) ((void*) (&_zero)), sizeof(_zero));
//		udp.write((uint8_t*) ((void*) (&_ts0)), sizeof(_ts0));
//		udp.write((uint8_t*) ((void*) (&_ts1)), sizeof(_ts1));
//		udp.write((uint8_t*) ((void*) (&_ts2)), sizeof(_ts2));
//
//	udp.endPacket();
//	udp.flush();
//}
//
//
// void RtpMidiPack_Class::write(UdpClass& udp, AppleMIDI_Invitation& in, IPAddress ip, uint16_t port)
//{
//	udp.beginPacket(ip, port);
//
//		udp.write(in.signature, sizeof(in.signature));
//		udp.write(in.command, sizeof(in.command));
//
//		// To appropriate endian conversion
//		uint32_t _version = AppleMIDI_Util::toEndian(in.version);
//		uint32_t _initiatorToken = AppleMIDI_Util::toEndian(in.initiatorToken);
//		uint32_t _ssrc = AppleMIDI_Util::toEndian(in.ssrc);
//
//		// write then out
//		udp.write((uint8_t*) ((void*) (&_version)), sizeof(_version));
//		udp.write((uint8_t*) ((void*) (&_initiatorToken)), sizeof(_initiatorToken));
//		udp.write((uint8_t*) ((void*) (&_ssrc)), sizeof(_ssrc));
//
//		udp.write((uint8_t*) in.sessionName, strlen(in.sessionName) + 1);
//
//	udp.endPacket();
//	udp.flush();
//}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

/*! \brief Generate and send a MIDI message from the values given.
 \param inType    The message type (see type defines for reference)
 \param inData1   The first data byte.
 \param inData2   The second data byte (if the message contains only 1 data byte,
 set this one to 0).
 \param inChannel The output channel on which the message will be sent
 (values from 1 to 16). Note: you cannot send to OMNI.

 This is an internal method, use it only if you need to send raw data
 from your code, at your own risks.
 */

 void RtpMidiPack_Class::send(MidiType inType, DataByte inData1, DataByte inData2, Channel inChannel)
{
 //   for (int i = 0; i < MAX_SESSIONS; i++)
  //  {
//            if (Sessions[i].ssrc != 0)
//        //		{
        internalSend(inType, inData1, inData2, inChannel);
        //		}
  //  }

    return;
}


 void RtpMidiPack_Class::send(MidiType inType, DataByte inData1, DataByte inData2)
{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//    {
//        if (Sessions[i].ssrc != 0)
//        {
            internalSend(/*Sessions[i], */inType, inData1, inData2);
//        }
//    }

    return;
}


 void RtpMidiPack_Class::send(MidiType inType, DataByte inData)
{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//    {
//        if (Sessions[i].ssrc != 0)
//        {
            internalSend(/*Sessions[i], */inType, inData);
//        }
//    }

    return;
}


 void RtpMidiPack_Class::send(MidiType inType)
{
//    for (int i = 0; i < MAX_SESSIONS; i++)
//    {
//        if (Sessions[i].ssrc != 0)
//        {
            internalSend(inType);
//        }
//    }

    return;
}

/*! \brief Generate and send a MIDI message from the values given.
 \param inType    The message type (see type defines for reference)
 \param inData1   The first data byte.
 \param inData2   The second data byte (if the message contains only 1 data byte,
 set this one to 0).
 \param inChannel The output channel on which the message will be sent
 (values from 1 to 16). Note: you cannot send to OMNI.

 This is an internal method, use it only if you need to send raw data
 from your code, at your own risks.
 */

 void RtpMidiPack_Class::internalSend(MidiType inType, DataByte inData1, DataByte inData2, Channel inChannel)
{
    // Then test if channel is valid
    if (inChannel >= MIDI_CHANNEL_OFF ||
        inChannel == MIDI_CHANNEL_OMNI ||
        inType < NoteOff)
    {

#if APPLEMIDI_USE_RUNNING_STATUS
        mRunningStatus_TX = InvalidType;
#endif
        return; // Don't send anything
    }

    if (inType <= PitchBend)  // Channel messages
    {
        // Protection: remove MSBs on data
        inData1 &= 0x7F;
        inData2 &= 0x7F;

        _rtpMidi.sequenceNr++;
        _rtpMidi.timestamp = _rtpMidiClock.Now();
        _rtpMidi.beginWrite();

        // Length
        uint8_t length = 3;
        _rtpMidi.write(&length, 1);

        // Command Section

        const StatusByte status = getStatus(inType, inChannel);

#if APPLEMIDI_USE_RUNNING_STATUS
        // Check Running Status
        if (mRunningStatus_TX != status)
        {
            // New message, memorise and send header
            mRunningStatus_TX = status;
            _rtpMidi.write(&mRunningStatus_TX, 1);
        }
#else
        // Don't care about running status, send the status byte.
        _rtpMidi.write((void*)&status, sizeof(status));
#endif

        // Then send data
        _rtpMidi.write(&inData1, sizeof(inData1));
        if (inType != ProgramChange && inType != AfterTouchChannel)
            _rtpMidi.write(&inData2, sizeof(inData2));

        _rtpMidi.endWrite();

        return;
    }
    else if (inType >= TuneRequest && inType <= SystemReset)
        internalSend(inType); // System Real-time and 1 byte.

}


 void RtpMidiPack_Class::internalSend(MidiType inType)
{
    _rtpMidi.sequenceNr++;
    //	_rtpMidi.timestamp =
    _rtpMidi.beginWrite(/*_contentUDP, session.contentIP, session.contentPort*/);

    uint8_t length = 1;
    _rtpMidi.write(&length, 1);

    byte octet = (byte) inType;

    switch (inType)
    {
        case TuneRequest: // Not really real-time, but one byte anyway.
        case Clock:
        case Start:
        case Stop:
        case Continue:
        case ActiveSensing:
        case SystemReset:
            _rtpMidi.write(&octet, 1);
            break;
        default:
            // Invalid Real Time marker
            break;
    }

    _rtpMidi.endWrite();

    // Do not cancel Running Status for real-time messages as they can be
    // interleaved within any message. Though, TuneRequest can be sent here,
    // and as it is a System Common message, it must reset Running Status.
#if APPLEMIDI_USE_RUNNING_STATUS
    if (inType == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}


 void RtpMidiPack_Class::internalSend(/*Session_t& session, */MidiType inType, DataByte inData)
{
    _rtpMidi.sequenceNr++;
    //	_rtpMidi.timestamp =
    _rtpMidi.beginWrite(/*_contentUDP, session.contentIP, session.contentPort*/);

    uint8_t length = 2;
    _rtpMidi.write(&length, 1);

    DataByte octet = (DataByte) inType;

    switch (inType)
    {
        case TimeCodeQuarterFrame: // Not really real-time, but one byte anyway.
            _rtpMidi.write(&octet, 1);
            _rtpMidi.write(&inData, 1);
            break;
        default:
            // Invalid Real Time marker
            break;
    }

    _rtpMidi.endWrite();

    // Do not cancel Running Status for real-time messages as they can be
    // interleaved within any message. Though, TuneRequest can be sent here,
    // and as it is a System Common message, it must reset Running Status.
#if APPLEMIDI_USE_RUNNING_STATUS
    if (inType == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}

 void RtpMidiPack_Class::internalSend(/*Session_t& session, */MidiType inType, DataByte inData1, DataByte inData2)
{
    _rtpMidi.sequenceNr++;
    //	_rtpMidi.timestamp =
    _rtpMidi.beginWrite(/*_contentUDP, session.contentIP, session.contentPort*/);

    uint8_t length = 3;
    _rtpMidi.write(&length, 1);

    DataByte octet = (DataByte) inType;

    switch (inType)
    {
        case SongPosition: // Not really real-time, but one byte anyway.
            _rtpMidi.write(&octet, 1);
            _rtpMidi.write(&inData1, 1);
            _rtpMidi.write(&inData2, 1);
            break;
        default:
            // Invalid Real Time marker
            break;
    }

    _rtpMidi.endWrite();

    // Do not cancel Running Status for real-time messages as they can be
    // interleaved within any message. Though, TuneRequest can be sent here,
    // and as it is a System Common message, it must reset Running Status.
#if APPLEMIDI_USE_RUNNING_STATUS
    if (inType == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}


 StatusByte RtpMidiPack_Class::getStatus(MidiType inType, Channel inChannel) const
{
    return ((byte) inType | ((inChannel - 1) & 0x0F));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

/*! \brief Send a Note On message
 \param inNoteNumber  Pitch value in the MIDI format (0 to 127).
 \param inVelocity    Note attack velocity (0 to 127). A NoteOn with 0 velocity
 is considered as a NoteOff.
 \param inChannel     The channel on which the message will be sent (1 to 16).

 Take a look at the values, names and frequencies of notes here:
 http://www.phys.unsw.edu.au/jw/notes.html
 */

void RtpMidiPack_Class::noteOn(DataByte inNoteNumber, DataByte inVelocity, Channel  inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("< Note On (c=");
    LOGD(inChannel);
    LOGD(", n=");
    LOGD(inNoteNumber);
    LOGD(", v=");
    LOGD(inVelocity);
    LOGD(")");
#endif

//#if APPLEMIDI_USE_EVENTS
    if (mNoteOnSendingEvent != 0)
        mNoteOnSendingEvent(inChannel, inNoteNumber, inVelocity);
//#endif

    send(NoteOn, inNoteNumber, inVelocity, inChannel);

//#if APPLEMIDI_USE_EVENTS
    if (mNoteOnSendEvent != 0)
        mNoteOnSendEvent(inChannel, inNoteNumber, inVelocity);
//#endif
}

/*! \brief Send a Note Off message
 \param inNoteNumber  Pitch value in the MIDI format (0 to 127).
 \param inVelocity    Release velocity (0 to 127).
 \param inChannel     The channel on which the message will be sent (1 to 16).

 Note: you can send NoteOn with zero velocity to make a NoteOff, this is based
 on the Running Status principle, to avoid sending status messages and thus
 sending only NoteOn data. This method will always send a real NoteOff message.
 Take a look at the values, names and frequencies of notes here:
 http://www.phys.unsw.edu.au/jw/notes.html
 */

 void RtpMidiPack_Class::noteOff(DataByte inNoteNumber, DataByte inVelocity, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("< Note Off (c=");
    LOGD(inChannel);
    LOGD(", n=");
    LOGD(inNoteNumber);
    LOGD(", v=");
    LOGD(inVelocity);
    LOGD(")");
#endif

//#if APPLEMIDI_USE_EVENTS
    if (mNoteOffSendingEvent != 0)
        mNoteOffSendingEvent(inChannel, inNoteNumber, inVelocity);
//#endif

    send(NoteOff, inNoteNumber, inVelocity, inChannel);

//#if APPLEMIDI_USE_EVENTS
    if (mNoteOffSendEvent != 0)
        mNoteOffSendEvent(inChannel, inNoteNumber, inVelocity);
//#endif
}

/*! \brief Send a Program Change message
 \param inProgramNumber The Program to select (0 to 127).
 \param inChannel       The channel on which the message will be sent (1 to 16).
 */

 void RtpMidiPack_Class::programChange(DataByte inProgramNumber, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("sendProgramChange ProgramNumber:");
    LOGD(inProgramNumber);
    LOGD(" Channel:");
    LOGD(inChannel);
#endif

    send(ProgramChange, inProgramNumber, 0, inChannel);
}

/*! \brief Send a Control Change message
 \param inControlNumber The controller number (0 to 127).
 \param inControlValue  The value for the specified controller (0 to 127).
 \param inChannel       The channel on which the message will be sent (1 to 16).
 @see MidiControlChangeNumber
 */

 void RtpMidiPack_Class::controlChange(DataByte inControlNumber, DataByte inControlValue, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("sendControlChange Number:");
    LOGD(inControlNumber);
    LOGD(" Value:");
    LOGD(inControlValue);
    LOGD(" Channel:");
    LOGD(inChannel);
#endif

    send(ControlChange, inControlNumber, inControlValue, inChannel);
}

/*! \brief Send a Polyphonic AfterTouch message (applies to a specified note)
 \param inNoteNumber  The note to apply AfterTouch to (0 to 127).
 \param inPressure    The amount of AfterTouch to apply (0 to 127).
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */

 void RtpMidiPack_Class::polyPressure(DataByte inNoteNumber, DataByte inPressure, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("sendPolyPressure Note:");
    LOGD(inNoteNumber);
    LOGD(" Pressure:");
    LOGD(inPressure);
    LOGD(" Channel:");
    LOGD(inChannel);
#endif

    send(AfterTouchPoly, inNoteNumber, inPressure, inChannel);
}

/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param inPressure    The amount of AfterTouch to apply to all notes.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */

 void RtpMidiPack_Class::afterTouch(DataByte inPressure, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("sendafterTouch ");
    LOGD(" Pressure:");
    LOGD(inPressure);
    LOGD(" Channel:");
    LOGD(inChannel);
#endif

    send(AfterTouchChannel, inPressure, 0, inChannel);
}

/*! \brief Send a Pitch Bend message using a signed integer value.
 \param inPitchValue  The amount of bend to send (in a signed integer format),
 between MIDI_PITCHBEND_MIN and MIDI_PITCHBEND_MAX,
 center value is 0.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */

 void RtpMidiPack_Class::pitchBend(int inPitchValue, Channel inChannel)
{
#if (APPLEMIDI_DEBUG)
    LOGD("pitchBend ");
    LOGD(" Pitch:");
    LOGD(inPitchValue);
    LOGD(" Channel:");
    LOGD(inChannel);
#endif

    const unsigned int bend = inPitchValue - MIDI_PITCHBEND_MIN;
    send(PitchBend, (bend & 0x7F), (bend >> 7) & 0x7F, inChannel);
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param inPitchValue  The amount of bend to send (in a floating point format),
 between -1.0f (maximum downwards bend)
 and +1.0f (max upwards bend), center value is 0.0f.
 \param inChannel     The channel on which the message will be sent (1 to 16).
 */

 void RtpMidiPack_Class::pitchBend(double inPitchValue, Channel inChannel)
{
    const int value = inPitchValue * MIDI_PITCHBEND_MAX;
    pitchBend(value, inChannel);
}

/*! \brief Generate and send a System Exclusive frame.
 \param inLength  The size of the array to send
 \param inArray   The byte array containing the data to send
 \param inArrayContainsBoundaries When set to 'true', 0xF0 & 0xF7 bytes
 (start & stop SysEx) will NOT be sent
 (and therefore must be included in the array).
 default value for ArrayContainsBoundaries is set to 'false' for compatibility
 with previous versions of the library.
 */

 void RtpMidiPack_Class::sysEx(unsigned int inLength, const byte* inArray, 	bool inArrayContainsBoundaries)
{
    // USE SEND!!!!!


    _rtpMidi.sequenceNr++;
    //	_rtpMidi.timestamp =
    //	_rtpMidi.beginWrite(_contentUDP, session.contentIP, session.contentPort);

    uint8_t length = inLength + 1 + ((inArrayContainsBoundaries) ? 0 : 2);
    _rtpMidi.write(&length, 1);

    uint8_t type = SystemExclusive;
    _rtpMidi.write(&type, 1);

    if (!inArrayContainsBoundaries)
    {
        uint8_t octet = 0xF0;
        _rtpMidi.write(&octet, 1);
    }

    _rtpMidi.write((void*)inArray, inLength);

    if (!inArrayContainsBoundaries)
    {
        uint8_t octet = 0xF7;
        _rtpMidi.write(&octet, 1);
    }

    _rtpMidi.endWrite();

    // Do not cancel Running Status for real-time messages as they can be
    // interleaved within any message. Though, TuneRequest can be sent here,
    // and as it is a System Common message, it must reset Running Status.
#if APPLEMIDI_USE_RUNNING_STATUS
    if (inType == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
}

/*! \brief Send a Tune Request message.

 When a MIDI unit receives this message,
 it should tune its oscillators (if equipped with any).
 */

 void RtpMidiPack_Class::tuneRequest()
{
    send(TuneRequest);
}

/*! \brief .
 A device sends out an Active Sense message (at least once) every 300 milliseconds
 if there has been no other activity on the MIDI buss, to let other devices know
 that there is still a good MIDI connection between the devices.

 When a device receives an Active Sense message (from some other device), it should
 expect to receive additional Active Sense messages at a rate of one approximately
 every 300 milliseconds, whenever there is no activity on the MIDI buss during that
 time. (Of course, if there are other MIDI messages happening at least once every 300
 mSec, then Active Sense won't ever be sent. An Active Sense only gets sent if there
 is a 300 mSec "moment of silence" on the MIDI buss. You could say that a device that
 sends out Active Sense "gets nervous" if it has nothing to do for over 300 mSec, and
 so sends an Active Sense just for the sake of reassuring other devices that this device
 still exists). If a message is missed (ie, 0xFE nor any other MIDI message is received
 for over 300 mSec), then a device assumes that the MIDI connection is broken, and
 turns off all of its playing notes (which were turned on by incoming Note On messages,
 versus ones played on the local keyboard by a musician). Of course, if a device never
 receives an Active Sense message to begin with, it should not expect them at all. So,
 it takes one "nervous" device to start the process by initially sending out an Active
 Sense message to the other connected devices during a 300 mSec moment of silence
 on the MIDI bus.

 (http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec/sense.htm)
 */

 void RtpMidiPack_Class::activeSensing()
{
    send(ActiveSensing);
}

/*! \brief
 */

 void RtpMidiPack_Class::start()
{
    send(Start);
}

/*! \brief
 */

 void RtpMidiPack_Class::_continue()
{
    send(Continue);
}

/*! \brief
 */

 void RtpMidiPack_Class::stop()
{
    send(Stop);
}

/*! \brief Send a MIDI Time Code Quarter Frame.

 \param inTypeNibble      MTC type
 \param inValuesNibble    MTC data
 See MIDI Specification for more information.
 */

 void RtpMidiPack_Class::timeCodeQuarterFrame(DataByte inTypeNibble, DataByte inValuesNibble)
{
    const byte data = (((inTypeNibble & 0x07) << 4) | (inValuesNibble & 0x0F));
    timeCodeQuarterFrame(data);
}

/*! \brief Send a MIDI Time Code Quarter Frame.

 See MIDI Specification for more information.
 \param inData  if you want to encode directly the nibbles in your program,
 you can send the byte here.
 */

 void RtpMidiPack_Class::timeCodeQuarterFrame(DataByte inData)
{
    send(TimeCodeQuarterFrame, inData);
}

/*! \brief Send a Song Position Pointer message.
 \param inBeats    The number of beats since the start of the song.
 */

 void RtpMidiPack_Class::songPosition(unsigned int inBeats)
{
    byte octet1 = inBeats & 0x7F;
    byte octet2 = (inBeats >> 7) & 0x7F;

    send(SongPosition, octet1, octet2);
}

/*! \brief Send a Song Select message */

 void RtpMidiPack_Class::songSelect(DataByte inSongNumber)
{
    byte octet = inSongNumber & 0x7F;

    send(SongSelect, octet);
}


/*! \brief Send a Song Select message */

 void RtpMidiPack_Class::systemReset()
{
    send(SystemReset);
}

/*! \brief Send a Song Select message */

 void RtpMidiPack_Class::clock()
{
    send(Clock);
}

/*! \brief Send a Song Select message */

 void RtpMidiPack_Class::tick()
{
    send(Tick);
}


// ------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------


 void RtpMidiPack_Class::OnConnected(void(*fptr)(uint32_t, char*))    { mConnectedCallback = fptr; }
 void RtpMidiPack_Class::OnDisconnected(void(*fptr)(uint32_t))      { mDisconnectedCallback = fptr; }

 void RtpMidiPack_Class::OnReceiveNoteOff(void(*fptr)(byte channel, byte note, byte velocity))          { mNoteOffCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveNoteOn(void(*fptr)(byte channel, byte note, byte velocity))           { mNoteOnCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveAfterTouchPoly(void(*fptr)(byte channel, byte note, byte pressure))   { mAfterTouchPolyCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveControlChange(void(*fptr)(byte channel, byte number, byte value))     { mControlChangeCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveProgramChange(void(*fptr)(byte channel, byte number))                 { mProgramChangeCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveAfterTouchChannel(void(*fptr)(byte channel, byte pressure))           { mAfterTouchChannelCallback = fptr; }
 void RtpMidiPack_Class::OnReceivePitchBend(void(*fptr)(byte channel, int bend))                        { mPitchBendCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveSystemExclusive(void(*fptr)(byte* array, byte size))                  { mSystemExclusiveCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveTimeCodeQuarterFrame(void(*fptr)(byte data))                          { mTimeCodeQuarterFrameCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveSongPosition(void(*fptr)(unsigned int beats))                         { mSongPositionCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveSongSelect(void(*fptr)(byte songnumber))                              { mSongSelectCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveTuneRequest(void(*fptr)(void))                                        { mTuneRequestCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveClock(void(*fptr)(void))                                              { mClockCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveStart(void(*fptr)(void))                                              { mStartCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveContinue(void(*fptr)(void))                                           { mContinueCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveStop(void(*fptr)(void))                                               { mStopCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveActiveSensing(void(*fptr)(void))                                      { mActiveSensingCallback = fptr; }
 void RtpMidiPack_Class::OnReceiveSystemReset(void(*fptr)(void))                                        { mSystemResetCallback = fptr; }
