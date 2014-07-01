#ifndef SCDF_SENSOR_H
#define SCDF_SENSOR_H

#include "Sensor.h"

namespace scdf {

    class SensorAudioInputImpl;
    class SensorAudioInput : public Sensor
    {
        SensorAudioInputImpl *sImpl;
    public:
        SensorAudioInput(){}
        SensorAudioInput(SensorType type);
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();
        s_int32 GetRate();

    };

}

#endif
