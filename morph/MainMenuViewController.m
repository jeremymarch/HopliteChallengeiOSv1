//
//  MainMenuViewController.m
//  morph
//
//  Created by Jeremy on 9/30/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import "MainMenuViewController.h"
#import "DetailViewController.h"

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
    
    [self performSegueWithIdentifier:@"showDetail"sender:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.navigationController setNavigationBarHidden:YES];
    
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
    
    double corner = 12.0;
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
    
    int w = self.view.frame.size.width;
    int h = 100;
    int p = 10;
    
    [self.HCButton setFrame:CGRectMake(p, 170, w / 2 - (p*3), h)];
    [self.HPButton setFrame:CGRectMake(w / 2 + (p*2), 170, w / 2 - (p*3), h)];
    
    [self.SPButton setFrame:CGRectMake(p, 320, w / 2 - (p*3), h)];
    [self.MCButton setFrame:CGRectMake(w / 2 + (p*2), 320, w / 2 - (p*3), h)];
    
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

- (void) animatePopUpShow:(id)sender
{
    if (self.popupShown)
    {
        [UIView animateWithDuration:0.3 delay:0.0 options:0
                         animations:^{
                             self.popup.frame = CGRectMake(0,[UIScreen mainScreen].bounds.size.height - 10, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
                             self.navigationItem.rightBarButtonItem.title = @"Units";
                             
                         }
                         completion:nil];
        [self.view bringSubviewToFront:self.popup];
        self.popupShown = FALSE;
    }
    else
    {
        [UIView animateWithDuration:0.3 delay:0.0 options:0
                         animations:^{
                             self.popup.frame = CGRectMake(0,0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
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
