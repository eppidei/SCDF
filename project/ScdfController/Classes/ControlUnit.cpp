/*
 * ControlUnit.cpp
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#include "ControlUnit.h"
#include <sstream>
#include "MultiSender.h"
#include "SCDFCItems.h"
#include "headers/ADE.h"

using namespace ScdfCtrl;

ControlUnit* ControlUnit::Create(Type t)
{
	switch (t) {
		case Wire: return new ControlUnitWire();
		case Blow : return new ControlUnitBlow();
        case Snap : return new ControlUnitSnap();
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

//void ControlUnit::SetItem(ItemBase* iv)
//{
//	itemView = iv;
//}

//ItemBase* ControlUnit::GetItem()
//{
//	return itemView;
//}



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

// BLOW
void ControlUnitBlow::Release()
{
    if (!ADEcontext) return;
    ADE_Release(ADEcontext, BLOW_FLAG);
    ADEcontext=NULL;
}

void ControlUnitBlow::Init(s_int32 numFrames, s_int32 rate)
{
    if (!ADEcontext)
        ADE_Init(&ADEcontext, BLOW_FLAG, numFrames, rate);
}

void ControlUnitBlow::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    ADE_SCDF_Input_Int_T sensorData;
    
    GetSensorData(buffer, scdf::AudioInput, sensorData);
    
    ADE_Step(ADEcontext,BLOW_FLAG,&sensorData);
    ADE_SCDF_Output_Int_T *output=ADE_GetOutBuff(ADEcontext,BLOW_FLAG);

    int s=0;
    if (output->state)
        s=1;
    
    //LOGD("BLOW STATE %d\n",s);
    for (int i=0;i<output->n_data;++i)
    {
        LOGD("BLOW DATA %f\n",output->p_data[i]);
        lastValue=output->p_data[i];
        GetSender()->SendValue(output->p_data[i]);
        UpdateUI();
    }
}

//SNAP
void ControlUnitSnap::Release()
{
    ADE_Release(ADEcontext, SNAP_FLAG);
    ADEcontext=NULL;
}

void ControlUnitSnap::Init(s_int32 numFrames, s_int32 rate)
{
    if (!ADEcontext)
        ADE_Init(&ADEcontext, SNAP_FLAG, numFrames, rate);
}

void ControlUnitSnap::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    ADE_SCDF_Input_Int_T sensorData;
    
    GetSensorData(buffer, scdf::AudioInput, sensorData);
    
    ADE_Step(ADEcontext,SNAP_FLAG,&sensorData);
    ADE_SCDF_Output_Int_T *output=ADE_GetOutBuff(ADEcontext,SNAP_FLAG);

    int s=0;
    if (output->state)
        s=1;
    
    LOGD("SNAP STATE %d\n",s);

    lastValue=s;
    GetSender()->SendValue(s*GetMax());
    UpdateUI();
}
