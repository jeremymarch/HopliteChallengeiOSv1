//
//  TutorialViewController.h
//  Hoplite Challenge
//
//  Created by Jeremy on 5/30/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TutorialPageViewController : UIPageViewController  <UIPageViewControllerDataSource>
@property (assign, nonatomic) NSInteger numPages;
@property (assign, nonatomic) NSInteger index;
@property (strong, nonatomic) NSArray *pageNames;
@property (strong, nonatomic) UIButton *closeButton;
@end
