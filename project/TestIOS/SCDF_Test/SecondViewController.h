//
//  SecondViewController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SecondViewController : UIViewController


{
    IBOutlet UITextField             *outputIp;
    IBOutlet UITextField             *outputPort;
    IBOutlet UILabel                 *inputIp;
    
    NSString                         *addressString;
    int                              actualPort;
}

@property (nonatomic, retain) IBOutlet UITextField *outputIp;
@property (nonatomic, retain) IBOutlet UITextField *outputPort;


@end
