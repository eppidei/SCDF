//
//  SensorsManager.h
//  SCDF
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "TypeDefinitions.h"
#include "Sensor.h"

namespace scdf
{
    
    class Sensor;
    class SensorSettings;
    class SensorsManager
    {
        std::vector<int> wereActive;
    public:

    	friend SensorsManager *theSensorManager();

    	s_bool IsSensorAvailable(SensorType type);
        Sensor *CreateSensor(SensorType type);
        s_bool DestroySensor(SensorType type);

        s_bool InitSensor(SensorType type, SensorSettings &settings);
        s_bool StartSensor(SensorType type);
        s_bool StopSensor(SensorType type);

        void CreateAllSensors();
        void DestroyAllSensors();

        s_int32 GetRate(SensorType type);
        s_int32 GetNumFramesPerCallback(SensorType type);
        s_int32 GetNumChannels(SensorType type);
        s_bool SensorActivated(SensorType type);
        
        void SetRate(SensorType type, s_int32 rate);
        void SetBufferSize(SensorType type, s_int32 size);

        s_bool StopAllSensors();
        s_bool StartAllSensors();
        s_bool StartPrecActiveSensors();
       
    private:

        SensorsManager(){}
        typedef std::map<SensorType, Sensor*>::iterator SensorsIterator;
        typedef std::pair<SensorType, Sensor*> SensorPair;
        std::map<SensorType, Sensor*> sensors;
        Sensor *GetSensor(SensorType type);

    };

    SensorsManager *theSensorManager();

}
