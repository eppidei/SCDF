//
//  UtilitiesiOs.m
//  ScdfController
//
//  Created by marcobertola on 21/02/15.
//
//

#include <string>
#include <mach/mach_time.h>
#include "TypeDefinitions.h"
#include "OsUtilities.h"
#import <ifaddrs.h>
#import <arpa/inet.h>

using std::string;

#if defined ( SCDF_TEST ) || defined (SCDF_PLOT)
std::string scdf::GetPatchesDirectory() {return scdf::GetUserDataDirectory() + "/patches";}
#endif

std::string getIPAddress()
{
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return [address UTF8String];
    
}

std::vector<int> ipAddressStringToVector(std::string address)
{
     std::vector<int> ipValues;
    
    if (address.compare("error")!=0)
    {
        std::string delimiter = ".";
        
        size_t pos = 0;
        std::string token;
        while ((pos = address.find(delimiter)) != std::string::npos) {
            token = address.substr(0, pos);
            //std::cout << token << std::endl;
            ipValues.push_back(atoi(token.c_str()));
            address.erase(0, pos + delimiter.length());
        }
        
        ipValues.push_back(atoi(address.c_str()));
    } else
    {
        ipValues.push_back(127);
        ipValues.push_back(0);
        ipValues.push_back(0);
        ipValues.push_back(1);
    }
    
    assert(ipValues.size()==4);
    
    return ipValues;
}


std::vector<int> getIPAddressAsVector()
{
    return ipAddressStringToVector(getIPAddress());
}



bool IsTablet()
{
    if ( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad )
    {
        return true; 
    }
    return false;
}

namespace scdf {

    
    string GetDirectoryForFile(string file, string ext)
    {
        NSString* _file = [NSString stringWithUTF8String:file.c_str()];
        NSString* _ext = [NSString stringWithUTF8String:ext.c_str()];
        NSString* filePath = [[NSBundle mainBundle] pathForResource:_file
                                                             ofType:_ext];
        if (nil==filePath) return "";
        
        return [filePath UTF8String];
    }
    string GetAppDataDirectory()
    {
        NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString *appSupportDirectory = [paths objectAtIndex:0];
        
        NSString *fullPath = [appSupportDirectory stringByAppendingPathComponent:bundleIdentifier];
        
        NSError *error;
        BOOL isDir;
        
        if(![[NSFileManager defaultManager] fileExistsAtPath:appSupportDirectory isDirectory:&isDir])
            [[NSFileManager defaultManager] createDirectoryAtPath:appSupportDirectory withIntermediateDirectories:YES attributes:nil error:&error];
        
        if(![[NSFileManager defaultManager] fileExistsAtPath:fullPath isDirectory:&isDir])
            [[NSFileManager defaultManager] createDirectoryAtPath:fullPath withIntermediateDirectories:YES attributes:nil error:&error];
        
#ifdef _DEBUG
        BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:fullPath isDirectory:&isDir];
        assert(exists);
#endif
        
        
        return [fullPath UTF8String];
    }
    std::string GetUserDataDirectory()
    {
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        
#ifdef _DEBUG
        BOOL isDir;
        BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:documentsDirectory isDirectory:&isDir];
        assert(exists);
#endif

        return [documentsDirectory UTF8String];
    }
    bool CreateDirectory(std::string path)
    {
        NSString* directoryPath = [NSString stringWithUTF8String:path.c_str()];
        NSError *error;
        
        bool result=true;
        if (![[NSFileManager defaultManager] createDirectoryAtPath:directoryPath
                                       withIntermediateDirectories:NO
                                                        attributes:nil
                                                             error:&error])
        {
            std::string errorString=[error.localizedDescription UTF8String];
            LOGD("Create directory error: %s\n", errorString.c_str());
            result=false;
        }
        return result;
    }
    
    bool ListFilesInDirectory(std::string folderPath, std::vector<std::string>& fileList)
    {
        NSString* _directoryPath = [NSString stringWithUTF8String:folderPath.c_str()];
        NSError * error;
        NSArray * directoryContents =  [[NSFileManager defaultManager]
                                        contentsOfDirectoryAtPath:_directoryPath error:&error];
        
        for (int i=0;i<[directoryContents count];++i)
        {
            fileList.push_back([directoryContents[i] UTF8String]);
        }
        return true;
    }
    bool DeleteFile(std::string filename)
    {
        NSError *error;
        NSFileManager *fileMgr = [NSFileManager defaultManager];
        NSString* patchesPath = [NSString stringWithUTF8String:GetPatchesDirectory().c_str()];
        NSString *filePath = [patchesPath stringByAppendingPathComponent:[NSString stringWithUTF8String:filename.c_str()]];
        return [fileMgr removeItemAtPath:filePath error:&error];
    }
    bool DoesFileExist(std::string filename)
    {
        NSString* patchesPath = [NSString stringWithUTF8String:GetPatchesDirectory().c_str()];
        NSString* file = [patchesPath stringByAppendingPathComponent:[NSString stringWithUTF8String:filename.c_str()]];
        BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:file];
        return fileExists==TRUE;
    }
}