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


UIView *flipContainer;
UIView *frontSideTest;
UIView *backSideTest;
-(void)turnUp
{
    flipContainer = self.view;
    [backSideTest removeFromSuperview];
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.5];
    [UIView setAnimationTransition:UIViewAnimationTransitionFlipFromLeft forView:flipContainer cache:YES];
    [UIView setAnimationDuration:0.5];
    CGAffineTransform transform = CGAffineTransformMakeScale(1, 1);
    flipContainer.transform = transform;
    [UIView commitAnimations];
    [flipContainer addSubview:frontSideTest];
}
-(void)turnDown
{
    flipContainer = self.view;
    [frontSideTest removeFromSuperview];
    [UIView beginAnimations:nil context:NULL];
    [UIView setAnimationDuration:0.5];
    [UIView setAnimationTransition:UIViewAnimationTransitionFlipFromRight forView:flipContainer cache:YES];
    [UIView setAnimationDuration:0.5];
    CGAffineTransform transform = CGAffineTransformMakeScale(1, 1);
    flipContainer.transform = transform;
    [UIView commitAnimations];
    [flipContainer addSubview:backSideTest];
}

-(void)loadAccents
{
    self.stemLabel.text = @"Accents";
    self.stemLabel.hidden = false;
    self.startTime = CACurrentMediaTime();
}

-(void)loadVocabulary
{
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    if (!frontSideTest)
    {
        frontSideTest = [[UIView alloc] initWithFrame:CGRectMake(0, 0, screenSize.width, screenSize.height)];
        
        self.vocabFront = [[UILabel alloc] initWithFrame:CGRectMake(0, (screenSize.height/2)-30, screenSize.width, 60)];
        vocabFront.textAlignment = NSTextAlignmentCenter;
        vocabFront.font = [UIFont fontWithName:self.greekFont size:28.0];
        [frontSideTest addSubview:vocabFront];
        [frontSideTest setBackgroundColor:[UIColor whiteColor]];
    }
    
    if (!backSideTest)
    {
        backSideTest = [[UIView alloc] initWithFrame:CGRectMake(0, 0, screenSize.width, screenSize.height)];
        
        self.vocabBack = [[UILabel alloc] initWithFrame:CGRectMake(0, (screenSize.height/2)-30, screenSize.width, 60)];
        vocabBack.textAlignment = NSTextAlignmentCenter;
        vocabBack.font = [UIFont fontWithName:self.systemFont size:28.0];
        [backSideTest addSubview:vocabBack];
        [backSideTest setBackgroundColor:[UIColor whiteColor]];
        [self.view addSubview:backSideTest];
    }
    
    [vocabFront setText: @""];
    [vocabBack setText: @""];
    
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Vocab" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    
    if ([self.levels count] > 0)
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq IN %@", self.levels];
        //NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq > 0"];
        [request setPredicate:predicate];
    }
    else
    {
        [request setPredicate:NULL];
    }
    
    NSInteger count = [moc countForFetchRequest:request error:&error];
    
    NSLog(@"count: %ld", (long)count);
    
    if (count > 0)
    {
        // Set example predicate and sort orderings...
        NSUInteger offsetStart = 0;
        int randomNumber = arc4random() % count;
        NSUInteger offset = offsetStart + randomNumber;
        [request setFetchOffset:offset];
        
        [request setFetchLimit:1];
        
        NSArray *array = [moc executeFetchRequest:request error:&error];
        
        if (array == nil)
        {
            // Deal with error...
            NSLog(@"Error: Def not found by id: %d.", 1);
            return;
        }
        NSLog(@"here + %lu", (unsigned long)offset);
        [vocabFront setText: [[array lastObject] valueForKey:@"word"]];
        [vocabBack setText: [[array lastObject] valueForKey:@"def"]];
    }
    
    [self turnUp];
    
    //self.stemLabel.text = @"Accents";
    self.stemLabel.hidden = false;
    self.startTime = CACurrentMediaTime();
}

-(void)setLevelArray: (NSMutableArray*)array
{
    NSLog(@"set level array");
    self.buttonStates = array;
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE)
        return;
    
    if (self.front) //then show back
    {
        if (self.cardType == 2) //Principal Parts
        {
            self.stemLabel.hidden = true;
            self.backLabel.hidden = false;
            self.singLabel.hidden = true;
            self.pluralLabel.hidden = true;
        }
        else if (self.cardType == 3) //Endings
        {
            self.stemLabel.hidden = true;
            self.backLabel.hidden = true;
            self.singLabel.hidden = false;
            self.pluralLabel.hidden = false;
            
        }
        else if (self.cardType == 5) //Accents
        {
            
        }
        else if (self.cardType == 1) //Verbs
        {
            //self.backLabel.hidden = false;
            if (self.verbQuestionType != MULTIPLE_CHOICE)
            {
                [self stopTimer];
                
                if (self.animate)
                {
                    //NSLog(@"type: %@", self.changedForm.text);
                    self.changedForm.textAlignment = NSTextAlignmentLeft;
                    [self typeLabel:self.changedForm withString:self.changedForm.text withInterval:self.typeInterval];
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
        else if (self.cardType == 4) //Vocab Training
        {
            [self turnDown];
        }
        self.front = false;
        
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = false;
    }
    else //load new card
    {
        if (self.cardType == 1 && self.verbQuestionType == SELF_PRACTICE)
        {
            return;
        }
        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = true;
    }
}

-(void) loadNext
{
    if (self.cardType == 3)
        [self loadEnding];
    else if (self.cardType == 2)
        [self loadPrincipalPart];
    else if (self.cardType == 5)
        [self loadAccents];
    else if (self.cardType == 1)
        [self loadMorphTraining];
    else if (self.cardType == 4)
        [self loadVocabulary];
    
    [self stopTimer];
    self.timeLabel.hidden = YES;
    
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

/*
-(void)printPPToNSLog
{
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Verbs" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    [request setPredicate:NULL];
    
    NSSortDescriptor *sortDescriptor = [[NSSortDescriptor alloc] initWithKey:@"hq" ascending:YES];
    NSArray *sortDescriptors = @[sortDescriptor];
    
    [request setSortDescriptors:sortDescriptors];
    
    NSArray *array = [moc executeFetchRequest:request error:&error];
    
    if (array == nil)
    {
        // Deal with error...
        NSLog(@"Error: Def not found by id: %d.", 1);
        return;
    }
    
    NSMutableString *s = [[NSMutableString alloc] initWithString:@""];
    
    for(int i = 0; i < [array count]; i++)
    {
        Verbs *v = [array objectAtIndex:i];
        [s appendFormat:@"{ %i, 0, %li, \"%@\", \"%@\", \"%@\", \"%@\", \"%@\", \"%@\" },\n", i, (long)[v.hq integerValue], v.present, v.future, v.aorist, v.perfectactive, v.perfectmid, v.aoristpassive ];
    }
    NSLog(@"%@", s);
}
*/

//works even if only one.
-(NSString*)selectRandomFromCSV:(NSString *)str
{
    NSArray *myArr = [str componentsSeparatedByString:@", "];
    NSUInteger randomIndex = arc4random() % [myArr count];
    
    return [myArr objectAtIndex:randomIndex];
}
/*
-(void)typingLabel:(NSTimer*)theTimer
{
    NSString *theString = [theTimer.userInfo objectForKey:@"string"];
    int currentCount = [[theTimer.userInfo objectForKey:@"currentCount"] intValue];
    UILabel *l = [theTimer.userInfo objectForKey:@"label"];
    
    currentCount++;
    //NSLog(@"%@", [theString substringToIndex:currentCount]);
    
    [theTimer.userInfo setObject:[NSNumber numberWithInt:currentCount] forKey:@"currentCount"];
    
    if (currentCount > theString.length - 1)
    {
        [theTimer invalidate];
    }
    
    [l setText:[theString substringToIndex:currentCount]];
}

- (void)asyncTypingLabel:(NSString*)newText characterDelay:(NSTimeInterval)delay label:(UILabel*)l
{
    [l setText:@""];
    
    for (int i=0; i<newText.length; i++)
    {
        dispatch_async(dispatch_get_main_queue(),
                       ^{
                           [l setText:[NSString stringWithFormat:@"%@%C", l.text, [newText characterAtIndex:i]]];
                       });
        
        [NSThread sleepForTimeInterval:delay];
    }
}
*/
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

//http://stackoverflow.com/questions/11686642/letter-by-letter-animation-for-uilabel
-(void)typeLabel:(UILabel*)l withString:(NSString*)string withInterval:(double)interval
{
    //BOOL async = NO;
    
    if ([string length] < 1)
        return;
    /*
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;

    CGSize size = [string sizeWithAttributes:@{NSFontAttributeName: l.font}];
    
    // Values are fractional -- you should take the ceilf to get equivalent values
    CGSize adjustedSize = CGSizeMake(ceilf(size.width), ceilf(size.height));
    
    [l setFrame: CGRectMake((screenSize.width - adjustedSize.width) / 2, l.frame.origin.y, adjustedSize.width, adjustedSize.height)];
    */
    [self centerLabel:l withString:string];
    
    //l.textAlignment = NSTextAlignmentLeft;
    /*
    if (async)
    {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
                       ^{
                           [self asyncTypingLabel:string characterDelay:interval label:l];
                       });
    }
    else
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setObject:string forKey:@"string"];
        [dict setObject:@0 forKey:@"currentCount"];
        [dict setObject:l forKey:@"label"];
        NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:interval target:self selector:@selector(typingLabel:) userInfo:dict repeats:YES];
        [timer fire];
    }
    else
    { */
        for (int i = 0; i < [string length]; i++)
        {
            dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * i * NSEC_PER_SEC));
            dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
            {
                [l setText:[string substringToIndex:i+1]];
            });
        }
    //}
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
    NSLog(@"mf pressed");
    
    if (self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        if (!self.mfPressed)
        {
            CGFloat halfTime = self.HCTime / 2;
            self.startTime += halfTime;
        
            self.mfPressed = YES;
        }
    }
    else if (self.verbQuestionType == HOPLITE_PRACTICE)
    {
        if (!self.mfPressed)
        {
            CGFloat halfTime = self.HCTime / 2;
            self.startTime += halfTime;
            
            self.mfPressed = YES;
        }
    }
}

-(void)preCheckVerbTimeout
{
    self.timeLabel.text = @"0.00 sec";
    self.timeLabel.hidden = NO;
    [self stopTimer];
    [self checkVerb];
}

-(void)preCheckVerbSubmit
{
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
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    BOOL isCorrect = NO;
    BOOL debug = NO;
    
    if (self.front)
    {
        //if ([self.textfield.text isEqual:self.changedForm.text])
        
        UCS2 test[512];
        UCS2 check[512];
        int testLen = 0;
        int checkLen = 0;
        utf8_to_ucs2_string([self.textfield.text UTF8String], test, &testLen);
        utf8_to_ucs2_string([self.changedForm.text UTF8String], check, &checkLen);
            
        if (compareForms(test, testLen, check, checkLen))
        {
            NSLog(@"correct");
            CGSize size = [self.textfield.text sizeWithAttributes:@{NSFontAttributeName: self.textfield.font}];
            [self.greenCheckView setFrame:CGRectMake((self.view.frame.size.width + size.width) / 2 + 15, self.greenCheckView.frame.origin.y, self.greenCheckView.frame.size.width,self.greenCheckView.frame.size.height)];
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
            [self.redXView setFrame:CGRectMake((self.view.frame.size.width + size.width) / 2 + 15, self.redXView.frame.origin.y, self.redXView.frame.size.width,self.redXView.frame.size.height)];
            self.redXView.hidden = NO;
            
            if (!self.soundDisabled)
            {
                AudioServicesPlaySystemSound(BuzzSound);
            }
            self.textfield.textColor = [UIColor grayColor];
            isCorrect = NO;
        }
        
        if (!isCorrect || debug)
        {
            if (self.animate)
            {
                NSString *temp = self.changedForm.text;
                self.changedForm.text = @"";
                self.changedForm.hidden = NO;
                self.changedForm.textAlignment = NSTextAlignmentLeft;
                self.continueButton.hidden = NO;
                self.backButton.hidden = NO;
                self.continueButton.enabled = NO;
                self.backButton.enabled = NO;
                [self.continueButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
                [self.backButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
                
                dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
                    [self typeLabel:self.changedForm withString:temp withInterval:self.typeInterval];
                    
                    dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC));
                    dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                        self.continueButton.enabled = YES;
                        self.backButton.enabled = YES;
                        [self.continueButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
                        [self.backButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
                    });
                });
            }
            else
            {
                self.changedForm.hidden = NO;
                dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC));
                dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                    self.continueButton.hidden = NO;
                    self.backButton.hidden = NO;
                });
            }
        }
        else
        {
            dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC));
            dispatch_after(popTime2, dispatch_get_main_queue(), ^(void){
                self.continueButton.hidden = NO;
                self.backButton.hidden = NO;
            });
        }
        
        self.front = false;
        self.textfield.enabled = false;
    }
    else
    {
        self.redXView.hidden = YES;
        self.greenCheckView.hidden = YES;
        
        [self loadNext];
        self.textfield.text = @"";
        self.timeLabel.hidden = true;
    }
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
    self.displayLink.frameInterval = 5;
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
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
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

-(void) loadMorphTraining
{
    VerbFormC vf;
    Verb *v;
    int bufferLen = 1024;
    char buffer[bufferLen];

    //v = &verbs[13];
    v = getRandomVerb( self->units, self->numUnits);
    vf.verb = v;
    //vf.verb = &verbs[28];
    //don't use dash for first verb form.
    do
    {
        generateForm(&vf);
        
    } while (!getForm(&vf, buffer, bufferLen, false, false) || !strncmp(buffer, "—", 1));
    
    NSString *distractors = nil;
    NSArray *distractorArr = nil;
    NSString *origForm = nil;
    NSString *newForm = nil;
    NSString *newDescription = nil;
    
    origForm = [NSString stringWithUTF8String: (const char*)buffer];
    origForm = [self selectRandomFromCSV:origForm];
    self.origStr = origForm;
    
    getForm(&vf, buffer, bufferLen, false, true);
    self.origStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    self.origStrDecomposed = [self selectRandomFromCSV:self.origStrDecomposed]; //FIXME, What if not same as orig form
    
    //getAbbrevDescription(&vf, buffer, bufferLen);
    //NSString *origDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    /*
    vf.person = SECOND;
    vf.number = SINGULAR;
    vf.tense = PRESENT;
    vf.voice = PASSIVE;
    vf.mood = INDICATIVE;
    getForm(&vf, buffer, bufferLen, true, false);
     */
    do
    {
        changeFormByDegrees(&vf, 2);
    } while (!getForm(&vf, buffer, bufferLen, true, false) || !strncmp(buffer, "—", 1));
    
    newForm = [NSString stringWithUTF8String: (const char*)buffer];
    self.changedStr = newForm;
    
    getForm(&vf, buffer, bufferLen, true, true);
    self.changedStrDecomposed = [NSString stringWithUTF8String: (const char*)buffer];
    
    getAbbrevDescription(&vf, buffer, bufferLen);
    newDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        //stops pause when updating button text
        //http://stackoverflow.com/questions/18946490/how-to-stop-unwanted-uibutton-animation-on-title-change
        
        getDistractorsForChange(&vf, &vf, 3, buffer);
        distractors = [NSString stringWithUTF8String: (const char*)buffer];
        distractorArr = [distractors componentsSeparatedByString:@"; "];
    }
    
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
    self.MCButtonA.hidden = YES;
    self.MCButtonB.hidden = YES;
    self.MCButtonC.hidden = YES;
    self.MCButtonD.hidden = YES;
    
    if (self.verbQuestionType == SELF_PRACTICE || self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE)
    {
        self.changedForm.text = newForm;
    }
    else if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        //stops pause when updating button text
        //http://stackoverflow.com/questions/18946490/how-to-stop-unwanted-uibutton-animation-on-title-change
        
        //[UIView setAnimationsEnabled:NO];
        
        //newForm = [self selectRandomFromCSV:newForm];
        
        [self.MCButtonA setTitle:[distractorArr objectAtIndex:0] forState:UIControlStateNormal];
        [self.MCButtonB setTitle:[distractorArr objectAtIndex:1] forState:UIControlStateNormal];
        [self.MCButtonC setTitle:[distractorArr objectAtIndex:2] forState:UIControlStateNormal];
        [self.MCButtonD setTitle:[distractorArr objectAtIndex:3] forState:UIControlStateNormal];
        
        //[UIView setAnimationsEnabled:YES]; //re-enables view animation
    }
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    /******* NOW SETUP THE VIEW: static layout which should be done elsewhere ******/
    
    self.expanded = NO;
    self.mfPressed = NO;
    self.textfield.text = @"";
    self.textfield.textColor = [UIColor blackColor];
    self.continueButton.hidden = YES;
    self.backButton.hidden = YES;
    self.redXView.hidden = YES;
    self.greenCheckView.hidden = YES;
    
    self.origForm.backgroundColor = [UIColor whiteColor];
    self.origForm.layer.cornerRadius = 8;
    [self.origForm.layer setMasksToBounds:YES];
    self.changedForm.backgroundColor = [UIColor whiteColor];
    self.changedForm.layer.cornerRadius = 8;
    [self.changedForm.layer setMasksToBounds:YES];
    
    [self.verbModeButton setFrame:CGRectMake(self.view.frame.size.width - 60 - 6, 6, 60, 36.0)];
    
    self.stemLabel.textAlignment = NSTextAlignmentCenter;
    
    /* init stuff */
    
    self.origForm.hidden = NO;
    self.stemLabel.hidden = NO;
    self.backLabel.hidden = YES;
    self.changedForm.hidden = YES;
    self.correctButton.hidden = YES;
    self.incorrectButton.hidden = YES;
    
    if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        NSInteger sidePadding = 9;
        NSInteger verticalPadding = 9;
        NSInteger mcHeight = 58;
        
        NSInteger mcWidth = self.view.frame.size.width - (sidePadding * 2); //was screenSize.width
        NSInteger topStart = self.view.frame.size.height - (4*mcHeight) - (4*verticalPadding);//270;
        double cornerRadius = mcHeight / 2.2;
        float borderWidth = 4.0f;
        UIColor *borderColor = [UIColor blackColor];
        UIColor *textColor = [UIColor blackColor];
        UIColor *backgroundColor = [UIColor whiteColor];
        
        //random order for distractors
        for (NSInteger i = self.mcButtonsOrder.count - 1; i > 0; i--)
        {
            [self.mcButtonsOrder exchangeObjectAtIndex:i withObjectAtIndex:arc4random_uniform(i+1)];
        }
        
        NSInteger i = 0;
        
        for (UIButton *mcButton in self.mcButtons)
        {
            NSInteger n = [[self.mcButtonsOrder objectAtIndex:i] integerValue];
            
            [mcButton.layer setMasksToBounds:YES];
            mcButton.layer.borderWidth = borderWidth;
            mcButton.layer.borderColor = borderColor.CGColor;
            mcButton.layer.cornerRadius = cornerRadius;
            [mcButton setTitleColor:textColor forState:UIControlStateNormal];
            mcButton.backgroundColor = backgroundColor;

            [mcButton setFrame:CGRectMake(sidePadding, topStart + ((mcHeight + verticalPadding) * n), mcWidth, mcHeight)];

            i++;
        }

        self.stemLabel.layer.borderWidth = 0.0;
        [self.stemLabel setFrame:CGRectMake(0, 140, self.view.frame.size.width, 100.0)];
        [self.origForm setFrame:CGRectMake(0, 80, self.view.frame.size.width, 50.0)];
    }
    
    if (!self.animate)
    {
        if (self.verbQuestionType == HOPLITE_PRACTICE)
        {
            [self.origForm setFrame:CGRectMake(0, 74, self.view.frame.size.width, 60.0)];
            [self.changeTo setFrame:CGRectMake(0, 120, self.view.frame.size.width, 60.0)];
            [self.stemLabel setFrame:CGRectMake(0, 150, self.view.frame.size.width, 60.0)];
            [self.changedForm setFrame:CGRectMake(0, 340, self.view.frame.size.width, 60.0)];
            [self.textfield setFrame:CGRectMake(0, 278, self.view.frame.size.width, 60.0)];
            
            self.origForm.text = origForm;
            self.changeTo.text = @"Change to";
            self.stemLabel.text = newDescription;
        }
        else if (self.verbQuestionType == SELF_PRACTICE)
        {
            self.origForm.textAlignment = NSTextAlignmentCenter;
            self.changedForm.textAlignment = NSTextAlignmentCenter;
            [self.stemLabel setFrame:CGRectMake(0, 70, self.view.frame.size.width, 240.0)];
            [self.changedForm setFrame:CGRectMake(0, 200, self.view.frame.size.width, 240.0)];
        }
    }
    else if (self.animate)
    {
        if (self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE || self.verbQuestionType == SELF_PRACTICE || self.verbQuestionType == MULTIPLE_CHOICE)
        {
            double f = self.view.frame.size.height;
            self.origForm.text = @"";
            self.changeTo.text = @"";
            self.stemLabel.text = @"";
            if (self.verbQuestionType == SELF_PRACTICE || self.verbQuestionType == MULTIPLE_CHOICE)
                self.textfield.hidden = YES;
            else
                self.textfield.hidden = NO;
            CGSize size = [@"ξφψΑΒ" sizeWithAttributes:@{NSFontAttributeName: self.origForm.font }];
            CGSize sizeS = [@"ABCD" sizeWithAttributes:@{NSFontAttributeName: self.stemLabel.font }];
            
            [self.origForm setFrame:CGRectMake(0, f/6, self.view.frame.size.width, size.height + 10)];
            [self.changeTo setFrame:CGRectMake(0, f/3.4, self.view.frame.size.width, sizeS.height + 10)];
            [self.stemLabel setFrame:CGRectMake(0, f/3.4+34, self.view.frame.size.width, sizeS.height + 10)];
            [self.textfield setFrame:CGRectMake(10, f/2.1, self.view.frame.size.width - 20, size.height + 10)];
            [self.changedForm setFrame:CGRectMake(10, f/1.7, self.view.frame.size.width - 20, size.height + 10)];
            
            //[self.continueButton setFrame:CGRectMake((screenSize.width - self.continueButton.frame.size.width) / 2, f/1.3, self.continueButton.frame.size.width, self.continueButton.frame.size.height)];
            [self.continueButton setFrame:CGRectMake((screenSize.width / 2) - 2, screenSize.height - 70, (screenSize.width / 2) + 4, 70)];
            [self.backButton setFrame:CGRectMake(-2, screenSize.height - 70, (screenSize.width / 2) + 2, 70)];
            
            
            //self.textfield.layer.borderWidth = 1.0;
            //self.changedForm.layer.borderWidth = 1.0;
            
            self.changedForm.numberOfLines = 0;
            self.changedForm.lineBreakMode = NSLineBreakByWordWrapping;
            self.changeTo.textColor = [UIColor grayColor];
            self.stemLabel.textColor = [UIColor grayColor];
            
            self.origForm.textAlignment = NSTextAlignmentLeft;
            self.changeTo.textAlignment = NSTextAlignmentLeft;
            self.stemLabel.textAlignment = NSTextAlignmentLeft;
            
            
            
            //http://stackoverflow.com/questions/15335649/adding-delay-between-execution-of-two-following-lines
            dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.9 * NSEC_PER_SEC));
            dispatch_after(popTime, dispatch_get_main_queue(), ^(void)
            {
                if (self.animate)
                {
                    [self typeLabel:self.origForm withString:origForm withInterval:self.typeInterval];
                }
                else
                {
                    self.origForm.hidden = NO;
                }
                dispatch_time_t popTime2 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.8 * NSEC_PER_SEC));
                dispatch_after(popTime2, dispatch_get_main_queue(), ^(void)
                {
                    if (self.animate)
                    {
                        [self typeLabel:self.changeTo withString:@"Change to:" withInterval:self.typeInterval];
                    }
                    else
                    {
                        self.changeTo.hidden = NO;
                    }
                    //self.changeTo.text = @"Change to:";
                    //self.changeTo.textAlignment = NSTextAlignmentCenter;

                    dispatch_time_t popTime3 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC));
                    dispatch_after(popTime3, dispatch_get_main_queue(), ^(void)
                    {
                        if (self.animate)
                        {
                            [self typeLabel:self.stemLabel withString:newDescription withInterval:self.typeInterval];
                        }
                        else
                        {
                            self.stemLabel.hidden = NO;
                        }
                        //self.stemLabel.text = newDescription;
                        //self.stemLabel.textAlignment = NSTextAlignmentCenter;
                       
                        
                        dispatch_time_t popTime4 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.7 * NSEC_PER_SEC));
                        dispatch_after(popTime4, dispatch_get_main_queue(), ^(void)
                        {
                            if (self.verbQuestionType == HOPLITE_PRACTICE || self.verbQuestionType == HOPLITE_CHALLENGE )
                            {
                                self.textfield.enabled = YES;
                                [self.textfield becomeFirstResponder];
                            }
                            else if (self.verbQuestionType == MULTIPLE_CHOICE)
                            {
                                self.MCButtonA.hidden = NO;
                                self.MCButtonB.hidden = NO;
                                self.MCButtonC.hidden = NO;
                                self.MCButtonD.hidden = NO;
                            }
                            dispatch_time_t popTime5 = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.4 * NSEC_PER_SEC));
                            dispatch_after(popTime5, dispatch_get_main_queue(), ^(void)
                            {
                                self.timeLabel.hidden = NO;
                                //self.timeLabel.font = [UIFont fontWithName:@"Menlo" size:22.0];
                                self.timeLabel.font = [UIFont fontWithName:@"HelveticaNeue-Light" size:24.0];
                                [self startTimer];
                            });
                        });
                        
                    });
                });
            });
        }
        else
        {
            [self.changedForm setFrame:CGRectMake(6, 340, self.view.frame.size.width - 12, self.changedForm.frame.size.height)];
            
            //for acceleration see this:
            //http://stackoverflow.com/questions/5100811/algorithm-to-control-acceleration-until-a-position-is-reached
            
            //http://stackoverflow.com/questions/21848864/uilabel-animation-up-and-down
            self.origForm.alpha = 1;
            self.origForm.frame = CGRectMake(6, screenBound.size.height, screenBound.size.width - 12, 60);
            
            
            [UIView animateWithDuration:0.3
                                  delay:0
                                options:0//UIViewAnimationOptionCurveEaseIn
                             animations:^{
                                 self.origForm.frame = CGRectMake(self.origForm.frame.origin.x, 80, self.origForm.frame.size.width, self.origForm.frame.size.height); // 200 is considered to be center
                                 self.origForm.alpha = 1;
                             }
                             completion:nil ];
            
            self.stemLabel.frame = CGRectMake(0, screenBound.size.height, screenBound.size.width, 100);
            
            [UIView animateWithDuration:0.3
                                  delay:0.5
                                options:0//UIViewAnimationOptionCurveEaseIn
                             animations:^{
                                 self.stemLabel.frame = CGRectMake(self.stemLabel.frame.origin.x, 170, self.stemLabel.frame.size.width, self.stemLabel.frame.size.height);                             self.stemLabel.alpha = 1;
                             }
                             completion:nil ];
        }
    }

    self.startTime = CACurrentMediaTime();
}

-(void) loadEnding
{
    /*
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Endings" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    
    if ([self.levels count] > 0)
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq IN %@", self.levels];
        //NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq > 0"];
        [request setPredicate:predicate];
    }
    else
    {
        [request setPredicate:NULL];
    }
    
    NSInteger count = [moc countForFetchRequest:request error:&error];
    
    //NSLog(@"count: %ld", (long)count);
    
    // Set example predicate and sort orderings...
    NSUInteger offsetStart = 0;
    int randomNumber = arc4random() % count;
    NSUInteger offset = offsetStart + randomNumber;
    [request setFetchOffset:offset];
    
    [request setFetchLimit:1];
    
    NSArray *array = [moc executeFetchRequest:request error:&error];
    
    if (array == nil)
    {
        // Deal with error...
        NSLog(@"Error: Def not found by id: %d.", 1);
        return;
    }
     
     self.stemLabel.text = [NSString stringWithFormat:@"%@ %@ %@", [[array lastObject] valueForKey:@"tense"], [[array lastObject] valueForKey:@"voice"], [[array lastObject] valueForKey:@"mood"]];
     
     self.singLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[[array lastObject] valueForKey:@"fs"], [[array lastObject] valueForKey:@"ss"], [[array lastObject] valueForKey:@"ts"]];
     
     self.pluralLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[[array lastObject] valueForKey:@"fp"], [[array lastObject] valueForKey:@"sp"], [[array lastObject] valueForKey:@"tp"]];
    */
    
     int bufferLen = 1024;
     char buffer[bufferLen];
     int units[20] = { 1,2,3,4,5,6,7 };
     int numUnits = 7;
    /*
     if ([self.levels count] > 0)
     {
         int i;
         numUnits = 0;
         for (i = 0; i < [self.levels count]; i++)
         {
             units[i] = (int)[[self.levels objectAtIndex:i] integerValue];
             numUnits++;
         }
     }
     */
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


-(void) loadPrincipalPart
{
    /*
    //NSLog(@"typeabc: %ld", [[self.detailItem  valueForKey:@"sort"] integerValue]);
    
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Verbs" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];

    if ([self.levels count] > 0)
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq IN %@", self.levels];
        //NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq > 0"];
        [request setPredicate:predicate];
    }
    else
    {
        [request setPredicate:NULL];
    }
    
    NSInteger count = [moc countForFetchRequest:request error:&error];
    
    NSLog(@"count: %ld", (long)count);
    
    // Set example predicate and sort orderings...
    NSUInteger offsetStart = 0;
    int randomNumber = arc4random() % count;
    NSUInteger offset = offsetStart + randomNumber;
    [request setFetchOffset:offset];
    [request setFetchLimit:1];
    
    NSArray *array = [moc executeFetchRequest:request error:&error];
    
    if (array == nil)
    {
        // Deal with error...
        NSLog(@"Error: Def not found by id: %d.", 1);
        return;
    }
    
    //NSLog(@"num results: %lu, count: %lu", (unsigned long)[array count], (unsigned long)count);
    */
    
    int bufferLen = 1024;
    char buffer[bufferLen];
    long units[20] = { 1,2,3,4,5,6,7,8,9,10,11 };
    int numUnits = 11;
    if ([self.levels count] > 0)
    {
        int i;
        numUnits = 0;
        for (i = 0; i < [self.levels count]; i++)
        {
            units[i] = [[self.levels objectAtIndex:i] integerValue];
            numUnits++;
        }
    }
    
    Verb *v = getRandomVerb(units, numUnits);//&verbs[13];//
    VerbFormC vf;
    vf.verb = v;
    
    generateForm(&vf);
    getForm(&vf, buffer, bufferLen, false, false);
    NSString *frontForm = [NSString stringWithUTF8String: (const char*)buffer];
    frontForm = [self selectRandomFromCSV:frontForm];
    
    getPrincipalParts(v, buffer, bufferLen);
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
    if (1) //one line each
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
    self.stemLabel.text = frontForm;
    //self.backLabel.text = self.backCard;
    self.backLabel.attributedText = attrString;
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    if ( [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait )
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 240)/2, screenSize.width - 40, 240.0)];
    }
    else
    {
        [self.stemLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
        [self.backLabel setFrame:CGRectMake(20, (screenSize.width - 240)/2, screenSize.height - 40, 240.0)];
    }
    
    self.stemLabel.hidden = false;
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
    // Update the user interface for the detail item.
    /*
    if (self.detailItem) {
        self.detailDescriptionLabel.text = [[self.detailItem valueForKey:@"item"] description];
    }
    */
    if (frontSideTest && frontSideTest.superview == self.view)
        [frontSideTest removeFromSuperview];
    
    if (backSideTest && backSideTest.superview == self.view)
        [backSideTest removeFromSuperview];
    
    
    
    self.cardType = [[self.detailItem  valueForKey:@"sort"] integerValue];
    if (!self.cardType)
        self.cardType = 1;
    
    if (1)//self.cardType != 1)
    {
        self.verbModeButton.hidden = YES;
    }
    else
    {
        self.verbModeButton.hidden = NO;
    }
    
    self.backLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.backLabel.textAlignment = NSTextAlignmentCenter;
    self.stemLabel.textAlignment = NSTextAlignmentCenter;
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
    self.backLabel.font = [UIFont fontWithName:self.systemFont size:self.fontSize];
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
    
    [self.timeLabel setFrame:CGRectMake(12, 6,  140, 30)];
    self.timeLabel.textAlignment = NSTextAlignmentLeft;
    
    [self loadNext];
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
        self->numUnits = 0;
        for (i = 0; i < [self.levels count]; i++)
        {
            self->units[i] = (int)[[self.levels objectAtIndex:i] integerValue];
            self->numUnits++;
        }
    }
    else //default if nothing selected
    {
        
        self->units[0] = 1;
        self->numUnits = 1;
    }
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

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.typeInterval = 0.023;
    
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
    
    self.cardType = 1;
    

    
    self.systemFont = @"HelveticaNeue-Light";
    self.greekFont = @"NewAthenaUnicode";
    self.fontSize = 26.0;
    
    self.popupShown = FALSE;
    self.popup = [[PopUp alloc] initWithFrame:CGRectMake (0, [UIScreen mainScreen].bounds.size.height + 200, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height)];
    [self.view addSubview:self.popup];
    
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
    self.backButton.hidden = YES;
    [self.backButton setTitle:@"Menu" forState:UIControlStateNormal];
    
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
    self.continueButton.layer.borderWidth = 2.0f;
    self.continueButton.layer.borderColor = [UIColor blackColor].CGColor;
    [self.backButton.layer setMasksToBounds:YES];
    self.backButton.layer.borderWidth = 2.0f;
    self.backButton.layer.borderColor = [UIColor blackColor].CGColor;
    
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
