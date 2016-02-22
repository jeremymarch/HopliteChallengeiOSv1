//
//  DetailViewController.h
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PopUp.h"
#import "keyboard.h"
#import "libmorph.h"

enum {
    HOPLITE_CHALLENGE = 0,
    HOPLITE_PRACTICE,
    SELF_PRACTICE,
    MULTIPLE_CHOICE
};

enum {
    CARD_VERBS = 1,
    CARD_PRINCIPAL_PARTS,
    CARD_ENDINGS,
        CARD_VOCAB,
    CARD_ACCENTS
};

@interface DetailViewController : UIViewController <UISplitViewControllerDelegate, vc2delegate, UIGestureRecognizerDelegate>
{
    VerbSeqOptions vsOptions;
    int verbSeq;
}

@property double typeInterval;
@property BOOL expanded;
@property BOOL mfPressed;
@property BOOL soundDisabled;
@property BOOL animationDisabled;
@property bool animate;
@property bool autoNav;
@property CFTimeInterval elapsedTimeLimit;
@property bool limitElapsedTime;

@property (strong, nonatomic) id detailItem;
@property NSInteger HCTime;
@property BOOL popupShown;
@property (nonatomic, retain) IBOutlet PopUp *popup;
@property (nonatomic, retain) CADisplayLink *displayLink;
@property NSMutableArray *seen;
@property NSMutableArray *buttonStates;
@property NSMutableArray *levels;
@property bool front;

@property NSString *backCard;
@property CFTimeInterval startTime;
@property NSInteger menuItem;
//@property (nonatomic, retain) Keyboard *keyboard;
@property (retain, nonatomic) IBOutlet UITextField *textfield;

@property (strong, retain) UIImageView *greenCheckView;
@property (strong, retain) UIImageView *redXView;

@property (strong, nonatomic) NSString *lemma;
@property (strong, nonatomic) NSString *origStr;
@property (strong, nonatomic) NSString *changedStr;
@property (strong, nonatomic) NSString *origStrDecomposed;
@property (strong, nonatomic) NSString *changedStrDecomposed;

@property (strong, nonatomic) NSString *greekFont;
@property (strong, nonatomic) NSString *systemFont;
@property double fontSize;
@property NSInteger cardType;
@property NSInteger verbQuestionType;
@property NSArray *mcButtons;
@property BOOL backgroundOrBorder;
@property NSMutableArray *mcButtonsOrder;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@property (strong, nonatomic) IBOutlet UILabel *MFLabel;
@property (strong, nonatomic) IBOutlet UILabel *changeTo;
@property (strong, nonatomic) IBOutlet UILabel *vocabFront;
@property (strong, nonatomic) IBOutlet UILabel *vocabBack;
@property (weak, nonatomic) IBOutlet UILabel *stemLabel;
@property (weak, nonatomic) IBOutlet UILabel *backLabel;
@property (weak, nonatomic) IBOutlet UILabel *timeLabel;

@property (weak, nonatomic) IBOutlet UIButton *menuButton;
@property (weak, nonatomic) IBOutlet UIButton *verbModeButton;
@property (weak, nonatomic) IBOutlet UIButton *continueButton;
@property (weak, nonatomic) IBOutlet UIButton *backButton;

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
-(void) loadNext;

@end
