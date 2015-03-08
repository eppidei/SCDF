/*
 * ControlPatch.cpp
 *
 *  Created on: 25 Jan 2015
 *      Author: athos
 */


#include "ControlPatch.h"
#include <sstream>
#include "Logging.h"
#include "OsUtilities.h"

ScdfCtrl::ControlPatch::ControlPatch()
{
	i = 2;
	c = new char;
	*c = 'a';
}

ScdfCtrl::ControlPatch::~ControlPatch()
{
	delete c;
}



void TestSerialization()
{
	std::string path1 = scdf::GetAppDataDirectory();
	std::string path2 = scdf::GetUserDataDirectory();
	LOGD("App: %s",path1.c_str());
	LOGD("App: %s",path2.c_str());

	std::stringstream ss; // any stream can be used
	{
		cereal::XMLOutputArchive oarchive(ss);
		ScdfCtrl::ControlPatch m1, m2, m3;
		*m1.c = 'b';
		m1.f.push_back(3.14);
		m2.i = 999;
		m3.f.push_back(6.667);
		m3.f.push_back(0.333);
		oarchive(m1, m2, m3); // Write the data to the archive
	}

	LOGD("ARCHIVE:/n %s",ss.str().c_str());

	{
		cereal::XMLInputArchive iarchive(ss); // Create an input archive
		ScdfCtrl::ControlPatch m1, m2, m3;
		iarchive(m1, m2, m3); // Read the data from the archive

		LOGD("m1: %c, %d, %f - v:%d",*m1.c,m1.i,m1.f[0],m1.vers);
		LOGD("m2: %c, %d, f size: %d - v:%d",*m2.c,m2.i,m2.f.size(),m2.vers);
		LOGD("m3: %c, %d, %f, %f - v:%d",*m3.c, m3.i,m3.f[0],m3.f[1],m3.vers);

	}
}

