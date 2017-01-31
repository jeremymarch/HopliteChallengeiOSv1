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
#import "HCColors.h"
#import "TutorialChildViewController.h"


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
        
        //[self.HCLabel setFrame:CGRectMake(0, 120, self.view.bounds.size.width, self.HCLabel.frame.size.height)];
        [self.HCLabel setFrame:CGRectMake(0, (sh/9.5)*2, self.view.bounds.size.width, self.HCLabel.frame.size.height)];
        [self.menuButton setFrame:CGRectMake(sw - 70 - 6, 24, 70, 36)];

        
    } completion:^(id<UIViewControllerTransitionCoordinatorContext> context) {

        // Code here will execute after the rotation has finished.
        // Equivalent to placing it in the deprecated method -[didRotateFromInterfaceOrientation:]
        //NSLog(@"size w: %f, h: %f", size.width, size.height);
    }];
}

- (void) showAbout:(id)sender
{
    //TutorialPageViewController *dvc = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialvc"];
    [self performSegueWithIdentifier:@"showTutorialSegue" sender:self];

    //[self.navigationController pushViewController:dvc animated:YES];
}

- (void) showSettings:(id)sender
{
    //TutorialPageViewController *dvc = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialvc"];
    [self performSegueWithIdentifier:@"showSettingsTableSegue" sender:self];
    
    //[self.navigationController pushViewController:dvc animated:YES];
}

- (void) showGame:(id)sender
{
    /*
    NSString *string = @"Hello, World!";
    AVSpeechUtterance *utterance = [[AVSpeechUtterance alloc] initWithString:string];
    utterance.voice = [AVSpeechSynthesisVoice voiceWithLanguage:@"en-US"];
    
    AVSpeechSynthesizer *speechSynthesizer = [[AVSpeechSynthesizer alloc] init];
    [speechSynthesizer speakUtterance:utterance];
    */
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
    
    //to preload About webview for faster load
    //http://stackoverflow.com/questions/21563801/preload-uiwebview-on-a-not-yet-displayed-uiviewcontroller
    TutorialChildViewController *initialViewController = [self.storyboard instantiateViewControllerWithIdentifier: @"tutorialIntro" ];
    [initialViewController.view layoutSubviews];

    double sw = self.view.frame.size.width;
    double sh = self.view.frame.size.height;
    double spacer = sh / 70;
    double buttonHeight = (sh / 9) - spacer;
    
    UIColor *textColor = [UIColor whiteColor];
    UIFont *textFont = [UIFont fontWithName:@"Helvetica" size:22.0];
    
    [self.navigationController setNavigationBarHidden:YES];
    
    //https://en.wikipedia.org/wiki/Wikipedia:Featured_picture_candidates/Gypsy_girl_mosaic_of_Zeugma
    //UIImage *logoImg = [UIImage imageNamed:@"MosaicOfZeugma.jpg"];
    //self.logoImgView = [[UIImageView alloc] initWithImage:logoImg];
    //[self.view addSubview:self.logoImgView];
    //[self.logoImgView setFrame:CGRectMake((sw - 2547/9) /2,self.view.frame.size.height/4 -46,2547/9,1658/9)];
    self.logoImgView.hidden = YES;
    
    
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
    
    [self.menuButton addTarget:self action:@selector(showSettings:)
              forControlEvents:UIControlEventTouchUpInside];
    
    [self.menuButton setFrame:CGRectMake(sw - 70 - spacer, 24, 70, 36)];
    self.menuButton.layer.borderWidth = 2.0;
    [self.menuButton setTitle:@"Settings" forState:UIControlStateNormal];
    [self.menuButton setTitleColor:[UIColor HCBlue] forState:UIControlStateNormal];
    self.menuButton.layer.borderColor = [UIColor HCBlue].CGColor;
    self.menuButton.layer.borderWidth = 2.0f;
    self.menuButton.layer.cornerRadius = 8;
    
    [self.aboutButton setFrame:CGRectMake(spacer, 24, 70, 36)];
    self.aboutButton.layer.borderWidth = 2.0;
    [self.aboutButton setTitle:@"About" forState:UIControlStateNormal];
    [self.aboutButton setTitleColor:[UIColor HCBlue] forState:UIControlStateNormal];
    self.aboutButton.layer.borderColor = [UIColor HCBlue].CGColor;
    self.aboutButton.layer.borderWidth = 2.0f;
    self.aboutButton.layer.cornerRadius = 8;
    [self.aboutButton addTarget:self action:@selector(showAbout:)
            forControlEvents:UIControlEventTouchUpInside];
    self.aboutButton.hidden = NO;
    
    
    [self.tempFormsButton addTarget:self action:@selector(showVerbs:)
            forControlEvents:UIControlEventTouchUpInside];
    
    //self.tempFormsButton.layer.borderColor = greenColor.CGColor;
    //self.tempFormsButton.layer.borderWidth = 2.0;
    
    [self.HCLabel setFrame:CGRectMake(0, (sh/10)*2, self.view.bounds.size.width, self.HCLabel.frame.size.height)];
    self.HCLabel.textAlignment = NSTextAlignmentCenter;
    
    UIFont *logoFont;
    if (sh > 569)
    {
        logoFont= [UIFont fontWithName:@"Helvetica" size:36.0]; //6S
    }
    else if (sh > 480)
    {
        logoFont= [UIFont fontWithName:@"Helvetica" size:32.0]; //5S
    }
    else
    {
        logoFont= [UIFont fontWithName:@"Helvetica" size:30.0]; //4S
    }
    [self.HCLabel setFont:logoFont];
        
        [self.HCButton setFrame:CGRectMake(spacer, sh - ((buttonHeight + spacer) * 5), sw - (2 * spacer), buttonHeight)];
        [self.gamesButton setFrame:CGRectMake(spacer, sh - ((buttonHeight + spacer) * 4), sw - (2 * spacer), buttonHeight)];
        [self.HPButton setFrame:CGRectMake(spacer, sh - ((buttonHeight + spacer) * 3), sw - (2 * spacer), buttonHeight)];
        [self.resultsButton setFrame:CGRectMake(spacer, sh - ((buttonHeight + spacer) * 2), sw - (2 * spacer), buttonHeight)];
        [self.tempFormsButton setFrame:CGRectMake(spacer, sh - ((buttonHeight + spacer) * 1), sw - (2 * spacer), buttonHeight)];
        
        self.SPButton.hidden = YES;
        self.MCButton.hidden = YES;
        
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
    
        self.HCButton.backgroundColor = [UIColor HCBlue];//UIColorFromRGB(0x43609c);
        self.HPButton.backgroundColor = [UIColor HCBlue];//UIColorFromRGB(0x43609c);
        
        self.gamesButton.backgroundColor = [UIColor HCDarkBlue];//UIColorFromRGB(0x43609c);
        self.resultsButton.backgroundColor = [UIColor HCDarkBlue];//UIColorFromRGB(0x43609c);
    
    
    self.tempFormsButton.backgroundColor = [UIColor HCLightBlue];
    
        [self.HCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.HPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.SPButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.MCButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.tempFormsButton setTitleColor:[UIColor HCDarkBlue] forState:UIControlStateNormal];
    
        [self.gamesButton setTitleColor:textColor forState:UIControlStateNormal];
        [self.resultsButton setTitleColor:textColor forState:UIControlStateNormal];
        
        [self.HCButton.titleLabel setFont:textFont];
        [self.HPButton.titleLabel setFont:textFont];
        [self.SPButton.titleLabel setFont:textFont];
        [self.MCButton.titleLabel setFont:textFont];
        [self.tempFormsButton.titleLabel setFont:textFont];
        
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
    
    //[self.LGILabel setFrame:CGRectMake(30, 45, self.LGILabel.frame.size.width, self.LGILabel.frame.size.height)];
    self.LGILabel.hidden = YES;
    
    
    
    
    //[self.EOPLabel setFrame:CGRectMake(180, (sh /2) - 34, self.EOPLabel.frame.size.width, self.EOPLabel.frame.size.height)];
    //[self.EOPLabel setFont:[UIFont fontWithName:@"NewAthenaUnicode" size:26.0]];
    //[self.view bringSubviewToFront:self.EOPLabel];
    self.EOPLabel.hidden = YES;
    
//[self.correctButton setFrame:CGRectMake(((w/2) - self.correctButton.frame.size.width) / 2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
    
//[self.incorrectButton setFrame:CGRectMake((((w/2) - self.correctButton.frame.size.width) / 2) + w/2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
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
