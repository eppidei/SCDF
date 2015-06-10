//
//  AudioWaveView.m
//  SCDF_Test
//
//  Created by Marco Bertola on 18/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import "AudioWaveView.h"
#import "UIBezierPath+Interpolation.h"

@implementation AudioWaveView

const int bufferSize = 512;
const int borderSize = 0;
#define ARC4RANDOM_MAX      0x100000000


//#define USE_TIMER_FOR_TESTING
#define TIMER_RATE 0.05


void AudioWaveListener::draw_buffer(s_sample *p_buff, unsigned int buff_len)
{
    if(audioWaveViewRef)
        [audioWaveViewRef OnNewBufferReady:p_buff withSize:buff_len];
}

- (void) setListener: (AudioWaveListener* ) listener
{
    audioListener = listener;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        audioListener = nil;
        
        [self setBackgroundColor:[UIColor grayColor]];
        
        bufferData = (s_sample*)malloc(bufferSize*sizeof(s_sample));
        bufferDataTemp = (s_sample*)malloc(bufferSize*sizeof(s_sample));
        for(int i = 0; i<bufferSize; i++)
        {
            s_sample value = (s_sample)i/bufferSize;
            value = 0;
            bufferData[i]=(value);
            bufferDataTemp[i]=(value);
        }
        
        currentBufferSize = bufferSize;
        bufferLock = [[NSObject alloc] init];
        
        
#ifdef USE_TIMER_FOR_TESTING
        bufferDataTest = (s_sample*)malloc(bufferSize*sizeof(s_sample));
        [self generateRandomBuffer];
        [NSTimer scheduledTimerWithTimeInterval:TIMER_RATE
                                         target:self
                                       selector:@selector(onTimerTriggered:)
                                       userInfo:nil
                                        repeats:YES];
#endif
    }
    return self;
}

-(void) dealloc
{
    [super dealloc];
    
    free(bufferData);
    free(bufferDataTemp);
    free(bufferDataTest);
    
    [bufferLock release];
    
}

- (void) BufferSizeCheck: (int) size
{
    if(size!=currentBufferSize)
    {
        free(bufferData);
        bufferData = (s_sample*)malloc(size*sizeof(s_sample));
        
        free(bufferDataTemp);
        bufferDataTemp = (s_sample*)malloc(size*sizeof(s_sample));
        
        currentBufferSize = size;
    }
}

- (void) refresh
{
    [self setNeedsDisplay];
    
}

- (void) OnNewBufferReady: (s_sample *) buff withSize: (int) size
{
    @synchronized (bufferLock){
        
        [self BufferSizeCheck:size];
        memcpy(bufferData, buff, size*sizeof(s_sample));
    }
    // [self setNeedsDisplay];
    [self performSelectorOnMainThread:@selector(refresh) withObject:nil waitUntilDone:NO];
}


//
- (void) generateRandomBuffer
{
    for(int i = 0; i<bufferSize; i++)
    {
        bufferDataTest[i]=((s_sample)arc4random() / ARC4RANDOM_MAX);
    }
    
    [self OnNewBufferReady:bufferDataTest withSize:bufferSize];
}

- (void) onTimerTriggered: (NSTimer *) tm
{
    [self generateRandomBuffer];
    [self setNeedsDisplay];
    
}

- (void)drawRect:(CGRect)rect
{
    // Drawing code
    
    
    bool useBezier = true;
    bool fillPlot = true;
    
    @synchronized (bufferLock){
        memcpy(bufferDataTemp, bufferData, currentBufferSize*sizeof(s_sample));
    }
    s_sample bufferStep = (s_sample)currentBufferSize/(s_sample)self.frame.size.width;
    
    CGRect frame = self.frame;
    int yCenter = frame.size.height/2;
    
    
    if(useBezier)
    {
       
        NSMutableArray *interpolationPoints = [[NSMutableArray alloc]init];
        
        
        CGPoint ptStart;
        ptStart.x = 0;
        ptStart.y =  borderSize + yCenter;
        const char *encoding = @encode(CGPoint);
        [interpolationPoints addObject:[NSValue valueWithBytes:&ptStart objCType:encoding]];
        
        bool somethingToDraw = false;
        for(int i = 0; i<frame.size.width; i+=6)
        {
            CGPoint pt1;
            pt1.x = i;
            pt1.y =  borderSize + ((frame.size.height-(2*borderSize))*bufferDataTemp[(int)((i)*bufferStep)]);
            [interpolationPoints addObject:[NSValue valueWithBytes:&pt1 objCType:encoding]];
            
            if(0!=pt1.y)
                somethingToDraw = true;
        }
        
        
        CGPoint ptEnd;
        ptEnd.x = frame.size.width;
        ptEnd.y =  borderSize + yCenter;
        [interpolationPoints addObject:[NSValue valueWithBytes:&ptEnd objCType:encoding]];
        
        
        if (somethingToDraw) {
            // Curve drawing
            UIBezierPath *path = [UIBezierPath interpolateCGPointsWithHermite: interpolationPoints closed:NO];
            
            
            if (path) {
                [[UIColor blackColor] setStroke];
                path.lineWidth = 1.5;
                [path stroke];
                
                if(fillPlot)
                {
                    [[UIColor blackColor] setFill];
                    path.usesEvenOddFillRule = YES;
                    [path fill];
                }
            }

        }
        
        [interpolationPoints release];

    } else
    {
        
        CGPoint pt1;
        pt1.x = 0;
        pt1.y =  borderSize + ((frame.size.height - (2*borderSize))*bufferDataTemp[0]);
        
        
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGMutablePathRef path = CGPathCreateMutable();
        CGPathMoveToPoint(path, nil, pt1.x, pt1.y);
        CGPathAddLineToPoint(path, nil, pt1.x, yCenter);
        
        
        bool somethingToDraw = false;
        for(int i = 1; i<frame.size.width; i++)
        {
            
            CGPoint pt2;
            pt2.x = i;
            pt2.y =  borderSize + ((frame.size.height-(2*borderSize))*bufferDataTemp[(int)((i)*bufferStep)]);
            CGPathAddLineToPoint(path, nil, pt2.x, pt2.y);
            
            if(fillPlot)
                CGPathAddLineToPoint(path, nil, pt2.x, yCenter);
            
            if(0!=pt2.y)
                somethingToDraw = true;
        }
        
        CGContextAddPath(context, path);
        CGContextSetAllowsAntialiasing(context, NO);
        // Do the drawing
        
        if(somethingToDraw)
            CGContextDrawPath(context, kCGPathStroke);
    }
    
    
    

    
    
}


@end
