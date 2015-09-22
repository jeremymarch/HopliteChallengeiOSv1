//
//  PopUp.h
//  morph
//
//  Created by Jeremy on 11/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SettingsPopup : UIView <UITableViewDelegate, UITableViewDataSource>
@property (nonatomic, retain)
UITableView *table;
@property (nonatomic, retain) NSArray *labels;
@property (nonatomic, retain) NSMutableArray *buttons;
@property (nonatomic, retain) NSMutableArray *buttonStates;
@end

