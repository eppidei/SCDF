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


class ControlUnitPatch {

public:

	std::vector< std::unique_ptr<ControlUnit> > units;

	bool LoadFromFile(std::string file)
	{
		return false;
	}

	bool SaveToFile(std::string file)
	{
		return false;
	}

private:

	friend class cereal::access;

	template<class Archive>	void serialize(Archive & ar, std::uint32_t const version)
	{
		ar(CEREAL_NVP(units));
	}

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
