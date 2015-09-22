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

std::map<ControlUnit::Type, ControlUnit*> ControlUnit::activeUnits;
std::vector<ControlUnit*> ControlUnit::activeItemsUnits;
scdf::ThreadUtils::CustomMutex ControlUnit::controlUnitItemLock;

void ControlUnit::AttachUnit(ControlUnit* unit)
{
    if (NULL==unit) return;
    
    auto it=activeUnits.find(unit->GetType());
    
    if (it==activeUnits.end())
    {

        std::vector<scdf::SensorType> _typeList;
        switch (unit->GetType())
        {
            case ControlUnit::Blow:
            case ControlUnit::Snap:
                _typeList.push_back(scdf::AudioInput);
                break;
            case ControlUnit::Proximity:
                _typeList.push_back(scdf::Proximity);
                break;
            default:
                break;
        }
        activeUnits[unit->GetType()]=Create(unit->GetType());
        scdf::theSensorAPI()->AttachHarvesterListener(activeUnits[unit->GetType()], _typeList);
    }
    
    scdf::ThreadUtils::AutoLock kk(&controlUnitItemLock);
    activeItemsUnits.push_back(unit);
}

void ControlUnit::DetachUnit(ControlUnit* unit)
{
    if (NULL==unit) return;
    
    int unitTypeNum=0;
    int unitIndex=-1;
    for (int i=0;i<activeItemsUnits.size();++i)
    {
        if (activeItemsUnits[i]==unit)
            unitIndex=i;
        if (activeItemsUnits[i]->GetType()==unit->GetType())
            unitTypeNum++;
    }

    if(unitTypeNum==1)
    {
        scdf::theSensorAPI()->DetachHarvesterListener(activeUnits.find(unit->GetType())->second);
        delete activeUnits.find(unit->GetType())->second;
        activeUnits.erase(activeUnits.find(unit->GetType()));
    }

    if (-1==unitIndex) return;
    scdf::ThreadUtils::AutoLock kk(&controlUnitItemLock);
    activeItemsUnits.erase(activeItemsUnits.begin()+unitIndex);
}

ControlUnit* ControlUnit::Create(Type t)
{
	switch (t) {
		case Wire: return new ControlUnitWire();
		case Blow : return new ControlUnitBlow();
        case Snap : return new ControlUnitSnap();
        case Proximity : return new ControlUnitProximity();
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
	interface = NULL;
    receiverType=ReceiverType_stream;
}

ControlUnit::~ControlUnit()
{
}

void ControlUnit::SetMidiMessageType(MidiMessageType type)
{
    if (type==PitchBend)
        SetMax(pow(2,14));
    else
        SetMax(127);
    sender->SetMidiMessageType(type);
}

MultiSender* ControlUnit::GetSender()
{
	return sender.get();
}

bool ControlUnitWire::OnTouch(TouchEvent ev, float normValue)
{
	normVal = normValue;
	GetSender()->SendValue(GetValue());
	return true;
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

void ControlUnitDsp::TrySendValue(float normValue)
{
    lastValue = fmin(1,fmax(0,normValue));
    if (lastSentValue==GetValue()) return;
    lastSentValue = GetValue();
    GetSender()->SendValue(lastSentValue);
    UpdateUI();
}

void ControlUnitDsp::Release()
{
	return;
    if (!ADEcontext) return;
    LOGD("4\n");
    ADE_Release(ADEcontext, GetAlgoFlag());
    ADEcontext=NULL;
}

void ControlUnitDsp::Init(s_int32 numFrames, s_int32 rate)
{
    if (ADEcontext) return;
    ADE_Init(&ADEcontext, GetAlgoFlag(), numFrames, rate);
    ADE_Configure_params(ADEcontext, GetAlgoFlag(),rate);
}

void GetSensorData(std::vector<scdf::SensorData*> *harvestedData, scdf::SensorType packetType, ADE_SCDF_Input_Int_T &sensorData)
{
    for (int i=0;i<harvestedData->size();++i)
    {
        if ((*harvestedData)[i]->type==packetType)
        {
            sensorData.type=(ADE_SensorType_T)(*harvestedData)[i]->type;
            sensorData.rate=(*harvestedData)[i]->rate;
            sensorData.timeid=(*harvestedData)[i]->timeid;
            sensorData.num_frames=(*harvestedData)[i]->num_frames;
            sensorData.num_channels=(*harvestedData)[i]->num_channels;
            sensorData.timestamp=(*harvestedData)[i]->timestamp;
            sensorData.data=(*harvestedData)[i]->data;
            return;
        }
    }
}

void ControlUnitDsp::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    assert(ADEcontext!=NULL);
    if (!ADEcontext) return;
    
    scdf::ThreadUtils::AutoLock kk(&controlUnitItemLock);
    
    ADE_SCDF_Input_Int_T sensorData;
    
    GetSensorData(buffer, GetSensorTypeRequired(), sensorData);
    
    ADE_Step(ADEcontext,GetAlgoFlag(),&sensorData);
    ADE_SCDF_Output_Int_T *output=ADE_GetOutBuff(ADEcontext,GetAlgoFlag());
    
    for(int i=0;i<activeItemsUnits.size();++i)
    {
        if (activeItemsUnits[i]->GetType()==GetType())
            ((ControlUnitDsp*)activeItemsUnits[i])->OnHarvesterBufferReadyInstance(output);
    }
}

// BLOW
void ControlUnitBlow::OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output)
{
    switch (receiverType)
    {
        case ReceiverType_stream:
            for (int i=0;i<output->n_data;++i)
                TrySendValue(output->p_data[i]);
            break;
        case ReceiverType_state:
        {
            int v= output->state ? 1 : 0;
            TrySendValue(v);
        }
            break;
        case ReceiverType_toggle:
        {
            int v= output->toggle ? 1 : 0;
            TrySendValue(v);
        }
            break;
        default:
            break;
    }
}

//SNAP
void ControlUnitSnap::OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output)
{
    switch (receiverType)
    {
        case ReceiverType_state:
        {
            int v= output->state ? 1 : 0;
            TrySendValue(v);
        }
            break;
        case ReceiverType_toggle:
        {
            int v= output->toggle ? 1 : 0;
            TrySendValue(v);
        }
            break;
        default:
            break;
    }
}

//PROXIMITY
void ControlUnitProximity::OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output)
{
    switch (receiverType)
    {
        case ReceiverType_state:
        {
            int v= output->state ? 1 : 0;
            TrySendValue(v);
        }
            break;
        case ReceiverType_toggle:
        {
            int v= output->toggle ? 1 : 0;
            TrySendValue(v);
        }
            break;
        default:
            break;
    }
}
