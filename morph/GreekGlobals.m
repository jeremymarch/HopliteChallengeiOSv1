//
//  GreekGlobals.m
//  morph
//
//  Created by Jeremy on 7/28/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#import "GreekGlobals.h"

@implementation GreekGlobals
//http://www.cocoanetics.com/2009/05/the-death-of-global-variables/

static GreekGlobals *_sharedInstance;

- (id) init
{
    if (self = [super init])
    {
        // custom initialization
        //memset(board, 0, sizeof(board));
    }
    return self;
}

+ (GreekGlobals *) sharedInstance
{
    if (!_sharedInstance)
    {
        _sharedInstance = [[GreekGlobals alloc] init];
    }
    
    return _sharedInstance;
}

@end
