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
        s_int16 precision;
        s_uint64 timestamp;             // time of the sensor reading as reported by the sensor
        s_uint64 timeid;                // will be the same for all data harvested in the same call
        s_int32 num_samples;
        s_sample* data;
        
        SensorData() : data(NULL) {}
        ~SensorData() { if (data) delete data; }
    };
    
    class SensorAudioData : public SensorData {
    public:
        s_int32 numChannels;
    };

    class Sensor {

    	SensorType type;

    public:

        static Sensor* Create(SensorType type);
        static void Destroy(Sensor* sensor);
        
        virtual s_bool Setup(SensorSettings& settings) = 0;
        /* Returns false on setup failure.
         * If any of the settings parameter is not supported, it will be modified,
         * so that the user can retry the setup with the modified (supported) values.
         */
        
        virtual s_bool Start() = 0;
        virtual s_bool Stop()  = 0;
        
        void CALLBACK AddIncomingDataToQueue(SensorData *data);

    protected:
        
        SensorType GetType(){return type;};
        void SetType(SensorType _type){type = _type;}

       
    
    };
  
}