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

	ControlPatch();

	template<class Archive>	void serialize(Archive & archive)
	{
		archive( i, f );
	}

};





} // namespace end

#endif /* CONTROLPATCH_H_ */
