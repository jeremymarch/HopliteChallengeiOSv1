//
//  CustomTextField.m
//  morph
//
//  Created by Jeremy on 11/10/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import "CustomTextField.h"

@implementation CustomTextField

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
