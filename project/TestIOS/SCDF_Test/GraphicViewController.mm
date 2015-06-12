//
//  GraphicViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 10/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import "GraphicViewController.h"


#define TAB_BAR_SIZE 49

@interface GraphicViewController ()

@end

extern scdf::Receiver *audioReceiver;

@implementation GraphicViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    [waveView initPlot];
    
    AudioWaveListener *listener = new AudioWaveListener();
    listener->SetViewRef(waveView);
    if(audioReceiver)
        audioReceiver->SetListener(listener);
    
    
    [fillToggle setOn:(BOOL)[waveView IsFillPlot]];
    [bezierToggle setOn:(BOOL)[waveView IsDrawBezier]];
    [coordsToggle setOn:(BOOL)[waveView IsShowCoordinates]];
    
}

- (IBAction) toggleDrawBezier: (id) sender
{
    [waveView setDrawBezier:bezierToggle.on];
}

- (IBAction) toggleFillPlot: (id) sender
{
    [waveView setFillPlot:fillToggle.on];
}

- (IBAction) toggleShowCoord: (id) sender
{
     [waveView setShowCoordinates:coordsToggle.on];
}

-(void) viewWillAppear:(BOOL)animated
{
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
