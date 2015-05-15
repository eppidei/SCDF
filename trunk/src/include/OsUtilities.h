/*
 * OsUtilities.h
 *
 *  Created on: 31 Jan 2015
 *      Author: athos
 */

#ifndef OSUTILITIES_H_
#define OSUTILITIES_H_

#include <string>
#include <vector>

namespace scdf {

	std::string GetAppDataDirectory();
	std::string GetUserDataDirectory();
    std::string GetDirectoryForFile(std::string file, std::string ext);
    std::string GetPatchesDirectory();

	bool CreateDirectory(std::string name);
	bool SetCurrentDirectory(std::string path);
	bool DoesFileExist(std::string filename);
	bool DeleteFile(std::string filename);
	bool ListFilesInDirectory(std::string dirPath, std::vector<std::string>& fileList);
    
}

#endif /* OSUTILITIES_H_ */
