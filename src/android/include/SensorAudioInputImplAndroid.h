/*
 * SensorAudioInputImplAndroid.h
 *
 *  Created on: Jun 22, 2014
 *      Author: athos
 */

#ifndef SENSORAUDIOINPUTIMPLANDROID_H_
#define SENSORAUDIOINPUTIMPLANDROID_H_

#include "TypeDefinitions.h"
#include "Sensor.h"

namespace scdf {

    class SensorAudioInputImpl // : public SensorStandard
    {

    public:

        SensorAudioInputImpl();
        ~SensorAudioInputImpl();
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();

    };

}



#endif /* SENSORAUDIOINPUTIMPLANDROID_H_ */
