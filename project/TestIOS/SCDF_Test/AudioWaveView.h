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
    s_sample *bufferData;
    NSObject *bufferLock;
    
    s_sample *bufferDataTest;
    
    int currentBufferSize;
    
    AudioWaveListener *audioListener;

}

- (void) setListener: (AudioWaveListener* ) listener;

@end



