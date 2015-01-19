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
    AudioWaveView      *waveView;
    UIView             *topView;
    UIView             *bottomView;
    
    UILabel            *topLabel;
}

@end
