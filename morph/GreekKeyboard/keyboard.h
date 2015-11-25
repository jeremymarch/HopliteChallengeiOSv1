//
//  keyboard.h
//  philolog.us
//
//  Created by Jeremy March on 3/11/13.
//  Copyright (c) 2013 Private. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CustomButton.h"
#import "DeleteButton.h"
#import "OtherButton.h"

@protocol vc2delegate <NSObject>          //[3] (in vc2.h)
- (void) loadNext;
- (void) preCheckVerbSubmit;
- (void) multipleFormsPressed;
@end

 @protocol k2abc <NSObject>          //[3] (in vc2.h)
 - (void) resetKeyboard;
 @end
 

@interface Keyboard : UIView <UIInputViewAudioFeedback, k2abc>
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
    int device;
}
//http://stackoverflow.com/questions/14228191/pass-a-reference-to-viewcontroller-in-prepareforsegue
//@property (nonatomic, weak) UIViewController *targetViewController;
@property (nonatomic, weak) id <vc2delegate> delegate;
//@property (nonatomic, weak) id <DetailViewController> delegate;


@property (nonatomic, retain) id<UITextInput> targetTextInput;
@property (nonatomic, retain) NSMutableArray *keys;
@property (nonatomic, retain) NSArray *greekLetters;
@property (nonatomic, retain) NSArray *latinLetters;
@property (nonatomic, retain) DeleteButton *deleteButton;
@property (nonatomic, retain) OtherButton *submitButton;
@property (nonatomic, retain) OtherButton *multipleFormsButton;
@property (nonatomic, retain) NSString *greekFont;
@property BOOL mfPressedOnce;

- (id)initWithFrame:(CGRect)frame lang:(int)lang;

-(void)resetKeyboard;
-(void)setButtons:(int) lang;
-(void)setTargetViewController:(UIViewController*) vc;
-(void)addAccent:(int)accent;

@end
