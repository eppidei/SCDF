//
//  GraphicViewController.m
//  SCDF_Test
//
//  Created by Marco Bertola on 10/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import "GraphicViewController.h"

@interface GraphicViewController ()

@end

const int borderMargin = 200;

@implementation GraphicViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    topView = [[UIView alloc ]init];
    [topView setBackgroundColor:[UIColor whiteColor]];
    
     waveView  = [[AudioWaveView alloc] initWithFrame:self.view.frame];
    
    bottomView = [[UIView alloc ]init];
    [bottomView setBackgroundColor:[UIColor whiteColor]];
   
    // Do any additional setup after loading the view.
}

-(void) viewWillAppear:(BOOL)animated
{
    [topView setFrame:CGRectMake(0, 0, self.view.frame.size.width, borderMargin)];
    [self.view addSubview:topView];
    
    [waveView setFrame:CGRectMake(0, borderMargin, self.view.frame.size.width, self.view.frame.size.height- (2*borderMargin))];
    [self.view addSubview:waveView];
    
    [bottomView setFrame:CGRectMake(0, self.view.frame.size.height- borderMargin, self.view.frame.size.width, borderMargin)];
    [self.view addSubview:waveView];
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
