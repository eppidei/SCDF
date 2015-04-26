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

void ControlUnitDsp::SendValue(float normValue)
{
    lastValue = normValue;
    GetSender()->SendValue(GetValue());
    UpdateUI();
}

void ControlUnitDsp::InitADEContext(ADE_UINT32_T algoFlag, ADE_UINT32_T in_buff_len, ADE_FLOATING_T input_rate)
{
    if (ADEcontext) return;
    ADE_Init(&ADEcontext, algoFlag, in_buff_len, input_rate);
    ADE_Configure_params(ADEcontext, algoFlag);
}
void ControlUnitDsp::ReleaseADEContext(ADE_UINT32_T algoFlag)
{
    if (!ADEcontext) return;
    ADE_Release(ADEcontext, algoFlag);
    ADEcontext=NULL;
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
    ReleaseADEContext(BLOW_FLAG);
}

void ControlUnitBlow::Init(s_int32 numFrames, s_int32 rate)
{
    InitADEContext(BLOW_FLAG, numFrames, rate);
}

void ControlUnitBlow::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    assert(ADEcontext!=NULL);
    ADE_SCDF_Input_Int_T sensorData;
    
    GetSensorData(buffer, scdf::AudioInput, sensorData);
    
    ADE_Step(ADEcontext,BLOW_FLAG,&sensorData);
    ADE_SCDF_Output_Int_T *output=ADE_GetOutBuff(ADEcontext,BLOW_FLAG);
    
    //LOGD("BLOW STATE %d\n",s);
    switch (receiverType)
    {
        case ReceiverType_stream:
            for (int i=0;i<output->n_data;++i)
            {
                //LOGD("BLOW DATA %f\n",output->p_data[i]);
                SendValue(output->p_data[i]);
            }
            break;
        case ReceiverType_state:
        {
            int v= output->state ? 1 : 0;
            SendValue(v);
        }
            break;
        case ReceiverType_toggle:
            break;
        default:
            break;
    }
}

//SNAP
void ControlUnitSnap::Release()
{
    ReleaseADEContext(SNAP_FLAG);
}

void ControlUnitSnap::Init(s_int32 numFrames, s_int32 rate)
{
    InitADEContext(SNAP_FLAG, numFrames, rate);
}

void ControlUnitSnap::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    assert(ADEcontext!=NULL);
    
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
