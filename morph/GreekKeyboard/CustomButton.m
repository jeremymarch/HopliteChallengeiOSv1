//
//  CustomButton.m
//
//  Created by Chris Jones.
//  Copyright 2011 Chris Jones. All rights reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import "CustomButton.h"
#import "Common.h"
#import <QuartzCore/QuartzCore.h>

enum {
    IPAD = 0,
    IPHONE = 1
};

@implementation CustomButton
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
    if (self.device == IPHONE && !self.deleteButton)
    {
        CGRect buttonFrame = self.frame;
        buttonFrame = CGRectMake(self.frame.origin.x + (self.frame.size.width / 2) - ((self.frame.size.width / 4) + (self->hPadding)), self.frame.origin.y + self->buttonDownAddHeight, self->width, self->height);
        
        //@"῾", @"᾿", @"´", @"˜", @"¯", @"ͺ"
        NSString *l = self.titleLabel.text;
        if ([l isEqual:@"῾"] || [l isEqual:@"᾿"] || [l isEqual:@"´"] || [l isEqual:@"˜"] || [l isEqual:@"¯"] || [l isEqual:@"ͺ"])
        {
            self.titleLabel.font = [UIFont fontWithName:self.font size:50];
            if ([l isEqual: @"ͺ"])
            {
                self.titleEdgeInsets = UIEdgeInsetsMake(-42, 0, 0, 0);
            }
            else
            {
                self.titleEdgeInsets = UIEdgeInsetsMake(20, 0, 0, 0);
            }
        }
        else
        {
            self.titleLabel.font = [UIFont fontWithName:self.font size:24];
            self.titleEdgeInsets = UIEdgeInsetsMake(0, 0, 0, 0);
        }
        [self setFrame: buttonFrame];
    }
    
    [self setNeedsDisplay];
}

-(void) touchUpOutside:sender
{
    self.selected = NO;
    if (self.device == IPHONE && !self.deleteButton)
    {
        CGRect buttonFrame = self.frame;
        buttonFrame = CGRectMake(self.frame.origin.x + (self.frame.size.width / 2) - ((self.frame.size.width / 4) + (self->hPadding)), self.frame.origin.y + self->buttonDownAddHeight, self->width, self->height);
        
        self.titleEdgeInsets = UIEdgeInsetsMake(0, 0, 0, 0);
        self.titleLabel.font = [UIFont fontWithName:self.font size:22];
        [self setFrame: buttonFrame];
    }
    
    [self setNeedsDisplay];
}

-(void) touchDown:sender
{
    self.selected = YES;
    if (self.device == IPHONE && !self.deleteButton)
    {
        CGRect buttonFrame = CGRectMake(self.frame.origin.x + (self.frame.size.width / 2) - (self.frame.size.width - (2 * self->hPadding)), self.frame.origin.y - self->buttonDownAddHeight, (self.frame.size.width - (self->hPadding * 2)) * 2, self.frame.size.height + self->buttonDownAddHeight + 4);
        
        //the following order of these three lines make layoutsubviews only be called once,
        //instead of twice on ios 5.0 on iPhone.  Strange.
        NSString *l = self.titleLabel.text;
        if ([l isEqual:@"῾"] || [l isEqual:@"᾿"] || [l isEqual:@"´"] || [l isEqual:@"˜"] || [l isEqual:@"¯"] || [l isEqual:@"ͺ"])
        {
            self.titleLabel.font = [UIFont fontWithName:self.font size:60];
            if ([l isEqual: @"ͺ"])
            {
                self.titleEdgeInsets = UIEdgeInsetsMake(-95, 0, 0, 0);
            }
            else
            {
                self.titleEdgeInsets = UIEdgeInsetsMake(-25, 0, 0, 0);
            }
        }
        else
        {
            self.titleEdgeInsets = UIEdgeInsetsMake(-52.0, 0, 0, 0);
            self.titleLabel.font = [UIFont fontWithName:self.font size:44];
        }
        [self setFrame: buttonFrame];
    }

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
        
        if ([self.titleLabel.text isEqual:@"῾"] || [self.titleLabel.text isEqual:@"᾿"] || [self.titleLabel.text isEqual:@"´"] || [self.titleLabel.text isEqual:@"˜"] || [self.titleLabel.text isEqual:@"¯"] || [self.titleLabel.text isEqual:@"ͺ"])
        {
            self.diacriticButton = YES;
        }
        else
        {
            self.diacriticButton = NO;
        }
        
        if (self.diacriticButton)
            self.titleLabel.font = [UIFont fontWithName:self.font size:34];
        else if (self.device == IPHONE)
            self.titleLabel.font = [UIFont fontWithName:self.font size:22];
        else
            self.titleLabel.font = [UIFont fontWithName:self.font size:24];
        [self setTitleColor:[UIColor blackColor] forState:(UIControlStateNormal)];
        [self setTitle:text forState:UIControlStateNormal];
        //[self setTitleShadowColor:[UIColor whiteColor] forState:UIControlStateNormal];
        //self.titleLabel.shadowOffset = CGSizeMake(0.0, 1.0);
        
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
    CGContextRef context = UIGraphicsGetCurrentContext();
    UIColor *diacriticBackground = [UIColor colorWithRed:(170/255.0) green:(170/255.0) blue:(180/255.0) alpha:1.0];

    if (self.diacriticButton)
    {
        self.titleLabel.textColor = [UIColor whiteColor];
    }
    
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
    if (self.device == IPHONE && self.selected && !self.deleteButton)
        outerPath = createDepressedButtonForRect(outerRect, buttonRadius + 2);
    else if (self.device == IPAD)
        outerPath = createRoundedRectForRect(outerRect, buttonRadius + 2);
    else
        outerPath = createRoundedRectForRect(outerRect, buttonRadius);
    //add bottom shadow
    
    CGContextSaveGState(context);
    
    if (self.device == IPHONE && self.selected)
    {
        CGContextSetShadowWithColor(context, CGSizeMake(0, 0), 1.0, [UIColor blackColor].CGColor);
    }
    
    CGContextAddPath(context, outerPath);
    
    //set background color for up state
    if (self.diacriticButton)
        CGContextSetFillColorWithColor(context, diacriticBackground.CGColor);
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
            
            CGContextRestoreGState(context);
            CFRelease(highlightPath);
        }
	}
	else
    {
        //Down state
        CGContextSaveGState(context);
        
        CGContextAddPath(context, outerPath);
        CGContextClip(context);
        
        CGContextSaveGState(context);
        
        CGContextAddPath(context, outerPath);

        //set background color for downstate
        if (self.device == IPAD)
        {
            CGContextSetFillColorWithColor(context, buttonDark.CGColor);
        }
        else
        {
            if (self.device == IPAD)
            {
                if (self.diacriticButton)
                    CGContextSetFillColorWithColor(context, diacriticBackground.CGColor);
                else
                    CGContextSetFillColorWithColor(context, buttonDark.CGColor);
            }
            else
            {
                if (self.diacriticButton)
                    CGContextSetFillColorWithColor(context, diacriticBackground.CGColor);
                else
                    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
            }
        }
        
        CGContextFillPath(context);

        CGContextRestoreGState(context);
	}
    
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

@end
