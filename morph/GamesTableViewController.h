//
//  GamesTableViewController.h
//  Hoplite Challenge
//
//  Created by Jeremy on 5/6/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GamesTableViewController : UITableViewController
@property (retain,atomic) NSDateFormatter *df;
@property (retain,atomic) NSMutableArray *gameResults;
@end
