/* SaveSettings.cpp
 *  Created on: Jul 19, 2014
 *      Author: athos */

#include "SaveSettings.h"

using namespace scdf;

SaveSettings *SaveSettings::_instance=NULL;


void SaveSettings::SaveIntValue(s_int32 _value, std::string valueKey)
{

}
void SaveSettings::SaveStringValue(std::string _value, std::string valueKey)
{

}

void  SaveSettings::SaveBoolValue(s_bool _value, std::string valueKey)
{

}


//////////// ************* LOAD ****************** ////////////////

s_int32  SaveSettings::LoadIntValue(std::string valueKey)
{
   return 0;
}

std::string* SaveSettings::LoadStringValue(std::string valueKey)
{
    return new std::string("");
}

s_bool SaveSettings::LoadBoolValue(std::string valueKey)
{
	return true;
}




