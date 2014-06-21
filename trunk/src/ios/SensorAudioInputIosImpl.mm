//
//  SensorAudioInputImpl.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "Sensor.h"
#import <AVFoundation/AVAudioSession.h>
#import "CAStreamBasicDescription.h"
#import "SensorAudioInputIosImpl.h"


using namespace scdf;

@implementation AudioEventsListener

- (void)Attach:(scdf::SensorAudioInputImpl *)_audioSensor
{
    audioSensor=_audioSensor;
}

- (void)handleInterruption:(NSNotification *)notification
{
    try {
        UInt8 theInterruptionType = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue];
        NSLog(@"Session interrupted > --- %s ---\n", theInterruptionType == AVAudioSessionInterruptionTypeBegan ? "Begin Interruption" : "End Interruption");
        
        if (theInterruptionType == AVAudioSessionInterruptionTypeBegan) {
            audioSensor->Stop();
        }
        
        if (theInterruptionType == AVAudioSessionInterruptionTypeEnded) {
            // make sure to activate the session
            NSError *error = nil;
            [[AVAudioSession sharedInstance] setActive:YES error:&error];
            if (nil != error) NSLog(@"AVAudioSession set active failed with error: %@", error);
            
            audioSensor->Start();
        }
    } catch(NSException * e) {
    }
}


- (void)handleRouteChange:(NSNotification *)notification
{
    UInt8 reasonValue = [[notification.userInfo valueForKey:AVAudioSessionRouteChangeReasonKey] intValue];
    AVAudioSessionRouteDescription *routeDescription = [notification.userInfo valueForKey:AVAudioSessionRouteChangePreviousRouteKey];
    
    NSLog(@"Route change:");
    switch (reasonValue) {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
            NSLog(@"     NewDeviceAvailable");
            break;
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
            NSLog(@"     OldDeviceUnavailable");
            break;
        case AVAudioSessionRouteChangeReasonCategoryChange:
            NSLog(@"     CategoryChange");
            NSLog(@" New Category: %@", [[AVAudioSession sharedInstance] category]);
            break;
        case AVAudioSessionRouteChangeReasonOverride:
            NSLog(@"     Override");
            break;
        case AVAudioSessionRouteChangeReasonWakeFromSleep:
            NSLog(@"     WakeFromSleep");
            break;
        case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
            NSLog(@"     NoSuitableRouteForCategory");
            break;
        default:
            NSLog(@"     ReasonUnknown");
    }
    
    NSLog(@"Previous route:\n");
    NSLog(@"%@", routeDescription);
}

- (void)handleMediaServerReset:(NSNotification *)notification
{
    NSLog(@"Media server has reset");
    
    usleep(25000); //wait here for some time to ensure that we don't delete these objects while they are being accessed elsewhere
    
    // rebuild the audio chain
    
    AudioSettings s;
    audioSensor->Setup(s);
    audioSensor->Start();
}

@end

struct CallbackData {
    AudioUnit               rioUnit;
    scdf::Sensor            *sensorRef;
    CallbackData(): rioUnit(NULL) {}
} callbackData;

// Render callback function
static OSStatus	performRender (void                         *inRefCon,
                               AudioUnitRenderActionFlags 	*ioActionFlags,
                               const AudioTimeStamp 		*inTimeStamp,
                               UInt32 						inBusNumber,
                               UInt32 						inNumberFrames,
                               AudioBufferList              *ioData)
{
    OSStatus err = noErr;
    
    
    err = AudioUnitRender(callbackData.rioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
    
    s_int32  numberOfFrames = inNumberFrames;
    s_float* audioData = new s_float[numberOfFrames];
    
    Float32 *frame = (Float32 *) ioData->mBuffers[0].mData; // now use only one channel
    
    for (s_int32 i=0; i<inNumberFrames; ++i)
    {
        audioData[i] = frame[i];
    }
    
    
    scdf::SensorData *sData = new scdf::SensorData();
    sData->type = scdf::AudioInput;
    sData->data = (char*)audioData;
    sData->num_samples=inNumberFrames;
    sData->rate=44100;
    callbackData.sensorRef->AddIncomingDataToQueue(sData);
    
    
    
    // now mute the output
    s_bool muteAudioOutput = true;
    if(muteAudioOutput)
    {
        for (UInt32 i=0; i<ioData->mNumberBuffers; ++i)
            memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
    }
    
    return err;
}

SensorAudioInputImpl::SensorAudioInputImpl(SensorType type/*, ScdfPipe *destPipe*/)
{
    listener = [[AudioEventsListener alloc] init];
            
}

SensorAudioInputImpl::~SensorAudioInputImpl()
{
    [listener release];
}

s_bool SensorAudioInputImpl::Setup(scdf::SensorSettings &settings)
{
    SetupAudioChain(settings);
    return true;
}

s_bool SensorAudioInputImpl::Start()
{
    OSStatus err = AudioOutputUnitStart(_rioUnit);
    if (err) {
        NSLog(@"couldn't start AURemoteIO: %d", (int)err);
        return false;
    }
    
    return true;
}

s_bool SensorAudioInputImpl::Stop()
{
    OSStatus err = AudioOutputUnitStop(_rioUnit);
    if (err) {
        NSLog(@"couldn't stop AURemoteIO: %d", (int)err);
        return false;
    }
    return err;
    
    return true;
}

void SensorAudioInputImpl::SetupIOUnit(scdf::SensorSettings &settings)
{
    try {
        // Create a new instance of AURemoteIO
        
        AudioComponentDescription desc;
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_RemoteIO;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;
        
        AudioComponent comp = AudioComponentFindNext(NULL, &desc);
        AudioComponentInstanceNew(comp, &_rioUnit);
        
        //  Enable input and output on AURemoteIO
        //  Input is enabled on the input scope of the input element
        //  Output is enabled on the output scope of the output element
        
        UInt32 one = 1;
        AudioUnitSetProperty(_rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof(one));
        AudioUnitSetProperty(_rioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &one, sizeof(one));
        
        // Explicitly set the input and output client formats
        // sample rate = 44100, num channels = 1, format = 32 bit floating point
        
        CAStreamBasicDescription ioFormat = CAStreamBasicDescription(44100, 1, CAStreamBasicDescription::kPCMFormatFloat32, false);
        AudioUnitSetProperty(_rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &ioFormat, sizeof(ioFormat));
        AudioUnitSetProperty(_rioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &ioFormat, sizeof(ioFormat));
        
        // Set the MaximumFramesPerSlice property. This property is used to describe to an audio unit the maximum number
        // of samples it will be asked to produce on any single given call to AudioUnitRender
        UInt32 maxFramesPerSlice = 4096;
        AudioUnitSetProperty(_rioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maxFramesPerSlice, sizeof(UInt32));
        
        // Get the property value back from AURemoteIO. We are going to use this value to allocate buffers accordingly
        UInt32 propSize = sizeof(UInt32);
        AudioUnitGetProperty(_rioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maxFramesPerSlice, &propSize);
        
        
        
        // We need references to certain data in the render callback
        // This simple struct is used to hold that information
        
        
        
        callbackData.rioUnit = _rioUnit;
        callbackData.sensorRef = this;
        
#define kInputBus 0
        // Set the render callback on AURemoteIO
        AURenderCallbackStruct renderCallback;
        renderCallback.inputProc = performRender;
        renderCallback.inputProcRefCon = this;      /////??????????????????????????
        AudioUnitSetProperty(_rioUnit,
                             kAudioUnitProperty_SetRenderCallback,
                             kAudioUnitScope_Global,
                             kInputBus,
                             &renderCallback,
                             sizeof(renderCallback));
        
        // Initialize the AURemoteIO instance
        AudioUnitInitialize(_rioUnit);
    }
    
    catch (NSException *e) {
        NSLog(@"Error returned from setupIOUnit: %@, %@", e.name, e.reason);
    }
    catch (...) {
        NSLog(@"Unknown error returned from setupIOUnit");
    }
    
    return;
}


void SensorAudioInputImpl::SetupAudioChain(scdf::SensorSettings &settings)
{
    SetupAudioSession(settings);
    SetupIOUnit(settings);
}

void SensorAudioInputImpl::SetupAudioSession(scdf::SensorSettings &settings)
{
    try {
        // Configure the audio session
        AVAudioSession *sessionInstance = [AVAudioSession sharedInstance];
        
        // we are going to play and record so we pick that category
        NSError *error = nil;
        [sessionInstance setCategory:AVAudioSessionCategoryPlayAndRecord error:&error];
        if (error) {
            NSLog(@"couldn't set session's audio categor %@", error);
            error = nil;
        }
        
        
        // set the buffer duration to 5 ms
        NSTimeInterval bufferDuration = .005;
        [sessionInstance setPreferredIOBufferDuration:bufferDuration error:&error];
        if(error)
        {
            NSLog(@"couldn't set session's I/O buffer duration %@", error);
            error = nil;
        }
        
        
        
        // set the session's sample rate
        [sessionInstance setPreferredSampleRate:44100 error:&error];
        if (error) {
            NSLog(@"couldn't set session's preferred sample rate %@", error);
            error = nil;
        }
        
        
        // add interruption handler
        [[NSNotificationCenter defaultCenter] addObserver:listener
                                                 selector:@selector(handleInterruption:)
                                                     name:AVAudioSessionInterruptionNotification
                                                   object:sessionInstance];
        
        // we don't do anything special in the route change notification
        [[NSNotificationCenter defaultCenter] addObserver:listener
                                                 selector:@selector(handleRouteChange:)
                                                     name:AVAudioSessionRouteChangeNotification
                                                   object:sessionInstance];
        
        // if media services are reset, we need to rebuild our audio chain
        [[NSNotificationCenter defaultCenter] addObserver:listener
                                                 selector:@selector(handleMediaServerReset:)
                                                     name:AVAudioSessionMediaServicesWereResetNotification
                                                   object:sessionInstance];
        
        // activate the audio session
        [[AVAudioSession sharedInstance] setActive:YES error:&error];
        if (error) {
            NSLog(@"couldn't set session active %@", error);
            error = nil;
        }
        
    }
    
    catch (NSException *e) {
        NSLog(@"Error returned from setupAudioSession: %@, %@", e.name, e.reason);
    }
    catch (...) {
        NSLog(@"Unknown error returned from setupAudioSession");
    }
    
    return;
}


