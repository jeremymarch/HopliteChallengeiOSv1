//
//  HCResult.h
//  Hoplite Challenge
//
//  Created by Jeremy on 4/27/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HCResult : NSObject
@property NSInteger person;
@property NSInteger number;
@property NSInteger tense;
@property NSInteger voice;
@property NSInteger mood;
@property NSInteger verb;
@property NSString *stem;
@property NSString *wrongAnswer;
@property NSString *correctAnswer;
@property Boolean correct;
@property NSInteger timestamp;
@property NSString *elapsedTime;

- (id) initWithValues:(NSInteger)person number:(NSInteger)number;
@end
