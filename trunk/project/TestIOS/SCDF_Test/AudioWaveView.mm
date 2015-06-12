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

const int bufferSize = 1024;
const int borderSize = 0;
#define ARC4RANDOM_MAX      0x100000000

#ifdef DEBUG
//#define USE_TIMER_FOR_TESTING
#endif
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

- (bool) IsShowCoordinates;
{
    return showCoordinates;
}

- (void) setShowCoordinates: (bool) show
{
    showCoordinates = show;
}


- (bool) IsDrawBezier
{
    return drawBezier;
}

- (bool) IsFillPlot
{
    return fillPlot;
}

- (void) setDrawBezier: (bool) draw
{
    drawBezier = draw;
}

- (void) setFillPlot: (bool) doFill
{
    fillPlot = doFill;
}

- (void) initPlot
{
    // Initialization code
    audioListener = nil;
    drawBezier = true;
    fillPlot = true;
    showCoordinates = true;
    
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
    
    CGRect screenRect = [[UIScreen mainScreen]bounds];
    
    int numberOfSection = 4;
    int sectionSize = (fmax((int)screenRect.size.width, (int)screenRect.size.height))/numberOfSection;
    int labelOffset = 10;
    
    
    CGRect frame1;
    frame1.origin.x = labelOffset ;
    frame1.origin.y = 10;
    frame1.size.height = 20;
    frame1.size.width = 100;
    label1 = [[UILabel alloc] initWithFrame:frame1];
    [label1 setText: [NSString stringWithFormat:@"%d",0]];
    [self addSubview:label1];
    
    
    frame1.origin.x = sectionSize + labelOffset ;
    label2 = [[UILabel alloc] initWithFrame:frame1];
    [label2 setText:[NSString stringWithFormat:@"%d",(bufferSize/4)]];
    [self addSubview:label2];
    
    frame1.origin.x = 2*sectionSize + labelOffset ;
    label3 = [[UILabel alloc] initWithFrame:frame1];
    [label3 setText:[NSString stringWithFormat:@"%d",2*(bufferSize/4)]];
    [self addSubview:label3];
    
    frame1.origin.x = 3*sectionSize + labelOffset ;
    label4 = [[UILabel alloc] initWithFrame:frame1];
    [label4 setText:[NSString stringWithFormat:@"%d",3*(bufferSize/4)]];
    [self addSubview:label4];
    
    frame1.origin.x = 4*sectionSize - 50 ;
    label5 = [[UILabel alloc] initWithFrame:frame1];
    [label5 setText:[NSString stringWithFormat:@"%d",4*(bufferSize/4)]];
    [self addSubview:label5];
    
    
    labelsContainer = [[NSMutableArray alloc] init];
    [labelsContainer addObject:label1];
    [labelsContainer addObject:label2];
    [labelsContainer addObject:label3];
    [labelsContainer addObject:label4];
    [labelsContainer addObject:label5];
    
    
    
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

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
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
    [labelsContainer release];
    
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
    
    CGFloat alphaValue = 0.5f;
    
    
    @synchronized (bufferLock){
        memcpy(bufferDataTemp, bufferData, currentBufferSize*sizeof(s_sample));
    }
    
    if(NULL==bufferDataTemp)
        return;
    
    s_sample bufferStep = (s_sample)currentBufferSize/(s_sample)self.frame.size.width;
    
    CGRect frame = self.frame;
    int yCenter = frame.size.height/2;
    
    
    if(drawBezier)
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
                    UIColor *color= [UIColor blackColor];
                    [[color colorWithAlphaComponent:alphaValue] setFill];
                    
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
    
    
    // DRAW VERTICAL LINES
    int numberOfSection = 4;

    if(showCoordinates)
    {
        int sectionSize = frame.size.width/numberOfSection;
        
        for (int i = 0; i<numberOfSection+1;i++)
        {
            CGContextRef context = UIGraphicsGetCurrentContext();
            
            float dash[2]={6 ,5};
            CGContextSetLineDash(context, 0, dash, 2);
            
            CGPoint pt1;
            pt1.x = i*sectionSize;
            pt1.y = 0;
            
            CGPoint pt2;
            pt2.x = i*sectionSize;
            pt2.y = frame.size.height;
            
            CGMutablePathRef path2 = CGPathCreateMutable();
            CGPathMoveToPoint(path2, nil, pt1.x, pt1.y);
            CGPathAddLineToPoint(path2, nil, pt2.x, pt2.y);
            
            CGContextAddPath(context, path2);
            
            CGContextDrawPath(context, kCGPathStroke);
            
            
            
            CGPoint ptText;
            ptText.x = i*sectionSize+ 10;
            ptText.y = 0 + 10;
            
            [[labelsContainer objectAtIndex:i] setText:[NSString stringWithFormat:@"%d",i*(bufferSize/4)]];
        }
    }
    
    for (int i = 0; i<numberOfSection+1;i++)
    {
        [[labelsContainer objectAtIndex:i] setHidden:!showCoordinates];
    }
    

    
    
}


@end
