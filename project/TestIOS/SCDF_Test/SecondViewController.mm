//
//  SecondViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 07/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SecondViewController.h"
#include "UDPSendersManager.h"
#include "SaveSettings.h"
#include "Sensor.h"
#import <ifaddrs.h>
#import <arpa/inet.h>
#include <string>
#include "UdpSender.h"

const int max_number_ip_port = 65535;

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
   
    [self setupNetwork];
    
    [self setupInterface];
    
    [self UpdateLabels];
}

- (void) setupNetwork
{
    // Load values from settings
    std::string *ip = scdf::SaveSettings::Instance()->LoadStringValue(ADDRESS_IP_KEY);
    s_bool multiOutput = scdf::SaveSettings::Instance()->LoadBoolValue(MULTI_ROUTE_KEY);
    s_bool routingType = scdf::SaveSettings::Instance()->LoadBoolValue(OUTPUT_TYPE);
    actualPort = scdf::SaveSettings::Instance()->LoadIntValue(PORT_IP_KEY);
    
     // Check Values from settings
    if(ip)
    {
        addressString = *ip;
        delete ip;
        ip = NULL;
    } else
        addressString = DEFAULT_IP_ADDRESS;
    
    if (!actualPort) {
        actualPort = DEFAULT_UDP_PORT_BASE;
    }
    
    // Setup Framework
    [self SetOutputPort:actualPort];
    [self SetOutputAddress:addressString];
    [self setMultiOutputActiove:multiOutput];
    [self setRoutingType:routingType];
    
    
    // Update Interface
    multiOutputSwitch.on = multiOutput;
    outputTypeSegmented.selectedSegmentIndex = routingType;
    
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
    
    
    [pickerCointainerView removeFromSuperview];
    pickerViewPortIsVisible = NO;
    pickerViewPort.delegate = self;
    
    
    
    pickerCointainerView.layer.cornerRadius = 5;
    pickerCointainerView.layer.masksToBounds = YES;
    [pickerCointainerView.layer setBorderWidth:1.0];
    [pickerCointainerView.layer setBorderColor:[[UIColor blackColor] CGColor]];
    
   

}

- (void) viewTapped {
    if (outputIp.isFirstResponder) {[outputIp resignFirstResponder];}
    else if(outputPort.isFirstResponder) {[outputPort resignFirstResponder];}
}


- (void)textFieldDidEndEditing:(UITextField *)textField
{
    if(textField==outputIp){
        NSString *address = [outputIp text];
        addressString = [address UTF8String];
        [self SetOutputAddress:addressString];
        
        
        
    } else if (textField==outputPort) {
        NSString *textUpdated = [outputPort text];
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
    
    [buttonPort setTitle:[NSString stringWithFormat:@"%d", actualPort] forState:UIControlStateNormal];
    [buttonPort setTitle:[NSString stringWithFormat:@"%d", actualPort] forState:UIControlStateHighlighted];
    
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

- (IBAction) toggleActiveSender: (id) sender
{
    UISwitch *senderSwitch = sender;
    scdf::UDPSendersManager::Instance()->GetSender()->Activate(senderSwitch.on);
}

- (IBAction) callPickerIpPortView:(id)sender
{
    

    if(pickerViewPortIsVisible)
    {
        [pickerCointainerView removeFromSuperview];
        pickerViewPortIsVisible = NO;
    } else
    {
        
        [self.view addSubview:pickerCointainerView];
        
        int value = actualPort %10;
        [pickerViewPort selectRow:value inComponent:4 animated:YES];
        
        value = (actualPort/10) %10;
        [pickerViewPort selectRow:value inComponent:3 animated:YES];
        
        value = (actualPort/100) %10;
        [pickerViewPort selectRow:value inComponent:2 animated:YES];
        
        value = (actualPort/1000) %10;
        [pickerViewPort selectRow:value inComponent:1 animated:YES];
        
        value = (actualPort/10000) %10;
        [pickerViewPort selectRow:value inComponent:0 animated:YES];
        
        pickerViewPortIsVisible = YES;
    }
    
}

- (IBAction) acquirePortValue:(id)sender
{
    s_int32 rowValue = [pickerViewPort selectedRowInComponent:4];
    rowValue+= 10*[pickerViewPort selectedRowInComponent:3];
    rowValue+= 100*[pickerViewPort selectedRowInComponent:2];
    rowValue+= 1000*[pickerViewPort selectedRowInComponent:1];
    rowValue+= 10000*[pickerViewPort selectedRowInComponent:0];
    
   
    [self SetOutputPort:rowValue];
    [self UpdateLabels];
}



#pragma mark pickerView delegatas

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 5;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return 10;
}

- (NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [NSString stringWithFormat:@"%d", row];
}


#pragma mark from interface to framekork

- (void) setRoutingType: (NSInteger) routingType
{
    scdf::UDPSendersManager::Instance()->SetUseOSCPackaging(1==routingType);
}

- (void) setMultiOutputActiove: (BOOL) active
{
    scdf::UDPSendersManager::Instance()->SetMultiOutput(active);
}

-  (void) SetOutputPort: (s_int32) outputUdpPort
{
    if (outputUdpPort>max_number_ip_port) {
        outputUdpPort = max_number_ip_port;
    }
    
    actualPort = outputUdpPort;
    scdf::UDPSendersManager::Instance()->SetOutputPort(outputUdpPort);
}

-  (void) SetOutputAddress: (std::string) outputUdpIP
{
    scdf::UDPSendersManager::Instance()->SetOutputAddress(outputUdpIP);
}

@end

