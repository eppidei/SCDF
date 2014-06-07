#include "Sensor.h"

namespace scdf {
    
    class ScdfPipe;
    class SensorStandard : public Sensor {
        
    public:

        SensorStandard* Create(SensorType type, ScdfPipe *destPipe);
        s_bool Setup(SensorSettings settings);        
        s_bool Start();
        s_bool Stop();

    };  

}
