//
//  SettingsTableTableViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 6/4/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "SettingsTableTableViewController.h"
#import "VerbsTableViewController.h"
#import "HCColors.h"

@interface SettingsTableTableViewController ()

@end

@implementation SettingsTableTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self->useSwitch = YES;
    self->unitsOrOptions = UNITS;
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    self.buttonStates = [[NSMutableArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:@"Levels"]];
    
    self.buttons = [[NSMutableArray alloc] initWithCapacity:NUM_LEVELS];
    
    for (int i = 0; i < NUM_LEVELS; i++)
    {
        [self.buttons insertObject:[NSString stringWithFormat:@"Unit %i", (i + 1)] atIndex:i];
        //[self.buttonStates insertObject:[NSNumber numberWithBool:NO] atIndex:i];
    }
    
    self.optionLabels = @[@"Disable Sound"];
    
    //self.viewHeader = [[UIView alloc]initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, 100)];
    self.viewHeader = [[UIView alloc]initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, 62)];
    [self.viewHeader setBackgroundColor:[UIColor whiteColor]];
    
    UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
    
    self.viewHeaderLabel = [[UILabel alloc]initWithFrame:CGRectMake(8, 12, 100, 62)];
    self.viewHeaderLabel.textColor = [UIColor HCBlue];
    self.viewHeaderLabel.text = @"H&Q Units";
    self.viewHeaderLabel.font = [UIFont fontWithName:@"Helvetica" size:20.0];
    [self.viewHeader addSubview:self.viewHeaderLabel];
    
    //bottom border of view header:
    CALayer *bottomBorder = [CALayer layer];
    bottomBorder.frame = CGRectMake(0.0f, self.viewHeader.frame.size.height, self.viewHeader.frame.size.width, 1.0f);
    bottomBorder.backgroundColor = [UIColor colorWithWhite:0.8f alpha:1.0f].CGColor;
    [self.viewHeader.layer addSublayer:bottomBorder];
    
    
    
    
    self.closeButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width - 70 - 10, 22, 70, 36)];
    [self.closeButton setTitle:@"Close" forState:UIControlStateNormal];
    [self.closeButton setTitleColor:blueColor forState:UIControlStateNormal];
    self.closeButton.layer.borderColor = blueColor.CGColor;
    self.closeButton.layer.borderWidth = 2.0f;
    self.closeButton.layer.cornerRadius = 8;
    self.closeButton.backgroundColor = [UIColor whiteColor];
    [self.viewHeader addSubview:self.closeButton];
    [self.closeButton addTarget:self
                         action:@selector(closeSettings:)
               forControlEvents:UIControlEventTouchDown];

    /*
     Temp disabling segment, change height of viewheader back to 100 when re-enable
    self.segment = [[UISegmentedControl alloc] initWithItems:@[@"H&Q Units", @"Options"]];
    self.segment.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.95];
    [self.segment setFrame:CGRectMake(6, 62, self.view.frame.size.width - 12, 38)];
    self.segment.tintColor = blueColor;
    //[self.segment setSegmentedControlStyle:UISegmentedControlStyleBar];
    self.segment.selectedSegmentIndex = 0;
    [self.segment addTarget:self action:@selector(segmentValueChaged:) forControlEvents:UIControlEventValueChanged];
    [self.viewHeader addSubview:self.segment];
     */
    
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void) closeSettings:(id)sender
{
    //[self.navigationController popViewControllerAnimated:NO];
    //[self performSegueWithIdentifier:@"showTutorialSegue" sender:self];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    
    if(section == 0)
    {
        return self.viewHeader;
    }
    return nil;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if(section == 0)
    {
        return 64;//100;
    }
    return UITableViewAutomaticDimension;
}

#pragma mark - Table view data source

-(IBAction)segmentValueChaged:(id)sender
{
    switch (self.segment.selectedSegmentIndex)
    {
        case UNITS:
        {
            self->unitsOrOptions = UNITS;
            break;
        }
        case OPTIONS:
        {
            self->unitsOrOptions = OPTIONS;
            break;
        }
    }
    [self.tableView reloadData];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
#warning Incomplete implementation, return the number of sections
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (self->unitsOrOptions == UNITS)
        return [self.buttons count];
    else
        return [self.optionLabels count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *simpleTableIdentifier = @"SimpleTableItem";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (indexPath.row >= [self.buttonStates count] || [self.buttonStates objectAtIndex:indexPath.row] == nil )
        [self.buttonStates insertObject:[NSNumber numberWithBool:NO] atIndex:indexPath.row];
    
    
    if( cell == nil )
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    
    if (self->unitsOrOptions == UNITS)
        cell.textLabel.text = [self.buttons objectAtIndex:indexPath.row];
    else
        cell.textLabel.text = [self.optionLabels objectAtIndex:indexPath.row];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (self->useSwitch)
    {
        /*if (self->unitsOrOptions == OPTIONS && indexPath.row == 0)
        {
            self.HCTimeField = [[UITextField alloc] initWithFrame:CGRectMake(-10, 0, 50, 40)];
            self.HCTimeField.font = [UIFont fontWithName:@"Helvetica" size:16.0];
            self.HCTimeField.textAlignment = NSTextAlignmentCenter;
            //self.HCTimeField.inputView = self.modePicker;
            self.HCTimeField.keyboardType = UIKeyboardTypeNumberPad;
            
            UIToolbar* numberToolbar = [[UIToolbar alloc]initWithFrame:CGRectMake(0, 0, 320, 50)];
            numberToolbar.barStyle = UIBarStyleBlackTranslucent;
            numberToolbar.items = @[[[UIBarButtonItem alloc]initWithTitle:@"Cancel" style:UIBarButtonItemStyleBordered target:self action:@selector(cancelNumberPad)],
                                    [[UIBarButtonItem alloc]initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil],
                                    [[UIBarButtonItem alloc]initWithTitle:@"Apply" style:UIBarButtonItemStyleDone target:self action:@selector(doneWithNumberPad)]];
            [numberToolbar sizeToFit];
            self.HCTimeField.inputAccessoryView = numberToolbar;
            
            
            cell.accessoryView = self.HCTimeField;
            cell.selectionStyle = UITableViewCellSelectionStyleNone;
            
            if ([defaults objectForKey:@"HCTime"] && [[defaults objectForKey:@"HCTime"] respondsToSelector:@selector(length)])
            {
                self.HCTimeField.text = [defaults objectForKey:@"HCTime"];
            }
            else
            {
                //default
                self.HCTimeField.text = @"30";
                [defaults setObject:@"30" forKey:@"HCTime"];
                [defaults synchronize];
            }
        }
        else if (self->unitsOrOptions == OPTIONS && indexPath.row == VERBS_ROW)
        {
            //for Verbs, just show an arrow.
            cell.accessoryView = nil;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        }
        else
        { */
            cell.selectionStyle = UITableViewCellSelectionStyleNone;
            UISwitch *switchView = [[UISwitch alloc] initWithFrame:CGRectZero];
            cell.accessoryView = switchView;
            
            BOOL b = NO;
            if (self->unitsOrOptions == UNITS)
            {
                b = ([[self.buttonStates objectAtIndex:indexPath.row] boolValue] == YES);
            }
            else if (self->unitsOrOptions == OPTIONS)
            {
                switch (indexPath.row) {
                    case HC_TIMEOUT:
                        //mode row
                        break;
                    case DISABLE_SOUND:
                        b = ([defaults boolForKey:@"DisableSound"] == YES) ? YES : NO;
                        break;
                    default:
                        break;
                }
            }
            
            //if ( [[self.buttonStates objectAtIndex:indexPath.row] boolValue] == YES)
            //  [switchView setOn:YES animated:NO];
            //else
            [switchView setOn:b animated:NO];
            [switchView addTarget:self action:@selector(switchChanged:) forControlEvents:UIControlEventValueChanged];
        //}
    }
    else
    {
        if ( [[self.buttonStates objectAtIndex:indexPath.row] boolValue] == YES)
            cell.accessoryType = UITableViewCellAccessoryCheckmark;
        else
            cell.accessoryType = UITableViewCellAccessoryNone;
    }
    return cell;
}

-(void)doneWithNumberPad{
    NSString *numberFromTheKeyboard = self.HCTimeField.text;
    
    NSInteger myInt = [numberFromTheKeyboard intValue];
    if (myInt < 5 || myInt > 300)
    {
        numberFromTheKeyboard = @"30";
        self.HCTimeField.text = numberFromTheKeyboard;
    }
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:numberFromTheKeyboard forKey:@"HCTime"];
    [defaults synchronize];
    [self.HCTimeField resignFirstResponder];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if (!self->useSwitch)
    {
        UITableViewCell* cell = [tableView cellForRowAtIndexPath:indexPath];
        BOOL b = (cell.accessoryType == UITableViewCellAccessoryCheckmark) ? NO : YES;
        
        [self.buttonStates setObject:[NSNumber numberWithBool:b] atIndexedSubscript:indexPath.row];
        [[NSUserDefaults standardUserDefaults] setObject:self.buttonStates forKey:@"Levels"];
        [tableView reloadData];
    }
    if (self->unitsOrOptions == OPTIONS)
    {
        if (indexPath.row == VERBS_ROW)
        {
            NSLog(@"open verbs table1");
            VerbsTableViewController *vdvc = [self.storyboard instantiateViewControllerWithIdentifier:@"vtvc"];
            [self.navigationController pushViewController:vdvc animated:NO];
            
            //UINavigationController *nvc = (UINavigationController *)self.navigationController;
            //[[nvc.childViewControllers objectAtIndex:0] performSegueWithIdentifier:@"SegueToVerbsTable"sender:self];
        NSLog(@"open verbs table2");
        }
    }
    /*
     else if (1)
     {
     if (indexPath.row == 4)
     {
     VerbDetailViewController *vdvc = [self.storyboard instantiateViewControllerWithIdentifier:@"vdvc"];
     [self.navigationController pushViewController:dvc animated:NO];
     }
     }
     */
}

//http://stackoverflow.com/questions/13121139/select-uitableviews-row-when-clicking-on-uiswitch
- (void) switchChanged:(id)sender {
    UISwitch *switchInCell = (UISwitch *)sender;
    UITableViewCell * cell = (UITableViewCell*) switchInCell.superview;
    NSIndexPath *indexPath = [self.tableView indexPathForCell:cell];
    BOOL b = (switchInCell.on) ? YES : NO;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (self->unitsOrOptions == UNITS)
    {
        [self.buttonStates setObject:[NSNumber numberWithBool:b] atIndexedSubscript:indexPath.row];
        [defaults setObject:self.buttonStates forKey:@"Levels"];
    }
    else
    {
        switch (indexPath.row) {
            case HC_TIMEOUT:
                //[defaults setBool:b forKey:@"HCTime"];
                break;
            case DISABLE_SOUND:
                [defaults setBool:b forKey:@"DisableSound"];
                break;
            default:
                break;
        }
    }
    [defaults synchronize];
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

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

//http://stackoverflow.com/questions/26069874/what-is-the-right-way-to-handle-orientation-changes-in-ios-8
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    
    // Code here will execute before the rotation begins.
    // Equivalent to placing it in the deprecated method -[willRotateToInterfaceOrientation:duration:]
    
    [coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext> context) {
        
        // Place code here to perform animations during the rotation.
        // You can pass nil or leave this block empty if not necessary.
        
        //[self.segment setFrame:CGRectMake(6, 62, size.width - 12 , 38)];
        //[self.viewHeader setFrame:CGRectMake(0, 0, size.width, 100)];
        [self.viewHeader setFrame:CGRectMake(0, 0, size.width, 58)];
        [self.closeButton setFrame:CGRectMake(size.width - 70 - 6, 24, 70, 36)];
        
        
        
        
    } completion:^(id<UIViewControllerTransitionCoordinatorContext> context) {

        // Code here will execute after the rotation has finished.
        // Equivalent to placing it in the deprecated method -[didRotateFromInterfaceOrientation:]
        //NSLog(@"size w: %f, h: %f", size.width, size.height);
    }];
}

@end
