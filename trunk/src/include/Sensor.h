#pragma once

#include "TypeDefinitions.h"


#define CALLBACK
namespace scdf {
    
    enum SensorType { Invalid=-1, Accelerometer, Gyroscope, Magnetometer, Proximity, Light, AudioInput, NumTypes };

    class SensorSettings {
    public:
        s_int32 rate;
        s_bool broken;
    };

    class SensorData {
    public: 
        SensorType type;
        s_int32 rate;
        s_int16 precision;
        s_int64 timestamp;  // time of the sensor reading as reported by the sensor
        s_int64 timeid;     // will be the same for all data harvested in the same call
        void* data;         // TODO: actually a placeholder for now
    };

    class Sensor {

    public:

        static Sensor* Create(SensorType type);
        static void Destroy(Sensor* sensor);
        
        SensorType GetType();
        
        virtual s_bool Setup(SensorSettings settings) = 0;
        /* Returns false on setup failure.
         * If any of the settings parameter is not supported, it will be modified,
         * so that the user can retry the setup with the modified (supported) values.
         */
        
        virtual s_bool Start() = 0;
        virtual s_bool Stop()  = 0;
        
        void CALLBACK AddIncomingDataToQueue(SensorData *data);

    protected:

        SensorType type;
    
    };
  
}
