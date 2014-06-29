//
//  SensorStandardIosImpl.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import <UIKit/UIKit.h>
#import "Sensor.h"
#import "SensorStandard.h"

@class TimerIos;

namespace scdf {
    
    struct SensorsStandardIOSData
    {
        s_sample value1,value2,value3;
        s_double timestamp;
        
        SensorsStandardIOSData()
        {
            value1 = 0;
            value2 = 0;
            value3 = 0;
            timestamp = 0;
        }
        
    };
    
    class SensorStandardImpl : public SensorStandard
    {
        SensorType                  sensorTypeRef;
        
        static  CMMotionManager     *motionManager;
        TimerIos                    *timerProximity;
        NSTimeInterval updateInterval;
        
    public:
        void MySensorsCallback(SensorsStandardIOSData &sensorIOSData);
        SensorStandardImpl(SensorType _sensorType);
        ~SensorStandardImpl();
        s_bool Setup(scdf::SensorSettings settings);
        s_bool Start();
        s_bool Stop();
        
    };
}