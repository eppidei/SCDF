/*
 * ControlUnit.h
 *
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#ifndef CONTROLUNIT_H_
#define CONTROLUNIT_H_

#include "TypeDefinitions.h"
#include "SCDFCDefinitions.h"


#include "Colors.h"
#include "ScdfSensorAPI.h"

#include <memory>

#include <cereal/types/polymorphic.hpp>
#include "cereal/archives/xml.hpp"
#include "cereal/access.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/string.hpp"

#include "MultiSender.h"

namespace ScdfCtrl {

class ItemBase;

class ControlUnit {

public:

	enum Type { Wire, Blow };

	static ControlUnit* Create(Type t);
	static void Destroy(ControlUnit* cu);

	enum TouchEvent { TouchDown, TouchUp, TouchMove, TouchCode };
	virtual bool OnTouch(TouchEvent ev, float normValue) = 0;

	virtual float GetNormalizedValue() = 0;

	virtual int GetValue() { return GetNormalizedValue()*(GetMax()-GetMin()) + GetMin(); }

	virtual void SetMin(int _min) { min = _min;}
	virtual void SetMax(int _max) { max = _max;}
	virtual int GetMin() { return min; }
	virtual int GetMax() { return max; }

	virtual void SetRange(int min, int max) { SetMin(min); SetMax(max);}

	virtual bool SetEnabled(bool enabled) = 0;
	virtual bool IsEnabled() = 0;

	virtual Type GetType() = 0;
	virtual bool IsDSP() = 0;

	MultiSender* GetSender();

	void SetItem(ItemBase* iv);
	ItemBase* GetItem();

	ControlUnit();
	virtual ~ControlUnit();

protected:

	std::unique_ptr<MultiSender> sender;

	ItemBase* itemView; // knows position, name, color, current value

	int min;
	int max;

private:


 	friend class ::cereal::access;

	template <class Archive>
	void save( Archive & ar, std::uint32_t const version ) const
	{
		ar(CEREAL_NVP(sender));

//		ar(CEREAL_NVP(x));
//		ar(CEREAL_NVP(y));
//		ar(CEREAL_NVP(w));
//		ar(CEREAL_NVP(h));
	}

	template <class Archive>
	void load( Archive & ar, std::uint32_t const version )
	{
//		ar(CEREAL_NVP(value));
//		ar(CEREAL_NVP(name));
//		ar(CEREAL_NVP(colorId));
//		ar(CEREAL_NVP(sender));
//
//		ar(CEREAL_NVP(itemViewId));
//		ar(CEREAL_NVP(x));
//		ar(CEREAL_NVP(y));
//		ar(CEREAL_NVP(w));
//		ar(CEREAL_NVP(h));
	}

};


class ControlUnitWire : public ControlUnit
{

public:

	bool OnTouch(TouchEvent ev, float normValue) override;

	float GetNormalizedValue() override { return normVal; }

	bool SetEnabled(bool enabled) override { return false; }
	bool IsEnabled() override { return true; }

	Type GetType() override { return Wire; }
	bool IsDSP() override { return false; }

	ControlUnitWire() : ControlUnit() {}

private:

	float normVal;

	void SetValue(float newValue);


//	friend class ::cereal::access;
//
//	template <class Archive>
//	void save( Archive & ar, std::uint32_t const version ) const
//	{
//	    ar(cereal::base_class<ControlUnit>( this ));
//		//ar(CEREAL_NVP(midiNote));
//	}
//
//	template <class Archive>
//	void load( Archive & ar, std::uint32_t const version )
//	{
//	    ar(cereal::base_class<ControlUnit>( this ));
//		//ar(CEREAL_NVP(midiNote));
//	}
};


class ControlUnitDsp : public ControlUnit, public scdf::HarvesterListener
{

public:

    //void OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
    //{    }

	bool OnTouch(TouchEvent ev, float normValue) override;
	float GetNormalizedValue() override;

	bool SetEnabled(bool enabled) override;
	bool IsEnabled() override;

	bool IsDSP() override { return true; }

	ControlUnitDsp() : ControlUnit() { isEnabled = true;}

protected:

	float lastValue;

private:

	bool isEnabled;

//	friend class ::cereal::access;
//
//	template <class Archive>
//	void save( Archive & ar, std::uint32_t const version ) const
//	{
//	    ar(cereal::base_class<ControlUnit>( this ));
//		//ar(CEREAL_NVP(midiNote));
//	}
//
//	template <class Archive>
//	void load( Archive & ar, std::uint32_t const version )
//	{
//	    ar(cereal::base_class<ControlUnit>( this ));
//		//ar(CEREAL_NVP(midiNote));
//	}
};


class ControlUnitBlow : public ControlUnitDsp
{
public:

    void OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer);
    ControlUnitBlow() : ControlUnitDsp() {}
    Type GetType() { return Blow; }

private:


};

} // ScdfCtrl namespace end

// Register Derived Classes (needed for polymorfism on serialization)
//CEREAL_REGISTER_TYPE(ScdfCtrl::ControlUnitDerived);

CEREAL_CLASS_VERSION(ScdfCtrl::ControlUnit,0);

#endif /* CONTROLUNIT_H_ */
