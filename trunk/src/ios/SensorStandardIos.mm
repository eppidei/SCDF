//
//  SensorStandardIos.mm
//
//  Created by Marco Bertola on 07/06/14.
//
//



#import "SensorStandardIos.h"


@implementation SensorStandardIos

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