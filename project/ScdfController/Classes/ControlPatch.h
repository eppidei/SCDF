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



namespace ScdfCtrl {







class ControlPatch {

public:

	friend class cereal::access;

	int i;
	std::vector<float> f;
	char* c;
	uint32_t vers;

	std::vector<double*> pvec;

	ControlPatch();
	~ControlPatch();

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
	}

	template <class Archive>
	void load( Archive & ar, std::uint32_t const version )
	{
		ar(CEREAL_NVP(i));
		ar(CEREAL_NVP(f), CEREAL_NVP(*c) );
		vers = version;
	}

};

} // namespace end

CEREAL_CLASS_VERSION(ScdfCtrl::ControlPatch,1); // change version when you modify struct
// Object versioning, more at:
// http://uscilab.github.io/cereal/assets/doxygen/group__Utility.html
// http://uscilab.github.io/cereal/serialization_functions.html

#endif /* CONTROLPATCH_H_ */
