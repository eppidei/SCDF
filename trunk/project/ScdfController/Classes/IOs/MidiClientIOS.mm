
#import "MidiClientIOS.h"
#import <mach/mach_time.h>
#import "Label.h"
#import "MidiInConnection.h"



void showTestAlert(std::string message )
{
#ifdef _DEBUG
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"DEBUG"
                                                    message:[NSString stringWithUTF8String:message.c_str()]
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
    [alert show];
#endif
}


// For some reason, this is nut pulled in by the umbrella header
#import <CoreMIDI/MIDINetworkSession.h>

/// A helper that NSLogs an error message if "c" is an error code
#define NSLogError(c,str) do{if (c) NSLog(@"Error (%@): %ld:%@", str, (long)c,[NSError errorWithDomain:NSMachErrorDomain code:c userInfo:nil]);}while(false)

NSString * const SCDF_MidiSourceAddedNotification        = @"SCDF_MidiSourceAddedNotification";
NSString * const SCDF_MidiSourceRemovedNotification      = @"SCDF_MidiSourceRemovedNotification";
NSString * const SCDF_MidiDestinationAddedNotification   = @"SCDF_MidiDestinationAddedNotification";
NSString * const SCDF_MidiDestinationRemovedNotification = @"SCDF_MidiDestinationRemovedNotification";
NSString * const SCDF_MidiConnectionKey                  = @"connection";

//==============================================================================

static void SCDF_MIDINotifyProc(const MIDINotification *message, void *refCon);
static void SCDF_MIDIReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);
static void SCDF_MIDIVirtualDestinationReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);

@interface SCDF_Midi ()
- (void) scanExistingDevices;
- (MIDIPortRef) outputPort;

@property (nonatomic, retain) NSTimer *rescanTimer;
@end

#define SCDF__RELEASE(a) [a release]; a = nil;

//==============================================================================

static
NSString *NameOfEndpoint(MIDIEndpointRef ref)
{
    CFStringRef string = nil;
    OSStatus s = MIDIObjectGetStringProperty(ref, kMIDIPropertyDisplayName, ( CFStringRef*)&string);
    if ( s != noErr )
    {
        return @"Unknown name";
    }
    return (NSString*)CFBridgingRelease(string);
}



static
BOOL IsNetworkSession(MIDIEndpointRef ref)
{
    MIDIEntityRef entity = 0;
    MIDIEndpointGetEntity(ref, &entity);
    
    BOOL hasMidiRtpKey = NO;
    CFPropertyListRef properties = nil;
    OSStatus s = MIDIObjectGetProperties(entity, &properties, true);
    if (!s)
    {
        NSDictionary *dictionary = (NSDictionary*)(properties);
        hasMidiRtpKey = [dictionary valueForKey:@"apple.midirtp.session"] != nil;
        CFRelease(properties);
    }
    
    return hasMidiRtpKey;
}

//==============================================================================

@implementation SCDF_MidiConnection

@synthesize midi;
@synthesize endpoint;
@synthesize name;
@synthesize isNetworkSession;

- (id) initWithMidi:(SCDF_Midi*)m endpoint:(MIDIEndpointRef)e
{
    if ((self = [super init]))
    {
        midi                = m;
        endpoint            = e;
#if ! SCDF_MIDI_ARC
        name                = [NameOfEndpoint(e) retain];
#else
        name                = NameOfEndpoint(e);
#endif
        isNetworkSession    = IsNetworkSession(e);
    }
    return self;
}

@end

//==============================================================================

@interface SCDF_MidiSource ()
@property (strong, readwrite) NSArray *delegates;
@end

@implementation SCDF_MidiSource

- (id) initWithMidi:(SCDF_Midi*)m endpoint:(MIDIEndpointRef)e
{
    if ((self = [super initWithMidi:m endpoint:e]))
    {
        self.delegates = [NSArray array];
    }
    return self;
}

#if ! SCDF_MIDI_ARC
- (void)dealloc
{
    self.delegates = nil;
    [super dealloc];
}
#endif

- (void)addDelegate:(id<SCDF_MidiSourceDelegate>)delegate
{
    if (![_delegates containsObject:[NSValue valueWithPointer:(void*)delegate]])
    {
        self.delegates = [_delegates arrayByAddingObject:[NSValue valueWithPointer:(void*)delegate] /* avoid retain loop */];
    }
}

- (void)removeDelegate:(id<SCDF_MidiSourceDelegate>)delegate
{
    NSMutableArray *mutableDelegates = [_delegates mutableCopy];
    [mutableDelegates removeObject:[NSValue valueWithPointer:(void*)delegate]];
    self.delegates = mutableDelegates;
}

// NOTE: Called on a separate high-priority thread, not the main runloop
- (void) midiRead:(const MIDIPacketList *)pktlist
{
    NSArray *delegates = self.delegates;
    for (NSValue *delegatePtr in delegates)
    {
        id<SCDF_MidiSourceDelegate> delegate = (id<SCDF_MidiSourceDelegate>)[delegatePtr pointerValue];
        [delegate midiSource:self midiReceived:pktlist];
    }
}

NSString *StringFromPacket(const MIDIPacket *packet)
{
    // Note - this is not an example of MIDI parsing. I'm just dumping
    // some bytes for diagnostics.
    // See comments in PGMidiSourceDelegate for an example of how to
    // interpret the MIDIPacket structure.
    return [NSString stringWithFormat:@"  %u bytes: [%02x,%02x,%02x]",
            packet->length,
            (packet->length > 0) ? packet->data[0] : 0,
            (packet->length > 1) ? packet->data[1] : 0,
            (packet->length > 2) ? packet->data[2] : 0
            ];
}

static
void SCDF_MIDIReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
{
    const MIDIPacket *packet = &pktlist->packet[0];
    for (int i = 0; i < pktlist->numPackets; ++i) {
       
        Scdf::MidiInData data;
        
        (packet->length > 0) ? data.statusByte = packet->data[0] :data.statusByte=0;
        (packet->length > 1) ? data.dataByte1 =packet->data[1] : data.dataByte1=0;
        (packet->length > 2) ? data.dataByte2 =packet->data[2] : data.dataByte2=0;
        
        Scdf::MidiInConnection::NotifyReceiveMidiData(data);
       
        packet = MIDIPacketNext(packet);
    }
    
}

static
void SCDF_MIDIVirtualDestinationReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
{
    SCDF_Midi *midi = (__bridge SCDF_Midi*)readProcRefCon;
    SCDF_MidiSource *self = midi.virtualDestinationSource;
    [self midiRead:pktlist];
}

@end

//==============================================================================

@implementation SCDF_MidiDestination

- (id) initWithMidi:(SCDF_Midi*)m endpoint:(MIDIEndpointRef)e
{
    if ((self = [super initWithMidi:m endpoint:e]))
    {
        midi     = m;
        endpoint = e;
    }
    return self;
}

-(void)flushOutput
{
    MIDIFlushOutput(endpoint);
}

- (void) sendBytes:(const UInt8*)bytes size:(UInt32)size atPort:(int) _midiOutputPortIndex
{
    assert(size < 65536);
    Byte packetBuffer[size+100];
    MIDIPacketList *packetList = (MIDIPacketList*)packetBuffer;
    MIDIPacket     *packet     = MIDIPacketListInit(packetList);
    packet = MIDIPacketListAdd(packetList, sizeof(packetBuffer), packet, 0, size, bytes);
    
    [self sendPacketList:packetList atPort:_midiOutputPortIndex];
}

- (void) sendPacketList:(MIDIPacketList *)packetList atPort:(int) _midiOutputPortIndex
{
    // Send it
    OSStatus s = MIDISend(midi.outputPort, endpoint, packetList);
    NSLogError(s, @"Sending MIDI");
}

@end

//==============================================================================

@interface SCDF_MidiVirtualSourceDestination : SCDF_MidiDestination
@end

@implementation SCDF_MidiVirtualSourceDestination

- (void) sendPacketList:(MIDIPacketList *)packetList atPort:(int) _midiOutputPortIndex
{
    // Assign proper timestamps to packetList
    MIDIPacket *packet = &packetList->packet[0];
    for (int i = 0; i < packetList->numPackets; i++)
    {
        if ( packet->timeStamp == 0 )
        {
            packet->timeStamp = mach_absolute_time();
        }
        packet = MIDIPacketNext(packet);
    }
    
    // Send it
    OSStatus s = MIDIReceived(endpoint, packetList);
    NSLogError(s, @"Sending MIDI");
}

@end

//==============================================================================

@implementation SCDF_Midi

@synthesize delegate;
@synthesize sources,destinations;
@synthesize virtualSourceDestination,virtualDestinationSource,virtualEndpointName;
@dynamic    networkEnabled;

+ (BOOL)midiAvailable
{
    return [[[UIDevice currentDevice] systemVersion] floatValue] >= 4.2;
}

- (NSString *) getNameOfEndpointAtIndex: (int) index
{
    return NameOfEndpoint(MIDIGetDestination(index));;
}

- (id) init
{
    if ((self = [super init]))
    {
        sources      = [NSMutableArray new];
        destinations = [NSMutableArray new];
        
        OSStatus s = MIDIClientCreate((CFStringRef)@"SCDF_Midi MIDI Client", SCDF_MIDINotifyProc, (__bridge void*)self, &client);
        NSLogError(s, @"Create MIDI client");
        
        s = MIDIOutputPortCreate(client, (CFStringRef)@"SCDF_Midi Output Port", &outputPort);
        NSLogError(s, @"Create output MIDI port");
        
        s = MIDIInputPortCreate(client, (CFStringRef)@"SCDF_Midi Input Port", SCDF_MIDIReadProc, (void *)(self), &inputPort);
        NSLogError(s, @"Create input MIDI port");
        
        [self scanExistingDevices];
        
        self.rescanTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(scanExistingDevices) userInfo:nil repeats:YES];
    }
    
    return self;
}

- (void) dealloc
{
    [_rescanTimer invalidate];
    self.rescanTimer = nil;
    
    if (outputPort)
    {
        OSStatus s = MIDIPortDispose(outputPort);
        NSLogError(s, @"Dispose MIDI port");
    }
    
    if (inputPort)
    {
        OSStatus s = MIDIPortDispose(inputPort);
        NSLogError(s, @"Dispose MIDI port");
    }
    
    if (client)
    {
        OSStatus s = MIDIClientDispose(client);
        NSLogError(s, @"Dispose MIDI client");
    }
    
    self.virtualEndpointName = nil;
    self.virtualSourceEnabled = NO;
    self.virtualDestinationEnabled = NO;
    
#if ! SCDF_MIDI_ARC
    [sources release];
    [destinations release];
    [super dealloc];
#endif
}

- (NSUInteger) numberOfConnections
{
    return sources.count + destinations.count;
}

- (MIDIPortRef) outputPort
{
    return outputPort;
}

-(BOOL)networkEnabled
{
    return [MIDINetworkSession defaultSession].enabled;
}

-(void)setNetworkEnabled:(BOOL)networkEnabled
{
    MIDINetworkSession* session = [MIDINetworkSession defaultSession];
    session.enabled = networkEnabled;
    session.connectionPolicy = MIDINetworkConnectionPolicy_Anyone;
}

-(BOOL)virtualSourceEnabled
{
    return virtualSourceDestination != nil;
}


-(void)setVirtualSourceEnabled:(BOOL)virtualSourceEnabled
{
    if (virtualSourceEnabled == self.virtualSourceEnabled) return;
    
    if (virtualSourceEnabled)
    {
        NSString *name
        = virtualEndpointName
        ? virtualEndpointName
        : [[[NSBundle mainBundle] infoDictionary] valueForKey:(NSString*)kCFBundleNameKey];
        OSStatus s = MIDISourceCreate(client, (__bridge CFStringRef)name, &virtualSourceEndpoint);
        NSLogError(s, @"Create MIDI virtual source");
        if (s) return;
        
        virtualSourceDestination = [[SCDF_MidiVirtualSourceDestination alloc] initWithMidi:self endpoint:virtualSourceEndpoint];
        
        [delegate midi:self destinationAdded:virtualSourceDestination];
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiDestinationAddedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:virtualSourceDestination
                                                                                               forKey:SCDF_MidiConnectionKey]];
        
    }
    else
    {
        [delegate midi:self destinationRemoved:virtualSourceDestination];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiDestinationRemovedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:virtualSourceDestination
                                                                                               forKey:SCDF_MidiConnectionKey]];
        
        SCDF__RELEASE(virtualSourceDestination);
        OSStatus s = MIDIEndpointDispose(virtualSourceEndpoint);
        NSLogError(s, @"Dispose MIDI virtual source");
        virtualSourceEndpoint = NULL;
    }
}

-(BOOL)virtualDestinationEnabled
{
    return virtualDestinationSource != nil;
}

-(void)setVirtualDestinationEnabled:(BOOL)virtualDestinationEnabled
{
    if (virtualDestinationEnabled == self.virtualDestinationEnabled) return;
    
    if (virtualDestinationEnabled)
    {
        NSString *name
        = virtualEndpointName
        ? virtualEndpointName
        : [[[NSBundle mainBundle] infoDictionary] valueForKey:(NSString*)kCFBundleNameKey];
        OSStatus s = MIDIDestinationCreate(client, (__bridge CFStringRef)name, SCDF_MIDIVirtualDestinationReadProc, (__bridge void*)self, &virtualDestinationEndpoint);
        NSLogError(s, @"Create MIDI virtual destination");
        if (s) return;
        
        // Attempt to use saved unique ID
        SInt32 uniqueID = [[NSUserDefaults standardUserDefaults] integerForKey:@"SCDF_MIDI Saved Virtual Destination ID"];
        if (uniqueID)
        {
            s = MIDIObjectSetIntegerProperty(virtualDestinationEndpoint, kMIDIPropertyUniqueID, uniqueID);
            if (s == kMIDIIDNotUnique)
            {
                uniqueID = 0;
            }
        }
        // Save the ID
        if (!uniqueID)
        {
            s = MIDIObjectGetIntegerProperty(virtualDestinationEndpoint, kMIDIPropertyUniqueID, &uniqueID);
            NSLogError(s, @"Get MIDI virtual destination ID");
            if (s == noErr)
            {
                [[NSUserDefaults standardUserDefaults] setInteger:uniqueID forKey:@"SCDF_MIDI Saved Virtual Destination ID"];
            }
        }
        
        virtualDestinationSource = [[SCDF_MidiSource alloc] initWithMidi:self endpoint:virtualDestinationEndpoint];
        
        [delegate midi:self sourceAdded:virtualDestinationSource];
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiSourceAddedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:virtualDestinationSource
                                                                                               forKey:SCDF_MidiConnectionKey]];
    }
    else
    {
        [delegate midi:self sourceRemoved:virtualDestinationSource];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiSourceRemovedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:virtualDestinationSource
                                                                                               forKey:SCDF_MidiConnectionKey]];
        
        SCDF__RELEASE(virtualDestinationSource);
        OSStatus s = MIDIEndpointDispose(virtualDestinationEndpoint);
        NSLogError(s, @"Dispose MIDI virtual destination");
        virtualDestinationEnabled = NO;
    }
}


//==============================================================================
#pragma mark Connect/disconnect

- (SCDF_MidiSource*) getSource:(MIDIEndpointRef)source
{
    for (SCDF_MidiSource *s in sources)
    {
        if (s.endpoint == source) return s;
    }
    return nil;
}

- (SCDF_MidiDestination*) getDestination:(MIDIEndpointRef)destination
{
    for (SCDF_MidiDestination *d in destinations)
    {
        if (d.endpoint == destination) return d;
    }
    return nil;
}

- (void) connectSource:(MIDIEndpointRef)endpoint
{
    SCDF_MidiSource *source = [[SCDF_MidiSource alloc] initWithMidi:self endpoint:endpoint];
    [sources addObject:source];
    [delegate midi:self sourceAdded:source];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiSourceAddedNotification
                                                        object:self
                                                      userInfo:[NSDictionary dictionaryWithObject:source
                                                                                           forKey:SCDF_MidiConnectionKey]];
    
    OSStatus s = MIDIPortConnectSource(inputPort, endpoint, (void*)(source));
    NSLogError(s, @"Connecting to MIDI source");
}

- (void) disconnectSource:(MIDIEndpointRef)endpoint
{
    SCDF_MidiSource *source = [self getSource:endpoint];
    
    if (source)
    {
        OSStatus s = MIDIPortDisconnectSource(inputPort, endpoint);
        NSLogError(s, @"Disconnecting from MIDI source");
        [sources removeObject:source];
        
        [delegate midi:self sourceRemoved:source];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiSourceRemovedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:source
                                                                                               forKey:SCDF_MidiConnectionKey]];
        
#if ! SCDF_MIDI_ARC
        [source release];
#endif
    }
}

- (void) connectDestination:(MIDIEndpointRef)endpoint
{
    SCDF_MidiDestination *destination = [[SCDF_MidiDestination alloc] initWithMidi:self endpoint:endpoint];
    [destinations addObject:destination];
    [delegate midi:self destinationAdded:destination];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiDestinationAddedNotification
                                                        object:self
                                                      userInfo:[NSDictionary dictionaryWithObject:destination
                                                                                           forKey:SCDF_MidiConnectionKey]];
}

- (void) disconnectDestination:(MIDIEndpointRef)endpoint
{
    SCDF_MidiDestination *destination = [self getDestination:endpoint];
    
    if (destination)
    {
        [destinations removeObject:destination];
        
        [delegate midi:self destinationRemoved:destination];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:SCDF_MidiDestinationRemovedNotification
                                                            object:self
                                                          userInfo:[NSDictionary dictionaryWithObject:destination
                                                                                               forKey:SCDF_MidiConnectionKey]];
#if ! SCDF_MIDI_ARC
        [destination release];
#endif
    }
}

- (void) scanExistingDevices
{
    const ItemCount numberOfDestinations = MIDIGetNumberOfDestinations();
    const ItemCount numberOfSources      = MIDIGetNumberOfSources();
    
    NSMutableArray *removedSources       = [NSMutableArray arrayWithArray:sources];
    NSMutableArray *removedDestinations  = [NSMutableArray arrayWithArray:destinations];
    
    for (ItemCount index = 0; index < numberOfDestinations; ++index)
    {
        MIDIEndpointRef endpoint = MIDIGetDestination(index);
        if (endpoint == virtualDestinationEndpoint) continue;
        
        BOOL matched = NO;
        for (SCDF_MidiDestination *destination in destinations)
        {
            if (destination.endpoint == endpoint)
            {
                [removedDestinations removeObject:destination];
                matched = YES;
                break;
            }
        }
        if (matched) continue;
        
        [self connectDestination:endpoint];
    }
    for (ItemCount index = 0; index < numberOfSources; ++index)
    {
        MIDIEndpointRef endpoint = MIDIGetSource(index);
        if (endpoint == virtualSourceEndpoint) continue;
        
        BOOL matched = NO;
        for (SCDF_MidiDestination *source in sources)
        {
            if (source.endpoint == endpoint)
            {
                [removedSources removeObject:source];
                matched = YES;
                break;
            }
        }
        if (matched) continue;
        
        [self connectSource:endpoint];
    }
    
    for (SCDF_MidiDestination *destination in removedDestinations)
    {
        [self disconnectDestination:destination.endpoint];
    }
    
    for (SCDF_MidiSource *source in removedSources)
    {
        [self disconnectSource:source.endpoint];
    }
}

//==============================================================================
#pragma mark Notifications

- (void) midiNotifyAdd:(const MIDIObjectAddRemoveNotification *)notification
{
    if (notification->child == virtualDestinationEndpoint || notification->child == virtualSourceEndpoint) return;
    
    if (notification->childType == kMIDIObjectType_Destination)
        [self connectDestination:(MIDIEndpointRef)notification->child];
    else if (notification->childType == kMIDIObjectType_Source)
        [self connectSource:(MIDIEndpointRef)notification->child];
}

- (void) midiNotifyRemove:(const MIDIObjectAddRemoveNotification *)notification
{
    if (notification->child == virtualDestinationEndpoint || notification->child == virtualSourceEndpoint) return;
    
    if (notification->childType == kMIDIObjectType_Destination)
        [self disconnectDestination:(MIDIEndpointRef)notification->child];
    else if (notification->childType == kMIDIObjectType_Source)
        [self disconnectSource:(MIDIEndpointRef)notification->child];
    
    [self onEndPointRemoved:(MIDIEndpointRef)notification->child];
}

- (void) midiNotify:(const MIDINotification*)notification
{
    
    
    switch (notification->messageID)
    {
        case kMIDIMsgObjectAdded:
            [self midiNotifyAdd:(const MIDIObjectAddRemoveNotification *)notification];
            break;
        case kMIDIMsgObjectRemoved:
            
#ifdef _DEBUG
            NSLog(@"midiNotify kMIDIMsgObjectRemoved");
#endif
            [self midiNotifyRemove:(const MIDIObjectAddRemoveNotification *)notification];
            break;
        case kMIDIMsgPropertyChanged:
        {
            //MIDIObjectPropertyChangeNotification *not =(const MIDIObjectPropertyChangeNotification *)notification;
            break;
        }
        case kMIDIMsgSetupChanged:
        case kMIDIMsgThruConnectionsChanged:
        case kMIDIMsgSerialPortOwnerChanged:
        case kMIDIMsgIOError:
            break;
    }
    
    
    Scdf::MidiOutConnection::NotifyMidiDeviceMenuListener();
}

void SCDF_MIDINotifyProc(const MIDINotification *message, void *refCon)
{
    SCDF_Midi *self = (SCDF_Midi *)(refCon);
    [self midiNotify:message];
}

//==============================================================================
#pragma mark MIDI Output

- (void) sendPacketList:(const MIDIPacketList *)packetList atPort:(int) _midiOutputPortIndex
{
        ItemCount itemPortIndex = _midiOutputPortIndex;
        MIDIEndpointRef outputEndpoint = MIDIGetDestination(itemPortIndex);
        if (outputEndpoint)
        {
            // Send it
        OSStatus s = MIDISend(outputPort, outputEndpoint, packetList);
#ifdef _DEBUG
            NSLogError(s, @"Sending MIDI");
#endif
        }
    
}

- (void) sendBytes:(const UInt8*)data size:(UInt32)size atPort:(int) _midiOutputPortIndex
{
#ifdef _DEBUG
    NSLog(@"%s(%u bytes to core MIDI)", __func__, unsigned(size));
#endif
    assert(size < 65536);
    Byte packetBuffer[size+100];
    MIDIPacketList *packetList = (MIDIPacketList*)packetBuffer;
    MIDIPacket     *packet     = MIDIPacketListInit(packetList);
    
    packet = MIDIPacketListAdd(packetList, sizeof(packetBuffer), packet, 0, size, data);
    
    [self sendPacketList:packetList atPort:_midiOutputPortIndex];
}

- (void) attachListenerAtIndex: (int) index forConnection: (Scdf::MidiOutConnection*) connection
{

     NSString *nameOfOutput = NameOfEndpoint(MIDIGetDestination(index));
    std::string nameKey = [nameOfOutput UTF8String];
    
    listenersMap[nameKey].push_back(connection);
    
   // showTestAlert("attachListenerAtIndex");
    
#ifdef _DEBUG
    NSLog(@"attachListenerAtIndex");
#endif
    
}

- (void) detachListenerAtIndex: (int) index
{
    NSString *nameOfOutput = NameOfEndpoint(MIDIGetDestination(index));
    std::string nameKey = [nameOfOutput UTF8String];
    
    if(listenersMap.size());
        listenersMap.erase(nameKey);
}

- (void) connectSourceEndopointAtIndex: (int) endpointIndex
{
    MIDIEndpointRef endpoint = MIDIGetSource(endpointIndex);
    ItemCount index = endpointIndex;
    MIDIPortConnectSource(inputPort, endpoint, (void*)&index);
}

- (void) disConnectSourceEndopointAtIndex: (int) endpointIndex
{
    MIDIEndpointRef endpoint = MIDIGetSource(endpointIndex);
    MIDIPortDisconnectSource(inputPort, endpoint);

}

- (void) onEndPointRemoved: (MIDIEndpointRef) endpoint
{
    NSString *nameOfOutput = NameOfEndpoint(endpoint);
    std::string nameKey = [nameOfOutput UTF8String];
    
    auto it = listenersMap.find(nameKey);
    std::vector<Scdf::MidiOutConnection*> connections = it->second;
    
    
    for(int i =0; i<connections.size(); i++)
    {
        Scdf::MidiOutConnection *connection = connections[i];
        if(NULL==connection) return;
        
        //showTestAlert("connection");
        
        Scdf::MidiConnectionListener *listener = connection->GetListenerConnectionLost();
        if(listener)
        {
           // showTestAlert("OnConnectionLost");
            listener->OnConnectionLost(connection);
        }
    }
}






@end
