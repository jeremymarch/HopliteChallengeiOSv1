//
//  TutorialPlainViewController.h
//  Hoplite Challenge
//
//  Created by Jeremy on 8/26/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PageTextView.h"

@interface TutorialPlainViewController : UIViewController <UITextViewDelegate>
@property (weak, nonatomic) IBOutlet PageTextView *textView;
@property (assign, nonatomic) NSInteger index;
@end
