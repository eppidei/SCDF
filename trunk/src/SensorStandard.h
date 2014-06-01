#include "Sensor.h"

namespace scdf {

    class SensorStandard : public Sensor {
        
    public:

        SensorStandard* Create(SensorType type, Pipe destPipe);
        s_bool Setup(SensorSettings* settings);        
        s_bool Start();
        s_bool Stop();

    };  

}
