#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "SensorStandard.h"

class ASensorEventQueue;

namespace scdf {

    class SensorStandardImpl : public SensorStandard
    {
    public:

    	ASensorEventQueue* sensorEventQueue; // make private and make callback a friend

        SensorStandardImpl(SensorType _type);
        ~SensorStandardImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();

    };

}

#endif
