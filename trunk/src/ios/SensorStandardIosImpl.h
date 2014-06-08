//
//  SensorStandardIosImpl.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "Sensor.h"
#import "SensorStandard.h"
#import "SensorStandardIos.h"

namespace scdf {
    
    class SensorStandardImpl : public SensorStandard
    {
        SensorStandardIos *sensorImpl;
        
    public:
        
        SensorStandardImpl(SensorType type);
        SensorStandardImpl();
        s_bool SetupImpl(scdf::SensorSettings settings);
        s_bool Start();
        s_bool Stop();
        
    };
}