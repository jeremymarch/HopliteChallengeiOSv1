//
//  TutorialPageViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 5/30/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "TutorialPageViewController.h"
#import "TutorialChildViewController.h"

@interface TutorialPageViewController ()

@end

@implementation TutorialPageViewController

- (void) viewWillLayoutSubviews
{
    [super viewWillLayoutSubviews];
    //[self.navigationController setNavigationBarHidden:NO];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    NSLog(@"page view did load");
    self.dataSource = self;
    //self.delegate = self;
    self.index = 0;
    self.pageNames = [NSArray arrayWithObjects:@"tutorialIntro", @"tutorialAcknowledgements", @"tutorialGamePlay", @"tutorialKeyboard", @"tutorialPinch", /*@"tutorialchild", @"tutorialchild2",*/ nil];
    self.numPages = [self.pageNames count];
    
    TutorialChildViewController *initialViewController = [self viewControllerAtIndex:self.index];
    
    NSArray *viewControllers = [NSArray arrayWithObject:initialViewController];
    
    
    [self setViewControllers:viewControllers direction:UIPageViewControllerNavigationDirectionForward animated:NO completion:nil];
    
    [self didMoveToParentViewController:self];
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [button addTarget:self action:@selector(closeTutorial:) forControlEvents:UIControlEventTouchUpInside];
    [button setTitle:@"Close" forState:UIControlStateNormal];
    button.frame = CGRectMake(self.view.frame.size.width - 70 - 6, 24, 70, 36);
    button.layer.borderWidth = 2.0;
    [button setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    button.layer.borderColor = [UIColor grayColor].CGColor;
    button.layer.borderWidth = 2.0f;
    button.layer.cornerRadius = 8;
    [self.view addSubview: button];
}

- (void) closeTutorial:(id)sender
{
    //[self.navigationController popViewControllerAnimated:NO];
    //[self performSegueWithIdentifier:@"showTutorialSegue" sender:self];
    [self dismissViewControllerAnimated:YES completion:nil];
}
     
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (TutorialChildViewController *)viewControllerAtIndex:(NSUInteger)index {
    
    //TutorialChildViewController *childViewController = [[TutorialChildViewController alloc] //initWithNibName:@"TutorialChildViewController" bundle:nil];
    //childViewController.index = index;
    
    TutorialChildViewController *page;
    /*
    if (index == 0)
    {
        page = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialchild"];
        page.index =  index;
    }
    else if (index == 1)
    {
        page = [self.storyboard instantiateViewControllerWithIdentifier:@"tutorialchild2"];
        page.index =  index;
    }
    else
    {
        page =  nil;
        page.index = 0;
    }
    */
    NSLog([self.pageNames objectAtIndex:index]);
    
    page = [self.storyboard instantiateViewControllerWithIdentifier: [self.pageNames objectAtIndex:index] ];
    page.index =  index;
    
    self.index = index;
    //[self presentViewController:myController animated:YES completion:nil];
    return page;
}

- (UIViewController *)pageViewController:(UIPageViewController *)pageViewController viewControllerBeforeViewController:(UIViewController *)viewController {
    
    NSUInteger index = [(TutorialChildViewController *)viewController index];
    if (index <= 0) {
        return nil;
    }
    return [self viewControllerAtIndex:index - 1];
}

- (UIViewController *)pageViewController:(UIPageViewController *)pageViewController viewControllerAfterViewController:(UIViewController *)viewController {
    
    NSUInteger index = [(TutorialChildViewController *)viewController index];
    if (index >= self.numPages - 1) {
        return nil;
    }
    return [self viewControllerAtIndex:index + 1];
}

- (NSInteger)presentationCountForPageViewController:(UIPageViewController *)pageViewController {
    // The number of items reflected in the page indicator.
    return self.numPages;
}

- (NSInteger)presentationIndexForPageViewController:(UIPageViewController *)pageViewController {
    // The selected item reflected in the page indicator.
    return 0;
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
