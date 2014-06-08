//
//  SensorStandardIosImpl.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import "TypeDefinitions.h"
#import "Sensor.h"


@interface SensorStandardIosImpl : NSObject

{
    
}

+ (CMMotionManager *) InitMotionManager;
+ (CMMotionManager *) GetMotionManager;

- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings)  settings;
- (void)   AssingPipe: (scdf::ScdfPipe *) destPipe;

@end
