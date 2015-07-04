//
//  Common.m
//  CoolTable
//
//  Created by Ray Wenderlich on 9/29/10.
//  Copyright 2010 Ray Wenderlich. All rights reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import "Common.h"

/*
void logRect(NSString *prefix, CGRect rect) {
    NSLog(@"%@: %f %f %f %f", prefix, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

CGRect rectFor1PxStroke(CGRect rect) {
    return CGRectMake(rect.origin.x + 0.5, rect.origin.y + 0.5, rect.size.width - 1, rect.size.height - 1);
}
*/
void drawLinearGradient(CGContextRef context, CGRect rect, UIColor *startColor, UIColor *endColor) {
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGFloat locations[] = { 0.0, 1.0 };
    
    NSArray *colors = [NSArray arrayWithObjects:(__bridge id) startColor.CGColor, (__bridge id) endColor.CGColor, nil];
    
    CGGradientRef gradient = CGGradientCreateWithColors(colorSpace, (__bridge CFArrayRef) colors, locations);
    
    CGPoint startPoint = CGPointMake(CGRectGetMidX(rect), CGRectGetMinY(rect));
    CGPoint endPoint = CGPointMake(CGRectGetMidX(rect), CGRectGetMaxY(rect));
    
    CGContextSaveGState(context);
    
    CGContextAddRect(context, rect);
    CGContextClip(context);
    //
    CGContextDrawLinearGradient(context, gradient, startPoint, endPoint, 0);
    CGContextRestoreGState(context);
    
    CGColorSpaceRelease(colorSpace);
    CGGradientRelease(gradient);
}
/*
void draw1PxStroke(CGContextRef context, CGPoint startPoint, CGPoint endPoint, CGColorRef color) {
    
    CGContextSaveGState(context);
    CGContextSetLineCap(context, kCGLineCapSquare);
    CGContextSetStrokeColorWithColor(context, color);
    CGContextSetLineWidth(context, 1.0);
    CGContextMoveToPoint(context, startPoint.x + 0.5, startPoint.y + 0.5);
    CGContextAddLineToPoint(context, endPoint.x + 0.5, endPoint.y + 0.5);
    CGContextStrokePath(context);
    CGContextRestoreGState(context);        
    
}
*/

void drawLinearGloss(CGContextRef context, CGRect rect, BOOL reverse) {

    UIColor *highlightStart = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.35];
	UIColor *highlightEnd = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.1];

    if (reverse) {
		
		CGRect half = CGRectMake(rect.origin.x, rect.origin.y+rect.size.height/2, rect.size.width, rect.size.height/2);    
		drawLinearGradient(context, half, highlightEnd, highlightStart);
	}
	else {
		CGRect half = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height/2);    
		drawLinearGradient(context, half, highlightStart, highlightEnd);
	}
}

void drawCurvedGloss(CGContextRef context, CGRect rect, CGFloat radius) {
	
	UIColor *glossStart = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.6];
	UIColor *glossEnd = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.1];

	//CGFloat radius = 60.0f; //radius of gloss
	
	CGMutablePathRef glossPath = CGPathCreateMutable();
	
	CGContextSaveGState(context);
    CGPathMoveToPoint(glossPath, NULL, CGRectGetMidX(rect), CGRectGetMinY(rect)-radius+rect.size.height/2);
	CGPathAddArc(glossPath, NULL, CGRectGetMidX(rect), CGRectGetMinY(rect)-radius+rect.size.height/2, radius, 0.75f*M_PI, 0.25f*M_PI, YES);	
	CGPathCloseSubpath(glossPath);
	CGContextAddPath(context, glossPath);
	CGContextClip(context);
	
	CGMutablePathRef buttonPath=createRoundedRectForRect(rect, 6.0f);
	
	CGContextAddPath(context, buttonPath);
	CGContextClip(context);
	
	CGRect half = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height/2);    

	drawLinearGradient(context, half, glossStart, glossEnd);
	CGContextRestoreGState(context);
    
	CGPathRelease(buttonPath);
	CGPathRelease(glossPath);
}

/*
CGMutablePathRef createArcPathFromBottomOfRect(CGRect rect, CGFloat arcHeight) {
    
    CGRect arcRect = CGRectMake(rect.origin.x, rect.origin.y + rect.size.height - arcHeight, 
                                rect.size.width, arcHeight);
    
    CGFloat arcRadius = (arcRect.size.height/2) + (pow(arcRect.size.width, 2) / (8*arcRect.size.height));
    CGPoint arcCenter = CGPointMake(arcRect.origin.x + arcRect.size.width/2, arcRect.origin.y + arcRadius);
    
    CGFloat angle = acos(arcRect.size.width / (2*arcRadius));
    CGFloat startAngle = radians(180) + angle;
    CGFloat endAngle = radians(360) - angle;
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddArc(path, NULL, arcCenter.x, arcCenter.y, arcRadius, startAngle, endAngle, 0);
    CGPathAddLineToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMinY(rect));
    CGPathAddLineToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMinY(rect));
    CGPathAddLineToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMaxY(rect));
    return path;    
    
}
*/
void drawX(CGContextRef context, CGRect rect, int offset, UIColor *color)
{
    //center and make the rect square
    if (rect.size.width > rect.size.height)
    {
        rect.origin.x += (rect.size.width - rect.size.height) / 2;
        rect.size.width = rect.size.height;
    }
    else
    {
        rect.origin.y += (rect.size.height - rect.size.width) / 2;
        rect.size.height = rect.size.width;
    }
    
    CGContextSaveGState(context);
    CGContextSetLineWidth(context, 2.5f);
    CGContextSetStrokeColorWithColor(context, color.CGColor);
    CGContextMoveToPoint(context, CGRectGetMinX(rect) + offset, CGRectGetMinY(rect) + offset);
    CGContextAddLineToPoint(context, CGRectGetMaxX(rect) - offset, CGRectGetMaxY(rect) - offset);
    
    CGContextMoveToPoint(context, CGRectGetMaxX(rect) - offset, CGRectGetMinY(rect) + offset);
    CGContextAddLineToPoint(context, CGRectGetMinX(rect) + offset, CGRectGetMaxY(rect) - offset);
    CGContextStrokePath(context);
    CGContextRestoreGState(context);
}

CGMutablePathRef createDepressedButtonForRect(CGRect rect, CGFloat radius) {
    int inset = 8;
    int topHeight = 56;
    int midHeight = 20;
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, CGRectGetMidX(rect), CGRectGetMinY(rect));
    //NE
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMinY(rect), CGRectGetMaxX(rect), CGRectGetMaxY(rect), radius);

    CGPathAddLineToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMinY(rect) + topHeight);
    CGPathAddLineToPoint(path, NULL, CGRectGetMaxX(rect) - inset, CGRectGetMinY(rect) + topHeight + midHeight);
    
    //CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMidY(rect), CGRectGetMaxX(rect) - inset, CGRectGetMidY(rect), radius);

    //SE
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect) - inset, CGRectGetMaxY(rect), CGRectGetMinX(rect) - inset, CGRectGetMaxY(rect), radius);
    
    //SW
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect) + inset, CGRectGetMaxY(rect), CGRectGetMinX(rect) + inset, CGRectGetMinY(rect), radius);

    
    CGPathAddLineToPoint(path, NULL, CGRectGetMinX(rect) + inset, CGRectGetMinY(rect) + topHeight + midHeight);
    CGPathAddLineToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMinY(rect) + topHeight);
    //CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect) + inset, CGRectGetMidY(rect), CGRectGetMinX(rect), CGRectGetMidY(rect), radius);
    
    //NW
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMinY(rect), CGRectGetMaxX(rect), CGRectGetMinY(rect), radius);
    CGPathCloseSubpath(path);
    
    return path;
}

CGMutablePathRef createRoundedRectForRect(CGRect rect, CGFloat radius) {
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, CGRectGetMidX(rect), CGRectGetMinY(rect));
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMinY(rect), CGRectGetMaxX(rect), CGRectGetMaxY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMaxY(rect), CGRectGetMinX(rect), CGRectGetMaxY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMaxY(rect), CGRectGetMinX(rect), CGRectGetMinY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMinY(rect), CGRectGetMaxX(rect), CGRectGetMinY(rect), radius);
    CGPathCloseSubpath(path);
    
    return path;        
}

CGMutablePathRef createRoundedRectForRectCCW(CGRect rect, CGFloat radius) {
    
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, NULL, CGRectGetMidX(rect), CGRectGetMinY(rect));
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMinY(rect), CGRectGetMinX(rect), CGRectGetMaxY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMinX(rect), CGRectGetMaxY(rect), CGRectGetMaxX(rect), CGRectGetMaxY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMaxY(rect), CGRectGetMaxX(rect), CGRectGetMinY(rect), radius);
    CGPathAddArcToPoint(path, NULL, CGRectGetMaxX(rect), CGRectGetMinY(rect), CGRectGetMinX(rect), CGRectGetMinY(rect), radius);
    CGPathCloseSubpath(path);
    
    return path;        
}

NSNumberFormatter *standardNumberFormatter() {
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    [formatter setMaximumSignificantDigits:3];
	[formatter setUsesSignificantDigits:YES];
    //[formatter setMinimumSignificantDigits:2];
    //[formatter setMaximumFractionDigits:4];
    //[formatter setMinimumIntegerDigits:1];
    return formatter;
}