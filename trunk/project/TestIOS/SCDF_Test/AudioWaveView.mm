//
//  AudioWaveView.m
//  SCDF_Test
//
//  Created by Marco Bertola on 18/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import "AudioWaveView.h"

@implementation AudioWaveView

const int bufferSize = 8192;
const int borderSize = 0;
#define ARC4RANDOM_MAX      0x100000000

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        [self setBackgroundColor:[UIColor grayColor]];
        
        bufferData = (double*)malloc(bufferSize*sizeof(double));
        [self generateRandomBuffer];
        
        
        [NSTimer scheduledTimerWithTimeInterval:0.002
                                         target:self
                                       selector:@selector(onTimerTriggered:)
                                       userInfo:nil
                                        repeats:YES];
    }
    return self;
}
//
- (void) generateRandomBuffer
{
    for(int i = 0; i<bufferSize; i++)
    {
        bufferData[i]=((double)arc4random() / ARC4RANDOM_MAX);
    }
}

- (void) onTimerTriggered: (NSTimer *) tm
{
    [self generateRandomBuffer];
    [self setNeedsDisplay];
    
}



- (void)drawRect:(CGRect)rect
{
    // Drawing code
    
    CGRect frame = self.frame;
    
    
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColor(context, CGColorGetComponents([[UIColor blueColor] CGColor]));
    double bufferStep = (double)bufferSize/(double)self.frame.size.width;
    
    for(int i = 0; i<frame.size.width - 1; i++)
    {
        
        CGPoint pt1;
        pt1.x = i;
        pt1.y =  borderSize + ((frame.size.height - (2*borderSize))*bufferData[(int)(i*bufferStep)]);
        
        CGPoint pt2;
        pt2.x = i;
        pt2.y =  borderSize + ((frame.size.height-(2*borderSize))*bufferData[(int)((i+1)*bufferStep)]);

        
        CGContextMoveToPoint(context,pt1.x,pt1.y);
        CGContextAddLineToPoint(context,pt2.x,pt2.y);
        CGContextStrokePath(context);
    }
}


@end
