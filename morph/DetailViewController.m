//
//  DetailViewController.m
//  morph
//
//  Created by Jeremy on 1/27/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import "AppDelegate.h"
#import "DetailViewController.h"
#import "VerbForm.h"
#import "libmorph.h"
#import "GreekForms.h"
#import <AudioToolbox/AudioToolbox.h>


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
//...
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
    self.buttonStates = array;
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
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
                self.changedForm.hidden = NO;
            }
            if (self.verbQuestionType == PRACTICE)
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
        if (self.cardType == 1 && self.verbQuestionType == PRACTICE)
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

-(void) loadMorphTraining
{
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    [self.verbModeButton setFrame:CGRectMake(self.view.frame.size.width - 60 - 6, 6, 60, 36.0)];
    
    //[self printPPToNSLog];
    self.stemLabel.hidden = NO;
    self.backLabel.hidden = YES;
    self.changedForm.hidden = YES;
    
    self.correctButton.hidden = YES;
    self.incorrectButton.hidden = YES;

    if (self.verbQuestionType == PRACTICE)
    {
        [self.correctButton.layer setMasksToBounds:YES];
        self.correctButton.layer.borderWidth = 2.0f;
        self.correctButton.layer.borderColor = [UIColor blackColor].CGColor;
        
        [self.incorrectButton.layer setMasksToBounds:YES];
        self.incorrectButton.layer.borderWidth = 2.0f;
        self.incorrectButton.layer.borderColor = [UIColor blackColor].CGColor;
        
        self.MCButtonA.hidden = YES;
        self.MCButtonB.hidden = YES;
        self.MCButtonC.hidden = YES;
        self.MCButtonD.hidden = YES;
        [self.stemLabel setFrame:CGRectMake(0, (screenSize.height - 240)/2, self.view.frame.size.width, 240.0)];
    }
    else if (self.verbQuestionType == MULTIPLE_CHOICE)
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

    //this should be done elsewhere
    int bufferLen = 1024;
    char buffer[bufferLen];
    NSInteger units[20] = { 1,2,3,4,5,6,7,8,9,10,11 };
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
    
    VerbFormC vf;
    Verb *v = getRandomVerb(units, numUnits);//&verbs[13];//
    vf.verb = v;
    
    //don't use dash for first verb form.
    do
    {
        generateForm(&vf);
        getForm(&vf, buffer);
    } while (!strncmp(buffer, "—", 1));
    
    NSString *origForm = [NSString stringWithUTF8String: (const char*)buffer];
    origForm = [self selectRandomFromCSV:origForm];

    getAbbrevDescription(&vf, buffer, bufferLen);
    //NSString *origDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    changeFormByDegrees(&vf, 2);
    getForm(&vf, buffer);
    NSString *newForm = [NSString stringWithUTF8String: (const char*)buffer];
    getAbbrevDescription(&vf, buffer, bufferLen);
    NSString *newDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        //stops pause when updating button text
        //http://stackoverflow.com/questions/18946490/how-to-stop-unwanted-uibutton-animation-on-title-change
        
        getDistractorsForChange(&vf, &vf, 3, buffer);
        NSString *distractors = [NSString stringWithUTF8String: (const char*)buffer];
        NSArray *distractorArr = [distractors componentsSeparatedByString:@"; "];
        
        [UIView setAnimationsEnabled:NO];
        
        newForm = [self selectRandomFromCSV:newForm];
        [self.MCButtonA setTitle:[distractorArr objectAtIndex:0] forState:UIControlStateNormal];
        [self.MCButtonA setTitle:[distractorArr objectAtIndex:0] forState:UIControlStateSelected];
        
        [self.MCButtonB setTitle:[distractorArr objectAtIndex:1] forState:UIControlStateNormal];
        [self.MCButtonB setTitle:[distractorArr objectAtIndex:1] forState:UIControlStateSelected];
        
        [self.MCButtonC setTitle:[distractorArr objectAtIndex:2] forState:UIControlStateNormal];
        [self.MCButtonC setTitle:[distractorArr objectAtIndex:2] forState:UIControlStateSelected];
        
        [self.MCButtonD setTitle:[distractorArr objectAtIndex:3] forState:UIControlStateNormal];
        [self.MCButtonD setTitle:[distractorArr objectAtIndex:3] forState:UIControlStateSelected];
        
        self.MCButtonA.hidden = NO;
        self.MCButtonB.hidden = NO;
        self.MCButtonC.hidden = NO;
        self.MCButtonD.hidden = NO;
        
        [UIView setAnimationsEnabled:NO]; //re-enables view animation
    }
    
    //self.stemLabel.text = [NSString stringWithFormat:@"Change\n\n(%@)\n\n %@\n\nto\n\n%@\n\n%@", origDescription, origForm, newDescription, newForm];
    self.origForm.text = origForm;
    self.origForm.hidden = NO;
    self.stemLabel.text = [NSString stringWithFormat:@"to\n\n%@", newDescription];
    
    if (self.verbQuestionType == PRACTICE)
    {
        //[self.stemLabel setCenter:self.view.center];
        [self.stemLabel setFrame:CGRectMake(0, 70, self.view.frame.size.width, 240.0)];
        self.stemLabel.textAlignment = NSTextAlignmentCenter;
        //[self.stemLabel sizeToFit];
        [self.changedForm setFrame:CGRectMake(0, 200, self.view.frame.size.width, 240.0)];
        self.changedForm.text = newForm;
    }
    self.stemLabel.hidden = NO;
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
    
    [self.stemLabel sizeToFit];
    [self.backLabel sizeToFit];
    [self.singLabel sizeToFit];
    [self.pluralLabel sizeToFit];
    
    [self.stemLabel setCenter:self.view.center];
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
    getForm(&vf, buffer);
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
    
    [self.stemLabel sizeToFit];
    [self.backLabel sizeToFit];
    
    [self.stemLabel setCenter:self.view.center];
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
    
    
    self.cardType = 1;
    self.cardType = [[self.detailItem  valueForKey:@"sort"] integerValue];
    
    if (self.cardType != 1)
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
    
    UIFont *greekFont = [UIFont fontWithName:self.greekFont size:30.0];
    
    self.origForm.font = greekFont;
    self.changedForm.font = greekFont;
    
    [self.MCButtonA.titleLabel setFont: greekFont];
    [self.MCButtonB.titleLabel setFont: greekFont];
    [self.MCButtonC.titleLabel setFont: greekFont];
    [self.MCButtonD.titleLabel setFont: greekFont];
    
    self.stemLabel.font = [UIFont fontWithName:self.systemFont size:26.0];
    self.backLabel.font = [UIFont fontWithName:self.systemFont size:26.0];
    self.singLabel.font = [UIFont fontWithName:self.greekFont size:26.0];
    self.pluralLabel.font = [UIFont fontWithName:self.greekFont size:26.0];
    
    
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
    
    [self.timeLabel setFrame:CGRectMake(self.view.frame.size.width - 90 - 60 - 6 - 6, 6
                                          ,  90, 30)];
    
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
        if (self.verbQuestionType == PRACTICE)
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
        if (self.verbQuestionType == PRACTICE)
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
    [self.navigationController setNavigationBarHidden:NO];
}

- (void)toggleVerbMode :(UIButton *)sender
{
    if (self.verbQuestionType == PRACTICE)
    {
        self.verbQuestionType = MULTIPLE_CHOICE;
        [sender setTitle:@"MC" forState:UIControlStateNormal];
    }
    else if (self.verbQuestionType == MULTIPLE_CHOICE)
    {
        self.verbQuestionType = PRACTICE;
        [sender setTitle:@"Self" forState:UIControlStateNormal];
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
    [self.navigationController setNavigationBarHidden:NO animated:YES];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.systemFont = @"HelveticaNeue";
    self.greekFont = @"NewAthenaUnicode";
    
    /*
    //if (!self.keyboard)
    //{
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
        self.keyboard = [[Keyboard alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 172.0) lang:1];
    }
    else
    {
        self.keyboard = [[Keyboard alloc] initWithFrame:CGRectMake(0.0, 0.0,  1024.0, 266.0) lang:1];
    }
    //}
    */
    //[self.textfield setInputView: self.keyboard];
    self.textfield.hidden = YES;
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    [[UIApplication sharedApplication] setStatusBarHidden:YES
                                            withAnimation:NO];
    [self.navigationController setNavigationBarHidden:YES animated:NO];
    self.wantsFullScreenLayout = YES;
    self.view.frame = [[UIScreen mainScreen] bounds];
    self.view.backgroundColor = [UIColor whiteColor];
    
    self.backgroundOrBorder = YES;

    NSString *dingPath = [[NSBundle mainBundle]
                            pathForResource:@"Ding" ofType:@"wav"];
    NSURL *dingURL = [NSURL fileURLWithPath:dingPath];
    AudioServicesCreateSystemSoundID((__bridge CFURLRef)dingURL, &DingSound);
    
    NSString *buzzPath = [[NSBundle mainBundle]
                          pathForResource:@"Buzz02" ofType:@"wav"];
    NSURL *buzzURL = [NSURL fileURLWithPath:buzzPath];
    AudioServicesCreateSystemSoundID((__bridge CFURLRef)buzzURL, &BuzzSound);
    
	// Do any additional setup after loading the view, typically from a nib.
    
    self.verbQuestionType = PRACTICE;  //MULTIPLE_CHOICE;//
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

    [self.incorrectButton addTarget:self
                 action:@selector(onSelectIncorrectMC:)
       forControlEvents:UIControlEventTouchDown];
    
    [self.correctButton addTarget:self
                 action:@selector(onSelectCorrectMC:)
       forControlEvents:UIControlEventTouchDown];
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
    {
        self.menuButton.hidden = YES;
    }
    else
    {
        [self.menuButton setFrame:CGRectMake(6, 6, 60, 36.0)];
        [self.menuButton setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        self.menuButton.layer.borderColor = [UIColor grayColor].CGColor;
        self.menuButton.layer.borderWidth = 2.0f;
        self.menuButton.layer.cornerRadius = 8;
        [self.menuButton addTarget:self
                            action:@selector(returnToRoot:)
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
    
    if (self.levels)
        [self.levels removeAllObjects];
    
    for (int i = 0; i < [self.buttonStates count]; i++)
    {
        if ([[self.buttonStates objectAtIndex:i] boolValue] == YES)
        {
            [self.levels addObject:[NSNumber numberWithInt:(i+1)]];
        }
    }
    
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
