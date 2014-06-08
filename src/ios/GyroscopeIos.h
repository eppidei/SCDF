//
//  GyroscopeIos.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIosImpl.h"
#import "ScdfPipe.h"

@interface GyroscopeIos : SensorStandardIosImpl

{
    scdf::ScdfPipe pipe;
    CMMotionManager *motionManager;
    
}


- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings) settings;
- (void)   AssingPipe: (scdf::ScdfPipe *) destPipe;

@end
