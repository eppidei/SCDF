//
//  ProximityIos.h
//  SCDF_Test
//
//  Created by Marco Bertola on 19/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIos.h"

@interface ProximityIos : SensorStandardIos



- (s_bool) Start;
- (s_bool) Stop;
- (s_bool) Setup:(scdf::SensorSettings) settings;

@end
