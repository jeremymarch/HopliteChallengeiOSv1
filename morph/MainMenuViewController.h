//
//  MainMenuViewController.h
//  morph
//
//  Created by Jeremy on 9/30/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PopUp.h"

@interface MainMenuViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIButton *HCButton;
@property (weak, nonatomic) IBOutlet UIButton *HPButton;
@property (weak, nonatomic) IBOutlet UIButton *SPButton;
@property (weak, nonatomic) IBOutlet UIButton *MCButton;
@property (weak, nonatomic) IBOutlet UIButton *menuButton;
@property NSInteger buttonPressed;
@property BOOL popupShown;
@property (nonatomic, retain) IBOutlet PopUp *popup;
@end