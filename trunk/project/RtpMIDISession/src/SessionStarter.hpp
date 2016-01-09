//
//  SessionStarter.hpp
//  RtpMIDISession
//
//  Created by andrea scuderi on 09/01/16.
//  Copyright © 2016 fuzone. All rights reserved.
//

#ifndef SessionStarter_hpp
#define SessionStarter_hpp

#include <stdio.h>
#include "ThreadUtils.h"
#include "mDNSEmbeddedAPI.h" // Defines the interface to the client layer above
#include "mDNSPosix.h" 

#pragma GCC visibility push(default)

//class AppleMIDIChannelsManager;
class SessionStarter
{
public:
    SessionStarter *Instance()
    {
        if(NULL==_instance)
            _instance=new SessionStarter();
        return _instance;
    }
    void Start();
    ~SessionStarter();
    
private:
    struct PosixService {
        ServiceRecordSet coreServ;
        PosixService *next;
        int serviceID;
    };
    static PosixService *gServiceList;
    static int gServiceID;
    static AppleMIDIChannelsManager appleMIDIChannelsManager;
    
    static SessionStarter *_instance;
    SessionStarter();
    static void SessionProcedure(void *param);
    static void RegistrationCallback(mDNS *const m, ServiceRecordSet *const thisRegistration, mStatus status);
    scdf::ThreadUtils::ThreadHandle handle;
    
    mDNSBool CheckThatRichTextNameIsUsable(const char *richTextName, mDNSBool printExplanation);
    mDNSBool CheckThatServiceTypeIsUsable(const char *serviceType, mDNSBool printExplanation);
    mDNSBool CheckThatPortNumberIsUsable(long portNumber, mDNSBool printExplanation);
    mStatus RegisterOurServices();
    mStatus RegisterOneService(const char *richTextName, const char *serviceType, const char *serviceDomain, const mDNSu8 text[], mDNSu16 textLen, long portNumber);
    void DeregisterOurServices(void);
    
    mDNSBool gDaemon;
    mDNS mDNSStorage;
    mDNS_PlatformSupport PlatformStorage;
};

#pragma GCC visibility pop

#endif /* SessionStarter_hpp */
