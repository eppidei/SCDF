//
//  FirstViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "FirstViewController.h"
#import <QuartzCore/QuartzCore.h>

#include "Sensor.h"
#include "SensorStandard.h"
#include "SensorAudioInput.h"
#include "SensorsManager.h"
#include "Harvester.h"

#define MAX_SENSOR_RATE_VALUE 200



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
    
    [accelRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateNormal];
    [accelRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateHighlighted];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Gyroscope);
    gyrosRateField.text = [NSString stringWithFormat:@"%d", rate];
    [gyrosRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateNormal];
    [gyrosRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateHighlighted];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Magnetometer);
    magneRateField.text = [NSString stringWithFormat:@"%d", rate];
    [magneRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateNormal];
    [magneRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateHighlighted];
    
    rate = scdf::theSensorManager()->GetRate(scdf::Proximity);
    proxyRateField.text = [NSString stringWithFormat:@"%d", rate];
    [proxyRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateNormal];
    [proxyRateButton setTitle:[NSString stringWithFormat:@"%d", rate] forState:UIControlStateHighlighted];
    
   
}

- (void) setupInterface
{
    sampleRateControl.selectedSegmentIndex = 4;
    bufferSizeRateControl.selectedSegmentIndex = 1;
    
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
    
    
    [pickerCointainerView removeFromSuperview];
    pickerViewIsVisible = NO;
    pickerView.delegate = self;
    
    
    pickerCointainerView.layer.cornerRadius = 5;
    pickerCointainerView.layer.masksToBounds = YES;
    [pickerCointainerView.layer setBorderWidth:1.0];
    [pickerCointainerView.layer setBorderColor:[[UIColor blackColor] CGColor]];
}

- (void) viewTapped {
   
    if(gyrosRateField.isFirstResponder) {[gyrosRateField resignFirstResponder];}
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

- (IBAction) changeSampleRateSensor: (id) sender
{
    
    UISegmentedControl *controller = (UISegmentedControl* )sender;
    
    s_int32 sampleRate = 0;
    switch (controller.selectedSegmentIndex) {
        case 0:
            sampleRate = 8000;
            break;
        case 1:
            sampleRate = 11025;
            break;
        case 2:
            sampleRate = 22050;
            break;
        case 3:
            sampleRate = 32000;
            break;
        case 4:
            sampleRate = 44100;
            break;
        case 5:
            sampleRate = 48000;
            break;
        default:
            sampleRate = 44100;
            break;
    }
    
    scdf::theSensorManager()->SetRate(scdf::AudioInput, sampleRate);

    
            
}
- (IBAction) changeBufferSizeSensor: (id) sender
{
    UISegmentedControl *controller = (UISegmentedControl* )sender;
    
    s_int32 bufferSize = 0;
    switch (controller.selectedSegmentIndex) {
        case 0:
            bufferSize = 256;
            break;
        case 1:
            bufferSize = 512;
            break;
        case 2:
            bufferSize = 1024;
            break;
        case 3:
            bufferSize = 2048;
            break;
        case 4:
            bufferSize = 4096;
            break;
        default:
            bufferSize = 512;
            break;
    }
    
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

- (IBAction) callPickerView:(id)sender
{
    UIButton *senderButton = (UIButton *) sender;
    int buttonIndex = -1;
    if(senderButton==accelRateButton)
        buttonIndex = 0;
    else if(senderButton==gyrosRateButton)
        buttonIndex = 1;
    else if(senderButton==magneRateButton)
        buttonIndex = 2;
    else if(senderButton==proxyRateButton)
        buttonIndex = 3;
    
    
    if(pickerViewIsVisible)
    {
        [pickerCointainerView removeFromSuperview];
        pickerViewIsVisible = NO;
    } else
    {
        
        [self.view addSubview:pickerCointainerView];

        sensorTypeInPickerView.selectedSegmentIndex = buttonIndex;
        
        [self UpdatePickerForIndex:buttonIndex];
        
        pickerViewIsVisible = YES;
    }
    
}

- (void) UpdatePickerForIndex: (s_int32) index
{
    s_int32 rate = -1;
    switch (index) {
        case 0:
            rate = scdf::theSensorManager()->GetRate(scdf::Accelerometer);
            break;
        case 1:
            rate = scdf::theSensorManager()->GetRate(scdf::Gyroscope);
            break;
        case 2:
            rate = scdf::theSensorManager()->GetRate(scdf::Magnetometer);
            break;
        case 3:
            rate = scdf::theSensorManager()->GetRate(scdf::Proximity);
            break;
        default:
            break;
    }
    
    
    [pickerView selectRow:rate inComponent:0 animated:YES];
    

}

- (IBAction) acquireSensorValue:(id)sender
{
    s_int32 rowValue = [pickerView selectedRowInComponent:0];
    s_int32 sensorIndex = sensorTypeInPickerView.selectedSegmentIndex;
    
    switch (sensorIndex) {
        case 0:
             [self setSensorRate:scdf::Accelerometer rate:rowValue];
            break;
        case 1:
           [self setSensorRate:scdf::Gyroscope rate:rowValue];
            break;
        case 2:
           [self setSensorRate:scdf::Magnetometer rate:rowValue];
            break;
        case 3:
           [self setSensorRate:scdf::Proximity rate:rowValue];
            break;
        default:
            break;
    }

}

- (IBAction) sensorIndexChanged:(id)sender
{
    [self UpdatePickerForIndex:sensorTypeInPickerView.selectedSegmentIndex];
}

#pragma mark pickerView delegatas

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return MAX_SENSOR_RATE_VALUE;
}

- (NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [NSString stringWithFormat:@"%d", row];
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
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Accelerometer);
            }
            break;
        }
        case scdf::Gyroscope:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Gyroscope);
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Gyroscope);
            }
            break;
        }
        case scdf::Magnetometer:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Magnetometer);
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Magnetometer);
            }
            break;
        }
        case scdf::Proximity:
        {
            if(state)
            {
                scdf::theSensorManager()->StartSensor(scdf::SensorType::Proximity);
            } else
            {
                scdf::theSensorManager()->StopSensor(scdf::SensorType::Proximity);
            }
            break;
        }
            
        default:
            break;
    }
}

@end
