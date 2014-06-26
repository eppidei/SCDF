//
//  FirstViewController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>

namespace scdf {
    class SensorsManager;
}
//class scdf::SensorsManager;

@interface FirstViewController : UIViewController

{
    scdf::SensorsManager   *sensorManager;
    
    IBOutlet UISwitch *audioSensorSwitch;
    IBOutlet UISwitch *accelerometerSensorSwitch;
    IBOutlet UISwitch *magnetometerSensorSwitch;
    IBOutlet UISwitch *gyroscopeSensorSwitch;
    IBOutlet UISwitch *proximitySensorSwitch;
    
}

- (IBAction) toggleAudioSensor: (id) sender;
- (IBAction) toggleAccelerometerSensor: (id) sender;
- (IBAction) toggleMagnetometerSensor: (id) sender;
- (IBAction) toggleGyroscopeSensor: (id) sender;
- (IBAction) toggleProximitySensor: (id) sender;

@end
