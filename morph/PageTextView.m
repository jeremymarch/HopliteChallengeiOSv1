//
//  PageTextView.m
//  Hoplite Challenge
//
//  Created by Jeremy on 8/30/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "PageTextView.h"

@implementation PageTextView

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    return YES;
}

//http://stackoverflow.com/questions/36198299/uitextview-disable-selection-allow-links
- (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
    if ([gestureRecognizer isKindOfClass:[UITapGestureRecognizer class]] )
    {
        NSLog(@"tap");
        [gestureRecognizer setEnabled:false];
    }
    else if ([gestureRecognizer isKindOfClass:[UILongPressGestureRecognizer class]]) {
        
        @try {
            id targetAndAction = ((NSMutableArray *)[gestureRecognizer valueForKey:@"_targets"]).firstObject;
            NSArray <NSString *>*actions = @[@"action=loupeGesture:",           // link: no, selection: shows circle loupe and blue selectors for a second
                                             @"action=longDelayRecognizer:",    // link: no, selection: no
                                             //@"action=smallDelayRecognizer:", // link: yes (no long press), selection: no
                                             @"action=oneFingerForcePan:",      // link: no, selection: shows rectangular loupe for a second, no blue selectors
                                             @"action=_handleRevealGesture:"
                                             ];  // link: no, selection: no
            for (NSString *action in actions) {
                if ([[targetAndAction description] containsString:action]) {
                    [gestureRecognizer setEnabled:false];
                }
            }
            
        }
        
        @catch (NSException *e) {
        }
        
        @finally {
            [super addGestureRecognizer: gestureRecognizer];
        }
    }
    else
    {
        [super addGestureRecognizer: gestureRecognizer];
    }
    
}

@end
