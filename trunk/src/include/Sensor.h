#pragma once

#include "TypeDefinitions.h"

namespace scdf {
    
    class Sensor;
    
    enum SensorType { Invalid=-1, Accelerometer, Gyroscope, Magnetometer, Proximity, Light, AudioInput, NumTypes };
    static const std::string SensorTypeString[]={"Accelerometer", "Gyroscope", "Magnetometer", "Proximity", "Light", "AudioInput" };
    static const s_int32 SensorTypeNumChannels[]={3, 3, 3, 1, 1, 1};


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

    class SensorData
    {
    public: 
        SensorType type;
        s_int32 rate;
        s_uint64 timeid;                // will be the same for all data harvested in the same call
        s_int32 num_frames;
        s_int32 num_channels;
        s_uint64 *timestamp;             // time of the sensor reading as reported by the sensor
        s_sample *data;
        
        SensorData(SensorType _type) : type(_type), data(NULL), timestamp(NULL), num_channels(0), num_frames(0), timeid(0), rate(0)
        {}
      /*  void Dealloc()
        {
            if (data)
                delete data;
            if (timestamp)
                delete timestamp;
        }*/
        ~SensorData()
        {
            if (data)
                delete data;
            if (timestamp)
                delete timestamp;
        }
    };
    
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
        SensorType GetType() { return type; }
        s_bool IsActive();

        // TODO: add getter for num channels (return 1 for standard sensors?)
        
        void CALLBACK AddIncomingDataToQueue(SensorData *data);

    protected:
        void SetActive(s_bool _active);
        void SetType(SensorType _type){type = _type;}
    
    };
  
}
