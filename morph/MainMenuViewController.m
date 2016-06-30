//
//  MainMenuViewController.m
//  morph
//
//  Created by Jeremy on 9/30/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import "MainMenuViewController.h"
#import "DetailViewController.h"
#import "VerbDetailViewController.h"
#import "ResultsViewController.h"
#import "TutorialPageViewController.h"
#import "VerbsTableViewController.h"

#define UIColorFromRGB(rgbValue) \
[UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 \
green:((float)((rgbValue & 0x00FF00) >>  8))/255.0 \
blue:((float)((rgbValue & 0x0000FF) >>  0))/255.0 \
alpha:1.0]

@interface MainMenuViewController ()

@end

@implementation MainMenuViewController

//http://stackoverflow.com/questions/26069874/what-is-the-right-way-to-handle-orientation-changes-in-ios-8
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    
    // Code here will execute before the rotation begins.
    // Equivalent to placing it in the deprecated method -[willRotateToInterfaceOrientation:duration:]
    
    [coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext> context) {
        
        // Place code here to perform animations during the rotation.
        // You can pass nil or leave this block empty if not necessary.
        
        double sw = size.width;
        double sh = size.height;

        /*
         [self.HCButton setFrame:CGRectMake(10, sh - 360, sw - 20, 80)];
         [self.gamesButton setFrame:CGRectMake(10, sh - 270, sw - 20, 80)];
         [self.HPButton setFrame:CGRectMake(10, sh - 180, sw - 20, 80)];
         [self.resultsButton setFrame:CGRectMake(10, sh - 90, sw - 20, 80)];
         
         [self.tempFormsButton setFrame:CGRectMake(10, sh - 90, sw - 20, 80)];
         */
        
        [self.HCButton setFrame:CGRectMake(10, sh - 400, sw - 20, 70)];
        [self.gamesButton setFrame:CGRectMake(10, sh - 320, sw - 20, 70)];
        [self.HPButton setFrame:CGRectMake(10, sh - 240, sw - 20, 70)];
        [self.resultsButton setFrame:CGRectMake(10, sh - 160, sw - 20, 70)];
        [self.tempFormsButton setFrame:CGRectMake(10, sh - 80, sw - 20, 70)];
        
        
        
        NSLog(@"Rotate menu: %f", size.width);
        
        [self.HCLabel setFrame:CGRectMake(0, 120, self.view.bounds.size.width, self.HCLabel.frame.size.height)];
        [self.menuButton setFrame:CGRectMake(sw - 70 - 6, 24, 70, 36)];
        
        [self.popup setFrame:CGRectMake (0, size.height + 200, size.width, size.height)];
        [self.popup.table setFrame:CGRectMake (0, size.height + 200, size.width, size.height)];
        

        
    } completion:^(id<UIViewControllerTransitionCoordinatorContext> context) {
        [self.popup setNeedsDisplay];
        // Code here will execute after the rotation has finished.
        // Equivalent to placing it in the deprecated method -[didRotateFromInterfaceOrientation:]
        //NSLog(@"size w: %f, h: %f", size.width, size.height);
    }];
}

- (void) showAbout:(id)sender
{/*
    NSLog(@"showabout1");
    //TutorialPageViewController *dvc = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialvc"];
    [self performSegueWithIdentifier:@"showTutorialSegue" sender:self];

    //[self.navigationController pushViewController:dvc animated:YES];
    NSLog(@"showabout2");
  */
}

- (void) showSettings:(id)sender
{
    //TutorialPageViewController *dvc = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialvc"];
    [self performSegueWithIdentifier:@"showSettingsTableSegue" sender:self];
    
    //[self.navigationController pushViewController:dvc animated:YES];
}

- (void) showGame:(id)sender
{
    UIButton *b = (UIButton*) sender;

    if ([b.titleLabel.text isEqualToString:@"Play"])
        self.buttonPressed = HOPLITE_CHALLENGE;
    else if ([b.titleLabel.text isEqualToString:@"Practice"])
        self.buttonPressed = HOPLITE_PRACTICE;
    else if ([b.titleLabel.text isEqualToString:@"Self Practice"])
        self.buttonPressed = SELF_PRACTICE;
    else if ([b.titleLabel.text isEqualToString:@"Multiple Choice"])
        self.buttonPressed = MULTIPLE_CHOICE;
    
    //[self performSegueWithIdentifier:@"showDetail"sender:self];
    //http://stackoverflow.com/questions/16209113/push-segue-in-xcode-with-no-animation
    //[[self navigationController] pushViewController:[self destinationViewController] animated:NO];
    
    //This way doesn't call prepareForSegue
    DetailViewController *dvc = [self.storyboard instantiateViewControllerWithIdentifier:@"dvc"];
    dvc.verbQuestionType = self.buttonPressed;
    [self.navigationController pushViewController:dvc animated:NO];
}

- (void) showResults:(id)sender
{
    //UIButton *b = (UIButton*) sender;
    
    //[self performSegueWithIdentifier:@"showResults" sender:self];
    //http://stackoverflow.com/questions/16209113/push-segue-in-xcode-with-no-animation
    //[[self navigationController] pushViewController:[self destinationViewController] animated:NO];
    
    //This way doesn't call prepareForSegue
    //DetailViewController *resultsVC = [self.storyboard instantiateViewControllerWithIdentifier:@"resultsVC"];
    //[self.navigationController pushViewController:resultsVC animated:YES];
    
    UINavigationController *nvc = (UINavigationController *)self.view.window.rootViewController;
    [[nvc.childViewControllers objectAtIndex:0] performSegueWithIdentifier:@"showResults" sender:self];
    
}

- (void) showGameResults:(id)sender
{
    //UIButton *b = (UIButton*) sender;
    
    //[self performSegueWithIdentifier:@"showResults" sender:self];
    //http://stackoverflow.com/questions/16209113/push-segue-in-xcode-with-no-animation
    //[[self navigationController] pushViewController:[self destinationViewController] animated:NO];
    
    //This way doesn't call prepareForSegue
    //DetailViewController *resultsVC = [self.storyboard instantiateViewControllerWithIdentifier:@"resultsVC"];
    //[self.navigationController pushViewController:resultsVC animated:YES];
    
    UINavigationController *nvc = (UINavigationController *)self.view.window.rootViewController;
    [[nvc.childViewControllers objectAtIndex:0] performSegueWithIdentifier:@"showGames" sender:self];
    
}

-(void)segToVerbs
{
    /*
    NSLog(@"open verbs table1");
    VerbsTableViewController *vdvc = [self.storyboard instantiateViewControllerWithIdentifier:@"vtvc"];
    [self.navigationController pushViewController:vdvc animated:NO];
    
    //UINavigationController *nvc = (UINavigationController *)self.navigationController;
    //[[nvc.childViewControllers objectAtIndex:0] performSegueWithIdentifier:@"SegueToVerbsTable"sender:self];
    NSLog(@"open verbs table2");
    */
    [self performSegueWithIdentifier:@"SegueToVerbsTable"sender:self];
}

- (void) showVerbs:(id)sender
{
    //UIButton *b = (UIButton*) sender;
    /*
    VerbDetailViewController *vtvc = [self.storyboard instantiateViewControllerWithIdentifier:@"vtvc"];
    [self.navigationController pushViewController:vtvc animated:YES];
    */
    
    [self performSegueWithIdentifier:@"SegueToVerbsTable"sender:self];
    //http://stackoverflow.com/questions/16209113/push-segue-in-xcode-with-no-animation
    //[[self navigationController] pushViewController:[self destinationViewController] animated:NO];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.navigationController setNavigationBarHidden:YES];
    //https://en.wikipedia.org/wiki/Wikipedia:Featured_picture_candidates/Gypsy_girl_mosaic_of_Zeugma
    UIImage *logoImg = [UIImage imageNamed:@"MosaicOfZeugma.jpg"];
    self.logoImgView = [[UIImageView alloc] initWithImage:logoImg];
    [self.view addSubview:self.logoImgView];
    self.logoImgView.hidden = YES;

    double sw = self.view.frame.size.width;
    double sh = self.view.frame.size.height;
    double bh = 100;
    double bw = sw / 2.3;//150;
    double v1 = sh/2;
    double v2 = sh/1.7;
    
    [self.logoImgView setFrame:CGRectMake((sw - 2547/9) /2,self.view.frame.size.height/4 -46,2547/9,1658/9)];

    
    self.popupShown = FALSE;
    self.popup = [[PopUp alloc] initWithFrame:CGRectMake (0, [UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height)];
    [self.view addSubview:self.popup];
    
    
    [self.HCButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    [self.HPButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    [self.SPButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    
    [self.MCButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    
    [self.resultsButton addTarget:self action:@selector(showResults:)
            forControlEvents:UIControlEventTouchUpInside];
    [self.gamesButton addTarget:self action:@selector(showGameResults:)
                 forControlEvents:UIControlEventTouchUpInside];
    
    /*
     [self.menuButton addTarget:self
     action:@selector(animatePopUpShow:)
     forControlEvents:UIControlEventTouchUpInside]; 
     */
    [self.menuButton addTarget:self action:@selector(showSettings:)
              forControlEvents:UIControlEventTouchUpInside];
    
    
    [self.aboutButton setFrame:CGRectMake(6, 24, 70, 36)];
    self.aboutButton.layer.borderWidth = 2.0;
    [self.aboutButton setTitle:@"About" forState:UIControlStateNormal];
    [self.aboutButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    self.aboutButton.layer.borderColor = [UIColor grayColor].CGColor;
    self.aboutButton.layer.borderWidth = 2.0f;
    self.aboutButton.layer.cornerRadius = 8;
    [self.aboutButton addTarget:self action:@selector(showAbout:)
            forControlEvents:UIControlEventTouchUpInside];
    self.aboutButton.hidden = YES;
    [self.tempFormsButton addTarget:self action:@selector(showVerbs:)
            forControlEvents:UIControlEventTouchUpInside];
    
    self.tempFormsButton.layer.borderColor = [UIColor blackColor].CGColor;
    self.tempFormsButton.layer.borderWidth = 2.0;
    
    if (0)
    {
        double corner = 15.0;
        double borderW = 2.0;
        
        //self.HCButton.titleLabel.numberOfLines = 2;
        
        self.HCButton.layer.borderWidth = borderW;
        self.HCButton.layer.cornerRadius = corner;
        self.HPButton.layer.borderWidth = borderW;
        self.HPButton.layer.cornerRadius = corner;
        self.SPButton.layer.borderWidth = borderW;
        self.SPButton.layer.cornerRadius = corner;
        self.MCButton.layer.borderWidth = borderW;
        self.MCButton.layer.cornerRadius = corner;
        
        [self.HCButton setFrame:CGRectMake(((sw/2) - bw) / 2, v1, bw, bh)];
        [self.HPButton setFrame:CGRectMake((((sw/2) - bw) / 2) + sw/2, v1, bw, bh)];
        
        [self.SPButton setFrame:CGRectMake(((sw/2) - bw) / 2, v2, bw, bh)];
        [self.MCButton setFrame:CGRectMake((((sw/2) - bw) / 2) + sw/2, v2, bw, bh)];
    }
    else if (0)
    {
        double sw = self.view.frame.size.width;
        double sh = self.view.frame.size.height;
        
        double bw = sw * 0.5;//150;
        double v1 = sh * 0.5;
        double v2 = sh * 0.75;
        double bh = sh / 4;
        
        [self.HCButton setFrame:CGRectMake(0 - 2, v1 + 2, bw + 2, bh)];
        [self.HPButton setFrame:CGRectMake(sw/2 - 2, v1 + 2, bw + 4, bh)];
        
        [self.SPButton setFrame:CGRectMake(0 - 2, v2, bw + 2, bh)];
        [self.MCButton setFrame:CGRectMake(sw/2 - 2, v2, bw + 4, bh)];
        
        /*
        [self.HCButton setBackgroundColor: UIColorFromRGB(0xCC4422)];
        [self.HPButton setBackgroundColor: UIColorFromRGB(0x22CC55)];
        [self.SPButton setBackgroundColor: UIColorFromRGB(0x4466CC)];
        [self.MCButton setBackgroundColor: UIColorFromRGB(0xFFAA00)];
         */
        self.HCButton.layer.borderWidth = 2.0;
        self.HPButton.layer.borderWidth = 2.0;
        self.SPButton.layer.borderWidth = 2.0;
        self.MCButton.layer.borderWidth = 2.0;
        
        UIColor *textColor = [UIColor blackColor];//[UIColor whiteColor];
        UIFont *textFont = [UIFont fontWithName:@"Helvetica" size:22.0];
        
        [self.HCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.SPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.MCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HCButton.titleLabel setFont:textFont];
        [self.HPButton.titleLabel setFont:textFont];
        [self.SPButton.titleLabel setFont:textFont];
        [self.MCButton.titleLabel setFont:textFont];
        
        self.HCButton.titleLabel.numberOfLines = 2;
        self.HPButton.titleLabel.numberOfLines = 2;
        self.SPButton.titleLabel.numberOfLines = 2;
        self.MCButton.titleLabel.numberOfLines = 2;
        /*
        self.HCButton.titleLabel.text = @"Hoplite Challenge";
        self.HCButton.titleLabel.backgroundColor = [UIColor yellowColor];
        self.HCButton.titleLabel.textAlignment = NSTextAlignmentCenter;
        self.HCButton.titleLabel.textColor = [UIColor redColor];
        */
    }
    else if (0)
    {
        double sw = self.view.frame.size.width;
        double sh = self.view.frame.size.height;
        
        double bw = sw * 0.5;//150;
        double v1 = sh * 0.5;
        double v2 = sh * 0.75;
        double bh = sh / 4;
        
        [self.HCButton setFrame:CGRectMake(0 - 2, v2, bw + 2, bh)];
        [self.HPButton setFrame:CGRectMake(sw/2 - 2, v2, bw + 4, bh)];
        self.SPButton.hidden = YES;
        self.MCButton.hidden = YES;
        //[self.SPButton setFrame:CGRectMake(0 - 2, v2, bw + 2, bh)];
        //[self.MCButton setFrame:CGRectMake(sw/2 - 2, v2, bw + 4, bh)];
        
        /*
         [self.HCButton setBackgroundColor: UIColorFromRGB(0xCC4422)];
         [self.HPButton setBackgroundColor: UIColorFromRGB(0x22CC55)];
         [self.SPButton setBackgroundColor: UIColorFromRGB(0x4466CC)];
         [self.MCButton setBackgroundColor: UIColorFromRGB(0xFFAA00)];
         */
        self.HCButton.layer.borderWidth = 2.0;
        self.HPButton.layer.borderWidth = 2.0;
        self.SPButton.layer.borderWidth = 2.0;
        self.MCButton.layer.borderWidth = 2.0;
        
        UIColor *textColor = [UIColor blackColor];//[UIColor whiteColor];
        UIFont *textFont = [UIFont fontWithName:@"Helvetica" size:22.0];
        
        [self.HCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.SPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.MCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HCButton.titleLabel setFont:textFont];
        [self.HPButton.titleLabel setFont:textFont];
        [self.SPButton.titleLabel setFont:textFont];
        [self.MCButton.titleLabel setFont:textFont];
        
        self.HCButton.titleLabel.numberOfLines = 2;
        self.HPButton.titleLabel.numberOfLines = 2;
        self.SPButton.titleLabel.numberOfLines = 2;
        self.MCButton.titleLabel.numberOfLines = 2;
        /*
         self.HCButton.titleLabel.text = @"Hoplite Challenge";
         self.HCButton.titleLabel.backgroundColor = [UIColor yellowColor];
         self.HCButton.titleLabel.textAlignment = NSTextAlignmentCenter;
         self.HCButton.titleLabel.textColor = [UIColor redColor];
         */
    }
    else
    {
        double sw = self.view.frame.size.width;
        double sh = self.view.frame.size.height;
        
        double bw = sw * 0.5;//150;
        double v1 = sh * 0.5;
        double v2 = sh * 0.75;
        double bh = sh / 4;
        
        [self.HCButton setFrame:CGRectMake(10, sh - 400, sw - 20, 70)];
        [self.gamesButton setFrame:CGRectMake(10, sh - 320, sw - 20, 70)];
        [self.HPButton setFrame:CGRectMake(10, sh - 240, sw - 20, 70)];
        [self.resultsButton setFrame:CGRectMake(10, sh - 160, sw - 20, 70)];
        [self.tempFormsButton setFrame:CGRectMake(10, sh - 80, sw - 20, 70)];
        
        self.SPButton.hidden = YES;
        self.MCButton.hidden = YES;
        //[self.SPButton setFrame:CGRectMake(0 - 2, v2, bw + 2, bh)];
        //[self.MCButton setFrame:CGRectMake(sw/2 - 2, v2, bw + 4, bh)];
        
        /*
         [self.HCButton setBackgroundColor: UIColorFromRGB(0xCC4422)];
         [self.HPButton setBackgroundColor: UIColorFromRGB(0x22CC55)];
         [self.SPButton setBackgroundColor: UIColorFromRGB(0x4466CC)];
         [self.MCButton setBackgroundColor: UIColorFromRGB(0xFFAA00)];
         
        self.HCButton.layer.borderWidth = 2.0;
        self.HPButton.layer.borderWidth = 2.0;
        self.SPButton.layer.borderWidth = 2.0;
        self.MCButton.layer.borderWidth = 2.0;
        */
        UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
        UIColor *greenColor = [UIColor colorWithRed:(0/255.0) green:(255/255.0) blue:(183/255.0) alpha:1.0];
        self.HCButton.backgroundColor = blueColor;//UIColorFromRGB(0x43609c);
        self.HPButton.backgroundColor = blueColor;//UIColorFromRGB(0x43609c);
        
        self.gamesButton.backgroundColor = greenColor;//UIColorFromRGB(0x43609c);
        self.resultsButton.backgroundColor = greenColor;//UIColorFromRGB(0x43609c);
        
        UIColor *textColor = [UIColor whiteColor];
        UIFont *textFont = [UIFont fontWithName:@"Helvetica" size:22.0];
        
        [self.HCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.SPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.MCButton setTitleColor:textColor forState:UIControlStateNormal];

        [self.gamesButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.resultsButton setTitleColor:textColor forState:UIControlStateNormal];
        
        [self.HCButton.titleLabel setFont:textFont];
        [self.HPButton.titleLabel setFont:textFont];
        [self.SPButton.titleLabel setFont:textFont];
        [self.MCButton.titleLabel setFont:textFont];
        
        [self.gamesButton.titleLabel setFont:textFont];
        [self.resultsButton.titleLabel setFont:textFont];
        
        self.HCButton.titleLabel.numberOfLines = 2;
        self.HPButton.titleLabel.numberOfLines = 2;
        self.SPButton.titleLabel.numberOfLines = 2;
        self.MCButton.titleLabel.numberOfLines = 2;
        /*
         self.HCButton.titleLabel.text = @"Hoplite Challenge";
         self.HCButton.titleLabel.backgroundColor = [UIColor yellowColor];
         self.HCButton.titleLabel.textAlignment = NSTextAlignmentCenter;
         self.HCButton.titleLabel.textColor = [UIColor redColor];
         */
    }
    [self.LGILabel setFrame:CGRectMake(30, 45, self.LGILabel.frame.size.width, self.LGILabel.frame.size.height)];
    self.LGILabel.hidden = YES;
    [self.HCLabel setFrame:CGRectMake(0, 120, self.view.bounds.size.width, self.HCLabel.frame.size.height)];
    self.HCLabel.textAlignment = NSTextAlignmentCenter;
    [self.EOPLabel setFrame:CGRectMake(180, (sh /2) - 34, self.EOPLabel.frame.size.width, self.EOPLabel.frame.size.height)];
    [self.EOPLabel setFont:[UIFont fontWithName:@"NewAthenaUnicode" size:26.0]];
    [self.view bringSubviewToFront:self.EOPLabel];
    self.EOPLabel.hidden = YES;
//[self.correctButton setFrame:CGRectMake(((w/2) - self.correctButton.frame.size.width) / 2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];

    
//[self.incorrectButton setFrame:CGRectMake((((w/2) - self.correctButton.frame.size.width) / 2) + w/2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
    
    [self.menuButton setFrame:CGRectMake(sw - 70 - 6, 24, 70, 36)];
    self.menuButton.layer.borderWidth = 2.0;
    [self.menuButton setTitle:@"Settings" forState:UIControlStateNormal];
    [self.menuButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    self.menuButton.layer.borderColor = [UIColor grayColor].CGColor;
    self.menuButton.layer.borderWidth = 2.0f;
    self.menuButton.layer.cornerRadius = 8;
    
    // Do any additional setup after loading the view.
}


- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([[segue identifier] isEqualToString:@"showDetail"])
    {
        DetailViewController *dvc = [segue destinationViewController];
        dvc.verbQuestionType = self.buttonPressed;
    }
    else if ([[segue identifier] isEqualToString:@"showResults"])
    {
        ResultsViewController *dvc = [segue destinationViewController];
        dvc.gameId = 1; //practice
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
}

- (void) animatePopUpShow:(id)sender
{
    if (self.popupShown)
    {
        [UIView animateWithDuration:0.3 delay:0.0 options:0
                         animations:^{
                             self.popup.frame = CGRectMake(0,[UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
                             self.navigationItem.rightBarButtonItem.title = @"Units";
                             
                         }
                         completion:nil];
        [self.view bringSubviewToFront:self.popup];
        [self.popup.HCTimeField resignFirstResponder];
        self.popupShown = FALSE;
    }
    else
    {
        [UIView animateWithDuration:0.3 delay:0.0 options:0
                         animations:^{
                             self.popup.frame = CGRectMake(0,18, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
                             self.navigationItem.rightBarButtonItem.title = @"Close";
                         }
                         completion:nil];
        [self.view bringSubviewToFront:self.popup];
        self.popupShown = TRUE;
    }
}


/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

/*
// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
    
    if ([segue.identifier isEqualToString:@"ShowResults"])
    {
        NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];
        
        NSInteger index = 0;
        // Configure the cell...
        for (int i = 0; i < indexPath.section; i++)
            index += self->verbsPerSection[i];
        
        index += indexPath.row;
        
        [[segue destinationViewController] setVerbIndex: index];
    }
}
*/

@end
