//
//  SensorsManager.h
//  SCDF
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "TypeDefinitions.h"
#include <map>

namespace scdf{
    
    class Sensor;
    class SensorSettings;
    class SensorsManager
    {
        typedef std::map<SensorType, Sensor*>::iterator SensorsIterator;
        typedef std::pair<SensorType, Sensor*> SensorPair;
        std::map<SensorType, Sensor*> sensors;
        Sensor *GetSensor(SensorType type);
    public:
        Sensor *CreateSensor(SensorType type);
        void InitSensor(SensorType type, SensorSettings settings);
        void ActivateSensor(SensorType type);
        void DeActivateSensor(SensorType type);
    };

}
