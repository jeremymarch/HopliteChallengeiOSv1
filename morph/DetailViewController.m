//
//  DetailViewController.m
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import "AppDelegate.h"
#import "DetailViewController.h"
#import "libmorph.h"
#import "GreekForms.h"
#import <AudioToolbox/AudioToolbox.h>
#import "HCColors.h"

//http://stackoverflow.com/questions/1560081/how-can-i-create-a-uicolor-from-a-hex-string
#define UIColorFromRGB(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]

SystemSoundID DingSound;
SystemSoundID BuzzSound;

@interface DetailViewController ()
@property (strong, nonatomic) UIPopoverController *masterPopoverController;

- (void)configureView;
@end

@implementation DetailViewController
@synthesize vocabBack, vocabFront;
#pragma mark - Managing the detail item

//for expanding uitextview
//http://stackoverflow.com/questions/50467/how-do-i-size-a-uitextview-to-its-content?rq=1
/*
 - (void)textViewDidChange:(UITextView *)textView
 {
 CGFloat fixedWidth = textView.frame.size.width;
 CGSize newSize = [textView sizeThatFits:CGSizeMake(fixedWidth, MAXFLOAT)];
 CGRect newFrame = textView.frame;
 newFrame.size = CGSizeMake(fmaxf(newSize.width, fixedWidth), newSize.height);
 textView.frame = newFrame;
 }
 */

-(void)setLevelArray: (NSMutableArray*)array
{
    NSLog(@"set level array");
    self.buttonStates = array;
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //if (self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE)
      //  return;
    
    if (self.front) //then show back
    {
        if (self.cardType == CARD_PRINCIPAL_PARTS) //Principal Parts
        {
            self.stemLabel.hidden = true;
            self.backLabel.hidden = false;
            self.singLabel.hidden = true;
            self.pluralLabel.hidden = true;
        }
        else if (self.cardType == CARD_ENDINGS) //Endings
        {
            self.stemLabel.hidden = true;
            self.backLabel.hidden = true;
            self.singLabel.hidden = false;
            self.pluralLabel.hidden = false;
            
        }
        else
        {
            return;
        }
        /*
        else if (self.cardType == CARD_ACCENTS) //Accents
        {
            
        }
        else if (self.cardType == CARD_VERBS) //Verbs
        {
            //self.backLabel.hidden = false;
            if (self.verbQuestionType != MULTIPLE_CHOICE)
            {
                [self stopTimer];
                
                if (self.animate)
                {
                    //NSLog(@"type: %@", self.changedForm.text);
                    self.changedForm.textAlignment = NSTextAlignmentLeft;
                    [self typeLabel:self.changedForm withString:self.changedForm.text withInterval:self.typeInterval  completion:nil];
                }

                self.changedForm.hidden = NO;

            }
            if (self.verbQuestionType == SELF_PRACTICE)
            {
                self.correctButton.hidden = NO;
                self.incorrectButton.hidden = NO;
            }
            if (self.verbQuestionType == MULTIPLE_CHOICE)
            {
                return;
            }
        }
        else if (self.cardType == CARD_VOCAB) //Vocab Training
        {
            [self turnDown];
        }
        */
        self.front = false;
        
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = NO;
    }
    else if (self.cardType == CARD_PRINCIPAL_PARTS || self.cardType == CARD_ENDINGS)//load new card
    {
        if (self.cardType == CARD_VERBS && self.verbQuestionType == SELF_PRACTICE)
        {
            return;
        }
        self.cardType = CARD_VERBS;
        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = NO;
    }
}

-(void) loadNext
{
    NSLog(@"loadnext");
    [[(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard] resetKeyboard];
    self.timeLabel.textColor = [UIColor blackColor];
    
    if (self.cardType == CARD_ENDINGS)
        [self loadEnding];
    //else if (self.cardType == CARD_PRINCIPAL_PARTS)
    //    [self loadPrincipalPart];
    else if (self.cardType == CARD_VERBS)
        ;//[self loadMorphTraining];
    
    [self stopTimer];

    self.MFLabel.hidden = YES;
    
    
    self.front = true;
}

-(NSArray*) changeForm:(NSArray*)form
{
    int randomNumber1 = arc4random() % 5;
    int randomNumber2 = randomNumber1;
    while (randomNumber2 == randomNumber1)
        randomNumber2 = arc4random() % 5;
    
    return form;
}

//works even if only one.
-(NSString*)selectRandomFromCSV:(NSString *)str
{
    NSArray *myArr = [str componentsSeparatedByString:@", "];
    NSUInteger randomIndex = arc4random() % [myArr count];
    
    return [myArr objectAtIndex:randomIndex];
}

-(void)centerLabel:(UILabel*)l withString:(NSString*)string setHeight:(Boolean)setHeight
{
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    string = [string stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
    
    CGSize size = [string sizeWithAttributes:@{NSFontAttributeName: l.font}];
    
    // Values are fractional -- you should take the ceilf to get equivalent values
    if (size.width > screenSize.width)
    {
        size.width = screenSize.width;
        //size.height = size.height * 3;
    }
    
    CGSize adjustedSize = CGSizeMake(ceilf(size.width), ceilf(size.height));
    double newHeight;
    if (setHeight)
        newHeight = adjustedSize.height + 14;
    else
        newHeight = l.frame.size.height;
    
    //NSLog(@"height %@ %f", string, newHeight);
    
    [l setFrame: CGRectMake((screenSize.width - adjustedSize.width) / 2, l.frame.origin.y, adjustedSize.width, newHeight)];
}

-(void)centerLabel:(UILabel*)l withAttributedString:(NSAttributedString*)string withSize:(CGSize)size
{
    //string = [string stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
    NSAttributedString *temp = l.attributedText;
    l.hidden = YES;
    l.attributedText = string;
    CGSize lsize = [l sizeThatFits:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX)];
 
    
    l.attributedText = temp;
    l.hidden = NO;
    // Values are fractional -- you should take the ceilf to get equivalent values
    if (lsize.width > size.width)
    {
        lsize.width = size.width;
        //size.height = size.height * 3;
    }
    
    CGSize adjustedSize = CGSizeMake(ceilf(lsize.width), ceilf(lsize.height));
    
    [l setFrame: CGRectMake((size.width - adjustedSize.width) / 2, l.frame.origin.y, adjustedSize.width, l.frame.size.height)];
}

//http://stackoverflow.com/questions/11686642/letter-by-letter-animation-for-uilabel

//for time envelopes see:
//http://stackoverflow.com/questions/5161465/how-to-create-custom-easing-function-with-core-animation
//http://stackoverflow.com/questions/14961581/cadisplaylink-running-lower-frame-rate-on-ios5-1
//http://stackoverflow.com/questions/29938707/animate-a-uiview-using-cadisplaylink-combined-with-camediatimingfunction-to
//http://netcetera.org/camtf-playground.html

-(void)typeLabel:(UILabel*)l withString:(NSString*)string withInterval:(double)interval setHeight:(Boolean)setHeight completion:(void (^)(void))done
{
    if ([string length] < 1)
    {
        l.text = @"";
        return;
    }

    [self centerLabel:l withString:string setHeight:setHeight];
    
    NSInteger i = 0;
    NSInteger j = 0;
    for (i = 0, j = 0; i < [string length]; i++)
    {
        //don't wait for space characters
        while ([string characterAtIndex:i] == ' ')
        {
            i++;
        }
        
        if (i >= [string length])
            break;
        
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        j++;
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
                       {
                           [l setText:[string substringToIndex:i+1]];
                       });
    }
    if (done)
    {
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), done);
    }
}

-(void)hideTypeLabel:(UILabel*)l withInterval:(double)interval completion:(void (^)(void))done
{
    NSString *string = l.text;
    
    //if ([string length] < 1)
    //    return;

    NSInteger i = 0;
    NSInteger j = 0;
    for (i = [string length] - 1, j = 0; i >= 0; i--)
    {
        //don't wait for space characters
        while ([string characterAtIndex:i] == ' ')
        {
            --i;
        }
        
        if (i < 0)
            break;
        
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        ++j;
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
                       {
                           [l setText:[string substringToIndex:i]];
                       });
    }
    if (done)
    {
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), done);
    }
}

-(void)hideTypeAttLabel:(UILabel*)l withInterval:(double)interval completion:(void (^)(void))done
{
    NSAttributedString *string = l.attributedText;
    
    if ([string length] < 1)
        return;
    
    NSInteger i = 0;
    NSInteger j = 0;
    for (i = [string length] - 1, j = 0; i >= 0; i--)
    {
        //don't wait for space characters
        NSAttributedString *space = [[NSAttributedString alloc] initWithString:@" "];
        while ([[string attributedSubstringFromRange:NSMakeRange(i,1)] isEqualToAttributedString: space])
        {
            --i;
        }
        
        if (i < 0)
            break;
        
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        ++j;
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
                       {
                           //[l setText:[string substringToIndex:i]];
                           l.attributedText = [string attributedSubstringFromRange:NSMakeRange(0,i)];
                       });
    }
    if (done)
    {
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), done);
    }
}

-(void)hideTypeTextField:(UITextField*)l withInterval:(double)interval completion:(void (^)(void))done
{
    NSString *string = l.text;
    
    NSInteger i = 0;
    NSInteger j = 0;
    for (i = [string length] - 1, j = 0; i >= 0; i--)
    {
        //don't wait for space characters
        while ([string characterAtIndex:i] == ' ')
        {
            --i;
        }
        
        if (i < 0)
            break;
        
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        ++j;
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
                       {
                           [l setText:[string substringToIndex:i]];
                       });
    }
    if (done)
    {
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), done);
    }
}

-(void)typeAttLabel:(UILabel*)l withString:(NSAttributedString*)string withInterval:(double)interval
{
    if ([string length] < 1)
    {
        //l.attributedText = string;
        return;
    }
    [self centerLabel:l withAttributedString:string withSize:self.view.frame.size];
    
    for (int i = 0, j = 0; i < [string length]; i++)
    {
        //don't wait for space characters
        NSAttributedString *space = [[NSAttributedString alloc] initWithString:@" "];
        while ([[string attributedSubstringFromRange:NSMakeRange(i,1)] isEqualToAttributedString: space])
        {
            i++;
        }
     
        if (i >= [string length])
            break;
        
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * j * NSEC_PER_SEC));
        j++;
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
                       {
                           l.attributedText = [string attributedSubstringFromRange:NSMakeRange(0,i+1)];
                       });
    }
}

void printUCS22(UCS2 *u, int len)
{
    NSLog(@"UCS2 Length: %d", len);
    for(int i = 0; i < len; i++)
        NSLog(@"UCS2 %d: %0x", i, u[i]);
}

-(void) backToMenu
{
    if (self.verbQuestionType == HOPLITE_CHALLENGE && self.lives > 0)
    {
        /*
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Alert"
                                                        message:@"Are you sure you want to quit this game?"
                                                       delegate:self
                                              cancelButtonTitle:@"No"
                                              otherButtonTitles:@"Yes", nil];
        [alert show];
        */
        //http://stackoverflow.com/questions/30498972/keyboard-will-appeared-automatically-in-ios-8-3-while-displaying-alertview-or-al
        Boolean isF = [self.textfield isFirstResponder];
        [self.textfield resignFirstResponder];
        
        UIAlertController * alertController = [UIAlertController alertControllerWithTitle:@"Alert"
                                                                                  message:@"Are you sure you want to quit this game?"
                                                                           preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction * noAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                                style:UIAlertActionStyleDefault
                                                              handler:^(UIAlertAction *action) {if (isF)
                                    {
                                        [self.textfield becomeFirstResponder];
                                    }}];
        
        UIAlertAction * yesAction = [UIAlertAction actionWithTitle:@"Yes"
                                                                style:UIAlertActionStyleDefault
                                                              handler:^(UIAlertAction *action) {
                                                                  [self preCheckVerbSubmit];
                                                                  [self.navigationController popViewControllerAnimated:NO];
                                                              }];
        
        [alertController addAction:yesAction];
        [alertController addAction:noAction];
        
        [self presentViewController:alertController animated:YES completion:nil];
    }
    else
    {
        [self preCheckVerbSubmit];
        [self.navigationController popViewControllerAnimated:NO];
    }
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch(buttonIndex) {
        case 0: //"No" pressed
            //do something?
            break;
        case 1: //"Yes" pressed
            //here you pop the viewController
            [self preCheckVerbSubmit];
            [self.textfield resignFirstResponder];
            [self.navigationController popViewControllerAnimated:NO];
            break;
    }
}

-(void)multipleFormsPressed
{
    self.MFLabel.hidden = NO;
    
    //if there are not multiple forms for the verb, mark it incorrect
    if ([self.changedForm.text rangeOfString:@","].location == NSNotFound)
    {
        self.mfPressed = YES;
        //this will mark form incorrect
        [self preCheckVerbSubmit];
        
        return;
    }
    
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        if (!self.mfPressed)
        {
            double halfTime = self.HCTime / 2;
            self.startTime += halfTime;
        }
    }
    else if (self.verbQuestionType == HOPLITE_PRACTICE)
    {
        if (!self.mfPressed)
        {
            CGFloat halfTime = self.HCTime / 2;
            self.startTime += halfTime;
        }
    }
    self.mfPressed = YES;
}

/* 
 called when time runs out
 */
-(void)preCheckVerbTimeout
{
    self.timeLabel.text = @"0.00 sec";
    self.timeLabel.textColor = [UIColor redColor];
    self.timeLabel.hidden = NO;
    [self stopTimer];
    [self checkVerb];
}

/* 
 called when Enter or continue is pressed
 */
-(void)preCheckVerbSubmit
{
    if (self.cardType == CARD_PRINCIPAL_PARTS)
    {
        if (self.front)
        {
            self.stemLabel.hidden = YES;
            self.backLabel.hidden = NO;
            self.front = NO;
        }
        else
        {
            self.cardType = CARD_VERBS;
            [self loadNext];
        }
        return;
    }
    else if (self.cardType == CARD_ENDINGS)
    {
        if (self.front)
        {
            self.stemLabel.hidden = YES;
            self.backLabel.hidden = NO;
            self.front = NO;
        }
        else
        {
            self.cardType = CARD_VERBS;
            [self loadNext];
        }
        return;
    }
    
    
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        CFTimeInterval elapsedTime = self.HCTime - (CACurrentMediaTime() - self.startTime);
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        //self.timeLabel.text = @"0.00 sec";
        self.timeLabel.hidden = NO;
        [self stopTimer];
    }
    else
    {
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = NO;
        [self stopTimer];
    }
    [self checkVerb];
}

-(void)checkVerb
{
    BOOL isCorrect = NO;
    BOOL debug = NO;
    int score = -1;
    
    if (self.front)
    {
        UCS2 givenForm[512];
        UCS2 expectedForm[512];
        int givenLen = 0;
        int expectedLen = 0;

        utf8_to_ucs2_string([self.textfield.text UTF8String], givenForm, &givenLen);
        utf8_to_ucs2_string([self.changedForm.text UTF8String], expectedForm, &expectedLen);
        
        NSString *eTime = [NSString stringWithFormat:@"%.02f", self.elapsedTimeForDB];
        
        if (compareFormsCheckMFRecordResult(expectedForm, expectedLen, givenForm, givenLen, self.mfPressed, [eTime UTF8String], &score))
        {
            NSLog(@"correct");
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            CGFloat gvX = (self.view.frame.size.width + size.width) / 2 + 15;
            //don't let it go off the screen
            if (gvX > self.view.frame.size.width - 26)
                gvX = self.view.frame.size.width - 26;
            [self.greenCheckView setFrame:CGRectMake(gvX, self.greenCheckView.frame.origin.y, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
            
            //[self.view bringSubviewToFront:self.greenCheckView];
            self.greenCheckView.layer.zPosition = 100;
            
            self.greenCheckView.hidden = NO;
            
            if (!self.soundDisabled)
            {
                AudioServicesPlaySystemSound(DingSound);
            }
            isCorrect = YES;
        }
        else
        {
            NSLog(@"not correct");
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            int offset;
            if (size.width == 0)
                offset = -8;
            else
                offset = 15;
            
            CGFloat rvX = (self.view.frame.size.width + size.width) / 2 + offset;
            //don't let it go off the screen
            if (rvX > self.view.frame.size.width - 26)
                rvX = self.view.frame.size.width - 26;
            [self.redXView setFrame:CGRectMake(rvX, self.redXView.frame.origin.y, self.redXView.frame.size.width,self.redXView.frame.size.height)];
            
            //[self.view bringSubviewToFront:self.redXView];
            //[self.view sendSubviewToBack:self.textfield];
            //[self.redXView setFrame:CGRectMake(20, 250, self.redXView.frame.size.width,self.redXView.frame.size.height)];
            //[self.textfield addSubview:self.redXView];
            self.redXView.layer.zPosition = 100;

            self.redXView.hidden = NO;
            
            
            if (!self.soundDisabled)
            {
                AudioServicesPlaySystemSound(BuzzSound);
            }
            self.textfield.textColor = [UIColor grayColor];
            isCorrect = NO;
            if (self.verbQuestionType == HOPLITE_CHALLENGE)
            {
                if (self.lives == 3)
                {
                    self.lives = 2;
                    self.life3.hidden = YES;
                }
                else if (self.lives == 2)
                {
                    self.lives = 1;
                    self.life2.hidden = YES;
                }
                else if (self.lives == 1)
                {
                    self.lives = 0;
                    self.life1.hidden = YES;
                    self.gameOverLabel.hidden = NO;
                    [self.continueButton setTitle:@"Play again?" forState:UIControlStateNormal];
                }
            }
        }
        NSLog(@"Score: %d", score);
        if (score > -1)
        {
            self.scoreLabel.text = [NSString stringWithFormat:@"%d", score];
            self.scoreLabel.hidden = NO;
        }
        else
        {
            self.scoreLabel.hidden = YES;
        }
        [self.view bringSubviewToFront: self.textfield];
        
        
        
        if (!isCorrect || debug)
        {
            if (self.animate)
            {
                NSString *temp = self.changedForm.text;
                self.changedForm.text = @"";
                self.changedForm.hidden = NO;
                self.changedForm.textAlignment = NSTextAlignmentLeft;
                if (!self.autoNav)
                {
                    self.continueButton.hidden = NO;
                    self.backButton.hidden = NO;
                    self.continueButton.enabled = NO;
                    self.backButton.enabled = NO;
                }
                
                dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
                    [self typeLabel:self.changedForm withString:temp withInterval:self.typeInterval setHeight:YES completion:nil];
                    
                    dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC));
                    dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                        if (!self.autoNav)
                        {
                            self.continueButton.enabled = YES;
                            self.backButton.enabled = YES;
                        }
                        else
                        {
                            self.redXView.hidden = YES;
                            self.greenCheckView.hidden = YES;
                            [self loadNext];
                            self.textfield.text = @"";
                            self.timeLabel.hidden = NO;
                        }
                    });
                });
            }
            else
            {
                self.changedForm.hidden = NO;
                dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC));
                dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                    if (!self.autoNav)
                    {
                        self.continueButton.hidden = NO;
                        self.backButton.hidden = NO;
                    }
                    else
                    {
                        self.redXView.hidden = YES;
                        self.greenCheckView.hidden = YES;
                        [self loadNext];
                        self.textfield.text = @"";
                        self.timeLabel.hidden = NO;
                    }
                });
            }
        }
        else
        {
            if (self.autoNavForCorrect)
            {
                self.changedForm.text = self.textfield.text;
                self.changedForm.frame = CGRectMake(self.textfield.frame.origin.x, self.textfield.frame.origin.y + 5, self.textfield.frame.size.width, self.textfield.frame.size.height);
                [self centerLabel:self.changedForm withString:self.changedForm.text setHeight:NO];
                self.changedForm.hidden = NO;
                self.textfield.hidden = YES;
                dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){

                [self hideTypeTextField:self.textfield withInterval:self.typeInterval completion:^{
                    [self hideTypeAttLabel:self.stemLabel withInterval:self.typeInterval completion:^{
                        [self hideTypeLabel:self.changeTo withInterval:self.typeInterval completion:^{
                            [self hideTypeLabel:self.origForm withInterval:self.typeInterval completion:^{

                                [UIView animateWithDuration:0.35f
                                                      delay:0.5f
                                     usingSpringWithDamping:1.0f
                                      initialSpringVelocity:0.4f
                                                    options:UIViewAnimationOptionCurveEaseIn
                                                 animations:^{
                                                     self.changedForm.text = self.changedStr;//in case it is decomposed
                                                     [self.view bringSubviewToFront:self.changedForm];
                                                     [self.view bringSubviewToFront:self.greenCheckView];
                                                     [self.changedForm setFrame:CGRectMake(self.changedForm.frame.origin.x, self.origForm.frame.origin.y, self.changedForm.frame.size.width, self.changedForm.frame.size.height)
                                                      ];
                                                     [self.greenCheckView setFrame:CGRectMake(self.greenCheckView.frame.origin.x, self.origForm.frame.origin.y, self.greenCheckView.frame.size.width, self.greenCheckView.frame.size.height)
                                                      ];
                                                 }
                                                 completion:^(BOOL finished){
                                                     //switch
                                                     dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                                                     dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                                                     self.redXView.hidden = YES;
                                                     self.greenCheckView.hidden = YES;
                                                     self.timeLabel.hidden = NO;
                                                     UILabel *temp;
                                                     temp = self.origForm;
                                                     self.origForm = self.changedForm;
                                                     self.changedForm = temp;
                                                         dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                                                         dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                                                     [self loadNext];
                                                         });
                                                         });
                                                 }];
                                
                                
                            }];
                        }];
                    }];
                }];
                });
                return;
            }
            else
            {
                self.continueButton.hidden = NO;
                self.backButton.hidden = NO;
            }
        }
        
        self.front = false;
        self.textfield.userInteractionEnabled = NO;
    }
    else //if back
    {
        self.timeLabel.text = @"30.00 sec";
        //[self cleanUp:!(self.useNewAnimation && verbSeq < vsOptions.repsPerVerb)];
        if (self.verbQuestionType == HOPLITE_CHALLENGE && self.lives < 1)
        {
            [self resetToPlayAgain];
        }
        else
        {
            [self loadNext3];
        }
    }
}

-(void) resetToPlayAgain
{
    self.life1.hidden = NO;
    self.life2.hidden = NO;
    self.life3.hidden = NO;
    self.lives = 3;
    self.scoreLabel.text = @"0";
    resetVerbSeq();
    [self loadNext3];
    [self.continueButton setTitle:@"Continue" forState:UIControlStateNormal];
    self.gameOverLabel.hidden = YES;
}

- (BOOL)isAcceptableTextLength:(NSUInteger)length {
    return length <= 50;
}
//http://stackoverflow.com/questions/9144593/how-do-i-limit-characters-in-uitextview
- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)string {
    return [self isAcceptableTextLength:self.textfield.text.length + string.length - range.length];
}

-(IBAction)checkIfCorrectLength:(id)sender{
    if (![self isAcceptableTextLength:self.textfield.text.length]) {
        // do something to make text shorter
    }
}

-(void) loadNext3
{
    NSLog(@"next3");
    [[(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard] resetKeyboard];
    self.timeLabel.textColor = [UIColor blackColor];
    [self stopTimer];
    self.MFLabel.hidden = YES;
    self.front = true;
    bool newGame = (self->verbSeq < 0) ? true : false;
    NSLog(@"next3a ");
    int type = nextVerbSeq(&self->verbSeq, &self->vf1, &self->vf2, &self->vsOptions);
    NSLog(@"next3b");
    if (type == VERB_SEQ_PP)
    {
        NSLog(@"principal parts!");
        self.cardType = CARD_PRINCIPAL_PARTS;
        [self loadPrincipalPart:&self->vf2];
        return;
    }
    else if (type == VERB_SEQ_CHANGE || type == VERB_SEQ_CHANGE_NEW)
    {
        NSLog(@"next3c");
        if (newGame)
            [self loadMorphTraining];
        else
            [self cleanUp:(type == VERB_SEQ_CHANGE_NEW)];
    }
    NSLog(@"next3d");
}

-(void)cleanUp: (Boolean)reset /* onComplete:(void (^)(void))onComplete */
{
    NSLog(@"cleanup: %d", reset);
    //this means it was correct
    if (self.expanded)
    {
        [self unexpand];
    }
    
    if (self.changedForm.hidden == YES)
    {
        
        self.changedForm.text = self.textfield.text;
        self.changedForm.frame = CGRectMake(self.textfield.frame.origin.x, self.textfield.frame.origin.y, self.textfield.frame.size.width, self.textfield.frame.size.height);
        [self centerLabel:self.changedForm withString:self.changedForm.text setHeight:NO];
        self.changedForm.hidden = NO;
        self.textfield.hidden = YES;
    }
    
    self.redXView.hidden = YES;
    self.greenCheckView.hidden = YES;
    self.continueButton.hidden = YES;
    
    //if we move the last form up or wipe it all clean and start with a new verb
    if (!reset)
    {
        [self hideTypeTextField:self.textfield withInterval:self.typeInterval completion:^{
            [self hideTypeAttLabel:self.stemLabel withInterval:self.typeInterval completion:^{
                [self hideTypeLabel:self.changeTo withInterval:self.typeInterval completion:^{
                    [self hideTypeLabel:self.origForm withInterval:self.typeInterval completion:^{
                        //self.timeLabel.hidden = true;
                        [UIView animateWithDuration:0.35f
                                              delay:0.5f
                             usingSpringWithDamping:1.0f
                              initialSpringVelocity:0.4f
                                            options:UIViewAnimationOptionCurveEaseIn
                                         animations:^{
                                             [self.view bringSubviewToFront:self.changedForm];
                                             [self.changedForm setFrame:CGRectMake(self.changedForm.frame.origin.x, self.origForm.frame.origin.y, self.changedForm.frame.size.width, self.origForm.frame.size.height)
                                              ];
                                         }
                                         completion:^(BOOL finished){
                                             
                                             //switch
                                             UILabel *temp;
                                             temp = self.origForm;
                                             self.origForm = self.changedForm;
                                             self.changedForm = temp;
                                             //[self loadNext];
                                             [self loadMorphTraining];
                                         }];
                    }];
                }];
            }];
        }];
    }
    else
    {
        [self hideTypeLabel:self.changedForm withInterval:self.typeInterval completion:^{
            [self hideTypeTextField:self.textfield withInterval:self.typeInterval completion:^{
                [self hideTypeAttLabel:self.stemLabel withInterval:self.typeInterval completion:^{
                    [self hideTypeLabel:self.changeTo withInterval:self.typeInterval completion:^{
                        [self hideTypeLabel:self.origForm withInterval:self.typeInterval completion:^{
                            //[self loadNext];
                            [self loadMorphTraining];
                        }];
                    }];
                }];
            }];
        }];
    }
    self.timeLabel.hidden = NO;
}

-(void)startTimer
{
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        self.timeLabel.text = [NSString stringWithFormat:@"%ld.00 sec", (long)self.HCTime];
    }
    else
    {
        self.timeLabel.text = @"0.00 sec";
    }
    self.startTime = CACurrentMediaTime();
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(runTimer)];
    self.displayLink.frameInterval = 1;
    [self.displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void)runTimer
{
    CFTimeInterval elapsedTime;
    elapsedTime = CACurrentMediaTime() - self.startTime;
    
    //this will be stored in db
    self.elapsedTimeForDB = elapsedTime;
    elapsedTime = self.HCTime - self.elapsedTimeForDB;
    
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        if (elapsedTime < 0)
        {
            self.timeLabel.text = @"0.00 sec";
            [self preCheckVerbTimeout];
        }
        else
        {
            self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        }
    }
    else
    {
        if (self.limitElapsedTime && self.elapsedTimeForDB > self.elapsedTimeLimit)
        {
            self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", self.elapsedTimeLimit];
            [self stopTimer];
            //[self preCheckVerbTimeout];
        }
        else
        {
            self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", self.elapsedTimeForDB];
        }
    }
}

-(void)stopTimer
{
    //update the timer label once more so it's accurate
    //CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
    //self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
    if (self.displayLink)
    {
        [self.displayLink invalidate];
    }
    self.displayLink = nil;
}

-(NSMutableAttributedString *)attributedDescription:(NSString*)d1 newDescription:(NSString*)d2
{
    NSArray *ad1 = [d1 componentsSeparatedByString: @" "];
    NSArray *ad2 = [d2 componentsSeparatedByString: @" "];
    NSMutableAttributedString *a = [[NSMutableAttributedString alloc] initWithString:d2];
    
    for (int i = 0; i < 5; i++)
    {
        NSUInteger start = 0;
        NSUInteger len = 0;
        if ( ![[ad1 objectAtIndex:i] isEqualToString: [ad2 objectAtIndex:i]] )
        {
            for(int j = 0; j < i; j++)
            {
                start += [[ad2 objectAtIndex:j] length] + 1;
            }
            len = [[ad2 objectAtIndex:i] length];
            
        }
        [a addAttribute:NSFontAttributeName
                  value:[UIFont fontWithName:@"HelveticaNeue-Bold" size:self.fontSize]
                  range:NSMakeRange(start, len)];
    }
    
    return a;
}

void dispatchAfter(double delay, void (^block)(void))
{
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC));
    dispatch_after(popTime, dispatch_get_main_queue(), block);
}

-(void) loadMorphTraining
{
    VerbFormC *vf1Loc = &self->vf1;
    VerbFormC *vf2Loc = &self->vf2;
    
    int bufferLen = 1024;
    char buffer[bufferLen];

    //int type = nextVerbSeq(&self->verbSeq, &vf1, &vf2, &self->vsOptions);
    int type = VERB_SEQ_CHANGE_NEW;
    if (type == VERB_SEQ_PP)
    {
        NSLog(@"principal parts!");
        self.cardType = CARD_PRINCIPAL_PARTS;
        [self loadPrincipalPart:vf2Loc];
        return;
    }
    
    getForm(vf1Loc, buffer, bufferLen, false, false);
    
    NSString *origForm = nil;
    NSString *newForm = nil;
    NSString *newDescription = nil;
    
    self.lemma = [NSString stringWithUTF8String: (const char*)vf1Loc->verb->present];
    if (self->verbSeq == 1 && self->vsOptions.startOnFirstSing)
    {
        //use lemma rather than a possibly contracted form
        origForm = [NSString stringWithUTF8String: (const char*)vf1Loc->verb->present];
    }
    else
    {
        origForm = [NSString stringWithUTF8String: (const char*)buffer];
    }
    //origForm = [self selectRandomFromCSV:origForm];
    origForm = [origForm stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
    self.origStr = origForm;
    
    getForm(vf1Loc, buffer, bufferLen, true, true);
    self.origStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    //self.origStrDecomposed = [self selectRandomFromCSV:self.origStrDecomposed]; //FIXME, What if not same as orig form
    self.origStrDecomposed = [self.origStrDecomposed stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
    
    getAbbrevDescription(vf1Loc, buffer, bufferLen);
    NSString *origDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    getForm(vf2Loc, buffer, bufferLen, false, false);
    newForm = [NSString stringWithUTF8String: (const char*)buffer];
    self.changedStr = newForm;
    
    getForm(vf2Loc, buffer, bufferLen, true, true);
    self.changedStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    
    getAbbrevDescription(vf2Loc, buffer, bufferLen);
    newDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    NSAttributedString *attDesc = [self attributedDescription: origDescription newDescription:newDescription];
    
    /*** Set label and button text ***/
    //self.origForm.text = origForm;

    if (self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        //self.textfield.hidden = NO;
        //[self.textfield becomeFirstResponder];
    }
    else
    {
        [self.textfield resignFirstResponder];
        self.textfield.hidden = YES;
    }
    
    self.textfield.hidden = YES;
    
    if (self.verbQuestionType == SELF_PRACTICE || self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        self.changedForm.text = newForm;
    }
    
    /******* NOW SETUP THE VIEW: static layout which should be done elsewhere ******/
    
    self.expanded = NO;
    self.mfPressed = NO;
    self.textfield.text = @"";
    self.textfield.textColor = [UIColor blackColor];
    self.continueButton.hidden = YES;
    //self.backButton.hidden = YES;
    self.redXView.hidden = YES;
    self.greenCheckView.hidden = YES;
    
    self.origForm.backgroundColor = [UIColor whiteColor];
    self.origForm.layer.cornerRadius = 8;
    [self.origForm.layer setMasksToBounds:YES];
    self.changedForm.backgroundColor = [UIColor whiteColor];
    self.changedForm.layer.cornerRadius = 8;
    [self.changedForm.layer setMasksToBounds:YES];
    
    [self.verbModeButton setFrame:CGRectMake(self.view.frame.size.width - 60 - 6, 6, 60, 36.0)];
    
    self.stemLabel.textAlignment = NSTextAlignmentLeft;//NSTextAlignmentCenter;
    self.changedForm.textAlignment = NSTextAlignmentLeft;
    
    /* init stuff */
    
    self.origForm.hidden = NO;
    self.stemLabel.hidden = NO;
    self.backLabel.hidden = YES;
    self.changedForm.hidden = YES;
    self.correctButton.hidden = YES;
    self.incorrectButton.hidden = YES;
    self.backLabel.hidden = YES;
    
    if (verbSeq == 1 || !self.useNewAnimation)
        self.origForm.text = @"";
    self.changeTo.text = @"";
    self.changeTo.hidden = NO;
    self.stemLabel.text = @"";
    self.stemLabel.attributedText = nil;

    [self positionWidgetsToSize:self.view.frame.size];
    
    self.changedForm.numberOfLines = 0;
    self.origForm.numberOfLines = 0; //fixes truncation with multiple forms
    self.changedForm.lineBreakMode = NSLineBreakByWordWrapping;
    self.changeTo.textColor = [UIColor grayColor];
    self.stemLabel.textColor = [UIColor grayColor];
    
    self.origForm.textAlignment = NSTextAlignmentLeft;
    self.changeTo.textAlignment = NSTextAlignmentLeft;
    self.stemLabel.textAlignment = NSTextAlignmentLeft;
    
    //http://stackoverflow.com/questions/15335649/adding-delay-between-execution-of-two-following-lines
    //or maybe try this:  http://stackoverflow.com/questions/17949511/the-proper-way-of-doing-chain-animations
    
    if (verbSeq == 1 || !self.useNewAnimation)
    {
        if (!self.readDirectionsOnce)
        {
            self.readDirectionsOnce =  YES;
        dispatchAfter( 0.8, ^(void)
                      {
                          //self.stemLabel.hidden = NO;
                          //self.stemLabel.textAlignment = NSTextAlignmentLeft;
                          self.origForm.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
                          [self typeLabel:self.origForm withString:@"Change the given form..." withInterval:self.typeInterval setHeight:NO completion:^{
                              
                              dispatchAfter( 1.8, ^(void)
                                            {
                                                [self hideTypeLabel:self.origForm withInterval:self.typeInterval completion:^(void)
                                                 {
                                                     
                              self.origForm.font = [UIFont fontWithName:self.greekFont size:self.greekFontSize];
                          [self typeLabel:self.origForm withString:origForm withInterval:self.typeInterval setHeight:NO completion:nil];
                          
                          dispatchAfter( 0.9, ^(void)
                                        {
                                            //self.origForm.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
                                            self.stemLabel.textColor = [UIColor blackColor];
                                            [self typeLabel:self.stemLabel withString:@"...to the form indicated." withInterval:self.typeInterval setHeight:NO completion:^{
                                                
                                                dispatchAfter( 1.5, ^(void)
                                                              {
                                                                  [self hideTypeLabel:self.stemLabel withInterval:self.typeInterval completion:^(void)
                                                                   {
                                            
                                            //[self typeLabel:self.stemLabel withString:newDescription withInterval:self.typeInterval];
                                            self.stemLabel.textColor = [UIColor grayColor];
                                            self.stemLabel.attributedText = nil;
                                            [self typeAttLabel:self.stemLabel withString: attDesc withInterval:self.typeInterval];
                                            
                                            dispatchAfter( 0.7, ^(void)
                                                          {
                                                              self.textfield.text = @"";
                                                              self.textfield.userInteractionEnabled = YES;
                                                              self.textfield.hidden = NO;
                                                              [self.textfield becomeFirstResponder];
                                                              
                                                              dispatchAfter( 0.4, ^(void)
                                                                            {
                                                                                self.timeLabel.hidden = NO;
                                                                                [self startTimer];
                                                                            });
                                                          });
                                        
                                            
                                                                   }];});
                                                                   }];
                                            });
                      
                                                 }];});
                          
                          }];
                          }
                           
                           );
        }
        else
        {
        dispatchAfter( 0.8, ^(void)
                      {
                          [self typeLabel:self.origForm withString:origForm withInterval:self.typeInterval setHeight:NO completion:nil];
                          
                          dispatchAfter( 1.0, ^(void)
                                        {
                                            //[self typeLabel:self.stemLabel withString:newDescription withInterval:self.typeInterval];
                                            self.stemLabel.attributedText = nil;
                                            [self typeAttLabel:self.stemLabel withString: attDesc withInterval:self.typeInterval];
                                            
                                            dispatchAfter( 0.7, ^(void)
                                                          {
                                                              self.textfield.text = @"";
                                                              self.textfield.userInteractionEnabled = YES;
                                                              self.textfield.hidden = NO;
                                                              [self.textfield becomeFirstResponder];
                                                              
                                                              dispatchAfter( 0.4, ^(void)
                                                                            {
                                                                                self.timeLabel.hidden = NO;
                                                                                [self startTimer];
                                                                            });
                                                          });
                                        });
                      });
        }
    }
    else
    {
        dispatchAfter( 0.4, ^(void)
                      {
                          //[self typeLabel:self.stemLabel withString:newDescription withInterval:self.typeInterval];
                          self.stemLabel.attributedText = nil;
                          [self typeAttLabel:self.stemLabel withString: attDesc withInterval:self.typeInterval];
                          
                          dispatchAfter( 0.7, ^(void)
                                        {
                                            self.textfield.text = @"";
                                            self.textfield.userInteractionEnabled = YES;
                                            self.textfield.hidden = NO;
                                            
                                            [self.textfield becomeFirstResponder];
                                            
                                            dispatchAfter( 0.4, ^(void)
                                                          {
                                                              self.timeLabel.hidden = NO;
                                                              [self startTimer];
                                                          });
                                        });
                      });
    }

    self.startTime = CACurrentMediaTime();
}

-(void) loadEnding
{
     int bufferLen = 1024;
     char buffer[bufferLen];
     int units[20] = { 1,2,3,4,5,6,7 };
     int numUnits = 7;

     getRandomEndingAsString(units, numUnits, buffer, bufferLen);
    
     NSString *endings = [NSString stringWithUTF8String: (const char*)buffer];
    
     NSArray *Es = [endings componentsSeparatedByString:@"; "];
     
    
    self.stemLabel.text = [NSString stringWithFormat:@"%@", [Es objectAtIndex:0]];
    
    self.singLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[Es objectAtIndex:1], [Es objectAtIndex:2], [Es objectAtIndex:3]];
    
    self.pluralLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[Es objectAtIndex:4], [Es objectAtIndex:5], [Es objectAtIndex:6]];
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    /*
    CGSize sFS = [[[array lastObject] valueForKey:@"fs"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:self.greekFont size:26.0] }];
    CGSize sSS = [[[array lastObject] valueForKey:@"ss"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:self.greekFont size:26.0] }];
    CGSize sTS = [[[array lastObject] valueForKey:@"ts"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:self.greekFont size:26.0] }];
    CGSize sFP = [[[array lastObject] valueForKey:@"fp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:self.greekFont size:26.0] }];
    CGSize sSP = [[[array lastObject] valueForKey:@"sp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:self.greekFont size:26.0] }];
    CGSize sTP = [[[array lastObject] valueForKey:@"tp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
     */
    
    if ( [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait )
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.width * 1/3) - 50, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.width * 2/3) - 46, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
    }
    else
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.height * 1/3) - 20,50,  screenSize.height - 40, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.height * 2/3) - 50,50
                                              ,  screenSize.height - 40, 240.0)];
    }
    
    self.stemLabel.hidden = false;
    self.backLabel.hidden = true;
    self.timeLabel.hidden = NO;
    self.singLabel.hidden = true;
    self.pluralLabel.hidden = true; 
    self.front = true;
    
    //[self.stemLabel sizeToFit];
    [self.backLabel sizeToFit];
    [self.singLabel sizeToFit];
    [self.pluralLabel sizeToFit];
    
    //[self.stemLabel setCenter:self.view.center];
    [self.backLabel setCenter:self.view.center];

    self.startTime = CACurrentMediaTime();
}


-(void) loadPrincipalPart:(VerbFormC *)vf
{
    int bufferLen = 1024;
    char buffer[bufferLen];

    self.stemLabel.hidden = YES;
    self.origForm.hidden = YES;
    self.changedForm.hidden = YES;
    self.changeTo.hidden = YES;
    
    getForm(vf, buffer, bufferLen, false, false);
    NSString *frontForm = [NSString stringWithUTF8String: (const char*)buffer];
    frontForm = [self selectRandomFromCSV:frontForm];
    
    getPrincipalParts(vf->verb, buffer, bufferLen);
    NSString *principalParts = [NSString stringWithUTF8String: (const char*)buffer];
     
    NSArray *PPs = [principalParts componentsSeparatedByString:@"; "];
    
    NSString *from = @", ";
    NSString *to = @" | "; // /, |, or
    //change , to /
    
    NSString *pres = [[PPs objectAtIndex:0] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *fut = [[PPs objectAtIndex:1] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *aor = [[PPs objectAtIndex:2] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *perf = [[PPs objectAtIndex:3] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *perfmid = [[PPs objectAtIndex:4] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *aorpass = [[PPs objectAtIndex:5] stringByReplacingOccurrencesOfString: from withString:to];
    
    NSString *spacer = @"—";//"@"--";

    NSMutableAttributedString* attrString; 
    if (1) //one line each111
    {
        
    /*self.backCard*/NSString *temp = [NSString stringWithFormat:@"%@\n%@\n%@\n%@\n%@\n%@", pres.length > 0 ? pres : spacer, fut.length > 0 ? fut : spacer, aor.length > 0 ? aor : spacer, perf.length > 0 ? perf : spacer, perfmid.length > 0 ? perfmid : spacer, aorpass.length > 0 ? aorpass : spacer];
        
         attrString = [[NSMutableAttributedString alloc] initWithString:temp];
         NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
         [style setLineSpacing:6];
         [attrString addAttribute:NSParagraphStyleAttributeName value:style range:NSMakeRange(0, [attrString length])];
         //self.backCard.attributedText = attrString;
    }
    else
    {
        self.backCard = [NSString stringWithFormat:@"%@, %@, %@, %@, %@, %@", pres.length > 0 ? pres : spacer, fut.length > 0 ? fut : spacer, aor.length > 0 ? aor : spacer, perf.length > 0 ? perf : spacer, perfmid.length > 0 ? perfmid : spacer, aorpass.length > 0 ? aorpass : spacer];
    }
    self.origForm.text = @"";
    self.origForm.hidden = NO;
    [self centerLabel:self.origForm withString:frontForm setHeight:NO];
    [self typeLabel:self.origForm withString:frontForm withInterval:self.typeInterval setHeight:NO completion:nil];
    //self.backLabel.text = self.backCard;
    self.backLabel.attributedText = attrString;
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    if (1)// [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait )
    {
        //[self.stemLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
    }
    else
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
    }
    
    //self.stemLabel.hidden = false;
    self.backLabel.hidden = true;
    
    self.backLabel.textAlignment = NSTextAlignmentLeft;
    self.stemLabel.textAlignment = NSTextAlignmentLeft;
    
    //[self.stemLabel sizeToFit];
    [self.backLabel sizeToFit];
    
    //[self.stemLabel setCenter:self.view.center];
    [self.backLabel setCenter:self.view.center];
    
    self.front = true;
    
    self.startTime = CACurrentMediaTime();
}

- (void)setDetailItem:(id)newDetailItem
{
    if (_detailItem != newDetailItem) {
        _detailItem = newDetailItem;
        
        // Update the view.
        [self configureView];
    }

    if (self.masterPopoverController != nil) {
        [self.masterPopoverController dismissPopoverAnimated:YES];
    }        
}

- (void)configureView
{
    self.cardType = [[self.detailItem  valueForKey:@"sort"] integerValue];
    if (!self.cardType)
        self.cardType = CARD_VERBS;
    
    if (1)//self.cardType != CARD_VERBS)
    {
        self.verbModeButton.hidden = YES;
    }
    else
    {
        self.verbModeButton.hidden = NO;
    }
    
    self.backLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.backLabel.textAlignment = NSTextAlignmentCenter;
    //self.stemLabel.textAlignment = NSTextAlignmentCenter;
    self.stemLabel.numberOfLines = 0;
    self.backLabel.numberOfLines = 0;
    self.singLabel.numberOfLines = 0;
    self.pluralLabel.numberOfLines = 0;
    
    //NSString *font = @"GillSans";
    ///Users/jeremy/Dropbox/Code/cocoa/morphv5/morph/DetailViewController.mNSString *font = @"Kailasa";
    //NSString *font = @"ArialMT";
    
    UIFont *greekFont = [UIFont fontWithName:self.greekFont size:self.greekFontSize];
    
    self.origForm.font = greekFont;
    self.changedForm.font = greekFont;
    
    [self.MCButtonA.titleLabel setFont: greekFont];
    [self.MCButtonB.titleLabel setFont: greekFont];
    [self.MCButtonC.titleLabel setFont: greekFont];
    [self.MCButtonD.titleLabel setFont: greekFont];
    
    self.stemLabel.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
    self.changeTo.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
    self.backLabel.font = [UIFont fontWithName:self.greekFont size:self.greekFontSize];
    self.singLabel.font = [UIFont fontWithName:self.greekFont size:self.fontSize];
    self.pluralLabel.font = [UIFont fontWithName:self.greekFont size:self.fontSize];
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    if ( [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait )
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.width * 1/3) - 50, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.width * 2/3) - 46, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
    }
    else
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.height * 1/3) - 20,50,  screenSize.height - 40, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.height * 2/3) - 50,50
                                              ,  screenSize.height - 40, 240.0)];
    }
    
    [self.timeLabel setFrame:CGRectMake(self.view.bounds.size.width - 200, 6,  194, 30)];
    [self.MFLabel setFrame:CGRectMake(self.view.bounds.size.width - 120 - 42, 6,  42, 30)];
    self.timeLabel.textAlignment = NSTextAlignmentRight;
    self.changeTo.hidden = YES;
    self.stemLabel.text = @"";
    /*
    dispatchAfter( 0.4, ^(void)
                  {
                      self.stemLabel.hidden = NO;
                      self.stemLabel.textAlignment = NSTextAlignmentLeft;
                      [self typeLabel:self.stemLabel withString:@"Change the given form...\n...to the form indicated." withInterval:self.typeInterval setHeight:YES completion:^{
                          
                          dispatchAfter( 3, ^(void)
                                        {
                                            [self hideTypeLabel:self.stemLabel withInterval:self.typeInterval completion:^(void)
                                            {
                                                [self loadNext3];
                                            }];
                                        });
                      
                      
                      }
                      
                       ];});
    */
    [self loadNext3];
}

- (BOOL)prefersStatusBarHidden {
    //http://stackoverflow.com/questions/18979837/how-to-hide-ios-7-status-bar
    return YES;
}

-(void) onSelectIncorrectMC:(UIButton *)sender
{
    if (self.front)
    {
        [self stopTimer];
        if (self.verbQuestionType == MULTIPLE_CHOICE)
        {
            AudioServicesPlaySystemSound(BuzzSound);
            
            if (self.backgroundOrBorder)
            {
                sender.backgroundColor = [UIColor redColor];
                [sender setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
                self.MCButtonA.backgroundColor = [UIColor greenColor];
                [self.MCButtonA setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            }
            else
            {
                sender.layer.borderColor = [UIColor redColor].CGColor;
                self.MCButtonA.layer.borderColor = [UIColor greenColor].CGColor;
            }
        }
        self.front = false;
        
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = NO;
    }
    else
    {
        if (self.verbQuestionType == SELF_PRACTICE)
        {
            AudioServicesPlaySystemSound(BuzzSound);
        }

        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = NO;
    }
}

-(void) onSelectCorrectMC:(UIButton *)sender
{
    if (self.front)
    {
        [self stopTimer];
        if (self.verbQuestionType == MULTIPLE_CHOICE)
        {
            AudioServicesPlaySystemSound(DingSound);
            
            if (self.backgroundOrBorder)
            {
                sender.backgroundColor = [UIColor greenColor];
                [sender setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            }
            else
            {
                sender.layer.borderColor = [UIColor greenColor].CGColor;
            }
        }
        self.front = false;
        
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = NO;
    }
    else
    {
        if (self.verbQuestionType == SELF_PRACTICE)
        {
            AudioServicesPlaySystemSound(DingSound);
        }
        
        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = NO;
    }
}

- (void)returnToRoot:(UIButton *)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.navigationController popToRootViewControllerAnimated:YES];
    [self.navigationController setNavigationBarHidden:YES];
}

- (void)toggleVerbMode :(UIButton *)sender
{
    if (self.verbQuestionType == SELF_PRACTICE)
    {
        self.verbQuestionType = MULTIPLE_CHOICE;
        [sender setTitle:@"MC" forState:UIControlStateNormal];
    }
    else if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        self.verbQuestionType = HOPLITE_PRACTICE;
        [sender setTitle:@"HC" forState:UIControlStateNormal];
    }
    
    else if (self.verbQuestionType == HOPLITE_PRACTICE)
    {
        self.verbQuestionType = SELF_PRACTICE;
        [sender setTitle:@"self" forState:UIControlStateNormal];
    }
    
    [self loadNext];
}

- (void)viewWillAppear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
    
    //need to fix this, so keyboard shows if necessary
    /*NSLog(@"stem: %@", self.stemLabel.text);
    if (![self.stemLabel.text isEqual: @"stem"])
    {
        [self.textfield becomeFirstResponder]; //be sure keyboard is shown if it can be
    } */
    
    //http://stackoverflow.com/questions/12591192/center-text-vertically-in-a-uitextview
    //see below
    [self.textfield addObserver:self forKeyPath:@"contentSize" options:(NSKeyValueObservingOptionNew) context:NULL];
}

//http://stackoverflow.com/questions/12591192/center-text-vertically-in-a-uitextview
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    UITextView *tv = object;
    CGFloat topCorrect = ([tv bounds].size.height - [tv contentSize].height * [tv zoomScale])/2.0;
    topCorrect = ( topCorrect < 0.0 ? 0.0 : topCorrect );
    //NSLog(@"content: %f, %f, %f", topCorrect, [tv bounds].size.height, [tv contentSize].height);
    [tv setContentInset:UIEdgeInsetsMake(topCorrect,0,0,0)];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
    
    [self.textfield removeObserver:self forKeyPath:@"contentSize" context:NULL];
}

-(void) setLevels
{
    if (self.levels)
        [self.levels removeAllObjects];
    
    self.buttonStates = nil;
    self.buttonStates = [[NSMutableArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:@"Levels"]];
    
    for (int i = 0; i < [self.buttonStates count]; i++)
    {
        if ([[self.buttonStates objectAtIndex:i] boolValue] == YES)
        {
            [self.levels addObject:[NSNumber numberWithInt:(i+1)]];
        }
    }
    
    if ([self.levels count] > 0)
    {
        int i;
        self->vsOptions.numUnits = 0;
        for (i = 0; i < [self.levels count]; i++)
        {
            self->vsOptions.units[i] = (int)[[self.levels objectAtIndex:i] integerValue];
            self->vsOptions.numUnits++;
        }
    }
    else //default if nothing selected
    {
        self->vsOptions.units[0] = 1;
        self->vsOptions.numUnits = 1;
    }
    self->vsOptions.startOnFirstSing = false;//true;
    self->vsOptions.degreesToChange = 2;
    self->vsOptions.repsPerVerb = 4;
    self->vsOptions.askPrincipalParts = false;//(self.verbQuestionType == HOPLITE_PRACTICE) ? true : false;
    self->vsOptions.isHCGame = (self.verbQuestionType == HOPLITE_CHALLENGE) ? true : false;
    self->vsOptions.practiceVerbID = -1; //invalid
    resetVerbSeq();
}

-(void)setMode
{
    NSString *s = [[NSUserDefaults standardUserDefaults] objectForKey:@"Mode"];
    if ([s isEqualToString:@"Hoplite Challenge"])
        self.verbQuestionType = HOPLITE_CHALLENGE;
    else if ([s isEqualToString:@"Self Practice"])
        self.verbQuestionType = SELF_PRACTICE;
    else if ([s isEqualToString:@"Multiple Choice"])
        self.verbQuestionType = MULTIPLE_CHOICE;
    else
        self.verbQuestionType = HOPLITE_PRACTICE;
    
    if  ( [[NSUserDefaults standardUserDefaults] boolForKey:@"DisableAnimations"] == YES)
        self.animate = NO;
    else
        self.animate = YES;
}

- (void)handlePinch:(UIPinchGestureRecognizer *)pinch
{
    //NSLog(@"Scale: %.2f | Velocity: %.2f",pinch.scale, pinch.velocity);
    CGFloat thresholdVelocity = 0;//4.0;
    
    if (self.front)
        return;
    
    if (pinch.scale > 1 && pinch.velocity > thresholdVelocity)
    {
        [self expand];
    }
    else if (pinch.velocity < -thresholdVelocity)
    {
        [self unexpand];
    }
}

-(void)expand //ie decompose
{
    if (!self.expanded)
    {
        NSString *newChanged = [self.changedStrDecomposed stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
        if (self.changedForm.hidden == YES)
        {
            //self.changedForm.frame = self.textfield.frame;
            //self.changedForm.hidden = NO;
            //self.textfield.hidden = YES;
            self.textfield.text = newChanged;
            
            //slide green check over
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            CGFloat gvX = (self.view.frame.size.width + size.width) / 2 + 15;
            //don't let it go off the screen
            if (gvX > self.view.frame.size.width - 26)
                gvX = self.view.frame.size.width - 26;
            [self.greenCheckView setFrame:CGRectMake(gvX, self.greenCheckView.frame.origin.y, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
        }
        else
        {
            self.changedForm.text = newChanged;
            [self centerLabel:self.changedForm withString:newChanged setHeight:NO];
        }
        self.expanded = YES;
        self.origForm.text = self.origStrDecomposed;
        [self centerLabel:self.origForm withString:self.origStrDecomposed setHeight:NO];
    }
}

-(void)unexpand //ie un-decompose
{
    if (self.expanded)
    {
        NSString *newChanged = [self.changedStr stringByReplacingOccurrencesOfString:@", " withString:@",\n"];
        if (self.changedForm.hidden == YES)
        {
            self.textfield.text = newChanged;
            //slide green check over
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            CGFloat gvX = (self.view.frame.size.width + size.width) / 2 + 15;
            //don't let it go off the screen
            if (gvX > self.view.frame.size.width - 26)
                gvX = self.view.frame.size.width - 26;
            [self.greenCheckView setFrame:CGRectMake(gvX, self.greenCheckView.frame.origin.y, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
        }
        else
        {
            self.changedForm.text = newChanged;
            [self centerLabel:self.changedForm withString:newChanged setHeight:NO];
        }
        self.expanded = NO;
        self.origForm.text = self.origStr;
        [self centerLabel:self.origForm withString:self.origStr setHeight:NO];
    }
}

- (NSString *)writeablePathForFile:(NSString*)fileName
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:fileName];
}

//do not allow selection of text.
-(void)textViewDidChangeSelection:(UITextView *)textView {
    //NSLog(@"Range: %d, %d", textView.selectedRange.location, textView.selectedRange.length);
    if (textView.selectedRange.length > 0)
    {
        textView.selectedRange = NSMakeRange(textView.selectedRange.location, 0);
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.readDirectionsOnce = NO;
    self.textfield.delegate = self;
    
    //we only need to call this on ios9 and up
    NSOperatingSystemVersion ios9_0_0 = (NSOperatingSystemVersion){9, 0, 0}; //this test is only ios 8 and up.
    if ([[NSProcessInfo processInfo] isOperatingSystemAtLeastVersion:ios9_0_0])
    {
        //http://stackoverflow.com/questions/32606655/how-to-hide-the-shortcut-bar-in-ios9
        //prevent redo/undo, clipboard from showing up on keyboard
        UITextInputAssistantItem* item = [self.textfield inputAssistantItem];
        item.leadingBarButtonGroups = @[];
        item.trailingBarButtonGroups = @[];
    }
    /*
     NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
     NSString *documentsDirectoryPath = [paths objectAtIndex:0];
     NSLog(@"D: %@", documentsDirectoryPath);
     //NSLog(@"D2: %d", chdir([documentsDirectoryPath UTF8String]));
    
    //for scoring?
    NSString *content = @"Put this in a file please.";
    NSData *fileContents = [content dataUsingEncoding:NSUTF8StringEncoding];
    
    [[NSFileManager defaultManager] createFileAtPath:[documentsDirectoryPath stringByAppendingString:@"/hcdata5"]
                                            contents:fileContents
                                          attributes:nil];
    */
    //UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
    
    NSString *dataFileName = @"hcdata";
    NSString *dataFileWithPath = [self writeablePathForFile:dataFileName];
    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:dataFileWithPath];
    if (!fileExists)
    {
        [[NSFileManager defaultManager] createFileAtPath:dataFileWithPath
                                            contents:[[NSMutableData alloc] initWithLength:0 ] // Setting size to sizeInBytes does not make any difference on the results
                                          attributes:nil ];
        NSLog(@"Data file created");
    }
    else
    {
        NSLog(@"Data file exists");
    }
    VerbSeqInit([dataFileWithPath UTF8String]);
    
    self->verbSeq = -1; //set this to invalid value so we know we're just starting.  see loadnext3 above
    self.elapsedTimeLimit = 4; //upper limit in seconds for practice mode
    self.limitElapsedTime = NO; //whether to enforce an upper time limit for practice mode
    self.typeInterval = 0.02;
    self.autoNav = NO;  //load next automatically or stop and show nav buttons?
    self.autoNavForCorrect = NO;//YES;
    self.useNewAnimation = YES; //where the old form moves up rather than being erased
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (1)//[defaults boolForKey:@"DisableSound"] == YES)
        self.soundDisabled = YES;
    else
        self.soundDisabled = NO;
    
    
    if (1)//[defaults boolForKey:@"DisableAnimation"] == YES)
        self.animate = YES;
    else
        self.animate = NO;
    
    if ([defaults integerForKey:@"HCTime"])
        self.HCTime = [[NSUserDefaults standardUserDefaults] integerForKey:@"HCTime"];
    else
        self.HCTime = 30;
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    self.systemFont = @"HelveticaNeue-Light";
    self.greekFont = @"NewAthenaUnicode";
    

    //NSLog(@"screensize: %f x %f", screenSize.width, screenSize.height);
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        self.timeFontSize = 24.0;
        self.fontSize = 30.0;
        self.greekFontSize = 40.0;
    }
    else if (screenSize.height > 569) //6S
    {
        self.timeFontSize = 24.0;
        self.fontSize = 28.0;
        self.greekFontSize = 36.0;
    }
    else if (screenSize.height > 480) //5S
    {
        self.timeFontSize = 22.0;
        self.fontSize = 24.0;
        self.greekFontSize = 32.0;
    }
    else //4S
    {
        self.timeFontSize = 20.0;
        self.fontSize = 24.0;
        self.greekFontSize = 28.0;
    }
    //self.changedForm.layer.borderColor = [UIColor blackColor];
    self.changedForm.lineBreakMode = NSLineBreakByWordWrapping;
    
    self.mfPressed = NO;
    self.expanded = NO;
    self.view.userInteractionEnabled = YES;
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]
                                       initWithTarget:self action:@selector(handlePinch:)];
    pinch.delegate = self;
    //pinch.tag = self;
    [self.view addGestureRecognizer:pinch];
    
    self.cardType = CARD_VERBS;

    /*
    self.popupShown = FALSE;
    self.popup = [[PopUp alloc] initWithFrame:CGRectMake (0, [UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height)];
    [self.view addSubview:self.popup];
    */
    self.scoreLabel.font = [UIFont fontWithName:@"HelveticaNeue-Light" size:self.timeFontSize];
    self.scoreLabel.textColor = [UIColor HCBlue];
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        self.scoreLabel.text = @"0";
        self.scoreLabel.hidden = NO;
    }
    self.timeLabel.font = [UIFont fontWithName:@"HelveticaNeue-Light" size:self.timeFontSize];
    self.MFLabel.font = [UIFont fontWithName:@"HelveticaNeue" size:self.timeFontSize - 2];
    self.MFLabel.layer.borderColor = [UIColor colorWithRed:(255/255.0) green:(56/255.0) blue:(0/255.0) alpha:1.0].CGColor;
    self.MFLabel.layer.cornerRadius = 4.0f;
    self.MFLabel.layer.borderWidth = 2.0f;
    
    //[self.backButton setFrame:CGRectMake(10, self.view.frame.size.height - 40, 50, 30)];
    /*
     //crashes if here, so put in appdelegate
    NSLog(@"keyboard loaded1");
    self.keyboard = nil;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
        self.keyboard = [[Keyboard alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 172.0) lang:1]; //was 320 x 172
    }
    else
    {
        self.keyboard = [[Keyboard alloc] initWithFrame:CGRectMake(0.0, 0.0,  1024.0, 266.0) lang:1];  //was 1024 x 266
    }
    */
    [(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard].delegate = self;
    
    //http://stackoverflow.com/questions/16868117/uitextview-that-expands-to-text-using-auto-layout
    [self.textfield setInputView: [(AppDelegate*)[[UIApplication sharedApplication] delegate] keyboard]];
    //[self.textfield setInputView: self.keyboard];
    
    self.textfield.font = [UIFont fontWithName:self.greekFont size:self.greekFontSize];
    self.textfield.frame = CGRectMake(10, self.textfield.frame.origin.y, screenSize.width - 20, 54.0);
    self.textfield.hidden = NO;
    self.textfield.delegate = self;

    //fix me add border to textview
    //[self.textfield setBorderStyle:UITextBorderStyleNone];
    //self.textfield.textAlignment =
    
    //self.gameOverLabel.frame = CGRectMake(0, self.gameOverLabel.frame.origin.y, screenSize.width, self.gameOverLabel.frame.size.height);
    [self.gameOverLabel setFrame:CGRectMake(0, 32, self.view.frame.size.width - 10, self.gameOverLabel.frame.size.height)];
    
    //self.textfield.contentVerticalAlignment;
    //self.textfield.contentHorizontalAlignment = UIControlContentHorizontalAlignment.Center;
    self.textfield.textAlignment = NSTextAlignmentCenter;
    //fix me removed this
    //self.textfield.adjustsFontSizeToFitWidth = true;
    
    //sets focus and raises keyboard
    //[self.textfield becomeFirstResponder];
    self.textfield.hidden = YES;
    //self.backButton.hidden = YES;
    [self.backButton setTitle:@"X" forState:UIControlStateNormal];
    self.backButton.titleLabel.font = [UIFont fontWithName:@"HelveticaNeue" size:20.0];
    self.backButton.layer.borderColor = [UIColor grayColor].CGColor;
    self.backButton.layer.borderWidth = 2.0;
    //UIImage *btnImage = [UIImage imageNamed:@"XClose.png"];
    //[self.backButton setImage:btnImage forState:UIControlStateNormal];
    
    
    [[UIApplication sharedApplication] setStatusBarHidden:YES
                                            withAnimation:NO];
    [self.navigationController setNavigationBarHidden:YES animated:NO];
    self.wantsFullScreenLayout = YES;
    self.view.frame = [[UIScreen mainScreen] bounds];
    self.view.backgroundColor = [UIColor whiteColor];
    //self.view.backgroundColor = UIColorFromRGB(0xEEEEEE);//[UIColor lightGrayColor];//[UIColor whiteColor];
    
    self.backgroundOrBorder = YES;
    
    UIImage *greencheck = [UIImage imageNamed:@"greencheck.png"];
    self.greenCheckView = [[UIImageView alloc] initWithImage:greencheck];
    [self.view addSubview:self.greenCheckView];
    [self.greenCheckView setFrame:CGRectMake(screenBound.size.width - 34,self.view.frame.size.height/2.1 + 9,26,26)];
    self.greenCheckView.hidden = YES;
    UIImage *redx = [UIImage imageNamed:@"redx.png"];
    self.redXView = [[UIImageView alloc] initWithImage:redx];
    [self.view addSubview:self.redXView];
    [self.redXView setFrame:CGRectMake(screenBound.size.width - 34,self.view.frame.size.height/2.1 + 9,22,22)];
    self.redXView.hidden = YES;
    
    if (!self.soundDisabled)
    {
        NSString *dingPath = [[NSBundle mainBundle]
                              pathForResource:@"Ding" ofType:@"wav"];
        NSURL *dingURL = [NSURL fileURLWithPath:dingPath];
        AudioServicesCreateSystemSoundID((__bridge CFURLRef)dingURL, &DingSound);
        
        NSString *buzzPath = [[NSBundle mainBundle]
                              pathForResource:@"Buzz02" ofType:@"wav"];
        NSURL *buzzURL = [NSURL fileURLWithPath:buzzPath];
        AudioServicesCreateSystemSoundID((__bridge CFURLRef)buzzURL, &BuzzSound);
    }
    
	// Do any additional setup after loading the view, typically from a nib.
    
    
    //self.verbQuestionType = HOPLITE_PRACTICE;//SELF_PRACTICE;  //MULTIPLE_CHOICE;
    [self.verbModeButton setTitle:@"MC" forState:UIControlStateNormal];

    self.levels = [NSMutableArray arrayWithObjects: nil];
    
    self.mcButtons = [NSArray arrayWithObjects: self.MCButtonA, self.MCButtonB, self.MCButtonC, self.MCButtonD, nil];
    self.mcButtonsOrder = [NSMutableArray arrayWithObjects: [NSNumber numberWithInt:0], [NSNumber numberWithInt:1], [NSNumber numberWithInt:2], [NSNumber numberWithInt:3], nil];
    
    for (UIButton *mcButton in self.mcButtons)
    {
        if (mcButton == self.MCButtonA)
        {
            [mcButton addTarget:self
                         action:@selector(onSelectCorrectMC:)
               forControlEvents:UIControlEventTouchDown];
        }
        else
        {
            [mcButton addTarget:self
                   action:@selector(onSelectIncorrectMC:)
         forControlEvents:UIControlEventTouchDown];
        }
        mcButton.hidden = YES;
    }

    [self.continueButton addTarget:self
                             action:@selector(preCheckVerbSubmit)
                   forControlEvents:UIControlEventTouchDown];
    
    [self.backButton addTarget:self
                            action:@selector(backToMenu)
                  forControlEvents:UIControlEventTouchDown];
    
    [self.incorrectButton addTarget:self
                 action:@selector(onSelectIncorrectMC:)
       forControlEvents:UIControlEventTouchDown];
    
    [self.correctButton addTarget:self
                 action:@selector(onSelectCorrectMC:)
       forControlEvents:UIControlEventTouchDown];
    
    [self.correctButton.layer setMasksToBounds:YES];
    self.correctButton.layer.borderWidth = 2.0f;
    self.correctButton.layer.borderColor = [UIColor blackColor].CGColor;
    
    [self.incorrectButton.layer setMasksToBounds:YES];
    self.incorrectButton.layer.borderWidth = 2.0f;
    self.incorrectButton.layer.borderColor = [UIColor blackColor].CGColor;
    
    int w = self.view.frame.size.width;
    [self.correctButton setFrame:CGRectMake(((w/2) - self.correctButton.frame.size.width) / 2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
    
    [self.incorrectButton setFrame:CGRectMake((((w/2) - self.correctButton.frame.size.width) / 2) + w/2, self.view.frame.size.height / 1.3, self.correctButton.frame.size.width, self.correctButton.frame.size.height)];
    
    [self.continueButton.layer setMasksToBounds:YES];
    self.continueButton.layer.borderWidth = 6.0f;
    self.continueButton.layer.borderColor = [UIColor whiteColor].CGColor;
    
    self.continueButton.backgroundColor = [UIColor HCBlue];// UIColorFromRGB(0x43609c);
    
    [self.continueButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [self.continueButton setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
    self.continueButton.layer.cornerRadius = 2.0f;
    
    [self.backButton.layer setMasksToBounds:YES];
    //self.backButton.layer.borderWidth = 6.0f;
    //self.backButton.layer.borderColor = [UIColor whiteColor].CGColor;
    //self.backButton.backgroundColor = UIColorFromRGB(0x43609c);
    [self.backButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    [self.backButton setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
    self.backButton.layer.cornerRadius = 2.0f;
    
    [self.continueButton setFrame:CGRectMake(0, screenSize.height - 70, (screenSize.width), 70)];
    [self.backButton setFrame:CGRectMake(6, 6, 33, 30)];
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
    {
        self.menuButton.hidden = YES;
    }
    else
    {
        self.menuButton.hidden = YES;
        /*
        [self.menuButton setFrame:CGRectMake(screenSize.width - 60 - 6, 6, 60, 36.0)];
        [self.menuButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        self.menuButton.layer.borderColor = [UIColor grayColor].CGColor;
        self.menuButton.layer.borderWidth = 2.0f;
        self.menuButton.layer.cornerRadius = 8;
        
        [self.menuButton addTarget:self
                            action:@selector(animatePopUpShow:)
       forControlEvents:UIControlEventTouchDown];
         */
    }
    
    [self.verbModeButton setFrame:CGRectMake(self.view.frame.size.width - 60 - 6, 6, 60, 36.0)];
    [self.verbModeButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    self.verbModeButton.layer.borderColor = [UIColor grayColor].CGColor;
    self.verbModeButton.layer.borderWidth = 2.0f;
    self.verbModeButton.layer.cornerRadius = 8;
    [self.verbModeButton addTarget:self
                        action:@selector(toggleVerbMode:)
              forControlEvents:UIControlEventTouchDown];
    
    [self setLevels];
    
    [self configureView];
    
    UIImage *lifeImage = [UIImage imageNamed:@"Life4X.png"];
    self.life1 = [[UIImageView alloc] initWithImage:lifeImage];
    self.life2 = [[UIImageView alloc] initWithImage:lifeImage];
    self.life3 = [[UIImageView alloc] initWithImage:lifeImage];
    [self.view addSubview:self.life1];
    [self.view addSubview:self.life2];
    [self.view addSubview:self.life3];
    [self.life1 setFrame:CGRectMake(screenSize.width - 26,42,20,20)];
    [self.life2 setFrame:CGRectMake(screenSize.width - 52,42,20,20)];
    [self.life3 setFrame:CGRectMake(screenSize.width - 78,42,20,20)];
    [self.view bringSubviewToFront:self.life1];
    [self.view bringSubviewToFront:self.life2];
    [self.view bringSubviewToFront:self.life3];
    
    if (self.verbQuestionType != HOPLITE_CHALLENGE)
    {
        self.life1.hidden = YES;
        self.life2.hidden = YES;
        self.life3.hidden = YES;
    }
    else
    {
        self.lives = 3;
    }
    [self positionWidgetsToSize:self.view.frame.size];
}


- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    NSLog(@"did rotateeeeee");
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    if ( [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait )
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.width * 1/3) - 50, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.width * 2/3) - 46, (screenSize.height - 240)/2, screenSize.width /2, 240.0)];
    }
    else
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        
        [self.singLabel setFrame:CGRectMake((screenSize.height * 1/3) - 20,50,  screenSize.height - 40, 240.0)];
        [self.pluralLabel setFrame:CGRectMake((screenSize.height * 2/3) - 50,50
                                              ,  screenSize.height - 40, 240.0)];
    }
}

//http://stackoverflow.com/questions/26069874/what-is-the-right-way-to-handle-orientation-changes-in-ios-8
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    NSLog(@"did resizeeee");
    // Code here will execute before the rotation begins.
    // Equivalent to placing it in the deprecated method -[willRotateToInterfaceOrientation:duration:]
    
    [coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext> context) {
        
        // Place code here to perform animations during the rotation.
        // You can pass nil or leave this block empty if not necessary.
        
        [self positionWidgetsToSize:size];
        
    } completion:^(id<UIViewControllerTransitionCoordinatorContext> context) {
        
        // Code here will execute after the rotation has finished.
        // Equivalent to placing it in the deprecated method -[didRotateFromInterfaceOrientation:]
        //NSLog(@"size w: %f, h: %f", size.width, size.height);
    }];
}

-(void) positionWidgetsToSize:(CGSize)size
{
    double f = size.height;
    double w = size.width;
    CGSize fsize = [@"ξφψΑΒ" sizeWithAttributes:@{NSFontAttributeName: self.origForm.font }];
    CGSize fsizeS = [@"ABCD" sizeWithAttributes:@{NSFontAttributeName: self.stemLabel.font }];
    
    [self.origForm setFrame:CGRectMake(self.origForm.frame.origin.x, f/6, self.origForm.frame.size.width, self.origForm.frame.size.height)];
    //[self.changeTo setFrame:CGRectMake(0, f/3.4, self.view.frame.size.width, fsizeS.height + 10)];
    
    //NSLog(@"Changed: %f", f/1.7);
    //self.changedForm.backgroundColor = [UIColor redColor];
    
    [self.life1 setFrame:CGRectMake(size.width - 27,33,20,20)];
    [self.life2 setFrame:CGRectMake(size.width - 53,33,20,20)];
    [self.life3 setFrame:CGRectMake(size.width - 79,33,20,20)];
    [self.gameOverLabel setFrame:CGRectMake(0, 28, size.width - 6, fsizeS.height)];
    
    [self.changeTo setFrame:CGRectMake(0, f/3.4+34, self.view.frame.size.width, fsizeS.height + 10)];
    [self.stemLabel setFrame:CGRectMake(0, f/3.4+34, self.view.frame.size.width, fsizeS.height + 10)];
    [self.textfield setFrame:CGRectMake(10, f/2.1, self.view.frame.size.width - 20, fsize.height + 10)];
    [self.changedForm setFrame:CGRectMake(10, f/1.7, self.view.frame.size.width - 20, fsize.height + 10)];
    
    [self.scoreLabel setFrame:CGRectMake(56, 2,  194, 30)];
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        [self.timeLabel setFrame:CGRectMake(size.width - 200, 2,  194, 30)];
    }
    else
    {
        [self.timeLabel setFrame:CGRectMake(size.width - 200, 6,  194, 30)];
    }
    [self.MFLabel setFrame:CGRectMake(size.width - 120 - 42, 4,  42, 30)];
    
    [self centerLabel:self.origForm withString:self.origForm.text setHeight:NO];
    [self centerLabel:self.changeTo withString:self.changeTo.text setHeight:NO];
    [self centerLabel:self.stemLabel withAttributedString:self.stemLabel.attributedText withSize:size];
    [self centerLabel:self.changedForm withString:self.changedForm.text setHeight:NO];
    
    [self.continueButton setFrame:CGRectMake(0, size.height - 70, (size.width), 70)];
    
    //new testing
    int topmargin = 52;
    int stemHeight = 38;
    int keyboardHeight = 206;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
        keyboardHeight = 206;
    }
    else
    {
        keyboardHeight = 308;
    }
    double heightFactor;
    if (f > 568)
        heightFactor = 1.7;
    else
        heightFactor = 1.1;
    
    int origandtextfieldheight = (self.view.frame.size.height - (topmargin * heightFactor) - keyboardHeight - stemHeight) / 2;
    //[self.changeTo setFrame:CGRectMake(0, f/3.4+34, w, fsizeS.height + 10)];
    
    [self.origForm setFrame:CGRectMake(self.origForm.frame.origin.x, topmargin, self.origForm.frame.size.width, origandtextfieldheight)];
    [self.stemLabel setFrame:CGRectMake(self.stemLabel.frame.origin.x, topmargin + origandtextfieldheight, w, stemHeight)];
    [self.textfield setFrame:CGRectMake(0, topmargin + origandtextfieldheight + stemHeight, w, origandtextfieldheight)];
    [self.changedForm setFrame:CGRectMake(0, topmargin + (origandtextfieldheight * 2) + stemHeight, w, origandtextfieldheight)];
    
    
    //make text in uitextview vertically centered
    CGFloat topCorrect = ([self.textfield bounds].size.height - [self.textfield contentSize].height * [self.textfield zoomScale])/2.0;
    topCorrect = ( topCorrect < 0.0 ? 0.0 : topCorrect );
    //NSLog(@"content: %f, %f, %f", topCorrect, [self.textfield bounds].size.height, [tv contentSize].height);
    [self.textfield setContentInset:UIEdgeInsetsMake(topCorrect,0,0,0)];
    
    
    self.origForm.textAlignment = UIControlContentVerticalAlignmentCenter;
    //fix me need to center text vertically
    //self.textfield.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    self.changedForm.textAlignment = UIControlContentVerticalAlignmentTop;
    /*
    self.origForm.layer.borderWidth = 1.0;
    self.origForm.layer.borderColor = [UIColor redColor].CGColor;
    self.changedForm.layer.borderWidth = 1.0;
    self.changedForm.layer.borderColor = [UIColor redColor].CGColor;
    self.stemLabel.layer.borderWidth = 1.0;
    self.stemLabel.layer.borderColor = [UIColor blueColor].CGColor;
    
    self.textfield.layer.borderWidth = 1.0;
    self.textfield.layer.borderColor = [UIColor greenColor].CGColor;
    */
    
    CGSize lsize = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
    int offset;
    if (lsize.width == 0)
        offset = -8;
    else
        offset = 15;
    CGFloat rvX = (size.width + lsize.width) / 2 + offset;
    //don't let it go off the screen
    if (rvX > self.view.frame.size.width - 26)
        rvX = self.view.frame.size.width - 26;
    
    double yy = self.textfield.frame.origin.y + ((self.textfield.frame.size.height - self.redXView.frame.size.height) /2);

    [self.redXView setFrame:CGRectMake(rvX, yy, self.redXView.frame.size.width,self.redXView.frame.size.height)];
    
    double yy2 = self.textfield.frame.origin.y + ((self.textfield.frame.size.height - self.greenCheckView.frame.size.height) /2);
    [self.greenCheckView setFrame:CGRectMake(rvX, yy2, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Split view

- (void)splitViewController:(UISplitViewController *)splitController willHideViewController:(UIViewController *)viewController withBarButtonItem:(UIBarButtonItem *)barButtonItem forPopoverController:(UIPopoverController *)popoverController
{
    barButtonItem.title = NSLocalizedString(@"Master", @"Master");
    [self.navigationItem setLeftBarButtonItem:barButtonItem animated:YES];
    self.masterPopoverController = popoverController;
}

- (void)splitViewController:(UISplitViewController *)splitController willShowViewController:(UIViewController *)viewController invalidatingBarButtonItem:(UIBarButtonItem *)barButtonItem
{
    // Called when the view is shown again in the split view, invalidating the button and popover controller.
    [self.navigationItem setLeftBarButtonItem:nil animated:YES];
    self.masterPopoverController = nil;
}

@end
