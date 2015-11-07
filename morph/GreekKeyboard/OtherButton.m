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
        self.titleLabel.textColor = [UIColor whiteColor];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    //NSLog(@"Draw");
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
    UIColor *redColor = [UIColor colorWithRed:(255/255.0) green:(0/255.0) blue:(0/255.0) alpha:1.0];
    
    UIColor *delIconColor = [UIColor colorWithRed:(110/255.0) green:(110/255.0) blue:(128/255.0) alpha:1.0];
    
    CGFloat buttonRadius;
    CGFloat outerSideMargin;
    CGFloat outerTopMargin;
    if (self.device == IPHONE)
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
    if ([self.titleLabel.text isEqual:@"Done"])
        CGContextSetFillColorWithColor(context, blueColor.CGColor);
    else
        CGContextSetFillColorWithColor(context, redColor.CGColor);
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
        
        if (self.device == IPAD)
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

        self.titleLabel.textColor = [UIColor whiteColor];
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
            CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
        else if (self.device == IPAD)
            CGContextSetFillColorWithColor(context, delIconColor.CGColor);
        CGContextFillPath(context);
        
        CGContextRestoreGState(context);
        
        
        if ([self.titleLabel.text isEqual:@"Done"])
        {
            self.titleLabel.textColor = blueColor;
        }
        else
        {
            self.titleLabel.textColor = redColor;
        }
    }
    
    CGContextSaveGState(context);
    
    // Draw border around button for ipad
    if (self.device == IPAD)
    {
        CGContextSaveGState(context);
        CGContextSetLineWidth(context, 0.5);
        CGContextSetStrokeColorWithColor(context, [UIColor darkGrayColor].CGColor);
        CGContextAddPath(context, outerPath);
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
