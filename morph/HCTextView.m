//
//  HCTextView.m
//  Hoplite Challenge
//
//  Created by Jeremy on 6/11/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "HCTextView.h"

@implementation HCTextView

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

//http://stackoverflow.com/questions/22644610/uilabel-text-strikethrough-with-color


//http://stackoverflow.com/questions/15745824/uitextfield-how-to-disable-the-paste
- (BOOL)canPerformAction:(SEL)action withSender:(id)sender {
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        [[UIMenuController sharedMenuController] setMenuVisible:NO animated:NO];
    }];
    return [super canPerformAction:action withSender:sender];
}

@end
