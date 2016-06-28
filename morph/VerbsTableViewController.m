//
//  VerbsTableViewController.m
//  morph
//
//  Created by Jeremy on 7/23/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#import "VerbsTableViewController.h"
#import "VerbDetailViewController.h"
#import "GreekForms.h"

#define NUM_LEVELS 15

@interface VerbsTableViewController ()

@end

@implementation VerbsTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.navigationController setNavigationBarHidden:NO];
    self.title = @"Verbs";
    self.navigationItem.titleView = [[UIView alloc] init]; //HIDES title from display
    //self.navigationItem.titleView.hidden = YES; //doesn't work, use above
    
    //Verbs per unit
    self->verbsPerSection[0] = 2;
    self->verbsPerSection[1] = 2;
    self->verbsPerSection[2] = 4;
    self->verbsPerSection[3] = 4;
    self->verbsPerSection[4] = 4;
    self->verbsPerSection[5] = 4;
    self->verbsPerSection[6] = 3;
    self->verbsPerSection[7] = 2;
    self->verbsPerSection[8] = 4;
    self->verbsPerSection[9] = 6;
    self->verbsPerSection[10] = 7;
    self->verbsPerSection[11] = 8;
    self->verbsPerSection[12] = 8;
    self->verbsPerSection[13] = 8;
    self->verbsPerSection[14] = 8;
    self->verbsPerSection[15] = 8;
    self->verbsPerSection[16] = 6;
    self->verbsPerSection[17] = 9;
    self->verbsPerSection[18] = 9;
    self->verbsPerSection[19] = 7;
    
    
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
#warning Potentially incomplete method implementation.
    // Return the number of sections.
    return NUM_LEVELS;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
#warning Incomplete method implementation.
    // Return the number of rows in the section.
    return self->verbsPerSection[section];
}
/*
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return [NSString stringWithFormat:@"Section %ld", section + 1];
}
*/

//http://stackoverflow.com/questions/7105747/how-to-change-font-color-of-the-title-in-grouped-type-uitableview
-(CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 30;
}

- (UIView *) tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    UIView *customTitleView = [ [UIView alloc] initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width, 30)];
    UILabel *titleLabel = [ [UILabel alloc] initWithFrame:CGRectMake(0, 0, tableView.bounds.size.width, 30)];
    
    titleLabel.text = [NSString stringWithFormat:@"  Unit %ld", section + 1];
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.backgroundColor = [UIColor blackColor];
    [customTitleView addSubview:titleLabel];
    
    return customTitleView;
}

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
        [self performSegueWithIdentifier:@"SegueToVerbDetail"sender:self];
    //}

    
    /*
     NSNumber *a = [object valueForKey:@"sort"];
     NSInteger b = [a integerValue];
     
     self.detailViewController.menuItem = b;
     self.detailViewController.menuItem = (long)[a integerValue];
     NSLog(@"item2: %ld, %ld, %ld", (long)[a integerValue], b, self.detailViewController.menuItem);
     */
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"VerbTableCell" forIndexPath:indexPath];
    NSInteger index = 0;
    // Configure the cell...
    for (int i = 0; i < indexPath.section; i++)
        index += self->verbsPerSection[i];
    
    index += indexPath.row;
    
    cell.textLabel.text = [NSString stringWithUTF8String: verbs[index].present];
    cell.textLabel.font = [UIFont fontWithName:@"NewAthenaUnicode" size:26.0];
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


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];
    
    NSInteger index = 0;
    // Configure the cell...
    for (int i = 0; i < indexPath.section; i++)
        index += self->verbsPerSection[i];
    
    index += indexPath.row;
    
    [[segue destinationViewController] setVerbIndex: index];
}


@end
