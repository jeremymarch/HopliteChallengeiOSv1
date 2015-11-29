//
//  OtherButton.m
//  morph
//
//  Created by Jeremy on 11/7/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import "OtherButton.h"
#import "Common.h"
#import <QuartzCore/QuartzCore.h>

enum {
    IPAD = 0,
    IPHONE = 1
};

@implementation OtherButton
@synthesize selected = _selected;
@synthesize toggled = _toggled;
@synthesize device = _device;

-(id) initWithCoder:(NSCoder *)aDecoder {
    if ((self = [super initWithCoder:aDecoder])) {
        self.opaque = NO;
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

-(void) touchUpInside:sender
{
    self.selected = NO;
    [self setNeedsDisplay];
}

-(void) touchUpOutside:sender
{
    self.selected = NO;
    [self setNeedsDisplay];
}

-(void) touchDown:sender
{
    self.selected = YES;
    [self setNeedsDisplay];
}

- (id)initWithText:(NSString *)text AndDevice: (int) device AndFont:(NSString*)font
{
    if ((self = [super initWithFrame:CGRectZero]))
    {
        //([self.titleLabel.text isEqual:@"MF"] || [self.titleLabel.text isEqual:@","]) &&
        self.mfPressed = NO;
        self.font = font;
        self.device = device;
        [self addTarget: self action: @selector(touchUpInside:) forControlEvents: UIControlEventTouchUpInside];
        [self addTarget: self action: @selector(touchUpOutside:) forControlEvents: UIControlEventTouchUpOutside];
        [self addTarget: self action: @selector(touchDown:) forControlEvents: UIControlEventTouchDown];
        
        self->width = 32; //includes left and right padding
        self->height = 54; //includes top and bottom padding
        self->hPadding = 3;
        self->vPadding = 8;
        self->topMargin = 4;
        self->buttonDownAddHeight = 62;
        
        [self setTitle:text forState:UIControlStateNormal];
        self.titleLabel.font = [UIFont fontWithName:self.font size:24.0];
        [self setTitleColor:[UIColor whiteColor] forState:(UIControlStateNormal)];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    //NSLog(@"Draw");
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
    UIColor *redColor = [UIColor colorWithRed:(255/255.0) green:(56/255.0) blue:(0/255.0) alpha:1.0];
    
    UIColor *delIconColor = [UIColor colorWithRed:(110/255.0) green:(110/255.0) blue:(128/255.0) alpha:1.0];
    
    CGFloat buttonRadius;
    CGFloat outerSideMargin;
    CGFloat outerTopMargin;
    if (1)//self.device == IPHONE)
    {
        buttonRadius = 4.0f;
        outerSideMargin = 2.75f;
        outerTopMargin = 5.0f;//7.0f;
    }
    else
    {
        buttonRadius = 6.0f;
        outerSideMargin = 6.0f;
        outerTopMargin = 6.0f;
    }
    
    CGRect outerRect = CGRectInset(self.bounds, outerSideMargin, outerTopMargin);
    
    CGMutablePathRef outerPath;
    if (self.device == IPAD)
        outerPath = createRoundedRectForRect(outerRect, buttonRadius + 2);
    else
        outerPath = createRoundedRectForRect(outerRect, buttonRadius);
    //add bottom shadow
    
    CGContextSaveGState(context);
    
    CGContextAddPath(context, outerPath);
    if ([self.titleLabel.text isEqual:@"Enter"])
        CGContextSetFillColorWithColor(context, blueColor.CGColor);
    else
    {
        if (self.mfPressed && !self.selected)
            CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
        else
            CGContextSetFillColorWithColor(context, redColor.CGColor);
    }
    CGContextFillPath(context);
    CGContextRestoreGState(context);
    
    if (!self.selected)
    {
        //up state
        // Draw gradient for outer path
        CGContextSaveGState(context);
        CGContextAddPath(context, outerPath);
        CGContextClip(context);
        
        CGContextRestoreGState(context);
        
        if (0)//self.device == IPAD)
        {
            //the 1px highlight on top
            CGContextSaveGState(context);
            CGRect highlightRect;
            if (self.device == IPAD)
                highlightRect = CGRectInset(outerRect, 2.0f, 2.0f);
            else
                highlightRect = CGRectInset(outerRect, 1.0f, 1.0f);
            CGMutablePathRef highlightPath = createRoundedRectForRect(highlightRect, buttonRadius);
            CGContextAddPath(context, outerPath);
            CGContextAddPath(context, highlightPath);
            CGContextEOClip(context);
            
            CGContextRestoreGState(context);
            CFRelease(highlightPath);
        }
        NSLog(@"mf is pressed");
        if (self.mfPressed)
            [self setTitleColor:redColor forState:(UIControlStateNormal)];
        else
            [self setTitleColor:[UIColor whiteColor] forState:(UIControlStateNormal)];
    }
    else
    {
        //Down state
        CGContextSaveGState(context);
        
        CGContextAddPath(context, outerPath);
        CGContextClip(context);
        
        CGContextSaveGState(context);
        
        CGContextAddPath(context, outerPath);
        if (self.device == IPHONE)
        {
            if ((self.mfPressed && !self.selected) || [self.titleLabel.text isEqual:@"Enter"])
                CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
            else
                CGContextSetFillColorWithColor(context, redColor.CGColor);
        }
        else if (self.device == IPAD)
        {
            if ((self.mfPressed && !self.selected) || [self.titleLabel.text isEqual:@"Enter"])
                CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
            else
                CGContextSetFillColorWithColor(context, redColor.CGColor);
        }
        CGContextFillPath(context);
        
        CGContextRestoreGState(context);
        
        
        if ([self.titleLabel.text isEqual:@"Enter"])
        {
            [self setTitleColor:blueColor forState:(UIControlStateNormal)];
        }
        else
        {
            if (self.mfPressed)
                [self setTitleColor:[UIColor whiteColor] forState:(UIControlStateNormal)];
            else
                [self setTitleColor:redColor forState:(UIControlStateNormal)];
        }
    }
    
    CGContextSaveGState(context);
    
    // Draw border around button for ipad
    if (self.mfPressed)//self.device == IPAD)
    {
        CGContextSaveGState(context);
        CGContextSetLineWidth(context, 3.0);
        
        if(self.selected)
            CGContextSetStrokeColorWithColor(context, [UIColor whiteColor].CGColor);
        else
            CGContextSetStrokeColorWithColor(context, redColor.CGColor);
        
        CGRect highlightRect = CGRectInset(outerRect, 1.0f, 1.0f);
        CGMutablePathRef highlightPath = createRoundedRectForRect(highlightRect, buttonRadius);
        CGContextAddPath(context, highlightPath);
        CGContextStrokePath(context);
        CGContextRestoreGState(context);
    }
    CFRelease(outerPath);
}
/*
 - (void)dealloc
 {
 [super dealloc];
 }
 */
@end
