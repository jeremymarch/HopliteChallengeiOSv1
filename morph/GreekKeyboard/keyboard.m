//
//  keyboard.m
//  philolog.us
//
//  Created by Jeremy March on 3/11/13.
//  Copyright (c) 2013 Private. All rights reserved.
//
#include <math.h> // for M_PI
#import <QuartzCore/QuartzCore.h>
#import "keyboard.h"

enum {
    GREEK = 1,
    LATIN = 2
};

#define   DEGREES_TO_RADIANS(degrees)  ((M_PI * degrees)/ 180)

#define BUTTON_DOWN_TOP_CORNER_RAD 5.0
#define BUTTON_DOWN_BOTTOM_CORNER_RAD 5.0
#define BUTTON_DOWN_UPPER_HEIGHT 56.0
#define BUTTON_DOWN_MIDDLE_HEIGHT 20.0
#define BUTTON_DOWN_LOWER_HEIGHT 28.0
#define BUTTON_DOWN_WIDTH_DIFF 10.0

@implementation Keyboard

@synthesize targetTextInput;
@synthesize keys;
@synthesize deleteButton;

- (BOOL) enableInputClicksWhenVisible {
    return YES;
}

/**
 * Set Frame to 320 x 218 for four rows, default height
 * For three rows use 320 x 174-164 depending on the margin you want.
 */
- (id)initWithFrame:(CGRect)frame lang:(int)theLang
{
    self = [super initWithFrame:frame];
    int device;
    if (self)
    {
        // Initialization code

        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        {
            self->windowWidth = 320;
            self->width = 32; //includes left and right padding
            self->height = 54; //includes top and bottom padding
            self->hPadding = 3;
            self->vPadding = 8;
            self->topMargin = 4;
            self->spaceWidth = 150;
            self->buttonDownAddHeight = 55;
            device = 1;
        }
        else
        {
            if ( UIDeviceOrientationIsPortrait([[UIDevice currentDevice] orientation]) )
                self->windowWidth = 768;
            else
                self->windowWidth = 1024;
            self->width = 93; //includes left and right padding
            self->height = 86; //includes top and bottom padding
            self->hPadding = 7;
            self->vPadding = 6;
            self->topMargin = 4;
            self->spaceWidth = 150;
            self->buttonDownAddHeight = 55;
            device = 0;
        }
        //[super setAutoresizingMask: UIViewAutoresizingNone];
        //self.autoresizingMask = UIViewAutoresizingNone; //so that the keyboard can have a custom height; actually not really needed.
        [self setAutoresizingMask: UIViewAutoresizingFlexibleWidth];
        [self setAutoresizesSubviews: NO];


        CAGradientLayer *gradient = [CAGradientLayer layer];
        gradient.frame = self.bounds;
        
        UIColor *lightColor;
        UIColor *darkColor;
        if (device == 0) //iPad
        {
            //lightColor = [UIColor colorWithRed:(187/255.0) green:(187/255.0) blue:(198/255.0) alpha:1.0];
            //darkColor = [UIColor colorWithRed:(143/255.0) green:(143/255.0) blue:(155/255.0) alpha:1.0];
            
            lightColor = [UIColor colorWithRed:(187/255.0) green:(187/255.0) blue:(198/255.0) alpha:1.0];
            darkColor = [UIColor colorWithRed:(148/255.0) green:(148/255.0) blue:(162/255.0) alpha:1.0];
        }
        else //iPhone
        {
            lightColor = [UIColor colorWithRed:(185/255.0) green:(185/255.0) blue:(197/255.0) alpha:1.0];
            darkColor = [UIColor colorWithRed:(130/255.0) green:(130/255.0) blue:(150/255.0) alpha:1.0];
        }
        gradient.colors = [NSArray arrayWithObjects:(id)[lightColor CGColor], (id)[darkColor CGColor], nil];
        [self.layer insertSublayer:gradient atIndex:0];

        //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didRotate:) name:UIDeviceOrientationDidChangeNotification object:nil];
        
        // Keep track of the textView/Field that we are editing
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(editingDidBegin:)
                                                     name:UITextFieldTextDidBeginEditingNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(editingDidBegin:)
                                                     name:UITextViewTextDidBeginEditingNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(editingDidEnd:)
                                                     name:UITextFieldTextDidEndEditingNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(editingDidEnd:)
                                                     name:UITextViewTextDidEndEditingNotification
                                                   object:nil];
        
        self.latinLetters = [NSArray arrayWithObjects: [NSArray arrayWithObjects:@"Q", @"W", @"E", @"R", @"T", @"Y", @"U", @"I", @"O", @"P", nil], [NSArray arrayWithObjects:@"A", @"S", @"D", @"F", @"G", @"H", @"J", @"K", @"L", nil], [NSArray arrayWithObjects:@"Z", @"X", @"C", @"V", @"B", @"N", @"M", nil], nil];
        
        self.greekLetters = [NSArray arrayWithObjects: [NSArray arrayWithObjects:@"Ε", @"Ρ", @"Τ", @"Υ", @"Θ", @"Ι", @"Ο", @"Π" /*, @"Ϝ" */, nil], [NSArray arrayWithObjects:@"Α", @"Σ", @"Δ", @"Φ", @"Γ", @"Η", @"Ξ", @"Κ", @"Λ", nil], [NSArray arrayWithObjects:@"Ζ", @"Χ", @"Ψ", @"Ω", @"Β", @"Ν", @"Μ", nil], nil];
        
        self.keys = [[NSMutableArray alloc] init];
        int numKeys = 26;
        for (int i = 0; i < numKeys; i++)
        {
            CustomButton* button = [[CustomButton alloc] initWithText:@"" AndDevice:device];
            //button.frame = CGRectMake(0 + (i * self->width), self->topMargin + (1 * self->height), self->width, self->height);

            [self.keys addObject: button];
            [self addSubview:button];

            //button.hidden = YES;
            [button addTarget:self action:@selector(keyboardLetterUpInside:) forControlEvents:UIControlEventTouchUpInside];
            [button addTarget:self action:@selector(keyboardLetterUpOutside:) forControlEvents:UIControlEventTouchUpOutside];
            [button addTarget:self action:@selector(keyboardLetterDown:) forControlEvents:UIControlEventTouchDown];
            //[button release];
        }
        self.deleteButton = [[CustomButton alloc] initWithText:@"XXX" AndDevice:device];
        [self addSubview:self.deleteButton];
        [self.deleteButton addTarget:self action:@selector(keyboardDeletePressed:) forControlEvents:UIControlEventTouchDown];
        
        [self setButtons:theLang];
    }
    return self;
}

-(void) layoutSubviews
{
    //NSLog(@"Layout sub views keyboard width: %f", self.bounds.size.width);

    if ( self.bounds.size.width < 321.0 )
    {
        self->windowWidth = 320;
        self->width = 32; //includes left and right padding
        self->height = 54; //includes top and bottom padding
        self->hPadding = 3;
        self->vPadding = 8;
        self->topMargin = 4;
        self->spaceWidth = 150;
        self->buttonDownAddHeight = 55;
        
        self->deleteWidth = self->height - 16 + 6;
    }
    else if ( self.bounds.size.width < 769.0 )
    {
        self->windowWidth = 768;
        self->width = 76; //includes left and right padding
        self->height = 76; //includes top and bottom padding
        self->hPadding = 5;
        self->vPadding = 6;
        self->topMargin = 14;
        self->spaceWidth = 150;
        self->buttonDownAddHeight = 55;
        
        self->deleteWidth = self->width;
    }
    else
    {
        self->windowWidth = 1024;
        self->width = 94; //includes left and right padding
        self->height = 86; //includes top and bottom padding
        self->hPadding = 7;
        self->vPadding = 6;
        self->topMargin = 4;
        self->spaceWidth = 150;
        self->buttonDownAddHeight = 55;
        
        self->deleteWidth = self->width;
    }
    
    NSArray *letterRows;
    
    if (self->lang == GREEK)
        letterRows = self.greekLetters;
    else
        letterRows = self.latinLetters;
    
    int letter = 0;
    int key = 0;
    double rowStart = 0;
    int row;
    int rows = [letterRows count];
    for (row = 0; row < rows; row++)
    {
        NSArray *letters = [letterRows objectAtIndex:row];
        int numLetters = [letters count];
        //account for the delete button on iPad
        if (row == 2 && self->windowWidth > 320)
            rowStart = (self.bounds.size.width - ((numLetters + 1) * self->width) ) / 2;
        else
            rowStart = (self.bounds.size.width - (numLetters * self->width) ) / 2;
        
        for (letter = 0; letter < numLetters; letter++)
        {
            CustomButton *button = [self.keys objectAtIndex:key++];
            //Check that it's not selected because layout subviews is called
            //when button is selected and this would negate that resizing on iphone
            if (button.selected == NO)
                button.frame = CGRectMake(rowStart + (letter * self->width), self->topMargin + (row * self->height), self->width, self->height);
        }
    }
    if (self->windowWidth < 321)
    {
        self.deleteButton.frame = CGRectMake(self->windowWidth - self->width - 15, self->topMargin + (2 * self->height), self->deleteWidth, self->height);
    }
    else
    {
        self.deleteButton.frame = CGRectMake(rowStart + (letter * self->width), self->topMargin + (2 * self->height), self->deleteWidth, self->height);
        //needed to redraw icon and x at correct size
        [self.deleteButton setNeedsDisplay];
    }
    if (self->lang == GREEK)
    {
        UIButton *button;
        button = [self.keys objectAtIndex:24];
        button.hidden = YES;
        button = [self.keys objectAtIndex:25];
        button.hidden = YES;
    }
    else
    {
        UIButton *button;
        button = [self.keys objectAtIndex:24];
        if (button.hidden == YES)
        {
            button.hidden = NO;
            [button setNeedsDisplay];
        }
        button = [self.keys objectAtIndex:25];
        if (button.hidden == YES)
        {
            button.hidden = NO;
            [button setNeedsDisplay];
        }
    }
}

-(void)setButtons:(int) theLang
{
    NSArray *letterRows;
    self->lang = theLang;
    if (self->lang == GREEK)
        letterRows = self.greekLetters;
    else
        letterRows = self.latinLetters;
    
    int letter = 0;
    int key = 0;
    for (int row = 0; row < [letterRows count]; row++)
    {
        NSArray *letters = [letterRows objectAtIndex:row];
        int numLetters = [letters count];
        
        for (letter = 0; letter < numLetters; letter++)
        {
            CustomButton *button = [self.keys objectAtIndex:key++];
            [button setTitle:[letters objectAtIndex:letter] forState:UIControlStateNormal];
        }
    }
    [self setNeedsLayout]; //def need this
}

#pragma mark - editingDidBegin/End

// Editing just began, store a reference to the object that just became the firstResponder
- (void)editingDidBegin:(NSNotification *)notification {
    if (![notification.object conformsToProtocol:@protocol(UITextInput)]) {
        self.targetTextInput = nil;
        return;
    }
    
    self.targetTextInput = notification.object;
}

// Editing just ended.
- (void)editingDidEnd:(NSNotification *)notification {
    self.targetTextInput = nil;
}

#pragma mark - Keypad IBAction's

- (IBAction)keyboardLetterDown:(UIButton *)sender
{
    [[UIDevice currentDevice] playInputClick];
    UIButton *button = sender;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
        [self bringSubviewToFront:button];
    }
    else
    {

    }

}

- (IBAction)keyboardLetterUpOutside:(UIButton *)sender
{
    /*UIButton *button = sender;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {

    }
    else
    {

    }
*/
}

- (IBAction)keyboardLetterUpInside:(UIButton *)sender {
    /*
    UIButton *button = sender;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
 
    }
    else
    {

    }
    */
    //Add letter to text field:
    if (!self.targetTextInput) {
        return;
    }
    
    NSString *numberPressed  = [sender titleLabel].text;
    if ([numberPressed length] == 0) {
        return;
    }
    
    UITextRange *selectedTextRange = self.targetTextInput.selectedTextRange;
    if (!selectedTextRange) {
        return;
    }
    [self textInput:self.targetTextInput replaceTextAtTextRange:selectedTextRange withString:[numberPressed lowercaseString]];
}

/**
 * To do: http://stackoverflow.com/questions/3284792/uibutton-touch-and-hold
 */
- (IBAction)keyboardDeletePressed:(UIButton *)sender
{
    if (!self.targetTextInput)
    {
        return;
    }
    [[UIDevice currentDevice] playInputClick];
    
    UITextRange *selectedTextRange = self.targetTextInput.selectedTextRange;
    if (!selectedTextRange) {
        return;
    }
    
    // Calculate the selected text to delete
    UITextPosition  *startPosition  = [self.targetTextInput positionFromPosition:selectedTextRange.start offset:-1];
    if (!startPosition) {
        return;
    }
    UITextPosition *endPosition = selectedTextRange.end;
    if (!endPosition) {
        return;
    }
    UITextRange *rangeToDelete = [self.targetTextInput textRangeFromPosition:startPosition
                                                                       toPosition:endPosition];
    
    [self textInput:self.targetTextInput replaceTextAtTextRange:rangeToDelete withString:@""];
}

// The clear button was just pressed on the number pad
- (IBAction)keyboardClearPressed:(UIButton *)sender {
    if (!self.targetTextInput) {
        return;
    }
    [[UIDevice currentDevice] playInputClick];
    
    UITextRange *allTextRange = [self.targetTextInput textRangeFromPosition:self.targetTextInput.beginningOfDocument
                                                                 toPosition:self.targetTextInput.endOfDocument];
    
    [self textInput:self.targetTextInput replaceTextAtTextRange:allTextRange withString:@""];
}

// The done button was just pressed on the number pad
- (IBAction)keyboardDonePressed:(UIButton *)sender {
    if (!self.targetTextInput) {
        return;
    }
    [[UIDevice currentDevice] playInputClick];
    
    // Call the delegate methods and resign the first responder if appropriate
    if ([self.targetTextInput isKindOfClass:[UITextView class]]) {
        UITextView *textView = (UITextView *)self.targetTextInput;
        if ([textView.delegate respondsToSelector:@selector(textViewShouldEndEditing:)]) {
            if ([textView.delegate textViewShouldEndEditing:textView]) {
                [textView resignFirstResponder];
            }
        }
    } else if ([self.targetTextInput isKindOfClass:[UITextField class]]) {
        UITextField *textField = (UITextField *)self.targetTextInput;
        if ([textField.delegate respondsToSelector:@selector(textFieldShouldEndEditing:)]) {
            if ([textField.delegate textFieldShouldEndEditing:textField]) {
                [textField resignFirstResponder];
            }
        }
    }
}

#pragma mark - text replacement routines

// Check delegate methods to see if we should change the characters in range
- (BOOL)textInput:(id <UITextInput>)textInput shouldChangeCharactersInRange:(NSRange)range withString:(NSString *)string
{
    if (!textInput) {
        return NO;
    }
    
    if ([textInput isKindOfClass:[UITextField class]]) {
        UITextField *textField = (UITextField *)textInput;
        if ([textField.delegate respondsToSelector:@selector(textField:shouldChangeCharactersInRange:replacementString:)]) {
            if (![textField.delegate textField:textField
                 shouldChangeCharactersInRange:range
                             replacementString:string]) {
                return NO;
            }
        }
    } else if ([textInput isKindOfClass:[UITextView class]]) {
        UITextView *textView = (UITextView *)textInput;
        if ([textView.delegate respondsToSelector:@selector(textView:shouldChangeTextInRange:replacementText:)]) {
            if (![textView.delegate textView:textView
                     shouldChangeTextInRange:range
                             replacementText:string]) {
                return NO;
            }
        }
    }
    return YES;
}

// Replace the text of the textInput in textRange with string if the delegate approves
- (void)textInput:(id <UITextInput>)textInput replaceTextAtTextRange:(UITextRange *)textRange withString:(NSString *)string {
    if (!textInput) {
        return;
    }
    if (!textRange) {
        return;
    }
    
    // Calculate the NSRange for the textInput text in the UITextRange textRange:
    int startPos                    = [textInput offsetFromPosition:textInput.beginningOfDocument
                                                         toPosition:textRange.start];
    int length                      = [textInput offsetFromPosition:textRange.start
                                                         toPosition:textRange.end];
    NSRange selectedRange           = NSMakeRange(startPos, length);
    
    if ([self textInput:textInput shouldChangeCharactersInRange:selectedRange withString:string]) {
        // Make the replacement:
        [textInput replaceRange:textRange withText:string];
    }
}

@end