//
//  GameResult.h
//  Hoplite Challenge
//
//  Created by Jeremy on 5/7/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface GameResult : NSObject
@property NSInteger gameId;
@property NSInteger date;
@property (retain,atomic) NSString *dateString;
@property NSInteger score;
@end
