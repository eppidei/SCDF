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

#include "headers/ADE.h"
#include "ThreadUtils.h"


namespace ScdfCtrl {

class ItemBase;

    class ControlUnitInterface
    {
    public:
        virtual void OnValueChanged()=0;
    };
    
    class ControlUnit : public scdf::HarvesterListener
    {
    public:
        ControlUnitInterface *interface;
        void SetInterface(ControlUnitInterface *_interface) {interface=_interface;}
        
        //enum mirrored with PurchaseProductIndex enum
        enum Type { Wire, Blow, Snap };
        
        enum ReceiverType { ReceiverType_stream, ReceiverType_state, ReceiverType_toggle};

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
            
        void SetMidiMessageType(MidiMessageType type);
        void SetReceiverType(ReceiverType rt) { receiverType=rt;}
        ControlUnit();
        virtual ~ControlUnit();

        static void AttachUnit(ControlUnit* unit);
        static void DetachUnit(ControlUnit* unit);
    protected:
        
        static std::map<ControlUnit::Type, ControlUnit*> activeUnits;
        static std::vector<ControlUnit*> activeItemsUnits;
        static scdf::ThreadUtils::CustomMutex controlUnitItemLock;
        
        std::unique_ptr<MultiSender> sender;

        int min;
        int max;
        ReceiverType receiverType;

        void UpdateUI() {if (interface) interface->OnValueChanged();}
        virtual void OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer) {}
        virtual void Init(s_int32 numFrames, s_int32 rate) {}
        virtual void Release() {}
        
    private:


        friend class ::cereal::access;

        template <class Archive>
        void save( Archive & ar, std::uint32_t const version ) const
        {
            ar(CEREAL_NVP(sender));
            ar(CEREAL_NVP(min));
            ar(CEREAL_NVP(max));

    //		ar(CEREAL_NVP(x));
    //		ar(CEREAL_NVP(y));
    //		ar(CEREAL_NVP(w));
    //		ar(CEREAL_NVP(h));
        }

        template <class Archive>
        void load( Archive & ar, std::uint32_t const version )
        {
            ar(CEREAL_NVP(sender));
            ar(CEREAL_NVP(min));
            ar(CEREAL_NVP(max));

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

        ControlUnitWire() : ControlUnit() { normVal=0;}

    private:

        float normVal;

        void SetValue(float newValue);


        friend class ::cereal::access;

        template <class Archive>
        void save( Archive & ar, std::uint32_t const version ) const
        {
            ar(cereal::base_class<ControlUnit>( this ));
            ar(CEREAL_NVP(normVal));
        }

        template <class Archive>
        void load( Archive & ar, std::uint32_t const version )
        {
            ar(cereal::base_class<ControlUnit>( this ));
            ar(CEREAL_NVP(normVal));
        }
    };



    class ControlUnitDsp : public ControlUnit
    {
    public:
        bool OnTouch(TouchEvent ev, float normValue) override;
        float GetNormalizedValue() override;

        bool SetEnabled(bool enabled) override;
        bool IsEnabled() override;

        bool IsDSP() override { return true; }

        ControlUnitDsp() : ControlUnit() , ADEcontext(NULL), lastValue(0), lastSentValue(-1) { isEnabled = true;}

        void Init(s_int32 numFrames, s_int32 rate) override;
        void Release() override;
        void OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer) override;
        virtual ADE_UINT32_T GetAlgoFlag() = 0;
        
    protected:
        
        float lastValue;
        int lastSentValue;
        ADE_T *ADEcontext;
        
        void TrySendValue(float value);
        virtual void OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output) = 0;
    private:

        bool isEnabled;

        friend class ::cereal::access;

        template <class Archive>
        void save( Archive & ar, std::uint32_t const version ) const
        {
            ar(cereal::base_class<ControlUnit>( this ));
            ar(CEREAL_NVP(isEnabled));
            ar(CEREAL_NVP(lastValue));
        }

        template <class Archive>
        void load( Archive & ar, std::uint32_t const version )
        {
            ar(cereal::base_class<ControlUnit>( this ));
            ar(CEREAL_NVP(isEnabled));
            ar(CEREAL_NVP(lastValue));
        }
    };


    class ControlUnitBlow : public ControlUnitDsp
    {
    protected:
        void OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output) override;
    public:
        ControlUnitBlow() : ControlUnitDsp() {}
        Type GetType() { return Blow; }
        ADE_UINT32_T GetAlgoFlag() { return BLOW_FLAG;}

    private:

        friend class ::cereal::access;

        template <class Archive>
        void save( Archive & ar, std::uint32_t const version ) const
        {
            ar(cereal::base_class<ControlUnitDsp>( this ));
        }

        template <class Archive>
        void load( Archive & ar, std::uint32_t const version )
        {
            ar(cereal::base_class<ControlUnitDsp>( this ));
        }

    };

    class ControlUnitSnap : public ControlUnitDsp
    {
    protected:
        void OnHarvesterBufferReadyInstance(ADE_SCDF_Output_Int_T *output) override;
    public:
        ControlUnitSnap() : ControlUnitDsp() {}
        Type GetType() { return Snap; }
        ADE_UINT32_T GetAlgoFlag() { return SNAP_FLAG;}
        
    private:
        
        friend class ::cereal::access;
        
        template <class Archive>
        void save( Archive & ar, std::uint32_t const version ) const
        {
            ar(cereal::base_class<ControlUnitDsp>( this ));
        }
        
        template <class Archive>
        void load( Archive & ar, std::uint32_t const version )
        {
            ar(cereal::base_class<ControlUnitDsp>( this ));
        }
        
    };
} // ScdfCtrl namespace end

// Register Derived Classes (needed for polymorfism on serialization)
CEREAL_REGISTER_TYPE(ScdfCtrl::ControlUnit);
CEREAL_REGISTER_TYPE(ScdfCtrl::ControlUnitWire);
CEREAL_REGISTER_TYPE(ScdfCtrl::ControlUnitDsp);
CEREAL_REGISTER_TYPE(ScdfCtrl::ControlUnitBlow);

CEREAL_CLASS_VERSION(ScdfCtrl::ControlUnit,0);

#endif /* CONTROLUNIT_H_ */
