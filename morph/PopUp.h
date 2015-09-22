//
//  PopUp.h
//  morph
//
//  Created by Jeremy on 11/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

enum {
    UNITS = 0,
    OPTIONS
};

@interface PopUp : UIView <UITableViewDelegate, UITableViewDataSource, UIPickerViewDataSource, UIPickerViewDelegate>
{
    int unitsOrOptions;
    int useSwitch;
}
@property (nonatomic, retain) UITableView *table;
@property (nonatomic, retain) UISegmentedControl *segment;
@property (nonatomic, retain) NSMutableArray *buttons;
@property (nonatomic, retain) NSArray *optionLabels;
@property (nonatomic, retain) NSArray *modePickerLabels;
@property (nonatomic, retain) NSMutableArray *buttonStates;
@property (nonatomic, retain) UIPickerView *modePicker;
@end
