//
//  GamesTableViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 5/6/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "GamesTableViewController.h"
#import "ResultsViewController.h" //because we transition to this
#import "GameResult.h"
#import "sqlite3.h"
#import "HCColors.h"

extern sqlite3 *db;

@implementation GamesTableViewController

NSDateFormatter *dateFormat;

int getGamesCallback(void *selfRef, int argc, char **argv, char **azColName) {
    
    GameResult *a = [[GameResult alloc ]init];
    
    a.gameId = atoi(argv[0]);
    a.date = atoi(argv[1]);
    long d = a.date;
    //a.dateString = [NSString stringWithFormat:@"%s", ctime(&d) ];
    
    NSDate *date = [NSDate dateWithTimeIntervalSince1970:d];

    //[dateFormat setDateFormat:@"dd/MM/yyyy HH:mm"];
    [dateFormat setDateFormat:@"MMMM dd - HH:mm"];
    a.dateString = [dateFormat stringFromDate:date];
    a.score = atoi(argv[2]);
    
    id selfRef2 = (__bridge id)selfRef;
    [[selfRef2 gameResults] addObject:a];
    
    return 0;
}

-(void) getGames
{
    char *err_msg = 0;
    [self.gameResults removeAllObjects];
    dateFormat = [[NSDateFormatter alloc] init];
    
    //Gameid 1 is the practice game, so don't show it
    int rc = sqlite3_exec(db, "SELECT gameid,timest,score FROM games WHERE gameid > 1 ORDER BY gameid DESC;", getGamesCallback, (__bridge void *)(self), &err_msg);
    
    dateFormat = nil;
}

- (void) viewWillLayoutSubviews
{
    [super viewWillLayoutSubviews];
    [self.navigationController setNavigationBarHidden:NO];
}

- (void)viewWillAppear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:NO animated:YES];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    
    //[self.tableView registerClass:UITableViewCell.self forCellReuseIdentifier:@"GameResultsCell"];
    
    //NSLog(@"Games loaded");
    self.gameResults = nil;
    self.gameResults = [[NSMutableArray alloc] init];
    
    [self getGames];
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (self.gameResults && section == 0)
    {
        NSLog(@"gameResults count: %lu", (unsigned long)[self.gameResults count]);
        return [self.gameResults count];
    }
    else
    {
        return 0;
    }
}

- (UIView *) tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UIView *customTitleView = [ [UIView alloc] initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width, 34)];
    customTitleView.backgroundColor = [UIColor HCDarkBlue];
    
    customTitleView.autoresizingMask = (UIViewAutoresizingFlexibleWidth |
                          UIViewAutoresizingFlexibleLeftMargin |
                          UIViewAutoresizingFlexibleRightMargin);
    
    UILabel *titleLabel = [ [UILabel alloc] initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width, 34)];
    titleLabel.text = [NSString stringWithFormat:@"  Game History"];
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.backgroundColor = [UIColor HCDarkBlue];
    
    UILabel *scoreLabel = [ [UILabel alloc] initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width - 34, 34)];
    scoreLabel.textAlignment = NSTextAlignmentRight;
    scoreLabel.text = [NSString stringWithFormat:@"Score"];
    scoreLabel.textColor = [UIColor whiteColor];
    
    scoreLabel.autoresizingMask = (
                                        UIViewAutoresizingFlexibleLeftMargin
                                        );
    
    //NSLayoutConstraint *horizontalConstraint = NSLayoutConstraint(item: newView, attribute: NSLayoutAttribute.CenterX, relatedBy: NSLayoutRelation.Equal, toItem: view, attribute: NSLayoutAttribute.CenterX, multiplier: 1, constant: 0)
    /*
    NSLayoutConstraint *con = [NSLayoutConstraint constraintWithItem:scoreLabel
                                                                            attribute:NSLayoutAttributeRight
                                                                            relatedBy:NSLayoutRelationEqual
                                                                               toItem:self.tableView
                                                                            attribute:NSLayoutAttributeRight
                                                                           multiplier:0.00
                                                                             constant:0];
    
    [self.tableView addConstraint:con];
    */
    [customTitleView addSubview:titleLabel];
    [customTitleView addSubview:scoreLabel];
    
    return customTitleView;
}

//http://stackoverflow.com/questions/7105747/how-to-change-font-color-of-the-title-in-grouped-type-uitableview
-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 34;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"GameResultsCell" forIndexPath:indexPath];
    //UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"GameResultsCell"];
    
    // Configure the cell...
    UILabel *gameLabel = (UILabel *)[cell viewWithTag:98];
    UILabel *scoreLabel = (UILabel *)[cell viewWithTag:99];
    
    GameResult *h = (GameResult*)[self.gameResults objectAtIndex:indexPath.row];
    
    //NSLog(@"Date: %@, row: %li", h.dateString, (long)indexPath.row);
    
    gameLabel.text = h.dateString;
    scoreLabel.text = [NSString stringWithFormat:@"%ld", (long)h.score];
    //cell.tag = h.gameId; //don't set tags here, it corrupts the rows for some reason
    
    cell.accessoryView = nil;
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    return cell;
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //NSManagedObject *object = [[self fetchedResultsController] objectAtIndexPath:indexPath];
    /*
     if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
     self.detailViewController.detailItem = object;
     
     [self.detailViewController setLevelArray: self.popup.buttonStates];
     [self.detailViewController configureView];
     }
     */
    //if([[object valueForKey:@"sort"] integerValue] == 5)
    //{
    //UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    [self performSegueWithIdentifier:@"GamesToResultsSegue" sender:indexPath];
    //}
    
    
    /*
     NSNumber *a = [object valueForKey:@"sort"];
     NSInteger b = [a integerValue];
     
     self.detailViewController.menuItem = b;
     self.detailViewController.menuItem = (long)[a integerValue];
     NSLog(@"item2: %ld, %ld, %ld", (long)[a integerValue], b, self.detailViewController.menuItem);
     */
}

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    
    NSIndexPath *indexPath = (NSIndexPath *)sender;
    ResultsViewController *dvc = segue.destinationViewController;

    GameResult *g = [self.gameResults objectAtIndex:indexPath.row];
    dvc.gameId = g.gameId;
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
