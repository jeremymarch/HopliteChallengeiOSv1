//
//  TutorialPlainViewController.m
//  Hoplite Challenge
//
//  Created by Jeremy on 8/26/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#import "TutorialPlainViewController.h"

@interface TutorialPlainViewController ()

@end

@implementation TutorialPlainViewController

- (void)viewDidLoad {
    [super viewDidLoad];
   
    self.textView.font = [UIFont systemFontOfSize:15];
    self.textView.autocorrectionType = UITextAutocorrectionTypeNo;
    self.textView.clipsToBounds = YES;
    self.textView.scrollEnabled=YES;
    //self.textView.editable=YES;
    self.textView.contentSize=self.textView.frame.size;
    self.textView.returnKeyType = UIReturnKeyDone;
    self.textView.contentInset    = UIEdgeInsetsMake(-8,0,-8,0);
    self.textView.autoresizingMask = UIViewAutoresizingFlexibleHeight;
    self.textView.layer.cornerRadius = 5;
    self.textView.clipsToBounds = YES;
    
    
    NSString *str = @"\n\n\n\nHoplite Challenge\n\n\nThe Hoplite Challenge is a verb conjugation game for Ancient Greek.  It is inspired by the Hoplite Challenge Cup, a contest held each summer between the students and faculty of the Latin/Greek Institute's Basic Program in Greek. After only six weeks in the course, students are prepared to challenge the faculty to see who can most accurately produce Ancient Greek verb forms under tight time constraints.  The spirit and intensity of the Hoplite Challenge Cup could never be adequately recreated in app form, but it is hoped that you will find the app both fun and challenging.\n\nThe app follows the units of the course's textbook, Hansen & Quinn's Greek: An Intensive Course.  From the 114 verbs introduced in Hansen & Quinn there are over 15,800 possible verb forms, not including additional forms given in the appendix.  The app will present users with a verb form and prompt them to change it to another form; users have 30 seconds to produce this new form.  It is expected that users will already have a good command of Ancient Greek verbs in advance; the app is aimed at perfecting and reinforcing this knowledge.\n\n";
 
    NSMutableAttributedString *str2 = [[NSMutableAttributedString alloc] initWithString:str];
    
    [str2 addAttribute:NSFontAttributeName
                 value:[UIFont fontWithName:@"HelveticaNeue" size:18]
                 range:NSMakeRange(0, [str length])];
    
    NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc] init];
    [paragraphStyle setAlignment:NSTextAlignmentLeft];
    [paragraphStyle setHeadIndent:10.0];
    [paragraphStyle setFirstLineHeadIndent:10.0];
    [paragraphStyle setTailIndent:-10.0];
    [str2 addAttribute: NSParagraphStyleAttributeName value:paragraphStyle range: NSMakeRange(0, [str length])];
    

    NSRange range1 = [str rangeOfString:@"Hoplite Challenge" options:NSCaseInsensitiveSearch];
    if (range1.location != NSNotFound)
    {
        NSMutableParagraphStyle *paragraphStyle = [[NSMutableParagraphStyle alloc] init];
        [paragraphStyle setAlignment:NSTextAlignmentCenter];
        
        [str2 addAttribute: NSParagraphStyleAttributeName value:paragraphStyle range: range1];
        
        [str2 addAttribute:NSFontAttributeName
                     value:[UIFont fontWithName:@"HelveticaNeue" size:28]
                     range:range1];
        
    }
    
    NSRange range2 = [str rangeOfString:@"Latin/Greek Institute" options:NSCaseInsensitiveSearch];
    if (range2.location != NSNotFound)
    {
        [str2 addAttribute: NSLinkAttributeName value: @"http://www.gc.cuny.edu/Page-Elements/Academics-Research-Centers-Initiatives/Centers-and-Institutes/Latin-Greek-Institute" range: range2];
    }
    
    NSRange range3 = [str rangeOfString:@"Greek: An Intensive Course" options:NSCaseInsensitiveSearch];
    if (range3.location != NSNotFound)
    {
        [str2 addAttribute:NSFontAttributeName
                     value:[UIFont fontWithName:@"HelveticaNeue-Italic" size:18]
                     range:range3];
    }
    
    self.textView.attributedText = str2;
    self.textView.dataDetectorTypes = UIDataDetectorTypeLink;
    self.textView.delegate = self;
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)textView:(UITextView *)textView shouldInteractWithURL:(NSURL *)URL inRange:(NSRange)characterRange
{
    
    return YES;
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
