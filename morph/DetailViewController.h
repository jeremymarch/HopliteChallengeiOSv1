//
//  DetailViewController.h
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController <UISplitViewControllerDelegate>

@property (strong, nonatomic) id detailItem;

@property NSMutableArray *seen;
@property NSMutableArray *buttonStates;
@property NSMutableArray *levels;
@property bool front;
@property NSString *backCard;
@property CFTimeInterval startTime;
@property NSInteger menuItem;
@property NSInteger cardType;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@property (weak, nonatomic) IBOutlet UILabel *stemLabel;
@property (weak, nonatomic) IBOutlet UILabel *backLabel;
@property (weak, nonatomic) IBOutlet UILabel *timeLabel;

@property (weak, nonatomic) IBOutlet UILabel *singLabel;
@property (weak, nonatomic) IBOutlet UILabel *pluralLabel;

-(void)setLevelArray: (NSMutableArray*)array;
@end
