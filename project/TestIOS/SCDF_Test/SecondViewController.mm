//
//  SecondViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SecondViewController.h"
#include "Sensor.h"
#import <ifaddrs.h>
#import <arpa/inet.h>
#include <string>

#define DEFUALT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 9000;

@interface SecondViewController ()

@end


@implementation SecondViewController


@synthesize outputIp;
@synthesize outputPort;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
   
    [self setupInterface];
    
    [self setupNetwork];
    
    [self UpdateLabels];
}

- (void) setupNetwork
{
    addressString = DEFUALT_ADDRESS;
    actualPort = DEFAULT_PORT;
    
    
    [self SetOutputPort:actualPort];
    [self SetOutputAddress:addressString];

}

- (void) setupInterface
{
    outputIp.delegate = (id <UITextFieldDelegate>)self;
    outputIp.returnKeyType = UIReturnKeyDone;
    outputIp.keyboardType = UIKeyboardTypeNumberPad;
    
    outputPort.delegate = (id <UITextFieldDelegate>)self;
    outputPort.returnKeyType = UIReturnKeyDone;
    outputPort.keyboardType = UIKeyboardTypeNumberPad;
    
    UITapGestureRecognizer *tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(viewTapped)];
    tapRecognizer.numberOfTapsRequired = 1;
    [self.view addGestureRecognizer:tapRecognizer];
    
   

}

- (void) viewTapped {
    if (outputIp.isFirstResponder) {[outputIp resignFirstResponder];}
    else if(outputPort.isFirstResponder) {[outputPort resignFirstResponder];}
}

/*- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    if ([textField canResignFirstResponder]) {
        [textField resignFirstResponder];
    }
    return YES;
}*/

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    if(textField==outputIp){
        NSString *address = [outputIp text];
        addressString = [address UTF8String];
        [self SetOutputAddress:addressString];
        
        
        
    } else if (textField==outputPort) {
        NSString *textUpdated = [[NSString stringWithString:[outputPort text]]autorelease];
        s_int32 port = [textUpdated intValue];
        actualPort = port;
        
        [self SetOutputPort:port];
        
        
    }
    
    [self UpdateLabels];
}

- ( void ) UpdateLabels
{
    outputIp.text = [NSString stringWithUTF8String:addressString.c_str()];
    outputPort.text = [NSString stringWithFormat:@"%d", actualPort];
    inputIp.text = [self getIPAddress];
    
}

- (NSString *)getIPAddress {
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return address;
    
}

#pragma mark IBActions

- (IBAction) multiOutputToggle: (id) sender
{
    UISwitch *multioutputToggle = (UISwitch *)sender;
    [self setMultiOutputActiove:multioutputToggle.on];
    
}

- (IBAction) changheOutputRouting:(id)sender
{
    UISegmentedControl *control = (UISegmentedControl *) sender;
    [self setRoutingType:control.selectedSegmentIndex];
    
}

#pragma mark from interface to framekork

- (void) setRoutingType: (NSInteger) routingType
{
    if(routingType==0)
    {
        LOGD("OUTPUT TO UDP \n");
        
    } else if(routingType==1)
    {
         LOGD("OUTPUT TO OSC \n");
    }
}

- (void) setMultiOutputActiove: (BOOL) active
{
    if(active)
    {
        LOGD("MULTI OUTPUT ON \n");
    } else
    {
        LOGD("MULTI OUTPUT OFF \n");
    }
}

-  (void) SetOutputPort: (s_int32) outputUdpPort
{
    LOGD("UDP Port Selected: %d\n", outputUdpPort);
}

-  (void) SetOutputAddress: (string) outputUdpIP
{
    LOGD("UPD IP Address selected %s\n", outputUdpIP.c_str());

}

@end

