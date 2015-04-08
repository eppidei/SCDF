/*
 * OsUtilities.cpp
 *
 *  Created on: 2 Apr 2015
 *      Author: athos
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "OsUtilities.h"
#include "Logging.h"

bool scdf::CreateDirectory(std::string name)
{
	#ifndef ANDROID
	mode_t nmode = S_IRWXU | S_IRWXG | S_IRWXO | S_IWRITE | S_IEXEC;
	#else
	mode_t nmode = S_IRWXU | S_IRWXG | S_IRWXO;
	#endif
	int res = mkdir(name.c_str(),nmode);
	if (res!=0) LOGE("ERROR CREATING DIR: %s", strerror(errno));
	return res==0;
}


bool scdf::SetCurrentDirectory(std::string path)
{
	int res = chdir(path.c_str());
	if (res != 0) LOGE("ERROR setting current dir (%s): %s",path.c_str(),strerror(errno));
	return res==0;
}


bool scdf::DoesFileExist(std::string filename)
{
	return access( filename.c_str(), F_OK ) != -1;
	// more info, more overhead:
	//struct stat sbuf;
	//int serr=stat(filename.c_str(), &sbuf);
	//return serr==0;
}

bool scdf::DeleteFile(std::string filename)
{
	int ret = remove(filename.c_str());
	if( ret!=0 )LOGE( "Error deleting file %s: %s",filename.c_str(),strerror(errno));
	return ret==0;
}

bool scdf::ListFilesInDirectory(std::string dirPath, std::vector<std::string>& fileList)
{
	fileList.clear();
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dirPath.c_str())) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		  if ( ent->d_type == DT_DIR ) continue;
		  fileList.push_back(ent->d_name);
	  }
	  closedir (dir);
	  return true;
	}
	return false;
}

/*


bool scdf::IsDirectory(std::string path, bool &isDirectory)
{
	struct stat sbuf;
	int serr=stat(path.c_str(), &sbuf);
	if(serr!=0) return false;
	isDirectory=S_ISDIR(sbuf.st_mode);
	return true;
}

template<bool foldersOnly> bool DoGetFolderFiles(tstring path, vector<tstring> &v)
{
	DIR *dirp=NULL;
	struct dirent *dp=NULL;

	std:vector<tstring> result;

	if ((dirp = opendir(path.c_str())) == NULL) {

		return false;
	}
	do {
		errno = 0;
		if ((dp = readdir(dirp)) != NULL) {

			if (dp->d_type==DT_DIR) {
				if(!foldersOnly) continue;
			} else if(foldersOnly) continue;
			tstring name(dp->d_name);
			tstring pathComplete = name;
			if(pathComplete==_T("..")||pathComplete==_T(".")) continue;

			result.push_back(pathComplete);
		}
	} while (dp != NULL);

	if (errno != 0)
	{
		//LOGE("error reading directory");
	}
	closedir(dirp);
	v=result;
	return true;
}

vector<tstring> GetFolderFiles(tstring path)
{
	vector<tstring> v;
	DoGetFolderFiles<false>(path, v);
	return v;
}
vector<tstring> GetFolderSubfolders(tstring path)
{
	vector<tstring> v;
	DoGetFolderFiles<true>(path, v);
	return v;
}


static void scdf::SplitPath(const std::string& in_path, std::vector<std::string>& split_path)
{
    size_t start = 0;
    size_t dirsep;
    do
    {
        dirsep = in_path.find_first_of("\\/", start);
        if (dirsep == std::string::npos)
            split_path.push_back(std::string(&in_path[start]));
        else if(dirsep !=0)
            split_path.push_back(std::string(&in_path[start], &in_path[dirsep]));
        start = dirsep + 1;
    } while (dirsep != std::string::npos);
}

std::string GetExtension(std::string &filename)
{
	size_t index=filename.find_last_of(".");
	if(index==string::npos) return filename;
	int indexPathSep=filename.find_last_of(PathSeparator);
	if(indexPathSep!=string::npos&&(indexPathSep>(int)index)) return filename;
	return filename.substr(index);
}


tstring GetExtensionWithoutDot(tstring extensionWithDot)
{
	size_t indexDot=extensionWithDot.find_last_of(_T('.'));
	if(string::npos==indexDot)
		return _T("");
	return extensionWithDot.substr(min((size_t)extensionWithDot.length()-1, indexDot+1));
}
*/






