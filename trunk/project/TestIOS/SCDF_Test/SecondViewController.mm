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
   
    outputIp.delegate = (id <UITextFieldDelegate>)self;
    outputIp.returnKeyType = UIReturnKeyDone;
    outputIp.keyboardType = UIKeyboardTypeNumberPad;
    
    outputPort.delegate = (id <UITextFieldDelegate>)self;
    outputPort.returnKeyType = UIReturnKeyDone;
    outputPort.keyboardType = UIKeyboardTypeNumberPad;
    
    addressString = [NSString stringWithFormat:@"%s", DEFUALT_ADDRESS];
    [addressString retain];
    actualPort = DEFAULT_PORT;
    
    [self UpdateLabels];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    if ([textField canResignFirstResponder]) {
        [textField resignFirstResponder];
    }
    return YES;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    NSString *textUpdated;
    if(textField==outputIp){
        textUpdated = [outputIp text];
        addressString = textUpdated;
        std::string adress([textUpdated UTF8String]);
        
        LOGD("UPD IP Address selected %s\n", adress.c_str());
        
        
    } else if (textField==outputPort) {
        textUpdated = [outputPort text];
        int port = [textUpdated intValue];
        actualPort = port;
        
        LOGD("UDP Port Selected: %d\n", actualPort);
    }
    
    [self UpdateLabels];
}

- ( void ) UpdateLabels
{
    outputIp.text = addressString;
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

@end

