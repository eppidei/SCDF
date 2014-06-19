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

@interface FirstViewController ()

@end

@implementation FirstViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    scdf::SensorSettings s_settings;
    s_settings.rate=1;
    scdf::Sensor *accelerometer = scdf::SensorStandard::Create(scdf::Accelerometer);
    
    accelerometer->Setup(s_settings);
    accelerometer->Start();
    
    /*scdf::Sensor *gyroscope = scdf::SensorStandard::Create(scdf::Gyroscope);
    gyroscope->Setup(s_settings);
    gyroscope->Start();
    
    scdf::Sensor *magnetometer = scdf::SensorStandard::Create(scdf::Magnetometer);
    
    magnetometer->Setup(s_settings);
    magnetometer->Start();
    
    scdf::SensorSettings audioSettings;
    scdf::Sensor *audioInputSensor = scdf::SensorStandard::Create(scdf::AudioInput);
    
    audioInputSensor->Setup(audioSettings);
    audioInputSensor->Start();*/
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
