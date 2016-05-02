//
//  HCResult.m
//  Hoplite Challenge
//
//  Created by Jeremy on 4/27/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "HCResult.h"

@implementation HCResult


- (id) init
{
    if (self = [super init])
    {

    }
    return self;
}

- (id) initWithValues:(NSInteger)person number:(NSInteger)number
{
    if (self = [super init])
    {
        self.person = person;
        self.number = number;
    }
    return self;
}

@end
