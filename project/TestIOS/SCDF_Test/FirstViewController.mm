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

@interface FirstViewController ()

@end

@implementation FirstViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    scdf::Sensor *accelerometer = scdf::SensorStandard::Create(scdf::Accelerometer);
    accelerometer->Start();
    
    scdf::Sensor *gyroscope = scdf::SensorStandard::Create(scdf::Gyroscope);
    //gyroscope->Start();
    
    scdf::Sensor *magnetometer = scdf::SensorStandard::Create(scdf::Magnetometer);
   // magnetometer->Start();
    
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
