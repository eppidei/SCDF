//
//  SensorsManager.h
//  SCDF
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "TypeDefinitions.h"


#define DEFAULT_SENORS_STANDARD_RATE 100
#define DEFAULT_AUDIO_BUFFER_SIZE 512

namespace scdf
{
    
    class Sensor;
    class SensorSettings;
    class SensorsManager
    {

    public:

    	friend SensorsManager *theSensorManager();

        Sensor *CreateSensor(SensorType type);
        s_bool DestroySensor(SensorType type);

        s_bool InitSensor(SensorType type, SensorSettings &settings);
        s_bool StartSensor(SensorType type);
        s_bool StopSensor(SensorType type);

        void CreateAllSensors();
        void DestroyAllSensors();

        s_int32 GetRate(SensorType type);
        s_int32 GetNumSamples(SensorType type);
        
        void SetRate(SensorType type, s_int32 rate);

        s_bool StopAllSensors();
        s_bool StartAllSensors();
       
    private:

        SensorsManager(){}
        typedef std::map<SensorType, Sensor*>::iterator SensorsIterator;
        typedef std::pair<SensorType, Sensor*> SensorPair;
        std::map<SensorType, Sensor*> sensors;
        Sensor *GetSensor(SensorType type);

    };

    SensorsManager *theSensorManager();

}
