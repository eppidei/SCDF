//
//  AccelerometerIos.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIos.h"
#import "ScdfPipe.h"


@interface AccelerometerIos : SensorStandardIos

{
    CMMotionManager *motionManager;
}


- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings) settings;


@end
