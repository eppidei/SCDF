/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if __APPLE__
// In Mac OS X 10.5 and later trying to use the daemon function gives a “‘daemon’ is deprecated”
// error, which prevents compilation because we build with "-Werror".
// Since this is supposed to be portable cross-platform code, we don't care that daemon is
// deprecated on Mac OS X 10.5, so we use this preprocessor trick to eliminate the error message.
#define daemon yes_we_know_that_daemon_is_deprecated_in_os_x_10_5_thankyou
#endif

#include <assert.h>
#include <stdio.h>          // For printf()
#include <stdlib.h>         // For exit() etc.
#include <string.h>         // For strlen() etc.
#include <unistd.h>         // For select()
#include <errno.h>          // For errno, EINTR
#include <signal.h>
#include <fcntl.h>

#if __APPLE__
#undef daemon
extern int daemon(int, int);
#endif

#include "mDNSEmbeddedAPI.h" // Defines the interface to the client layer above
#include "mDNSPosix.h"      // Defines the specific types needed to run mDNS on this platform
#include "mDNSUNP.h"        // For daemon()

#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark ***** Globals
#endif

static mDNS mDNSStorage;       // mDNS core uses this to store its globals
static mDNS_PlatformSupport PlatformStorage;  // Stores this platform's globals


#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark ***** Signals
#endif

//static volatile mDNSBool gReceivedSigUsr1;
//static volatile mDNSBool gReceivedSigHup;
//static volatile mDNSBool gStopNow;

// We support 4 signals.
//
// o SIGUSR1 toggles verbose mode on and off in debug builds
// o SIGHUP  triggers the program to re-read its preferences.
// o SIGINT  causes an orderly shutdown of the program.
// o SIGQUIT causes a somewhat orderly shutdown (direct but dangerous)
// o SIGKILL kills us dead (easy to implement :-)
//
// There are fatal race conditions in our signal handling, but there's not much
// we can do about them while remaining within the Posix space.  Specifically,
// if a signal arrives after we test the globals its sets but before we call
// select, the signal will be dropped.  The user will have to send the signal
// again.  Unfortunately, Posix does not have a "sigselect" to atomically
// modify the signal mask and start a select.

//static void HandleSigUsr1(int sigraised)
//// If we get a SIGUSR1 we toggle the state of the
//// verbose mode.
//{
//    assert(sigraised == SIGUSR1);
//    gReceivedSigUsr1 = mDNStrue;
//}
//
//static void HandleSigHup(int sigraised)
//// A handler for SIGHUP that causes us to break out of the
//// main event loop when the user kill 1's us.  This has the
//// effect of triggered the main loop to deregister the
//// current services and re-read the preferences.
//{
//    assert(sigraised == SIGHUP);
//    gReceivedSigHup = mDNStrue;
//}
//
//static void HandleSigInt(int sigraised)
//// A handler for SIGINT that causes us to break out of the
//// main event loop when the user types ^C.  This has the
//// effect of quitting the program.
//{
//    assert(sigraised == SIGINT);
//
//    if (gMDNSPlatformPosixVerboseLevel > 0) {
//        fprintf(stderr, "\nSIGINT\n");
//    }
//    gStopNow = mDNStrue;
//}
//
//static void HandleSigQuit(int sigraised)
//// If we get a SIGQUIT the user is desperate and we
//// just call mDNS_Close directly.  This is definitely
//// not safe (because it could reenter mDNS), but
//// we presume that the user has already tried the safe
//// alternatives.
//{
//    assert(sigraised == SIGQUIT);
//
//    if (gMDNSPlatformPosixVerboseLevel > 0) {
//        fprintf(stderr, "\nSIGQUIT\n");
//    }
//    mDNS_Close(&mDNSStorage);
//    exit(0);
//}

#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark ***** Parameter Checking
#endif





#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark ***** Command Line Arguments
#endif

static const char kDefaultPIDFile[]     = "/var/run/mDNSResponder.pid";
//static const char kDefaultServiceType[] = "_afpovertcp._tcp.";

enum {
    kDefaultPortNumber = 548
};



//static mDNSBool gAvoidPort53      = mDNStrue;

//static const char *gServiceType_control      = "_ws-midi._tcp";/*kDefaultServiceType;*/




//static const char *gServiceFile      = "";

//static const char *gPIDFile          = kDefaultPIDFile;

//static void ParseArguments(int argc, char **argv)
//// Parses our command line arguments into the global variables
//// listed above.
//{
//    int ch;
//
//    // Set gProgramName to the last path component of argv[0]
//
//    gProgramName = strrchr(argv[0], '/');
//    if (gProgramName == NULL) {
//        gProgramName = argv[0];
//    } else {
//        gProgramName += 1;
//    }
//
//    // Parse command line options using getopt.
//
//    do {
//        ch = getopt(argc, argv, "v:rn:t:d:p:f:dP:bx");
//        if (ch != -1) {
//            switch (ch) {
//            case 'v':
//                gMDNSPlatformPosixVerboseLevel = atoi(optarg);
//                if (gMDNSPlatformPosixVerboseLevel < 0 || gMDNSPlatformPosixVerboseLevel > 2) {
//                    fprintf(stderr,
//                            "%s: Verbose mode must be in the range 0..2\n",
//                            gProgramName);
//                    exit(1);
//                }
//                break;
//            case 'r':
//                gAvoidPort53 = mDNSfalse;
//                break;
//            case 'n':
//                gServiceName_control = optarg;
//                if ( !CheckThatRichTextNameIsUsable(gServiceName_control, mDNStrue) ) {
//                    exit(1);
//                }
//                break;
//            case 't':
//                gServiceType_control = optarg;
//                if ( !CheckThatServiceTypeIsUsable(gServiceType_control, mDNStrue) ) {
//                    exit(1);
//                }
//                break;
//            case 'd':
//                gServiceDomain = optarg;
//                break;
//            case 'p':
//                gPortNumber_control = atol(optarg);
//                if ( !CheckThatPortNumberIsUsable(gPortNumber_control, mDNStrue) ) {
//                    exit(1);
//                }
//                break;
//            case 'f':
//                gServiceFile = optarg;
//                break;
//            case 'b':
//                gDaemon = mDNStrue;
//                break;
//            case 'P':
//                gPIDFile = optarg;
//                break;
//            case 'x':
//                while (optind < argc)
//                {
//                    gServiceText[gServiceTextLen] = strlen(argv[optind]);
//                    mDNSPlatformMemCopy(gServiceText+gServiceTextLen+1, argv[optind], gServiceText[gServiceTextLen]);
//                    gServiceTextLen += 1 + gServiceText[gServiceTextLen];
//                    optind++;
//                }
//                ch = -1;
//                break;
//            case '?':
//            default:
//                PrintUsage();
//                exit(1);
//                break;
//            }
//        }
//    } while (ch != -1);
//
//    // Check for any left over command line arguments.
//
//    if (optind != argc) {
//        PrintUsage();
//        fprintf(stderr, "%s: Unexpected argument '%s'\n", gProgramName, argv[optind]);
//        exit(1);
//    }
//
//    // Check for inconsistency between the arguments.
//
//    if ( (gServiceName_control[0] == 0) && (gServiceFile[0] == 0) ) {
//        PrintUsage();
//        fprintf(stderr, "%s: You must specify a service name to register (-n) or a service file (-f).\n", gProgramName);
//        exit(1);
//    }
//}

#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark ***** Registration
#endif

typedef struct PosixService PosixService;











static mDNSBool ReadALine(char *buf, size_t bufSize, FILE *fp, mDNSBool skipBlankLines)
{
    size_t len;
    mDNSBool readNextLine;

    do {
        readNextLine = mDNSfalse;

        if (fgets(buf, bufSize, fp) == NULL)
            return mDNSfalse;   // encountered EOF or an error condition

        // These first characters indicate a blank line.
        if (buf[0] == ' ' || buf[0] == '\t' || buf[0] == '\r' || buf[0] == '\n') {
            if (!skipBlankLines)
                return mDNSfalse;
            readNextLine = mDNStrue;
        }
        // always skip comment lines
        if (buf[0] == '#')
            readNextLine = mDNStrue;

    } while (readNextLine);

    len = strlen( buf);
    if ( buf[len - 1] == '\r' || buf[len - 1] == '\n')
        buf[len - 1] = '\0';

    return mDNStrue;
}

static mStatus RegisterServicesInFile(const char *filePath)
{
    mStatus status = mStatus_NoError;
    FILE *      fp = fopen(filePath, "r");

    if (fp == NULL) {
        return mStatus_UnknownErr;
    }

    if (gMDNSPlatformPosixVerboseLevel > 1)
        fprintf(stderr, "Parsing %s for services\n", filePath);

    do {
        char nameBuf[256];
        char * name = nameBuf;
        char type[256];
        const char *dom = kDefaultServiceDomain;
        char rawText[1024];
        mDNSu8 text[sizeof(RDataBody)];
        unsigned int textLen = 0;
        char port[256];
        char *p;

        // Read the service name, type, port, and optional text record fields.
        // Skip blank lines while looking for the next service name.
        if (!ReadALine(name, sizeof(nameBuf), fp, mDNStrue))
            break;

        // Special case that allows service name to begin with a '#'
        // character by escaping it with a '\' to distiguish it from
        // a comment line.  Remove the leading '\' here before
        // registering the service.
        if (name[0] == '\\' && name[1] == '#')
            name++;

        if (gMDNSPlatformPosixVerboseLevel > 1)
            fprintf(stderr, "Service name: \"%s\"\n", name);

        // Don't skip blank lines in calls to ReadAline() after finding the
        // service name since the next blank line indicates the end
        // of this service record.
        if (!ReadALine(type, sizeof(type), fp, mDNSfalse))
            break;

        // see if a domain name is specified
        p = type;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = 0; // NULL terminate the <type>.<protocol> string
            // skip any leading whitespace before domain name
            p++;
            while (*p && (*p == ' ' || *p == '\t')) p++;
            if (*p)
                dom = p;
        }
        if (gMDNSPlatformPosixVerboseLevel > 1) {
            fprintf(stderr, "Service type: \"%s\"\n", type);
            fprintf(stderr, "Service domain: \"%s\"\n", dom);
        }

        if (!ReadALine(port, sizeof(port), fp, mDNSfalse))
            break;
        if (gMDNSPlatformPosixVerboseLevel > 1)
            fprintf(stderr, "Service port: %s\n", port);

        if (   !CheckThatRichTextNameIsUsable(name, mDNStrue)
               || !CheckThatServiceTypeIsUsable(type, mDNStrue)
               || !CheckThatPortNumberIsUsable(atol(port), mDNStrue))
            break;

        // read the TXT record fields
        while (1) {
            int len;
            if (!ReadALine(rawText, sizeof(rawText), fp, mDNSfalse)) break;
            if (gMDNSPlatformPosixVerboseLevel > 1)
                fprintf(stderr, "Text string: \"%s\"\n", rawText);
            len = strlen(rawText);
            if (len <= 255)
            {
                unsigned int newlen = textLen + 1 + len;
                if (len == 0 || newlen >= sizeof(text)) break;
                text[textLen] = len;
                mDNSPlatformMemCopy(text + textLen + 1, rawText, len);
                textLen = newlen;
            }
            else
                fprintf(stderr, "%s: TXT attribute too long for name = %s, type = %s, port = %s\n",
                        gProgramName, name, type, port);
        }

        status = RegisterOneService(name, type, dom, text, textLen, atol(port));
        if (status != mStatus_NoError) {
            // print error, but try to read and register other services in the file
            fprintf(stderr, "%s: Failed to register service, name \"%s\", type \"%s\", domain \"%s\", port %s\n",
                    gProgramName, name, type, dom, port);
        }

    } while (!feof(fp));

    if (!feof(fp)) {
        fprintf(stderr, "%s: Error reading service file %s\n", gProgramName, filePath);
        status = mStatus_UnknownErr;
    }

    assert(0 == fclose(fp));

    return status;
}


#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark **** Main
#endif

int main()//(int argc, char **argv)
{


//    signal(SIGHUP,  HandleSigHup);      // SIGHUP has to be sent by kill -HUP <pid>
//    signal(SIGINT,  HandleSigInt);      // SIGINT is what you get for a Ctrl-C
//    signal(SIGQUIT, HandleSigQuit);     // SIGQUIT is what you get for a Ctrl-\ (indeed)
//    signal(SIGUSR1, HandleSigUsr1);     // SIGUSR1 has to be sent by kill -USR1 <pid>



}
