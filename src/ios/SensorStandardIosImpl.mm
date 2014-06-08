//
//  SensorStandardIosImpl.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIosImpl.h"



@implementation SensorStandardIosImpl

static CMMotionManager *motionManager = nil;



+ (CMMotionManager *) InitMotionManager
{
    if(!motionManager)
    {
        motionManager = [[CMMotionManager alloc] init];
    }
    return motionManager;
}

+ (CMMotionManager *) GetMotionManager;
{
    return motionManager;
}

- (s_bool) Start { return false; }
- (s_bool) Stop { return false; }
- (s_bool) Setup:(scdf::SensorSettings) settings {return false; }
//- (void)   AssingPipe: (scdf::ScdfPipe *) destPipe{}



@end
