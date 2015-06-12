//
//  GraphicViewController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 10/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioWaveView.h"

@interface GraphicViewController : UIViewController

{
    IBOutlet    AudioWaveView      *waveView;
    
    IBOutlet    UISwitch        *bezierToggle;
    IBOutlet    UISwitch        *fillToggle;
    IBOutlet    UISwitch        *coordsToggle;
}


- (IBAction) toggleDrawBezier: (id) sender;
- (IBAction) toggleFillPlot: (id) sender;
- (IBAction) toggleShowCoord: (id) sender;

@end
