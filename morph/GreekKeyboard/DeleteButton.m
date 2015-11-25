//
//  DeleteButton.m
//  morph
//
//  Created by Jeremy on 11/7/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import "DeleteButton.h"
#import "Common.h"
#import <QuartzCore/QuartzCore.h>

enum {
    IPAD = 0,
    IPHONE = 1
};

@implementation DeleteButton
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
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    //NSLog(@"Draw");
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    UIColor *highlightStart = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.8];
    //CGColorRef highlightStop = [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.0].CGColor;
    
    
    UIColor *buttonLight;
    UIColor *buttonDark;
    if (self.device == IPHONE)
    {
        buttonLight = [UIColor colorWithRed:(229/255.0) green:(230/255.0) blue:(233/255.0) alpha:1.0];
        buttonDark = [UIColor colorWithRed:(203/255.0) green:(204/255.0) blue:(210/255.0) alpha:1.0];
    }
    else
    {
        buttonLight = [UIColor colorWithRed:(240/255.0) green:(240/255.0) blue:(243/255.0) alpha:1.0];
        buttonDark = [UIColor colorWithRed:(209/255.0) green:(209/255.0) blue:(215/255.0) alpha:1.0];
    }
    
    UIColor *buttonDownLight = [UIColor colorWithRed:(145/255.0) green:(145/255.0) blue:(160/255.0) alpha:1.0];
    UIColor *buttonDownDark = [UIColor colorWithRed:(130/255.0) green:(130/255.0) blue:(147/255.0) alpha:1.0];
    //UIColor *buttonDownLight = [UIColor colorWithRed:(120/255.0) green:(120/255.0) blue:(138/255.0) alpha:1.0];
    //UIColor *buttonDownDark = [UIColor colorWithRed:(110/255.0) green:(110/255.0) blue:(128/255.0) alpha:1.0];
    
    UIColor *delIconColorLight = [UIColor colorWithRed:(120/255.0) green:(120/255.0) blue:(138/255.0) alpha:1.0];
    UIColor *delIconColor = [UIColor colorWithRed:(110/255.0) green:(110/255.0) blue:(128/255.0) alpha:1.0];
    UIColor *xColor = [UIColor darkGrayColor];
    
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
    if (self.device == IPHONE)
        CGContextSetFillColorWithColor(context, delIconColor.CGColor);
    else
        CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
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
            //drawLinearGradient(context, CGRectMake(outerRect.origin.x, outerRect.origin.y, outerRect.size.width, outerRect.size.height/6), highlightStart, buttonLight);
            
            CGContextRestoreGState(context);
            CFRelease(highlightPath);
        }
    }
    else
    {
        //Down state
        // Draw gradient for outer path
        
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
    }

    //Draw delete button icon
    CGContextSaveGState(context);
    //if (self.device == IPAD)
    //    CGContextSetShadowWithColor(context, CGSizeMake(0, 1), 1.0, [UIColor whiteColor].CGColor);
    CGFloat delHeight = self.bounds.size.height;
    CGFloat delWidth = self.bounds.size.width;
    CGFloat topPadding;
    CGFloat sidePadding;
    CGFloat xIconPadding;
    if (self.device == IPAD)
    {
        topPadding = self.bounds.size.height / 2.8f;
        sidePadding = self.bounds.size.width / 3.0f;
        xIconPadding = self.bounds.size.width / 15.5f;//6.0f;
    }
    else
    {
        topPadding = self.bounds.size.height / 3.2f;
        sidePadding = self.bounds.size.width / 4.0f;
        xIconPadding = 4.0f;
    }
    //NSLog(@"H: %f, W: %f", self.bounds.size.height, self.bounds.size.width);
    
    CGFloat delRadius = 2.0f;
    CGFloat startX = ((delWidth + (1 * sidePadding)) * 1 / 3) - 1;
    
    CGMutablePathRef path = CGPathCreateMutable();
    
    CGPathMoveToPoint(path, NULL, startX, topPadding);
    //CGPathAddLineToPoint(path, NULL, delWidth - sidePadding, topPadding);
    CGPathAddArcToPoint(path, NULL, delWidth - sidePadding, topPadding, delWidth - sidePadding, delHeight - topPadding, delRadius);
    //CGPathAddLineToPoint(path, NULL, delWidth - sidePadding, delHeight - topPadding);
    CGPathAddArcToPoint(path, NULL, delWidth - sidePadding, delHeight - topPadding, startX, delHeight - topPadding
                        , delRadius);
    
    CGPathAddLineToPoint(path, NULL, startX, delHeight - topPadding);
    CGPathAddLineToPoint(path, NULL, sidePadding - 2, delHeight / 2.0f);
    
    CGPathCloseSubpath(path);
    
    CGContextAddPath(context, path);
    UIColor *iconColor;
    if (self.device == IPAD)
    {
        if (self.selected)
        {
            iconColor = buttonLight;
            xColor = delIconColor;
        }
        else
        {
            iconColor = delIconColor;
            xColor = buttonLight;
            
        }
    }
    else
    {
        if (self.selected)
        {
            iconColor = delIconColor;
            xColor = buttonLight;
        }
        else
        {
            iconColor = buttonLight;
            xColor = delIconColor;
        }
    }
    CGContextSetFillColorWithColor(context, iconColor.CGColor);
    CGContextFillPath(context);
    CGContextRestoreGState(context);
    CFRelease(path);
    
    CGContextSaveGState(context);
    CGRect xFrame = CGRectMake(startX, topPadding, delWidth - (sidePadding * 2) - (startX - sidePadding), delHeight - (topPadding * 2));
    
    drawX(context, xFrame, xIconPadding, xColor);
    
    CGContextRestoreGState(context);
    
    // Draw border around button for ipad
    if (0)//self.device == IPAD)
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
