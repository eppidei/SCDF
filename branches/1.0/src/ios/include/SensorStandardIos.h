//
//  SensorStandardIos.h
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import "TypeDefinitions.h"
#import "Sensor.h"

namespace scdf {
    class Sensor;
}

@interface SensorStandardIos : NSObject

{
    scdf::Sensor  *_sensorRef;
}

+ (CMMotionManager *) InitMotionManager;
+ (CMMotionManager *) GetMotionManager;

- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings)  settings;
- (void) SetRef:(scdf::Sensor*)  ref;

@end