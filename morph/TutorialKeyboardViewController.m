//
//  TutorialKeyboardViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 5/31/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//
#import "AppDelegate.h"
#import "TutorialKeyboardViewController.h"

@interface TutorialKeyboardViewController ()

@end

@implementation TutorialKeyboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    
}

-(void)viewWillAppear:(BOOL)animated{

    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    [(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard].delegate = self;
    
    //http://stackoverflow.com/questions/16868117/uitextview-that-expands-to-text-using-auto-layout
    [self.textfield setInputView: [(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard]];
    
    self.textfield.font = [UIFont fontWithName:@"NewAthenaUnicode" size:36.0];
    self.textfield.frame = CGRectMake(10, self.textfield.frame.origin.y, screenSize.width - 20, 54.0);
    [self.textfield setBorderStyle:UITextBorderStyleNone];
    [self.textfield becomeFirstResponder];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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
