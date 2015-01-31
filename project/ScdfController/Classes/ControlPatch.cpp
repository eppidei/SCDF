/*
 * ControlPatch.cpp
 *
 *  Created on: 25 Jan 2015
 *      Author: athos
 */


#include "ControlPatch.h"
#include <sstream>
#include "Logging.h"


ScdfCtrl::ControlPatch::ControlPatch()
{
	i = 2;
	f.push_back(3.14);
	f.push_back(1.767);
}

void TestSerialization()
{
	std::stringstream ss; // any stream can be used
	{
		cereal::XMLOutputArchive oarchive(ss);
		ScdfCtrl::ControlPatch m1, m2, m3;
		m2.i = 999;
		m3.f.push_back(6.667);
		oarchive(m1, m2, m3); // Write the data to the archive
	}

	{
		cereal::XMLInputArchive iarchive(ss); // Create an input archive
		ScdfCtrl::ControlPatch m1, m2, m3;
		iarchive(m1, m2, m3); // Read the data from the archive

		LOGD("m1 %i, %f, %f",m1.i,m1.f[0],m1.f[1]);
		LOGD("m2 %i, %f, %f",m2.i,m2.f[0],m2.f[1]);
		LOGD("m3 %i, %f, %f",m3.i,m3.f[0],m3.f[1]);

		LOGD("YO: %s",ss.str().c_str());

	}
}

