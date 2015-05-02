//
//  MasterViewController.h
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PopUp.h"

@class DetailViewController;

#import <CoreData/CoreData.h>

@interface MasterViewController : UITableViewController  <NSFetchedResultsControllerDelegate>

@property BOOL popupShown;
@property (nonatomic, retain) IBOutlet PopUp *popup;

@property (strong, nonatomic) DetailViewController *detailViewController;

@property (strong, nonatomic) NSFetchedResultsController *fetchedResultsController;
@property (strong, nonatomic) NSManagedObjectContext *managedObjectContext;

@end
