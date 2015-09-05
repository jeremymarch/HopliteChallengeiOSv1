//
//  DetailViewController.h
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "keyboard.h"

enum {
    PRACTICE = 0,
    MULTIPLE_CHOICE,
    TOURNAMENT
};

@interface DetailViewController : UIViewController <UISplitViewControllerDelegate>

@property (strong, nonatomic) id detailItem;

@property NSMutableArray *seen;
@property NSMutableArray *buttonStates;
@property NSMutableArray *levels;
@property bool front;
@property bool animate;
@property NSString *backCard;
@property CFTimeInterval startTime;
@property NSInteger menuItem;
@property (nonatomic, retain) Keyboard *keyboard;
@property (retain, nonatomic) IBOutlet UITextField *textfield;

@property NSString *greekFont;
@property NSString *systemFont;
@property NSInteger cardType;
@property NSInteger verbQuestionType;
@property NSArray *mcButtons;
@property BOOL backgroundOrBorder;
@property NSMutableArray *mcButtonsOrder;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@property (strong, nonatomic) IBOutlet UILabel *vocabFront;
@property (strong, nonatomic) IBOutlet UILabel *vocabBack;
@property (weak, nonatomic) IBOutlet UILabel *stemLabel;
@property (weak, nonatomic) IBOutlet UILabel *backLabel;
@property (weak, nonatomic) IBOutlet UILabel *timeLabel;

@property (weak, nonatomic) IBOutlet UIButton *menuButton;
@property (weak, nonatomic) IBOutlet UIButton *verbModeButton;

@property (weak, nonatomic) IBOutlet UIButton *correctButton;
@property (weak, nonatomic) IBOutlet UIButton *incorrectButton;

@property (weak, nonatomic) IBOutlet UIButton *MCButtonA;
@property (weak, nonatomic) IBOutlet UIButton *MCButtonB;
@property (weak, nonatomic) IBOutlet UIButton *MCButtonC;
@property (weak, nonatomic) IBOutlet UIButton *MCButtonD;

@property (weak, nonatomic) IBOutlet UILabel *origForm;
@property (weak, nonatomic) IBOutlet UILabel *changedForm;

@property (weak, nonatomic) IBOutlet UILabel *singLabel;
@property (weak, nonatomic) IBOutlet UILabel *pluralLabel;

-(void)setLevelArray: (NSMutableArray*)array;
- (void)configureView;
@end
