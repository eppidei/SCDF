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

using std::string;


bool IsTablet()
{
    if ( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad )
    {
        return true; 
    }
    return false;
}


s_uint64 now_ns()
{
    return  mach_absolute_time();
    
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
}