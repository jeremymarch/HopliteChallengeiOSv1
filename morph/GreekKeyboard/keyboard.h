//
//  keyboard.h
//  philolog.us
//
//  Created by Jeremy March on 3/11/13.
//  Copyright (c) 2013 Private. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CustomButton.h"

@interface Keyboard : UIView <UIInputViewAudioFeedback>
{
    int windowWidth;
    int width; //includes left and right padding
    int height; //includes top and bottom padding
    int hPadding;
    int vPadding;
    int topMargin;
    int spaceWidth;
    int buttonDownAddHeight;
    int lang;
    int deleteWidth;
}
@property (nonatomic, retain) id<UITextInput> targetTextInput;
@property (nonatomic, retain) NSMutableArray *keys;
@property (nonatomic, retain) NSArray *greekLetters;
@property (nonatomic, retain) NSArray *latinLetters;
@property (nonatomic, retain) CustomButton *deleteButton;

- (id)initWithFrame:(CGRect)frame lang:(int)lang;

-(void)setButtons:(int) lang;

@end
