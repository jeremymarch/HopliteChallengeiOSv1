//
//  VerbDetailViewController.h
//  morph
//
//  Created by Jeremy on 7/23/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "libmorph.h"

@interface VerbDetailViewController : UIViewController <UIGestureRecognizerDelegate>
@property (strong, nonatomic) UIScrollView *view;
@property NSInteger verbIndex;
@property BOOL expanded;

-(void)setVerbIndex2:(NSInteger)verbIndex;
-(NSInteger)verbIndex2;
-(NSString *) getPPString: (Verb *)verb;

@end
