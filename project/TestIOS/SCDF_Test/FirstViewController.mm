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
#include "Harvester.h"



scdf::SensorsManager *theSensorManager();

@interface FirstViewController ()

@end

@implementation FirstViewController

- (void) viewDidUnload
{
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self initSensors];
    
    [self setupInterface];
    
    [self UpdateSensorsValues];
    
}

- (void) initSensors
{
    scdf::SensorSettings settings;
    settings.rate = 50;
    
    scdf::theSensorManager()->InitSensor(scdf::Accelerometer, settings);
    scdf::theSensorManager()->InitSensor(scdf::Magnetometer, settings);
    scdf::theSensorManager()->InitSensor(scdf::Gyroscope, settings);
    scdf::theSensorManager()->InitSensor(scdf::Proximity, settings);
    
    scdf::SensorAudioSettings settingsAudio;
    settingsAudio.rate = 44100;
    settingsAudio.bufferSize = 512;
    settingsAudio.numChannels = 1;
    scdf::theSensorManager()->InitSensor(scdf::AudioInput, settingsAudio);
    
}

- (void) UpdateSensorsValues
{
    s_int32 rate = scdf::theSensorManager()->GetRate(scdf::Accelerometer);
    accelRateField.text = [NSString stringWithFormat:@"%d", rate];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Gyroscope);
    gyrosRateField.text = [NSString stringWithFormat:@"%d", rate];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Magnetometer);
    magneRateField.text = [NSString stringWithFormat:@"%d", rate];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Proximity);
    proxyRateField.text = [NSString stringWithFormat:@"%d", rate];
    
    rate = scdf::theSensorManager()->GetRate(scdf::AudioInput);
    audioRateField.text = [NSString stringWithFormat:@"%d", rate];
    
    s_int32 bufferSize = scdf::theSensorManager()->GetNumSamples(scdf::AudioInput);
    audioBufferSizeField.text = [NSString stringWithFormat:@"%d", bufferSize];
}

- (void) setupInterface
{
    audioBufferSizeField.returnKeyType = UIReturnKeyDefault;
    audioBufferSizeField.keyboardType = UIKeyboardTypeNumberPad;
    
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
     else if(audioBufferSizeField.isFirstResponder) {[audioBufferSizeField resignFirstResponder];}
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

- (IBAction)AudioBufferDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated = [textField text];
    s_int32 bufferSize = [textUpdated intValue];
    scdf::theSensorManager()->SetBufferSize(scdf::AudioInput, bufferSize);
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
    scdf::Harvester::Instance()->SetType(sensorType);
}


- (void ) setSensorRate: (scdf::SensorType) sensorType rate: (s_int32) rate
{
    scdf::theSensorManager()->SetRate(sensorType, rate);
    
    [self UpdateSensorsValues];
}


- (void ) toggleSensorType: (scdf::SensorType) sensorType withState: (BOOL) state
{
    
    switch (sensorType) {
        case scdf::AudioInput:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::AudioInput);
                LOGD("toggle AudioSensor ON \n");
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::AudioInput);
                LOGD("toggle AudioSensor OFF \n");
            }

            break;
        }
        case scdf::Accelerometer:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Accelerometer);
                LOGD("toggle Accelerometer Sensor ON \n");
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Accelerometer);
                LOGD("toggle Accelerometer Sensor OFF \n");
            }
            break;
        }
        case scdf::Gyroscope:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Gyroscope);
                LOGD("toggle Gyroscope Sensor ON \n");
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Gyroscope);
                LOGD("toggle Gyroscope Sensor OFF \n");
            }
            break;
        }
        case scdf::Magnetometer:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Magnetometer);
                LOGD("toggle Magnetometer Sensor ON \n");
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Magnetometer);
                LOGD("toggle Magnetometer Sensor OFF \n");
            }
            break;
        }
        case scdf::Proximity:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Proximity);
                LOGD("toggle Proximity Sensor ON \n");
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Proximity);
                LOGD("toggle Proximity Sensor OFF \n");
            }
            break;
        }
            
        default:
            break;
    }
}

@end
