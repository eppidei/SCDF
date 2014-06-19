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
    
    /*scdf::SensorSettings dataAccelerometer;
    scdf::Sensor *accelerometer = scdf::SensorStandard::Create(scdf::Accelerometer);
    dataAccelerometer.sensorRef = accelerometer;
    
    accelerometer->Setup(dataAccelerometer);
    //accelerometer->Start();
    
    scdf::SensorSettings dataGyro;
    scdf::Sensor *gyroscope = scdf::SensorStandard::Create(scdf::Gyroscope);
    dataGyro.sensorRef = gyroscope;
    
    gyroscope->Setup(dataGyro);
    //gyroscope->Start();
    
    scdf::SensorSettings dataMagnetometer;
    scdf::Sensor *magnetometer = scdf::SensorStandard::Create(scdf::Magnetometer);
     dataMagnetometer.sensorRef = magnetometer;
    
     magnetometer->Setup(dataMagnetometer);
    //magnetometer->Start();*/
    
    scdf::SensorSettings audioSettings;
    scdf::Sensor *audioInputSensor = scdf::SensorStandard::Create(scdf::AudioInput);
    
    audioInputSensor->Setup(audioSettings);
    audioInputSensor->Start();
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
