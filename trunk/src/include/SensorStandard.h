#include "Sensor.h"

namespace scdf {
    class SensorStandardImpl;
    
    class SensorStandard : public Sensor
    {
        SensorStandardImpl *sImpl;
        
    public:

        SensorStandard(){}
        SensorStandard(SensorType type);
        s_bool Setup(SensorSettings settings);        
        s_bool Start();
        s_bool Stop();

    };  

}
