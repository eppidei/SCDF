//
//  FirstViewController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface FirstViewController : UIViewController <UIPickerViewDelegate>

{
    
    BOOL pickerViewIsVisible;
    
    IBOutlet UISwitch *audioSensorSwitch;
    IBOutlet UISwitch *accelerometerSensorSwitch;
    IBOutlet UISwitch *magnetometerSensorSwitch;
    IBOutlet UISwitch *gyroscopeSensorSwitch;
    IBOutlet UISwitch *proximitySensorSwitch;
    
    
    IBOutlet UITextField *accelRateField;
    IBOutlet UITextField *magneRateField;
    IBOutlet UITextField *gyrosRateField;
    IBOutlet UITextField *proxyRateField;
    
    
    IBOutlet UIButton *accelRateButton;
    IBOutlet UIButton *magneRateButton;
    IBOutlet UIButton *gyrosRateButton;
    IBOutlet UIButton *proxyRateButton;
    
    IBOutlet UISegmentedControl *sampleRateControl;
    IBOutlet UISegmentedControl *bufferSizeRateControl;
    
    IBOutlet UIView       *pickerCointainerView;
    IBOutlet UIPickerView *pickerView;
    IBOutlet UISegmentedControl *sensorTypeInPickerView;
    
    
    
}

- (IBAction) toggleAudioSensor: (id) sender;
- (IBAction) toggleAccelerometerSensor: (id) sender;
- (IBAction) toggleMagnetometerSensor: (id) sender;
- (IBAction) toggleGyroscopeSensor: (id) sender;
- (IBAction) toggleProximitySensor: (id) sender;
- (IBAction) changeMasterSensor: (id) sender;
- (IBAction) changeSampleRateSensor: (id) sender;
- (IBAction) changeBufferSizeSensor: (id) sender;

- (IBAction) AccelerometerRateDidEndEditing:(UITextField *)textField;
- (IBAction) GyroscopeRateDidEndEditing:(UITextField *)textField;
- (IBAction) MagnetometerRateDidEndEditing:(UITextField *)textField;
- (IBAction) ProximityRateDidEndEditing:(UITextField *)textField;

- (IBAction) callPickerView:(id)sender;
- (IBAction) acquireSensorValue:(id)sender;
- (IBAction) sensorIndexChanged:(id)sender;





@end
