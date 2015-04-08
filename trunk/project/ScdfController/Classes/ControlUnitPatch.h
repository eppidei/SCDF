/*
 * ControlPatch.h
 *
 *  Created on: 25 Jan 2015
 *      Author: athos
 */

#ifndef CONTROLPATCH_H_
#define CONTROLPATCH_H_

#include "cereal/archives/xml.hpp"
#include "cereal/access.hpp"
#include <vector>
#include "cereal/types/vector.hpp"
#include "cereal/types/memory.hpp"
#include <memory>
#include "SCDFCItems.h"
#include "ControlUnit.h"

namespace ScdfCtrl {

class SerializableItemData {

public:
	int id;
	std::string name;
	float x;
	float y;
	int magValue;
	int color;
	bool isVertical;
	ControlUnit* unit;

	//friend class cereal::access;

	template<class Archive>	void serialize(Archive & ar, std::uint32_t const version)
	{
		ar(CEREAL_NVP(id));
		ar(CEREAL_NVP(name));
		ar(CEREAL_NVP(*unit));
		ar(CEREAL_NVP(x));
		ar(CEREAL_NVP(y));
		ar(CEREAL_NVP(magValue));
		ar(CEREAL_NVP(isVertical));
		ar(CEREAL_NVP(color));
	}

	SerializableItemData(ItemBase* item)
	{
		id = item->GetID();
		name = item->GetName();
		unit = item->GetControlUnit();
		x = item->getPositionX();
		y = item->getPositionY();
		color = item->GetColor();
		magValue = item->GetLayoutManager()->GetMagValue();
		isVertical = item->GetLayoutManager()->IsVertical();
	}

	SerializableItemData()
	{
		id = -1;
		name = "";
		unit = NULL;
		x = 0;
		y = 0;
		color = 0;
		magValue = 1;
		isVertical = false;
	}


};


class ControlUnitPatch {

public:

	std::vector<ItemBase*> items;

	bool LoadFromFile(std::string patchName); // pass patch name only, without path!
	bool SaveToFile(std::string patchName);
private:

//	friend class cereal::access;

//	template<class Archive>	void serialize(Archive & ar, std::uint32_t const version)
//	{
//		//ar(CEREAL_NVP());
//	}

};



class CerealTest {

public:

	friend class cereal::access;

	int i;
	std::vector<float> f;
	char* c;
	uint32_t vers;

	std::vector<double*> pvec;

	CerealTest();
	~CerealTest();

	/*template<class Archive>	void serialize(Archive & archiv, std::uint32_t const version)
	{
		archiv(CEREAL_NVP(i));
		archiv(CEREAL_NVP(f), CEREAL_NVP(*c) );
		vers = version;
	}*/


	template <class Archive>
	void save( Archive & ar, std::uint32_t const version ) const
	{
		ar(CEREAL_NVP(i));
		ar(CEREAL_NVP(f), CEREAL_NVP(*c) );

		int pvecSize = pvec.size();
		ar(CEREAL_NVP(pvecSize));

		for (int i=0; i<pvecSize; i++)
			ar(CEREAL_NVP(*pvec[i]));
	}

	template <class Archive>
	void load( Archive & ar, std::uint32_t const version )
	{
		ar(CEREAL_NVP(i));
		ar(CEREAL_NVP(f), CEREAL_NVP(*c) );
		vers = version;

		int pvecSize;
		ar(CEREAL_NVP(pvecSize));

		pvec.clear(); // leaking previous values

		for (int i=0; i<pvecSize; i++)
		{
			pvec.push_back(new double);
			ar(CEREAL_NVP(*pvec[i]));
		}
	}

};

} // namespace end

CEREAL_CLASS_VERSION(ScdfCtrl::ControlUnitPatch,0);
CEREAL_CLASS_VERSION(ScdfCtrl::CerealTest,1); // change version when you modify struct
// Object versioning, more at:
// http://uscilab.github.io/cereal/assets/doxygen/group__Utility.html
// http://uscilab.github.io/cereal/serialization_functions.html

#endif /* CONTROLPATCH_H_ */
