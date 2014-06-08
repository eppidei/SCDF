//
//  SensorStandardIos.h
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import "TypeDefinitions.h"
#import "Sensor.h"



@interface SensorStandardIos : NSObject

{
    scdf::Sensor  *sensorRef;
}

+ (CMMotionManager *) InitMotionManager;
+ (CMMotionManager *) GetMotionManager;

- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings)  settings;

@end