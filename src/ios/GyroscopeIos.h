//
//  GyroscopeIos.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIos.h"

@interface GyroscopeIos : SensorStandardIos

{
    CMMotionManager *motionManager;
}


- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings) settings;

@end
