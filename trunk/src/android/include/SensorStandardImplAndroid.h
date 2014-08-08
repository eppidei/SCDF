#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "SensorStandard.h"

class ASensorEventQueue;
class ASensor;

namespace scdf {

	class SensorStandardImpl : public SensorStandard
    {

    private:

		ASensorEventQueue* sensorEventQueue;
		const ASensor* androidSensor;
		s_int16 androidType;
		s_int32 currentRate;

    public:

		static s_bool IsAvailable(SensorType type);
		static SensorType AndroidToScdfType(int andType);
		static int ScdfToAndroidType(SensorType scdfType);

        SensorStandardImpl(SensorType _type);
        ~SensorStandardImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();
        s_int32 GetRate();
        s_int32 GetNumFramesPerCallback();
        s_int32 GetNumChannels();

        static int Callback(int fd, int events, void* data);

    };

}

#endif
