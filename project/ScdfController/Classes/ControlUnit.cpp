/*
 * ControlUnit.cpp
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#include "ControlUnit.h"
#include <sstream>
#include "MultiSender.h"
#include "SCDFCItems.h"

using namespace ScdfCtrl;

ControlUnit* ControlUnit::Create(Type t)
{
	switch (t) {
		case Wire: return new ControlUnitWire();
		case Blow : return new ControlUnitBlow();
		default: return nullptr;
	}
}

void ControlUnit::Destroy(ControlUnit* cu)
{
	delete cu;
}


ControlUnit::ControlUnit()
{
	sender.reset(new MultiSender());
	min = 0;
	max = 127;
	itemView = NULL;
}

ControlUnit::~ControlUnit()
{

}

MultiSender* ControlUnit::GetSender()
{
	return sender.get();
}

void ControlUnit::SetItem(ItemBase* iv)
{
	itemView = iv;
}

ItemBase* ControlUnit::GetItem()
{
	return itemView;
}



bool ControlUnitWire::OnTouch(TouchEvent ev, float normValue)
{
	normVal = normValue;
	GetSender()->SendValue(GetValue());
}




// DSP

bool ControlUnitDsp::OnTouch(TouchEvent ev, float normValue)
{
	// ignore value, just toggle enable
	if (TouchUp==ev) { SetEnabled(!IsEnabled());
		return true;
	}
	return false;
}

float ControlUnitDsp::GetNormalizedValue()
{
	return lastValue;
}

bool ControlUnitDsp::SetEnabled(bool enabled)
{
	isEnabled = enabled;
	return true;
}

bool ControlUnitDsp::IsEnabled()
{
	return isEnabled;
}


// BLOW

void ControlUnitBlow::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{

}

