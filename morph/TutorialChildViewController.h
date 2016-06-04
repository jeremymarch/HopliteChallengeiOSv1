//
//  TutorialChildViewController.h
//  Hoplite Challenge
//
//  Created by Jeremy on 5/30/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TutorialChildViewController : UIViewController
@property (assign, nonatomic) NSInteger index;
@property (strong, nonatomic) IBOutlet UILabel *screenNumber;
@property (strong, nonatomic) IBOutlet UIWebView *webView;
@end
