//
//  PopUp.m
//  morph
//
//  Created by Jeremy on 11/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import "PopUp.h"

@implementation PopUp

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
        
        
        self.buttonStates = [[NSMutableArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:@"Levels"]];

        self.buttons = [[NSMutableArray alloc] initWithCapacity:15];
        
        for (int i = 0; i < 15; i++)
        {
            [self.buttons insertObject:[NSString stringWithFormat:@"H&Q Unit %i", (i + 1)] atIndex:i];
            //[self.buttonStates insertObject:[NSNumber numberWithBool:NO] atIndex:i];
        }
        
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
    CGFloat y = 0;
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
    CGFloat y = 0;
    CGFloat width = self.frame.size.width;
    CGFloat height = self.frame.size.height - 64;
    CGRect tableFrame = CGRectMake(x, y, width, height);
    self.table.frame = tableFrame;
 }

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.buttons count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *simpleTableIdentifier = @"SimpleTableItem";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    
    if (indexPath.row >= [self.buttonStates count] || [self.buttonStates objectAtIndex:indexPath.row] == nil )
        [self.buttonStates insertObject:[NSNumber numberWithBool:NO] atIndex:indexPath.row];
    
    if ( [[self.buttonStates objectAtIndex:indexPath.row] boolValue] == YES)
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    else
        cell.accessoryType = UITableViewCellAccessoryNone;
    
    cell.textLabel.text = [self.buttons objectAtIndex:indexPath.row];
    //cell.backgroundColor = [UIColor blueColor];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    UITableViewCell* cell = [tableView cellForRowAtIndexPath:indexPath];
    BOOL b = NO;
    if (cell.accessoryType == UITableViewCellAccessoryCheckmark)
    {
        NSLog(@"no");
        b = NO;
    }
    else
    {
        NSLog(@"yes");
        b = YES;
    }
    [self.buttonStates setObject:[NSNumber numberWithBool:b] atIndexedSubscript:indexPath.row];
    [[NSUserDefaults standardUserDefaults] setObject:self.buttonStates forKey:@"Levels"];
    [tableView reloadData];
}

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
