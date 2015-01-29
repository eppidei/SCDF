//
//  AudioWaveView.m
//  SCDF_Test
//
//  Created by Marco Bertola on 18/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import "AudioWaveView.h"

@implementation AudioWaveView

const int bufferSize = 256;
const int borderSize = 0;
#define ARC4RANDOM_MAX      0x100000000

//#define USE_TIMER_FOR_TESTING


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
        [NSTimer scheduledTimerWithTimeInterval:0.002
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
    
    CGRect frame = self.frame;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColor(context, CGColorGetComponents([[UIColor blueColor] CGColor]));
    s_sample bufferStep = (s_sample)currentBufferSize/(s_sample)self.frame.size.width;
    
    @synchronized (bufferLock){
        memcpy(bufferDataTemp, bufferData, currentBufferSize*sizeof(s_sample));
     }
    
   
    for(int i = 0; i<frame.size.width - 1; i++)
        {
            
            CGPoint pt1;
            pt1.x = i;
            pt1.y =  borderSize + ((frame.size.height - (2*borderSize))*bufferDataTemp[(int)(i*bufferStep)]);
            
            CGPoint pt2;
            pt2.x = i;
            pt2.y =  borderSize + ((frame.size.height-(2*borderSize))*bufferDataTemp[(int)((i+1)*bufferStep)]);
            
            
            CGContextMoveToPoint(context,pt1.x,pt1.y);
            CGContextAddLineToPoint(context,pt2.x,pt2.y);
            CGContextStrokePath(context);
        }
}


@end
