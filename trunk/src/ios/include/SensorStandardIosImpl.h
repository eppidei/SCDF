//
//  SensorStandardIosImpl.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import "Sensor.h"
#import "SensorStandard.h"

//#import "SensorStandardIos.h"

namespace scdf {
    
    struct SensorsStandardIOSData
    {
        s_sample x,y,z;
        s_double timestamp;
    };
    
    class SensorStandardImpl : public SensorStandard
    {
        static CMMotionManager *motionManager;
        void MySensorsCallback(SensorsStandardIOSData &sensorIOSData);
    public:
        
        SensorStandardImpl();
        ~SensorStandardImpl();
        s_bool Setup(scdf::SensorSettings settings);
        s_bool Start();
        s_bool Stop();
        
        
    };
}