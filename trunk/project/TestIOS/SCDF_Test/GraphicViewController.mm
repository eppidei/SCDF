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

@implementation GraphicViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    graphicView  = [[UIView alloc] initWithFrame:self.view.frame];
    [graphicView setBackgroundColor:[UIColor greenColor]];
    [self.view addSubview:graphicView];
    // Do any additional setup after loading the view.
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
