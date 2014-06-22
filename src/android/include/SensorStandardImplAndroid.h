#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "Sensor.h"

namespace scdf {

    class SensorStandardImpl //: public SensorStandard
    {

    public:

        SensorStandardImpl(SensorType _type);
        ~SensorStandardImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();

    };

}

#endif
