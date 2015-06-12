//
//  AudioWaveView.h
//  SCDF_Test
//
//  Created by Marco Bertola on 18/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "Receiver.h"


@class AudioWaveView;


class AudioWaveListener: public scdf::Receiver::ReceiverListener
{
    AudioWaveView *audioWaveViewRef;
public:
    void draw_buffer(s_sample *p_buff, unsigned int buff_len);
    void SetViewRef(AudioWaveView* viewRef){audioWaveViewRef = viewRef;}
};

@interface AudioWaveView : UIView

{
    
    bool drawBezier;
    bool fillPlot;
    bool showCoordinates;
    
    s_sample *bufferData;
    s_sample *bufferDataTemp;
    NSObject *bufferLock;
    
    s_sample *bufferDataTest;
    
    int currentBufferSize;
    
    AudioWaveListener *audioListener;
    
    UILabel *label1;
    UILabel *label2;
    UILabel *label3;
    UILabel *label4;
    UILabel *label5;
    
    NSMutableArray *labelsContainer;
    

}

- (void) setListener: (AudioWaveListener* ) listener;
- (void) initPlot;
- (bool) IsDrawBezier;
- (bool) IsFillPlot;
- (bool) IsShowCoordinates;
- (void) setDrawBezier: (bool) draw;
- (void) setFillPlot: (bool) doFill;
- (void) setShowCoordinates: (bool) show;

@end



