#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "SensorStandard.h"

class ASensorEventQueue;

namespace scdf {

	class SensorStandardImpl : public SensorStandard
    {
    private:
		ASensorEventQueue* sensorEventQueue;

    public:

        SensorStandardImpl(SensorType _type);
        ~SensorStandardImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();

        static int Callback(int fd, int events, void* data);

    };

}

#endif
