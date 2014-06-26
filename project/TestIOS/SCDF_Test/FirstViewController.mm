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

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark User Actions

- (IBAction) toggleAudioSensor: (id) sender
{
    UISwitch *currentSensor = sender;
    if(currentSensor.on)
    {
        sensorManager->ActivateSensor(scdf::SensorType::AudioInput);
        LOGD("toggle AudioSensor ON \n");
    } else
    {
        sensorManager->DeActivateSensor(scdf::SensorType::AudioInput);
        LOGD("toggle AudioSensor OFF \n");
    }
    
}

- (IBAction) toggleAccelerometerSensor: (id) sender
{
    UISwitch *currentSensor = sender;
    if(currentSensor.on)
    {
        sensorManager->ActivateSensor(scdf::SensorType::Accelerometer);
         LOGD("toggle Accelerometer Sensor ON \n");
    } else
    {
        sensorManager->DeActivateSensor(scdf::SensorType::Accelerometer);
         LOGD("toggle Accelerometer Sensor OFF \n");
    }
}

- (IBAction) toggleMagnetometerSensor: (id) sender
{
    UISwitch *currentSensor = sender;
    if(currentSensor.on)
    {
        sensorManager->ActivateSensor(scdf::SensorType::Magnetometer);
         LOGD("toggle Magnetometer Sensor ON \n");
    } else
    {
        sensorManager->DeActivateSensor(scdf::SensorType::Magnetometer);
         LOGD("toggle Magnetometer Sensor OFF \n");
    }
}

- (IBAction) toggleGyroscopeSensor: (id) sender
{
    UISwitch *currentSensor = sender;
    if(currentSensor.on)
    {
        sensorManager->ActivateSensor(scdf::SensorType::Gyroscope);
         LOGD("toggle Gyroscope Sensor ON \n");
    } else
    {
        sensorManager->DeActivateSensor(scdf::SensorType::Gyroscope);
         LOGD("toggle Gyroscope Sensor OFF \n");
    }
}

- (IBAction) toggleProximitySensor: (id) sender
{
    UISwitch *currentSensor = sender;
    if(currentSensor.on)
    {
        sensorManager->ActivateSensor(scdf::SensorType::Proximity);
         LOGD("toggle Proximity Sensor ON \n");
    } else
    {
        sensorManager->DeActivateSensor(scdf::SensorType::Proximity);
         LOGD("toggle Proximity Sensor OFF \n");
    }    
}

@end
