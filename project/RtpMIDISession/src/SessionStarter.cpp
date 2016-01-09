//
//  SessionStarter.cpp
//  RtpMIDISession
//
//  Created by andrea scuderi on 09/01/16.
//  Copyright © 2016 fuzone. All rights reserved.
//

#include "FuzAppleMIDIChannel.hpp"
#include "SessionStarter.hpp"

SessionStarter *SessionStarter::_instance=NULL;
SessionStarter::PosixService *SessionStarter::gServiceList=NULL;
int SessionStarter::gServiceID=0;

static const char kDefaultServiceDomain[] = "local.";
static const char *gServiceName_stream      = "AugMIDI4_stream50004";
static const char *gServiceType_stream      = "_apple-midi._udp";/*kDefaultServiceType;*/
static int gPortNumber_stream               = 50004;/*kDefaultPortNumber;*/
static const char *gServiceDomain    = kDefaultServiceDomain;
static mDNSu8 gServiceText[sizeof(RDataBody)];
static mDNSu16 gServiceTextLen   = 0;
mDNSexport const char ProgramName[] = "mDNSResponderPosix";
static const char *gProgramName = ProgramName;

SessionStarter::SessionStarter()
{
    gDaemon=mDNSfalse;
}

void SessionStarter::Start()
{
    if(!CheckThatRichTextNameIsUsable(gServiceName_stream, mDNStrue))
    {
        LOGD("Error Service Name");
        return;
    }
    if ( !CheckThatPortNumberIsUsable(gPortNumber_stream, mDNStrue) )
    {
        LOGD("Error Port Number");
        return;
    }
    if ( !CheckThatServiceTypeIsUsable(gServiceType_stream, mDNStrue) )
    {
        LOGD("Error Service Type");
        return;
    }
    
    if (gMDNSPlatformPosixVerboseLevel > 0)
    {
        LOGD("%s: Starting in foreground mode, PID %ld\n", gProgramName, (long) getpid());
//        fprintf(stderr, "%s: Starting in foreground mode, PID %ld\n", gProgramName, (long) getpid());
    }
    
    // If we're told to run as a daemon, then do that straight away.
    // Note that we don't treat the inability to create our PID
    // file as an error.  Also note that we assign getpid to a long
    // because printf has no format specified for pid_t.
    
    
    mStatus status = mDNS_Init(&mDNSStorage, &PlatformStorage,
                       mDNS_Init_NoCache, mDNS_Init_ZeroCacheSize,
                       mDNS_Init_AdvertiseLocalAddresses,
                       mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext);
    if (status != mStatus_NoError)
    {
        LOGD("Error session init");
        return;
    }
    
    status = RegisterOurServices();
    if (status != mStatus_NoError)
    {
        LOGD("Error service reg");
        return;
    }
    
    this->handle=scdf::ThreadUtils::CreateThread((start_routine)SessionProcedure, this);
}

SessionStarter::~SessionStarter()
{
    scdf::ThreadUtils::TerminateThread(this->handle);
    
    DeregisterOurServices();
    mDNS_Close(&mDNSStorage);
//    
//    if (status == mStatus_NoError) {
//        result = 0;
//    } else {
//        result = 2;
//    }
//    if ( (result != 0) || (gMDNSPlatformPosixVerboseLevel > 0) ) {
//        fprintf(stderr, "%s: Finished with status %d, result %d\n", gProgramName, (int)status, result);
//    }
}

void SessionStarter::RegistrationCallback(mDNS *const m, ServiceRecordSet *const thisRegistration, mStatus status)
// mDNS core calls this routine to tell us about the status of
// our registration.  The appropriate action to take depends
// entirely on the value of status.
{
    switch (status) {
            
        case mStatus_NoError:
            debugf("Callback: %##s Name Registered",   thisRegistration->RR_SRV.resrec.name->c);
            appleMIDIChannelsManager.StartConnection();
            // Do nothing; our name was successfully registered.  We may
            // get more call backs in the future.
            break;
            
        case mStatus_NameConflict:
            debugf("Callback: %##s Name Conflict",     thisRegistration->RR_SRV.resrec.name->c);
            
            // In the event of a conflict, this sample RegistrationCallback
            // just calls mDNS_RenameAndReregisterService to automatically
            // pick a new unique name for the service. For a device such as a
            // printer, this may be appropriate.  For a device with a user
            // interface, and a screen, and a keyboard, the appropriate response
            // may be to prompt the user and ask them to choose a new name for
            // the service.
            //
            // Also, what do we do if mDNS_RenameAndReregisterService returns an
            // error.  Right now I have no place to send that error to.
            
            status = mDNS_RenameAndReregisterService(m, thisRegistration, mDNSNULL);
            assert(status == mStatus_NoError);
            break;
            
        case mStatus_MemFree:
            debugf("Callback: %##s Memory Free",       thisRegistration->RR_SRV.resrec.name->c);
            
            // When debugging is enabled, make sure that thisRegistration
            // is not on our gServiceList.
            
#if !defined(NDEBUG)
        {
            PosixService *cursor;
            
            cursor = gServiceList;
            while (cursor != NULL) {
                assert(&cursor->coreServ != thisRegistration);
                cursor = cursor->next;
            }
        }
#endif
            free(thisRegistration);
            break;
            
        default:
            debugf("Callback: %##s Unknown Status %ld", thisRegistration->RR_SRV.resrec.name->c, status);
            break;
    }
}

void SessionStarter::SessionProcedure(void *param)
{
    SessionStarter *session=(SessionStarter*)param;
    int nfds = 0;
    fd_set readfds;
    struct timeval timeout;
    int result;
    
    // 1. Set up the fd_set as usual here.
    // This example client has no file descriptors of its own,
    // but a real application would call FD_SET to add them to the set here
    FD_ZERO(&readfds);
    
    // 2. Set up the timeout.
    // This example client has no other work it needs to be doing,
    // so we set an effectively infinite timeout
    timeout.tv_sec = 0x3FFFFFFF;
    timeout.tv_usec = 0;
    
    // 3. Give the mDNSPosix layer a chance to add its information to the fd_set and timeout
    mDNSPosixGetFDSet(&session->mDNSStorage, &nfds, &readfds, &timeout);
    
    // 4. Call select as normal
    verbosedebugf("select(%d, %d.%06d)", nfds, timeout.tv_sec, timeout.tv_usec);
    result = select(nfds, &readfds, NULL, NULL, &timeout);
    
    if (result < 0)
    {
        verbosedebugf("select() returned %d errno %d", result, errno);
//        if (errno != EINTR) gStopNow = mDNStrue;
//        else
//        {
//            if (gReceivedSigUsr1)
//            {
//                gReceivedSigUsr1 = mDNSfalse;
//                gMDNSPlatformPosixVerboseLevel += 1;
//                if (gMDNSPlatformPosixVerboseLevel > 2)
//                    gMDNSPlatformPosixVerboseLevel = 0;
//                if ( gMDNSPlatformPosixVerboseLevel > 0 )
//                    fprintf(stderr, "\nVerbose level %d\n", gMDNSPlatformPosixVerboseLevel);
//            }
//            if (gReceivedSigHup)
//            {
//                if (gMDNSPlatformPosixVerboseLevel > 0)
//                    fprintf(stderr, "\nSIGHUP\n");
//                gReceivedSigHup = mDNSfalse;
//                DeregisterOurServices();
//                status = mDNSPlatformPosixRefreshInterfaceList(&mDNSStorage);
//                if (status != mStatus_NoError) break;
//                status = RegisterOurServices();
//                if (status != mStatus_NoError) break;
//            }
//        }
    }
    else
    {
        // 5. Call mDNSPosixProcessFDSet to let the mDNSPosix layer do its work
        mDNSPosixProcessFDSet(&session->mDNSStorage, &readfds);
        
        if (session->mDNSStorage.imsg.m.h.numAnswers!=0)
        {
            printf("here\n");
        }
        
        // 6. This example client has no other work it needs to be doing,
        // but a real client would do its work here
        // ... (do work) ...
    }

}

mStatus SessionStarter::RegisterOurServices()
{
    mStatus status = mStatus_NoError;
    
    if (gServiceName_stream[0] != 0)
    {
        status = RegisterOneService(gServiceName_stream,
                                        gServiceType_stream,
                                        gServiceDomain,
                                        gServiceText,
                                        gServiceTextLen,
                                        gPortNumber_stream);
    }
    /*if (status == mStatus_NoError && gServiceFile[0] != 0) {
     status = RegisterServicesInFile(gServiceFile);
     }*/
    return status;
}

void SessionStarter::DeregisterOurServices(void)
{
    PosixService *thisServ;
    int thisServID;
    
    while (gServiceList != NULL) {
        thisServ = gServiceList;
        gServiceList = thisServ->next;
        
        thisServID = thisServ->serviceID;
        
        mDNS_DeregisterService(&mDNSStorage, &thisServ->coreServ);
        
        if (gMDNSPlatformPosixVerboseLevel > 0) {
            fprintf(stderr,
                    "%s: Deregistered service %d\n",
                    gProgramName,
                    thisServ->serviceID);
        }
    }
}

mDNSBool SessionStarter::CheckThatRichTextNameIsUsable(const char *richTextName, mDNSBool printExplanation)
// Checks that richTextName is reasonable
// label and, if it isn't and printExplanation is true, prints
// an explanation of why not.
{
    mDNSBool result = mDNStrue;
    if (result && strlen(richTextName) > 63) {
        if (printExplanation) {
            fprintf(stderr,
                    "%s: Service name is too long (must be 63 characters or less)\n",
                    gProgramName);
        }
        result = mDNSfalse;
    }
    if (result && richTextName[0] == 0) {
        if (printExplanation) {
            fprintf(stderr, "%s: Service name can't be empty\n", gProgramName);
        }
        result = mDNSfalse;
    }
    return result;
}

mDNSBool SessionStarter::CheckThatServiceTypeIsUsable(const char *serviceType, mDNSBool printExplanation)
// Checks that serviceType is a reasonable service type
// label and, if it isn't and printExplanation is true, prints
// an explanation of why not.
{
    mDNSBool result;
    
    result = mDNStrue;
    if (result && strlen(serviceType) > 63) {
        if (printExplanation) {
            fprintf(stderr,
                    "%s: Service type is too long (must be 63 characters or less)\n",
                    gProgramName);
        }
        result = mDNSfalse;
    }
    if (result && serviceType[0] == 0) {
        if (printExplanation) {
            fprintf(stderr,
                    "%s: Service type can't be empty\n",
                    gProgramName);
        }
        result = mDNSfalse;
    }
    return result;
}

mDNSBool SessionStarter::CheckThatPortNumberIsUsable(long portNumber, mDNSBool printExplanation)
// Checks that portNumber is a reasonable port number
// and, if it isn't and printExplanation is true, prints
// an explanation of why not.
{
    mDNSBool result;
    
    result = mDNStrue;
    if (result && (portNumber <= 0 || portNumber > 65535)) {
        if (printExplanation) {
            fprintf(stderr,
                    "%s: Port number specified by -p must be in range 1..65535\n",
                    gProgramName);
        }
        result = mDNSfalse;
    }
    return result;
}

mStatus SessionStarter::RegisterOneService(const char *  richTextName,
                                  const char *  serviceType,
                                  const char *  serviceDomain,
                                  const mDNSu8 text[],
                                  mDNSu16 textLen,
                                  long portNumber)
{
    mStatus status;
    PosixService *      thisServ;
    domainlabel name;
    domainname type;
    domainname domain;
    
    status = mStatus_NoError;
    thisServ = (PosixService *) malloc(sizeof(*thisServ));
    if (thisServ == NULL) {
        status = mStatus_NoMemoryErr;
    }
    if (status == mStatus_NoError) {
        MakeDomainLabelFromLiteralString(&name,  richTextName);
        MakeDomainNameFromDNSNameString(&type, serviceType);
        MakeDomainNameFromDNSNameString(&domain, serviceDomain);
        status = mDNS_RegisterService(&mDNSStorage, &thisServ->coreServ,
                                      &name, &type, &domain, // Name, type, domain
                                      NULL, mDNSOpaque16fromIntVal(portNumber),
                                      text, textLen, // TXT data, length
                                      NULL, 0,      // Subtypes
                                      mDNSInterface_Any, // Interface ID
                                      RegistrationCallback, thisServ, 0); // Callback, context, flags
    }
    if (status == mStatus_NoError) {
        thisServ->serviceID = gServiceID;
        gServiceID += 1;
        
        thisServ->next = gServiceList;
        gServiceList = thisServ;
        
        if (gMDNSPlatformPosixVerboseLevel > 0) {
            fprintf(stderr,
                    "%s: Registered service %d, name \"%s\", type \"%s\", domain \"%s\",  port %ld\n",
                    gProgramName,
                    thisServ->serviceID,
                    richTextName,
                    serviceType,
                    serviceDomain,
                    portNumber);
        }
    } else {
        if (thisServ != NULL) {
            free(thisServ);
        }
    }
    return status;
}
