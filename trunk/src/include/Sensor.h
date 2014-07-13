#pragma once

#include "TypeDefinitions.h"

namespace scdf {
    
    class Sensor;
    
    enum SensorType { Invalid=-1, Accelerometer, Gyroscope, Magnetometer, Proximity, Light, AudioInput, NumTypes };
    static const std::string SensorTypeString[]={"Accelerometer", "Gyroscope", "Magnetometer", "Proximity", "Light", "AudioInput" };

    class SensorSettings {
    public:
        s_int32 rate;
        s_bool broken;
    };
    
    class SensorAudioSettings : public SensorSettings {
    public:
        s_int32 numChannels;
        s_int32 bufferSize;
    };

    class SensorData {
    public: 
        SensorType type;
        s_int32 rate;
        s_uint64 timestamp;             // time of the sensor reading as reported by the sensor
        s_uint64 timeid;                // will be the same for all data harvested in the same call
        s_int32 num_samples;
        s_int32 numChannels;
        s_sample* data;
        
        SensorData() : data(NULL), numChannels(0), num_samples(0), timestamp(0), timeid(0), rate(0) {}
        ~SensorData() { if (data) delete data; }
    };
    
    /*class SensorAudioData : public SensorData {
    public:
        s_int32 numChannels;
    };*/

    class Sensor {

    	SensorType type;
        s_bool active;

    public:

    	static s_bool IsAvailable(SensorType type);
        static Sensor* Create(SensorType type);
        static void Destroy(Sensor* sensor);
        
        virtual s_bool Setup(SensorSettings& settings) = 0;
        /* Returns false when something changed.
         * If the sensor could be set up with other settings, settings.broken
         * will be false
         */
        
        virtual s_bool Start() = 0;
        virtual s_bool Stop()  = 0;
        virtual s_int32 GetRate()  = 0;
        virtual s_int32 GetNumSamples()  = 0;
        virtual ~Sensor(){};
        s_bool IsActive() { return active; }
        SensorType GetType() { return type; }
        // TODO: add getter for num channels (return 1 for standard sensors?)
        
        void CALLBACK AddIncomingDataToQueue(SensorData *data);

    protected:
        void SetActive(s_bool _active) { active=_active;}
        void SetType(SensorType _type){type = _type;}
    
    };
  
}
