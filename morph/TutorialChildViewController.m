//
//  TutorialChildViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 5/30/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "TutorialChildViewController.h"

@interface TutorialChildViewController ()

@end

@implementation TutorialChildViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"page child view did load");
    
    NSString *path = [[NSBundle mainBundle] bundlePath];
    NSURL *baseURL = [NSURL fileURLWithPath:path];
    
    if ([self.title isEqualToString:@"tutorialintro"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialTitlePage" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:nil];
    }
    else if ([self.title isEqualToString:@"tutorialGamePlay"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialGamePlay" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:baseURL];
    }
    else if ([self.title isEqualToString:@"tutorialKeyboard"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialKeyboard" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:nil];
    }
    else if ([self.title isEqualToString:@"tutorialPinch"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialPinch" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:nil];
    }
    else if ([self.title isEqualToString:@"tutorialAcknowledgements"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialAcknowledgements" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:nil];
    }
    else if ([self.title isEqualToString:@"tutorialPractice"])
    {
        NSString *htmlFile = [[NSBundle mainBundle] pathForResource:@"tutorialPractice" ofType:@"html"];
        NSString* htmlString = [NSString stringWithContentsOfFile:htmlFile encoding:NSUTF8StringEncoding error:nil];
        [self.webView loadHTMLString:htmlString baseURL:nil];
    }
    
    self.webView.delegate = self;
    
    // Do any additional setup after loading the view.
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
    if (navigationType == UIWebViewNavigationTypeLinkClicked ) {
        [[UIApplication sharedApplication] openURL:[request URL]];
        return NO;
    }
    
    return YES;
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
