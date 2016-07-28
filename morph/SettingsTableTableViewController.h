//
//  SettingsTableTableViewController.h
//  Hoplite Challenge
//
//  Created by Jeremy on 6/4/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

enum {
    
    DISABLE_SOUND = 0,
    HC_TIMEOUT = 1,
    VERBS_ROW = 2
};

#define NUM_LEVELS 20

enum {
    UNITS = 0,
    OPTIONS
};

@interface SettingsTableTableViewController : UITableViewController
{
    int unitsOrOptions;
    int useSwitch;
}
@property (nonatomic, retain) UIButton *closeButton;
@property (nonatomic, retain) UIView *viewHeader;
@property (nonatomic, retain) UISegmentedControl *segment;
@property (nonatomic, retain) NSMutableArray *buttons;
@property (nonatomic, retain) NSArray *optionLabels;
@property (nonatomic, retain) NSArray *modePickerLabels;
@property (nonatomic, retain) NSMutableArray *buttonStates;
@property (nonatomic, retain) UIPickerView *modePicker;
@property (nonatomic, retain) UITextField *HCTimeField;

@end


