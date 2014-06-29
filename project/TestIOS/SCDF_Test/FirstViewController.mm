//
//  FirstViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "FirstViewController.h"

#include "Sensor.h"
#include "SensorStandard.h"
#include "SensorAudioInput.h"
#include "SensorsManager.h"

@interface FirstViewController ()

@end

@implementation FirstViewController

- (void) viewDidUnload
{
    delete sensorManager;
    sensorManager = NULL;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setupSensors];
    
    [self setupInterface];
    
}

- (void) setupSensors
{
    sensorManager = new scdf::SensorsManager();
    
    sensorManager->CreateSensor(scdf::AudioInput);
    sensorManager->CreateSensor(scdf::Accelerometer);
    sensorManager->CreateSensor(scdf::Gyroscope);
    sensorManager->CreateSensor(scdf::Magnetometer);
    sensorManager->CreateSensor(scdf::Proximity);
    
    
    scdf::SensorSettings s_settings;
    s_settings.rate=100;
    sensorManager->InitSensor(scdf::Accelerometer,s_settings);
    sensorManager->InitSensor(scdf::Magnetometer,s_settings);
    sensorManager->InitSensor(scdf::Gyroscope,s_settings);
    sensorManager->InitSensor(scdf::Proximity,s_settings);
    
    
    scdf::SensorAudioSettings audioSettings;
    audioSettings.bufferSize=512;
    sensorManager->InitSensor(scdf::AudioInput,audioSettings);

}

- (void) setupInterface
{
    audioRateField.returnKeyType = UIReturnKeyDefault;
    audioRateField.keyboardType = UIKeyboardTypeNumberPad;
    
    gyrosRateField.returnKeyType = UIReturnKeyDone;
    gyrosRateField.keyboardType = UIKeyboardTypeNumberPad;
    
    accelRateField.returnKeyType = UIReturnKeyDone;
    accelRateField.keyboardType = UIKeyboardTypeNumberPad;
    
    magneRateField.returnKeyType = UIReturnKeyDone;
    magneRateField.keyboardType = UIKeyboardTypeNumberPad;
    
    proxyRateField.returnKeyType = UIReturnKeyDone;
    proxyRateField.keyboardType = UIKeyboardTypeNumberPad;
    
    
    UITapGestureRecognizer *tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped)];
    tapRecognizer.numberOfTapsRequired = 1;
    [self.view addGestureRecognizer:tapRecognizer];
    
}

- (void) viewTapped {
    if (audioRateField.isFirstResponder) {[audioRateField resignFirstResponder];}
    else if(gyrosRateField.isFirstResponder) {[gyrosRateField resignFirstResponder];}
    else if(accelRateField.isFirstResponder) {[accelRateField resignFirstResponder];}
    else if(magneRateField.isFirstResponder) {[magneRateField resignFirstResponder];}
    else if(proxyRateField.isFirstResponder) {[proxyRateField resignFirstResponder];}
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



#pragma mark User Actions

- (IBAction) toggleAudioSensor: (id) sender
{
    UISwitch *currentSensor = sender;
    [self toggleSensorType:scdf::AudioInput withState:currentSensor.on];
    
}

- (IBAction) toggleAccelerometerSensor: (id) sender
{
    UISwitch *currentSensor = sender;
     [self toggleSensorType:scdf::Accelerometer withState:currentSensor.on];
   
}

- (IBAction) toggleMagnetometerSensor: (id) sender
{
    UISwitch *currentSensor = sender;
     [self toggleSensorType:scdf::Magnetometer withState:currentSensor.on];
   
}

- (IBAction) toggleGyroscopeSensor: (id) sender
{
    UISwitch *currentSensor = sender;
     [self toggleSensorType:scdf::Gyroscope withState:currentSensor.on];
    
}

- (IBAction) toggleProximitySensor: (id) sender
{
    UISwitch *currentSensor = sender;
    [self toggleSensorType:scdf::Proximity withState:currentSensor.on];
    
}

- (IBAction) changeMasterSensor: (id) sender
{
    UISegmentedControl *controller = (UISegmentedControl* )sender;
    switch (controller.selectedSegmentIndex) {
        case 0:
             LOGD("Master Sensor: Audio \n");
            [self setMasterSensor: scdf::AudioInput];
            break;
        case 1:
             LOGD("Master Sensor: Accelerometer \n");
            [self setMasterSensor: scdf::Accelerometer];
            break;
        case 2:
             LOGD("Master Sensor: Gryoscope \n");
            [self setMasterSensor: scdf::Gyroscope];
            break;
        case 3:
             LOGD("Master Sensor: Magnetometer \n");
            [self setMasterSensor: scdf::Magnetometer];
            break;
        case 4:
             LOGD("Master Sensor: Proxity \n");
            [self setMasterSensor: scdf::Proximity];
            break;
        default:
            break;
    }
}

- (IBAction)AudioRateDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 rate = [textUpdated intValue];
    [self setSensorRate:scdf::AudioInput rate:rate];
}

- (IBAction)AccelerometerRateDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 rate = [textUpdated intValue];
     [self setSensorRate:scdf::Accelerometer rate:rate];
}

- (IBAction)GyroscopeRateDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 rate = [textUpdated intValue];
     [self setSensorRate:scdf::Gyroscope rate:rate];
}

- (IBAction)MagnetometerRateDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 rate = [textUpdated intValue];
     [self setSensorRate:scdf::Magnetometer rate:rate];
}

- (IBAction)ProximityRateDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 rate = [textUpdated intValue];
    [self setSensorRate:scdf::Proximity rate:rate];
}



#pragma mark from interface to framekork

- (void) setMasterSensor: (scdf::SensorType) sensorType
{
    
}


- (void ) setSensorRate: (scdf::SensorType) sensorType rate: (s_int32) rate
{
    
    switch (sensorType) {
        case scdf::AudioInput:
        {
            LOGD("AudioInput rate: %d \n", rate);
            break;
        }
        case scdf::Accelerometer:
        {
            LOGD("Accelerometer rate: %d \n", rate);
            break;
        }
        case scdf::Gyroscope:
        {
            LOGD("Gyroscope rate: %d \n", rate);
            break;
        }
        case scdf::Magnetometer:
        {
            LOGD("Magnetometer rate: %d \n", rate);
            break;
        }
        case scdf::Proximity:
        {
            LOGD("Proximity rate: %d \n", rate);
            break;
        }
            
        default:
            break;
    }
}


- (void ) toggleSensorType: (scdf::SensorType) sensorType withState: (BOOL) state
{
    
    switch (sensorType) {
        case scdf::AudioInput:
        {
            if(state)
            {
                sensorManager->ActivateSensor(scdf::SensorType::AudioInput);
                LOGD("toggle AudioSensor ON \n");
            } else
            {
                sensorManager->DeActivateSensor(scdf::SensorType::AudioInput);
                LOGD("toggle AudioSensor OFF \n");
            }

            break;
        }
        case scdf::Accelerometer:
        {
            if(state)
            {
                sensorManager->ActivateSensor(scdf::SensorType::Accelerometer);
                LOGD("toggle Accelerometer Sensor ON \n");
            } else
            {
                sensorManager->DeActivateSensor(scdf::SensorType::Accelerometer);
                LOGD("toggle Accelerometer Sensor OFF \n");
            }
            break;
        }
        case scdf::Gyroscope:
        {
            if(state)
            {
                sensorManager->ActivateSensor(scdf::SensorType::Gyroscope);
                LOGD("toggle Gyroscope Sensor ON \n");
            } else
            {
                sensorManager->DeActivateSensor(scdf::SensorType::Gyroscope);
                LOGD("toggle Gyroscope Sensor OFF \n");
            }
            break;
        }
        case scdf::Magnetometer:
        {
            if(state)
            {
                sensorManager->ActivateSensor(scdf::SensorType::Magnetometer);
                LOGD("toggle Magnetometer Sensor ON \n");
            } else
            {
                sensorManager->DeActivateSensor(scdf::SensorType::Magnetometer);
                LOGD("toggle Magnetometer Sensor OFF \n");
            }
            break;
        }
        case scdf::Proximity:
        {
            if(state)
            {
                sensorManager->ActivateSensor(scdf::SensorType::Proximity);
                LOGD("toggle Proximity Sensor ON \n");
            } else
            {
                sensorManager->DeActivateSensor(scdf::SensorType::Proximity);
                LOGD("toggle Proximity Sensor OFF \n");
            }
            break;
        }
            
        default:
            break;
    }
}

@end
