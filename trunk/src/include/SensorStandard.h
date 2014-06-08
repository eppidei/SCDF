#include "Sensor.h"

namespace scdf {
    
    class SensorStandard : public Sensor {
        
    public:

        static SensorStandard* Create(SensorType type);
        s_bool Setup(SensorSettings settings);        
        s_bool Start();
        s_bool Stop();

    };  

}
