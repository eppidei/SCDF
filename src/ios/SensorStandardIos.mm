//
//  SensorStandardIos.cpp
//
//  Created by Marco Bertola on 07/06/14.
//
//

#import "Sensor.h"
#import "SensorStandard.h"
#import "AccelerometerIos.h"
#import "SensorStandardIosImpl.h"


using  scdf::SensorStandard;
using  scdf::SensorType;

namespace scdf {

class SensorStandardImpl : public SensorStandard {
    
    
    SensorStandardIosImpl *sensorImpl;
public:
    
    SensorStandardImpl(SensorType type, ScdfPipe *destPipe)
    {
        switch (type) {
            case scdf::Accelerometer:
                sensorImpl = [[AccelerometerIos alloc] init];
                [sensorImpl AssingPipe:destPipe];
                break;
            case scdf::Gyroscope:
                //
                break;
            case scdf::Magnetometer:
                //
                break;
            case scdf::Proximity:
                //
                break;
                
            default:
                sensorImpl = nil;
                break;
        }
    }
    SensorStandardImpl()
    {
        if(sensorImpl){
            [sensorImpl release];
            sensorImpl = nil;
        }
    }
    s_bool SetupImpl(scdf::SensorSettings settings)
    {
        [sensorImpl Setup:settings];
        return true;
    }
    s_bool Start()
    {
        return [sensorImpl Start];
    }
    s_bool Stop()
    {
        return [sensorImpl Stop];
    }
    
};
}


static SensorStandard *sensorStandardImpl = NULL; // TODO: When I have to delete this?

SensorStandard* scdf::SensorStandard::Create(SensorType type, ScdfPipe *destPipe)
{
    // if sensor is not available return null
    // otherwise, create it 
    sensorStandardImpl = new SensorStandardImpl(type,destPipe);
    // ios specific setup, store the pipe
    // return the just created sensor
    return sensorStandardImpl;
}

s_bool scdf::SensorStandard::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return sensorStandardImpl->Setup(settings);
}

s_bool scdf::SensorStandard::Start()
{
    return sensorStandardImpl->Stop();
}

s_bool scdf::SensorStandard::Stop()
{
     return sensorStandardImpl->Start();
}

