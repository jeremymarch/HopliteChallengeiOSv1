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
    if (action == @selector(copy:) || action == @selector(select:) || action == @selector(paste:))
        return NO;
    
    return [super canPerformAction:action withSender:sender];
}
/*
 //put this function in delegate to prevent selection of text.
 -(void)textViewDidChangeSelection:(UITextView *)textView {
 //NSLog(@"Range: %d, %d", textView.selectedRange.location, textView.selectedRange.length);
    if (textView.selectedRange.length > 0)
    {
        textView.selectedRange = NSMakeRange(textView.selectedRange.location, 0);
    }
 }
*/

//this prevents select all when double clicking, but not when you brush finger over text to select.
-(BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer {
    // Check for gestures to prevent
    if ([gestureRecognizer isKindOfClass:[UITapGestureRecognizer class]]) {
        // Check for double tap
        if (((UITapGestureRecognizer *)gestureRecognizer).numberOfTapsRequired == 2) {
            // Prevent the double tap
            return NO;
        }
    }
    
    // Always anything that makes it here
    return YES;
}

/*
 //http://stackoverflow.com/questions/36198299/uitextview-disable-selection-allow-links
 @implementation UITextView (NoFirstResponder)
 
 - (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
 if ([gestureRecognizer isKindOfClass:[UILongPressGestureRecognizer class]]) {
 
 @try {
 id targetAndAction = ((NSMutableArray *)[gestureRecognizer valueForKey:@"_targets"]).firstObject;
 NSArray <NSString *>*actions = @[@"action=loupeGesture:",           // link: no, selection: shows circle loupe and blue selectors for a second
 @"action=longDelayRecognizer:",    // link: no, selection: no
 //@"action=smallDelayRecognizer:", // link: yes (no long press), selection: no
 @"action=oneFingerForcePan:",      // link: no, selection: shows rectangular loupe for a second, no blue selectors
 @"action=_handleRevealGesture:"];  // link: no, selection: no
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
 }
 
 @end
 */

@end
