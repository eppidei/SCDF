//
//  SecondViewController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Sensor.h"

@interface SecondViewController : UIViewController


{
    IBOutlet UITextField             *outputIp;
    IBOutlet UITextField             *outputPort;
    IBOutlet UILabel                 *inputIp;
    
    IBOutlet UISwitch                *activeSenderSwitch;
    
    std::string                      addressString;
    s_int32                          actualPort;
}

@property (nonatomic, retain) IBOutlet UITextField *outputIp;
@property (nonatomic, retain) IBOutlet UITextField *outputPort;


- (IBAction) multiOutputToggle: (id) sender;
- (IBAction) changheOutputRouting:(id)sender;
- (IBAction) toggleActiveSender: (id) sender;


@end
