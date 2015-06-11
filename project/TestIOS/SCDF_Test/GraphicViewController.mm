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

const int borderMargin = 80;
extern scdf::Receiver *audioReceiver;

@implementation GraphicViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    topLabel = [[UILabel alloc]init];
    [topLabel setFont:[UIFont systemFontOfSize:20]];
    [topLabel setText:@"Wave data:"];
    
    topView = [[UIView alloc ]init];
    [topView setBackgroundColor:[UIColor whiteColor]];
    
     waveView  = [[AudioWaveView alloc] initWithFrame:self.view.frame];
    
    AudioWaveListener *listener = new AudioWaveListener();
    listener->SetViewRef(waveView);
    if(audioReceiver)
        audioReceiver->SetListener(listener);
    
    
    bottomView = [[UIView alloc ]init];
    [bottomView setBackgroundColor:[UIColor whiteColor]];
   
    // Do any additional setup after loading the view.
}

-(void) viewWillAppear:(BOOL)animated
{
    [topView setFrame:CGRectMake(0, 0, self.view.frame.size.width, borderMargin)];
    [self.view addSubview:topView];
    
    [waveView setFrame:CGRectMake(0, borderMargin, self.view.frame.size.width, self.view.frame.size.height- (2*borderMargin)- TAB_BAR_SIZE)];
    [self.view addSubview:waveView];
    
    [bottomView setFrame:CGRectMake(0, self.view.frame.size.height- borderMargin-TAB_BAR_SIZE, self.view.frame.size.width, borderMargin)];
    [self.view addSubview:waveView];
    
    
    int offsetLeftLabel = 10;
    CGRect frameLabel = topView.frame;
    frameLabel.origin.x = offsetLeftLabel;
    frameLabel.size.width -= offsetLeftLabel;
    [topView addSubview:topLabel];
    [topLabel setFrame:frameLabel];
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
