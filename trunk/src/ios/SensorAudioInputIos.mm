//
//  SensorsAudioInputIos.cpp
//
//  Created by Marco Bertola on 07/06/14.
//
//

#import "Sensor.h"
#import "SensorAudioInput.h"


using scdf::SensorAudioInput;
using scdf::SensorType;
using scdf::ScdfPipe;

namespace scdf {
    
    class SensorAudioInputImpl : public SensorAudioInput {
        
    public:
        
        SensorAudioInputImpl(SensorType type, ScdfPipe *destPipe)
        {
            
        }
        ~SensorAudioInputImpl()
        {
            
        }
        s_bool SetupImpl(scdf::SensorSettings settings)
        {
            return true;
        }
        s_bool Start()
        {
            return false;
        }
        s_bool Stop()
        {
            return false;
        }
        
    };

}

SensorAudioInput* scdf::SensorAudioInput::Create(SensorType type, ScdfPipe *destPipe)
{
    // if sensor is not available return null
    // otherwise, create it 
    SensorAudioInput* s = new SensorAudioInputImpl(type,destPipe);
    // ios specific setup, store the pipe
    // return the just created sensor
    return s;
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return  true;
}

s_bool scdf::SensorAudioInput::Start()
{
    return true;
}

s_bool scdf::SensorAudioInput::Stop()
{
    return true;
}
