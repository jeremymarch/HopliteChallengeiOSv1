//
//  PopUp.m
//  morph
//
//  Created by Jeremy on 11/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import "PopUp.h"

@implementation PopUp

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
    [self.table reloadData];
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        /*
         [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
         [[NSNotificationCenter defaultCenter] addObserver:self
         selector:@selector(orientationChanged:)
         name:UIDeviceOrientationDidChangeNotification
         object:nil];
         */
         self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin;
        
        //self.layer.borderColor = [[UIColor blackColor] CGColor];
        //self.layer.borderWidth = 2.0;
        self.alpha = 1.0;
        self->useSwitch = YES;
        self->unitsOrOptions = UNITS;
        self.backgroundColor = [UIColor whiteColor];
        
        self.closeButton = [[UIButton alloc] initWithFrame:CGRectMake(self.frame.size.width - 66 - 6, 6, 66, 36)];
        [self.closeButton setTitle:@"Close" forState:UIControlStateNormal];
        [self.closeButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        self.closeButton.layer.borderColor = [UIColor grayColor].CGColor;
        self.closeButton.layer.borderWidth = 2.0f;
        self.closeButton.layer.cornerRadius = 8;
        [self addSubview:self.closeButton];
        [self.closeButton addTarget:self.superview
                     action:@selector(animatePopUpShow:)
           forControlEvents:UIControlEventTouchDown];
        
        /*
        self.modePicker = [[UIPickerView alloc]initWithFrame:CGRectMake(0, 0, 100, 150)];
        self.modePicker.delegate = self;
        self.modePicker.dataSource = self;
        self.modePicker.showsSelectionIndicator = YES;
        */
        self.buttonStates = [[NSMutableArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:@"Levels"]];

        self.buttons = [[NSMutableArray alloc] initWithCapacity:15];
        
        for (int i = 0; i < 15; i++)
        {
            [self.buttons insertObject:[NSString stringWithFormat:@"Unit %i", (i + 1)] atIndex:i];
            //[self.buttonStates insertObject:[NSNumber numberWithBool:NO] atIndex:i];
        }
        
        self.optionLabels = @[@"Hoplite Challenge Timeout", @"Disable Animation", @"Disable Sound", @"White on Black", @"Include Dual", @"Verbs"];
        //self.modePickerLabels = @[@"Hoplite Challenge", @"Hoplite Practice", @"Self Practice", @"Multiple Choice"];
        
        self.segment=[[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"H&Q Units", @"Options", nil]];
        self.segment.tintColor = [UIColor grayColor];

        [self.segment setFrame:CGRectMake(0, 45, self.frame.size.width, 45)];
        //[self.segment setSegmentedControlStyle:UISegmentedControlStyleBar];
        self.segment.selectedSegmentIndex = 0;
        [self.segment addTarget:self action:@selector(segmentValueChaged:) forControlEvents:UIControlEventValueChanged];
        [self addSubview:self.segment];
        
        self.table = [self makeTableView];
        [self addSubview:self.table];
        
        self.table.delegate = self;
        self.table.dataSource = self;
        //[self.table reloadData];
    }
    return self;
}

-(UITableView *)makeTableView
{
    CGFloat x = 0;
    CGFloat y = 50.0 + 50.0;
    CGFloat width = self.frame.size.width;
    CGFloat height = self.frame.size.height - 64;
    CGRect tableFrame = CGRectMake(x, y, width, height);
    
    UITableView *tableView = [[UITableView alloc]initWithFrame:tableFrame style:UITableViewStylePlain];
    
    tableView.rowHeight = 45;
    tableView.sectionFooterHeight = 22;
    tableView.sectionHeaderHeight = 22;
    tableView.scrollEnabled = YES;
    tableView.showsVerticalScrollIndicator = YES;
    tableView.userInteractionEnabled = YES;
    tableView.bounces = YES;
    tableView.backgroundColor = [UIColor whiteColor];
    
    tableView.delegate = self;
    tableView.dataSource = self;
    
    return tableView;
}

-(void) layoutSubviews
{
    CGFloat x = 0;
    CGFloat y = 45 + 45;
    CGFloat width = self.frame.size.width;
    CGFloat height = self.frame.size.height - 64 - 50;
    CGRect tableFrame = CGRectMake(x, y, width, height);
    self.table.frame = tableFrame;
 }

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (self->unitsOrOptions == UNITS)
        return [self.buttons count];
    else
        return [self.optionLabels count];
}
//http://stackoverflow.com/questions/13121139/select-uitableviews-row-when-clicking-on-uiswitch
- (void) switchChanged:(id)sender {
    UISwitch *switchInCell = (UISwitch *)sender;
    UITableViewCell * cell = (UITableViewCell*) switchInCell.superview;
    NSIndexPath *indexPath = [self.table indexPathForCell:cell];
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
            case 0:
                //[defaults setBool:b forKey:@"HCTime"];
                break;
            case 1:
                [defaults setBool:b forKey:@"DisableAnimations"];
                break;
            case 2:
                [defaults setBool:b forKey:@"DisableSound"];
                break;
            case 3:
                [defaults setBool:b forKey:@"WhiteOnBlack"];
                break;
            case 4:
                [defaults setBool:b forKey:@"IncludeDual"];
                break;
            default:
                break;
        }
    }
    [defaults synchronize];
}

-(void)cancelNumberPad{
    [self.HCTimeField resignFirstResponder];
    self.HCTimeField.text = @"30";
}

-(void)doneWithNumberPad{
    NSString *numberFromTheKeyboard = self.HCTimeField.text;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:numberFromTheKeyboard forKey:@"HCTime"];
    [defaults synchronize];
    [self.HCTimeField resignFirstResponder];
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
        if (self->unitsOrOptions == OPTIONS && indexPath.row == 0)
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
        else
        {
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
                    case 0:
                        //mode row
                        break;
                    case 1:
                        b = ([defaults boolForKey:@"DisableAnimations"] == YES) ? YES : NO;
                        break;
                    case 2:
                        b = ([defaults boolForKey:@"DisableSound"] == YES) ? YES : NO;
                        break;
                    case 3:
                        b = ([defaults boolForKey:@"WhiteOnBlack"] == YES) ? YES : NO;
                        break;
                    case 4:
                        b = ([defaults boolForKey:@"IncludeDual"] == YES) ? YES : NO;
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
        }
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
        if (indexPath.row == 5)
        {
            //VerbDetailViewController *vdvc = [self.storyboard instantiateViewControllerWithIdentifier:@"vdvc"];
            //[self.navigationController pushViewController:dvc animated:NO];
            
            UINavigationController *nvc = (UINavigationController *)self.window.rootViewController;
            [[nvc.childViewControllers objectAtIndex:0] performSegueWithIdentifier:@"SegueToVerbsTable"sender:self];
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

/*  picker data source */
/*
- (void)pickerView:(UIPickerView *)pV didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    NSLog(@"picker pressed");
    NSIndexPath *indexPath=[NSIndexPath indexPathForRow:0 inSection:0];
    UITableViewCell *cell = [self.table cellForRowAtIndexPath:indexPath];
    UITextField *tf = (UITextField*)cell.accessoryView;
    tf.text = [self.modePickerLabels objectAtIndex:row];
    //tf.enabled = NO;
    [tf resignFirstResponder];
    
    [[NSUserDefaults standardUserDefaults] setObject:[self.modePickerLabels objectAtIndex:row] forKey:@"Mode"];
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return [self.modePickerLabels count];
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [self.modePickerLabels objectAtIndex:row];
}
*/
- (void)orientationChanged:(NSNotification *)notification
{
    UIDeviceOrientation deviceOrientation = [UIDevice currentDevice].orientation;
    CGRect rect = CGRectMake(0, 0, [[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.width);;
    
    switch (deviceOrientation) {
        case 1:
            [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortrait animated:NO];
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:0.1];
            self.transform = CGAffineTransformMakeRotation(0);
            self.bounds = [[UIScreen mainScreen] bounds];
            [UIView commitAnimations];
            break;
        case 2:
            [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortraitUpsideDown animated:NO];
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:0.1];
            self.transform = CGAffineTransformMakeRotation(-M_PI);
            self.bounds = [[UIScreen mainScreen] bounds];
            [UIView commitAnimations];
            break;
        case 3:
            [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight animated:NO];
            //rect = CGRectMake(0, 0, [[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.width);
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:0.1];
            self.transform = CGAffineTransformMakeRotation(M_PI_2);
            self.bounds = rect;
            [UIView commitAnimations];
            break;
        case 4:
            [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeLeft animated:NO];
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:0.1];
            //rect = CGRectMake(0, 0, [[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.width);
            self.transform = CGAffineTransformMakeRotation(-M_PI_2);
            self.bounds = rect;
            [UIView commitAnimations];
            break;
            
        default:
            break;
    }
}
@end
