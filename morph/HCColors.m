//
//  HCColors.m
//  Hoplite Challenge
//
//  Created by Jeremy on 8/13/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "HCColors.h"

@implementation UIColor (CustomColors)

//UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
//UIColor *darkBlueColor = [UIColor colorWithRed:(0/255.0) green:(70/255.0) blue:(183/255.0) alpha:1.0];
//UIColor *lightBlueColor = [UIColor colorWithRed:(160/255.0) green:(220/255.0) blue:(255/255.0) alpha:1.0];

+ (UIColor *)HCBlue {
    
    static UIColor *color;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        color = [UIColor colorWithRed:0.0 / 255.0
                                green:122 / 255.0
                                 blue:255.0 / 255.0
                                alpha:1.0];
    });
    
    return color;
}

+ (UIColor *)HCDarkBlue {
    
    static UIColor *color;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        color = [UIColor colorWithRed:0.0 / 255.0
                                green:0.0 / 255.0
                                 blue:110.0 / 255.0
                                alpha:1.0];
    });
    
    return color;
}

+ (UIColor *)HCLightBlue {
    
    static UIColor *color;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        color = [UIColor colorWithRed:140.0 / 255.0
                                green:220.0 / 255.0
                                 blue:255.0 / 255.0
                                alpha:1.0];
    });
    
    return color;
}

@end
