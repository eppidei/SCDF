

#import <UIKit/UIKit.h>
#import <CoreMIDI/CoreMIDI.h>
#include "MidiOutConnection.h"
#include "MidiOutConnectionIOS.h"




@class SCDF_Midi;
@class SCDF_MidiSource;

extern NSString * const SCDF_MidiSourceAddedNotification;
extern NSString * const SCDF_MidiSourceRemovedNotification;
extern NSString * const SCDF_MidiDestinationAddedNotification;
extern NSString * const SCDF_MidiDestinationRemovedNotification;
extern NSString * const SCDF_MidiConnectionKey;




/// Represents a source/destination for MIDI data
///
/// @see SCDF_MidiSource
/// @see SCDF_MidiDestination
@interface SCDF_MidiConnection : NSObject
{
    SCDF_Midi          *midi;
    MIDIEndpointRef  endpoint;
    NSString        *name;
    BOOL             isNetworkSession;
    
}
@property (nonatomic,readonly) SCDF_Midi          *midi;
@property (nonatomic,readonly) MIDIEndpointRef  endpoint;
@property (nonatomic,readonly) NSString        *name;
@property (nonatomic,readonly) BOOL             isNetworkSession;
@end


/// Delegate protocol for SCDF_MidiSource class.
/// Adopt this protocol in your object to receive MIDI events
///
/// IMPORTANT NOTE:
/// MIDI input is received from a high priority background thread
///
/// @see SCDF_MidiSource
@protocol SCDF_MidiSourceDelegate

// Raised on main run loop
/// NOTE: Raised on high-priority background thread.
///
/// To do anything UI-ish, you must forward the event to the main runloop
/// (e.g. use performSelectorOnMainThread:withObject:waitUntilDone:)
///
/// Be careful about autoreleasing objects here - there is no NSAutoReleasePool.
///
/// Handle the data like this:
///
///     // for some function HandlePacketData(Byte *data, UInt16 length)
///     const MIDIPacket *packet = &packetList->packet[0];
///     for (int i = 0; i < packetList->numPackets; ++i)
///     {
///         HandlePacketData(packet->data, packet->length);
///         packet = MIDIPacketNext(packet);
///     }
- (void) midiSource:(SCDF_MidiSource*)input midiReceived:(const MIDIPacketList *)packetList;

@end

/// Represents a source of MIDI data identified by CoreMIDI
///
/// @see SCDF_MidiSourceDelegate
@interface SCDF_MidiSource : SCDF_MidiConnection
- (void)addDelegate:(id<SCDF_MidiSourceDelegate>)delegate;
- (void)removeDelegate:(id<SCDF_MidiSourceDelegate>)delegate;

@property (strong, nonatomic, readonly) NSArray *delegates;
@end

//==============================================================================

/// Represents a destination for MIDI data identified by CoreMIDI
@interface SCDF_MidiDestination : SCDF_MidiConnection
{
}
- (void) flushOutput;
- (void) sendBytes:(const UInt8*)bytes size:(UInt32)size atPort:(int) _midiOutputPortIndex;
- (void) sendPacketList:(MIDIPacketList *)packetList atPort:(int) _midiOutputPortIndex;
@end

//==============================================================================

/// Delegate protocol for SCDF_Midi class.
///
/// @see SCDF_Midi
@protocol SCDF_MidiDelegate
- (void) midi:(SCDF_Midi*)midi sourceAdded:(SCDF_MidiSource *)source;
- (void) midi:(SCDF_Midi*)midi sourceRemoved:(SCDF_MidiSource *)source;
- (void) midi:(SCDF_Midi*)midi destinationAdded:(SCDF_MidiDestination *)destination;
- (void) midi:(SCDF_Midi*)midi destinationRemoved:(SCDF_MidiDestination *)destination;
@end

/// Class for receiving MIDI input from any MIDI device.
///
/// If you intend your app to support iOS 3.x which does not have CoreMIDI
/// support, weak link to the CoreMIDI framework, and only create a
/// SCDF_Midi object if you are running the right version of iOS.
///
/// @see SCDF_MidiDelegate



@interface SCDF_Midi : NSObject
{
    MIDIClientRef      client;
    MIDIPortRef        outputPort;
    MIDIPortRef        inputPort;
    NSString          *virtualEndpointName;
    MIDIEndpointRef    virtualSourceEndpoint;
    MIDIEndpointRef    virtualDestinationEndpoint;
    SCDF_MidiSource      *virtualDestinationSource;
    SCDF_MidiDestination *virtualSourceDestination;
    NSMutableArray    *sources, *destinations;
    
    
    std::map<std::string, std::vector<Scdf::MidiOutConnection*>> listenersMap;
}

+ (BOOL)midiAvailable;



@property (nonatomic,assign)   id<SCDF_MidiDelegate> delegate;
@property (nonatomic,readonly) NSUInteger         numberOfConnections;
@property (nonatomic,readonly) NSMutableArray    *sources;
@property (nonatomic,readonly) NSMutableArray    *destinations;
@property (nonatomic,readonly) SCDF_MidiSource      *virtualDestinationSource;
@property (nonatomic,readonly) SCDF_MidiDestination *virtualSourceDestination;
@property (nonatomic,retain)   NSString          *virtualEndpointName;
@property (nonatomic,assign)   BOOL               networkEnabled;
@property (nonatomic,assign)   BOOL               virtualSourceEnabled;
@property (nonatomic,assign)   BOOL               virtualDestinationEnabled;

/// Send a MIDI byte stream to every connected MIDI port
- (void) sendBytes:(const UInt8*)bytes size:(UInt32)size atPort:(int) _midiOutputPortIndex;
- (void) sendPacketList:(const MIDIPacketList *)packetList atPort:(int) _midiOutputPortIndex;
- (NSString *) getNameOfEndpointAtIndex: (int) index;
- (void) attachListenerAtIndex: (int) index forConnection: (Scdf::MidiOutConnection*) connection;
- (void) detachListenerAtIndex: (int) index;
- (void) onEndPointRemoved: (MIDIEndpointRef) endpoint;
- (void) connectSourceEndopointAtIndex: (int) endpointIndex;
- (void) disConnectSourceEndopointAtIndex: (int) endpointIndex;

@end
