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

#define UIColorFromRGB(rgbValue) \
[UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 \
green:((float)((rgbValue & 0x00FF00) >>  8))/255.0 \
blue:((float)((rgbValue & 0x0000FF) >>  0))/255.0 \
alpha:1.0]

@interface MainMenuViewController ()

@end

@implementation MainMenuViewController

- (void) showGame:(id)sender
{
    UIButton *b = (UIButton*) sender;
    
    if ([b.titleLabel.text isEqualToString:@"Hoplite Challenge"])
        self.buttonPressed = HOPLITE_CHALLENGE;
    else if ([b.titleLabel.text isEqualToString:@"Hoplite Practice"])
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
    
    [self.menuButton addTarget:self
                        action:@selector(animatePopUpShow:)
              forControlEvents:UIControlEventTouchUpInside];
    
    [self.HCButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    [self.HPButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    [self.SPButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    
    [self.MCButton addTarget:self action:@selector(showGame:)
            forControlEvents:UIControlEventTouchUpInside];
    /*
    [self.MCButton addTarget:self action:@selector(showVerbs:)
            forControlEvents:UIControlEventTouchUpInside];
    */
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
    else
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
    
    [self.LGILabel setFrame:CGRectMake(30, 45, self.LGILabel.frame.size.width, self.LGILabel.frame.size.height)];
    self.LGILabel.hidden = YES;
    [self.HCLabel setFrame:CGRectMake(30, 68, self.HCLabel.frame.size.width, self.HCLabel.frame.size.height)];
    [self.EOPLabel setFrame:CGRectMake(180, (sh /2) - 34, self.EOPLabel.frame.size.width, self.EOPLabel.frame.size.height)];
    [self.EOPLabel setFont:[UIFont fontWithName:@"NewAthenaUnicode" size:26.0]];
    [self.view bringSubviewToFront:self.EOPLabel];
    self.EOPLabel.hidden = YES;
//[self.correctButton setFrame:CGRectMake(((w/2) - self.correctButton.frame.size.width) / 2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];

    
//[self.incorrectButton setFrame:CGRectMake((((w/2) - self.correctButton.frame.size.width) / 2) + w/2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
    
    [self.menuButton setFrame:CGRectMake(sw - 84, 26, self.menuButton.frame.size.width + 20, self.menuButton.frame.size.height + 4)];
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

@end
