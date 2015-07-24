//
//  VerbDetailViewController.h
//  morph
//
//  Created by Jeremy on 7/23/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface VerbDetailViewController : UIViewController
@property (strong, nonatomic) UIScrollView *view;
@property NSInteger verbIndex;

-(void)setVerbIndex2:(NSInteger)verbIndex;
-(NSInteger)verbIndex2;
@end
