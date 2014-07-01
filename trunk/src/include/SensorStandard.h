#ifndef SCDF_SENSOR_STANDARD_H
#define SCDF_SENSOR_STANDARD_H

#include "Sensor.h"

namespace scdf {

    class SensorStandardImpl;
    
    class SensorStandard : public Sensor
    {
        SensorStandardImpl *sImpl;

    public:

        SensorStandard(){}
        SensorStandard(SensorType type);
        
        s_bool  Setup(SensorSettings& settings);
        s_bool  Start();
        s_bool  Stop();
        s_int32 GetRate();

    };  

}

#endif
