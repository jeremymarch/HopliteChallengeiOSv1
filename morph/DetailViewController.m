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
        self.timeLabel.hidden = false;
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
        self.timeLabel.hidden = true;
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

-(void)centerLabel:(UILabel*)l withString:(NSString*)string
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
    
    [l setFrame: CGRectMake((screenSize.width - adjustedSize.width) / 2, l.frame.origin.y, adjustedSize.width, adjustedSize.height)];
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
    
    [l setFrame: CGRectMake((size.width - adjustedSize.width) / 2, l.frame.origin.y, adjustedSize.width, adjustedSize.height)];
}

//http://stackoverflow.com/questions/11686642/letter-by-letter-animation-for-uilabel

//for time envelopes see:
//http://stackoverflow.com/questions/5161465/how-to-create-custom-easing-function-with-core-animation
//http://stackoverflow.com/questions/14961581/cadisplaylink-running-lower-frame-rate-on-ios5-1
//http://stackoverflow.com/questions/29938707/animate-a-uiview-using-cadisplaylink-combined-with-camediatimingfunction-to
//http://netcetera.org/camtf-playground.html

-(void)typeLabel:(UILabel*)l withString:(NSString*)string withInterval:(double)interval completion:(void (^)(void))done
{
    if ([string length] < 1)
    {
        l.text = @"";
        return;
    }

    [self centerLabel:l withString:string];
    
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
    [self.navigationController popViewControllerAnimated:NO];
}

-(void)multipleFormsPressed
{
    self.MFLabel.hidden = NO;
    self.mfPressed = YES;
    
    //if there are not multiple forms for the verb, mark it incorrect
    if ([self.changedForm.text rangeOfString:@","].location == NSNotFound)
    {
        //this will mark form incorrect
        [self preCheckVerbSubmit];
        
        return;
    }
    
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        if (!self.mfPressed)
        {
            CGFloat halfTime = self.HCTime / 2;
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
    
    if (self.front)
    {
        UCS2 givenForm[512];
        UCS2 expectedForm[512];
        int givenLen = 0;
        int expectedLen = 0;

        utf8_to_ucs2_string([self.textfield.text UTF8String], givenForm, &givenLen);
        utf8_to_ucs2_string([self.changedForm.text UTF8String], expectedForm, &expectedLen);
        
        if (compareFormsCheckMFRecordResult(expectedForm, expectedLen, givenForm, givenLen, self.mfPressed))
        {
            NSLog(@"correct");
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            CGFloat gvX = (self.view.frame.size.width + size.width) / 2 + 15;
            //don't let it go off the screen
            if (gvX > self.view.frame.size.width - 26)
                gvX = self.view.frame.size.width - 26;
            [self.greenCheckView setFrame:CGRectMake(gvX, self.greenCheckView.frame.origin.y, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
            
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
            self.redXView.hidden = NO;
            
            if (!self.soundDisabled)
            {
                AudioServicesPlaySystemSound(BuzzSound);
            }
            self.textfield.textColor = [UIColor grayColor];
            isCorrect = NO;
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
                    [self typeLabel:self.changedForm withString:temp withInterval:self.typeInterval completion:nil];
                    
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
                            self.timeLabel.hidden = true;
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
                        self.timeLabel.hidden = true;
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
                [self centerLabel:self.changedForm withString:self.changedForm.text];
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
                                                     self.timeLabel.hidden = YES;
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
        self.textfield.enabled = false;
    }
    else //if back
    {
        //[self cleanUp:!(self.useNewAnimation && verbSeq < vsOptions.repsPerVerb)];
        [self loadNext3];
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
    
    int type = nextVerbSeq(&self->verbSeq, &self->vf1, &self->vf2, &self->vsOptions);
    
    if (type == VERB_SEQ_PP)
    {
        NSLog(@"principal parts!");
        self.cardType = CARD_PRINCIPAL_PARTS;
        [self loadPrincipalPart:&self->vf2];
        return;
    }
    else if (type == VERB_SEQ_CHANGE || type == VERB_SEQ_CHANGE_NEW)
    {
        if (newGame)
            [self loadMorphTraining];
        else
            [self cleanUp:(type == VERB_SEQ_CHANGE_NEW)];
    }
}

-(void)cleanUp: (Boolean)reset /* onComplete:(void (^)(void))onComplete */
{
    NSLog(@"cleanup");
    //this means it was correct
    if (self.changedForm.hidden == YES)
    {
        self.changedForm.text = self.textfield.text;
        self.changedForm.frame = CGRectMake(self.textfield.frame.origin.x, self.textfield.frame.origin.y + 5, self.textfield.frame.size.width, self.textfield.frame.size.height);
        [self centerLabel:self.changedForm withString:self.changedForm.text];
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
                                             [self.changedForm setFrame:CGRectMake(self.changedForm.frame.origin.x, self.origForm.frame.origin.y, self.changedForm.frame.size.width, self.changedForm.frame.size.height)
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
    self.timeLabel.hidden = true;
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
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        elapsedTime = self.HCTime - (CACurrentMediaTime() - self.startTime);
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
        elapsedTime = CACurrentMediaTime() - self.startTime;
        if (self.limitElapsedTime && elapsedTime > self.elapsedTimeLimit)
        {
            self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", self.elapsedTimeLimit];
            [self stopTimer];
            //[self preCheckVerbTimeout];
        }
        else
        {
            self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
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
                  value:[UIFont fontWithName:@"HelveticaNeue-Bold" size:26.0]
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
    
    VerbFormC *vf1 = &self->vf1;
    VerbFormC *vf2 = &self->vf2;
    
    int bufferLen = 1024;
    char buffer[bufferLen];

    //int type = nextVerbSeq(&self->verbSeq, &vf1, &vf2, &self->vsOptions);
    int type = VERB_SEQ_CHANGE_NEW;
    if (type == VERB_SEQ_PP)
    {
        NSLog(@"principal parts!");
        self.cardType = CARD_PRINCIPAL_PARTS;
        [self loadPrincipalPart:vf2];
        return;
    }
    
    getForm(vf1, buffer, bufferLen, false, false);
    
    NSString *origForm = nil;
    NSString *newForm = nil;
    NSString *newDescription = nil;
    
    self.lemma = [NSString stringWithUTF8String: (const char*)vf1->verb->present];
    if (self->verbSeq == 1 && self->vsOptions.startOnFirstSing)
    {
        //use lemma rather than a possibly contracted form
        origForm = [NSString stringWithUTF8String: (const char*)vf1->verb->present];
    }
    else
    {
        origForm = [NSString stringWithUTF8String: (const char*)buffer];
    }
    origForm = [self selectRandomFromCSV:origForm];
    self.origStr = origForm;
    
    getForm(vf1, buffer, bufferLen, false, true);
    self.origStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    self.origStrDecomposed = [self selectRandomFromCSV:self.origStrDecomposed]; //FIXME, What if not same as orig form
    
    getAbbrevDescription(vf1, buffer, bufferLen);
    NSString *origDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    getForm(vf2, buffer, bufferLen, false, false);
    newForm = [NSString stringWithUTF8String: (const char*)buffer];
    self.changedStr = newForm;
    
    getForm(vf2, buffer, bufferLen, true, true);
    self.changedStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    
    getAbbrevDescription(vf2, buffer, bufferLen);
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
        dispatchAfter( 0.8, ^(void)
                      {
                          [self typeLabel:self.origForm withString:origForm withInterval:self.typeInterval completion:nil];
                          
                          dispatchAfter( 1.0, ^(void)
                                        {
                                            //[self typeLabel:self.stemLabel withString:newDescription withInterval:self.typeInterval];
                                            self.stemLabel.attributedText = nil;
                                            [self typeAttLabel:self.stemLabel withString: attDesc withInterval:self.typeInterval];
                                            
                                            dispatchAfter( 0.7, ^(void)
                                                          {
                                                              self.textfield.text = @"";
                                                              self.textfield.enabled = YES;
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
                                            self.textfield.enabled = YES;
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
    self.timeLabel.hidden = true;
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
    [self centerLabel:self.origForm withString:frontForm];
    [self typeLabel:self.origForm withString:frontForm withInterval:self.typeInterval completion:nil];
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

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
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
    
    UIFont *greekFont = [UIFont fontWithName:self.greekFont size:36.0];
    
    self.origForm.font = greekFont;
    self.changedForm.font = greekFont;
    
    [self.MCButtonA.titleLabel setFont: greekFont];
    [self.MCButtonB.titleLabel setFont: greekFont];
    [self.MCButtonC.titleLabel setFont: greekFont];
    [self.MCButtonD.titleLabel setFont: greekFont];
    
    self.stemLabel.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
    self.changeTo.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
    self.backLabel.font = [UIFont fontWithName:self.greekFont size:36.0];
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
        self.timeLabel.hidden = false;
    }
    else
    {
        if (self.verbQuestionType == SELF_PRACTICE)
        {
            AudioServicesPlaySystemSound(BuzzSound);
        }

        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = true;
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
        self.timeLabel.hidden = false;
    }
    else
    {
        if (self.verbQuestionType == SELF_PRACTICE)
        {
            AudioServicesPlaySystemSound(DingSound);
        }
        
        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = true;
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
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationNone];
    [self.navigationController setNavigationBarHidden:YES animated:YES];
}

- (void) animatePopUpShow:(id)sender
{
    [self.textfield resignFirstResponder];
    
    if (self.popupShown)
    {
        [self setLevels];
        [self setMode];
        
        [UIView animateWithDuration:0.3 delay:0.0 options:0
                         animations:^{
                             self.popup.frame = CGRectMake(0,[UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
                             self.navigationItem.rightBarButtonItem.title = @"Units";
                         }
                         completion:nil];
        [self.view bringSubviewToFront:self.popup];
        self.popupShown = FALSE;
        [self loadNext];
    }
    else
    {
        [self stopTimer];
        self.timeLabel.hidden = YES;
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
        if (!self.expanded)
        {
            if (self.changedForm.hidden == YES)
            {
                //self.changedForm.frame = self.textfield.frame;
                //self.changedForm.hidden = NO;
                //self.textfield.hidden = YES;
                self.textfield.text = self.changedStrDecomposed;
                
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
                self.changedForm.text = self.changedStrDecomposed;
                [self centerLabel:self.changedForm withString:self.changedStrDecomposed];
            }
            self.expanded = YES;
            self.origForm.text = self.origStrDecomposed;
            [self centerLabel:self.origForm withString:self.origStrDecomposed];
        }
    }
    else if (pinch.velocity < -thresholdVelocity)
    {
        if (self.expanded)
        {
            if (self.changedForm.hidden == YES)
            {
                self.textfield.text = self.changedStr;
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
                self.changedForm.text = self.changedStr;
                [self centerLabel:self.changedForm withString:self.changedStr];
            }
            self.expanded = NO;
            self.origForm.text = self.origStr;
            [self centerLabel:self.origForm withString:self.origStr];
        }
    }
}

- (NSString *)writeablePathForFile:(NSString*)fileName
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:fileName];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
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
    
    if ([defaults boolForKey:@"DisableSound"] == YES)
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

    self.systemFont = @"HelveticaNeue-Light";
    self.greekFont = @"NewAthenaUnicode";
    self.fontSize = 28.0;
    
    self.popupShown = FALSE;
    self.popup = [[PopUp alloc] initWithFrame:CGRectMake (0, [UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height)];
    [self.view addSubview:self.popup];
    
    //self.timeLabel.font = [UIFont fontWithName:@"Menlo" size:22.0];
    self.timeLabel.font = [UIFont fontWithName:@"HelveticaNeue-Light" size:24.0];
    self.MFLabel.font = [UIFont fontWithName:@"HelveticaNeue" size:24.0];
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
    
    self.textfield.font = [UIFont fontWithName:self.greekFont size:36.0];
    self.textfield.frame = CGRectMake(10, self.textfield.frame.origin.y, screenSize.width - 20, 54.0);
    self.textfield.hidden = NO;
    [self.textfield setBorderStyle:UITextBorderStyleNone];
    //self.textfield.textAlignment =
    
    //self.textfield.contentVerticalAlignment;
    //self.textfield.contentHorizontalAlignment = UIControlContentHorizontalAlignment.Center;
    self.textfield.textAlignment = NSTextAlignmentCenter;
    self.textfield.adjustsFontSizeToFitWidth = true;
    
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
    
    UIColor *blueColor = [UIColor colorWithRed:(0/255.0) green:(122/255.0) blue:(255/255.0) alpha:1.0];
    self.continueButton.backgroundColor = blueColor;// UIColorFromRGB(0x43609c);
    
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
        
        [self.menuButton setFrame:CGRectMake(screenSize.width - 60 - 6, 6, 60, 36.0)];
        [self.menuButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        self.menuButton.layer.borderColor = [UIColor grayColor].CGColor;
        self.menuButton.layer.borderWidth = 2.0f;
        self.menuButton.layer.cornerRadius = 8;
        [self.menuButton addTarget:self
                            action:@selector(animatePopUpShow:)
       forControlEvents:UIControlEventTouchDown];
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
}

//http://stackoverflow.com/questions/26069874/what-is-the-right-way-to-handle-orientation-changes-in-ios-8
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    
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
    CGSize fsize = [@"ξφψΑΒ" sizeWithAttributes:@{NSFontAttributeName: self.origForm.font }];
    CGSize fsizeS = [@"ABCD" sizeWithAttributes:@{NSFontAttributeName: self.stemLabel.font }];
    
    [self.origForm setFrame:CGRectMake(0, f/6, self.view.frame.size.width, fsize.height + 10)];
    //[self.changeTo setFrame:CGRectMake(0, f/3.4, self.view.frame.size.width, fsizeS.height + 10)];
    [self.changeTo setFrame:CGRectMake(0, f/3.4+34, self.view.frame.size.width, fsizeS.height + 10)];
    [self.stemLabel setFrame:CGRectMake(0, f/3.4+34, self.view.frame.size.width, fsizeS.height + 10)];
    [self.textfield setFrame:CGRectMake(10, f/2.1, self.view.frame.size.width - 20, fsize.height + 10)];
    [self.changedForm setFrame:CGRectMake(10, f/1.7, self.view.frame.size.width - 20, fsize.height + 10)];
    
    [self.timeLabel setFrame:CGRectMake(size.width - 200, 6,  194, 30)];
    [self.MFLabel setFrame:CGRectMake(size.width - 120 - 42, 6,  42, 30)];
    
    [self centerLabel:self.origForm withString:self.origForm.text ];
    [self centerLabel:self.changeTo withString:self.changeTo.text ];
    [self centerLabel:self.stemLabel withAttributedString:self.stemLabel.attributedText withSize:size];
    [self centerLabel:self.changedForm withString:self.changedForm.text ];
    
    [self.continueButton setFrame:CGRectMake(0, size.height - 70, (size.width), 70)];
    
    CGSize lsize = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
    int offset;
    if (lsize.width == 0)
        offset = -8;
    else
        offset = 15;
    CGFloat rvX = (size.width + size.width) / 2 + offset;
    //don't let it go off the screen
    if (rvX > self.view.frame.size.width - 26)
        rvX = self.view.frame.size.width - 26;
    [self.redXView setFrame:CGRectMake(rvX, self.redXView.frame.origin.y, self.redXView.frame.size.width,self.redXView.frame.size.height)];
    
    
    [self.greenCheckView setFrame:CGRectMake((size.width + lsize.width) / 2 + offset, self.textfield.frame.origin.y + 9, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
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
