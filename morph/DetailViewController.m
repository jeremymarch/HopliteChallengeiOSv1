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

Verb verbs[];

@interface DetailViewController ()
@property (strong, nonatomic) UIPopoverController *masterPopoverController;
- (void)configureView;
@end

@implementation DetailViewController

#pragma mark - Managing the detail item

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
        else if (self.cardType == 1) //Endings
        {
            self.stemLabel.hidden = true;
            self.backLabel.hidden = true;
            self.singLabel.hidden = false;
            self.pluralLabel.hidden = false;
            
        }
        else if (self.cardType == 3) //Accents
        {
            
        }
        else if (self.cardType == 4) //Morph Training
        {
            self.backLabel.hidden = false;
        }
        self.front = false;
        
        CFTimeInterval elapsedTime = CACurrentMediaTime() - self.startTime;
        self.timeLabel.text = [NSString stringWithFormat:@"%.02f sec", elapsedTime];
        self.timeLabel.hidden = false;
    }
    else //load new card
    {
        [self loadNext];
        self.front = true;
        self.timeLabel.hidden = true;
    }
}

-(void) loadNext
{
    if (self.cardType == 2)
        [self loadPrincipalPart];
    else if (self.cardType == 1)
        [self loadEnding];
    else if (self.cardType == 4)
        [self loadMorphTraining];
    else if (self.cardType == 3)
        [self loadAccents];
    
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

-(void) loadMorphTraining
{
    //[self printPPToNSLog];
    self.stemLabel.hidden = false;
    self.backLabel.hidden = true;
    /*
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Verbs" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    
    if ([self.levels count] > 0)
    {
        //NSLog(@"%@", self.levels);
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
    */
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    [self.stemLabel setFrame:CGRectMake(0, 0, screenSize.width, screenSize.height)];

    int bufferLen = 1024;
    char buffer[bufferLen];

    VerbFormC vf;
    
    Verb *v = getRandomVerb();//&verbs[13];//
    vf.verb = v;
    
    generateForm(&vf);
    getForm(&vf, buffer);
    
    //NSLog(@"Verb: %s %i %i %i %i %i", v->present, vf.person, vf.number, vf.tense, vf. voice, vf.mood);
    
    NSString *origForm = [NSString stringWithUTF8String: (const char*)buffer];
/*
    NSInteger i = [origForm rangeOfString:@","].location;
    if (i != NSNotFound)
    {
        origForm = [origForm substringToIndex:[origForm rangeOfString:@","].location];
    }
    i = [origForm rangeOfString:@" "].location;
    if (i != NSNotFound)
    {
        origForm = [origForm substringToIndex:[origForm rangeOfString:@" "].location];
    }
*/
    getAbbrevDescription(&vf, buffer, bufferLen);
    //NSString *origDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    changeFormByDegrees(&vf, 2);
    
    getForm(&vf, buffer);
    NSString *newForm = [NSString stringWithUTF8String: (const char*)buffer];
    
    getAbbrevDescription(&vf, buffer, bufferLen);
    NSString *newDescription = [NSString stringWithUTF8String: (const char*)buffer];
    
    //self.stemLabel.text = [NSString stringWithFormat:@"Change\n\n(%@)\n\n %@\n\nto\n\n%@\n\n%@", origDescription, origForm, newDescription, newForm];
    self.stemLabel.text = [NSString stringWithFormat:@"%@\n\nto\n\n%@\n\n", origForm, newDescription];
    
    [self.backLabel setFrame:CGRectMake(20, (screenSize.height - 20)/2, screenSize.width - 40, 240.0)];
    self.backLabel.text = newForm;
    
    [self.stemLabel sizeToFit];
    [self.stemLabel setCenter:self.view.center];
    
    self.stemLabel.hidden = false;
    self.startTime = CACurrentMediaTime();
}

-(void) loadEnding
{
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
    
    self.stemLabel.text = [NSString stringWithFormat:@"%@ %@ %@", [[array lastObject] valueForKey:@"tense"], [[array lastObject] valueForKey:@"voice"], [[array lastObject] valueForKey:@"mood"]];
    
    self.singLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[[array lastObject] valueForKey:@"fs"], [[array lastObject] valueForKey:@"ss"], [[array lastObject] valueForKey:@"ts"]];
    
    self.pluralLabel.text = [NSString stringWithFormat:@"%@\n%@\n%@",[[array lastObject] valueForKey:@"fp"], [[array lastObject] valueForKey:@"sp"], [[array lastObject] valueForKey:@"tp"]];
    
    CGRect screenBound = [[UIScreen mainScreen] bounds];
    CGSize screenSize = screenBound.size;
    
    CGSize sFS = [[[array lastObject] valueForKey:@"fs"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
    CGSize sSS = [[[array lastObject] valueForKey:@"ss"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
    CGSize sTS = [[[array lastObject] valueForKey:@"ts"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
    CGSize sFP = [[[array lastObject] valueForKey:@"fp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
    CGSize sSP = [[[array lastObject] valueForKey:@"sp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];
    CGSize sTP = [[[array lastObject] valueForKey:@"tp"] sizeWithAttributes:@{ NSFontAttributeName : [UIFont fontWithName:@"HelveticaNeue" size:26.0] }];

    
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
    
    NSLog(@"hq: %@", [[array lastObject] valueForKey:@"hq"]);
}

-(void)loadAccents
{
    self.stemLabel.text = @"Accents";
    self.stemLabel.hidden = false;
    self.startTime = CACurrentMediaTime();
}

-(void) loadPrincipalPart
{
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
    NSString *from = @", ";
    NSString *to = @" | "; // /, |, or
    //change , to /
    NSString *pres = [[[array lastObject] valueForKey:@"present"] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *fut = [[[array lastObject] valueForKey:@"future"] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *aor = [[[array lastObject] valueForKey:@"aorist"] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *perf = [[[array lastObject] valueForKey:@"perfectactive"] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *perfmid = [[[array lastObject] valueForKey:@"perfectmid"] stringByReplacingOccurrencesOfString: from withString:to];
    NSString *aorpass = [[[array lastObject] valueForKey:@"aoristpassive"] stringByReplacingOccurrencesOfString: from withString:to];
    
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
    self.stemLabel.text = pres;
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
    self.cardType = [[self.detailItem  valueForKey:@"sort"] integerValue];
    
    self.backLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.backLabel.textAlignment = NSTextAlignmentCenter;
    self.stemLabel.textAlignment = NSTextAlignmentCenter;
    self.stemLabel.numberOfLines = 0;
    self.backLabel.numberOfLines = 0;
    self.singLabel.numberOfLines = 0;
    self.pluralLabel.numberOfLines = 0;
    
    
    self.stemLabel.autoresizingMask = UIViewAutoresizingNone;
    self.backLabel.autoresizingMask = UIViewAutoresizingNone;
    //NSString *font = @"ArialMT";
    NSString *font = @"HelveticaNeue";
    //NSString *font = @"GillSans";
    //NSString *font = @"Kailasa";
    //NSString *font = @"ArialMT";
    //NSString *font = @"NewAthenaUnicode";
    //self.stemLabel.font = [UIFont fontWithName:@"NewAthenaUnicode" size:30.0];
    //self.backLabel.font = [UIFont fontWithName:@"NewAthenaUnicode" size:30.0];
    
    self.stemLabel.font = [UIFont fontWithName:font size:26.0];
    self.backLabel.font = [UIFont fontWithName:font size:26.0];
    self.singLabel.font = [UIFont fontWithName:font size:26.0];
    self.pluralLabel.font = [UIFont fontWithName:font size:26.0];
    
    
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

- (void)viewDidLoad
{
    [super viewDidLoad];
    
	// Do any additional setup after loading the view, typically from a nib.
    
    [self configureView];
    self.levels = [NSMutableArray arrayWithObjects: nil];
    
    if (self.levels)
        [self.levels removeAllObjects];
    
    for (int i = 0; i < [self.buttonStates count]; i++)
    {
        if ([[self.buttonStates objectAtIndex:i] boolValue] == YES)
        {
            [self.levels addObject:[NSNumber numberWithInt:(i+1)]];
        }
    }
    
    //[self.levels addObjectsFromArray:[[NSArray alloc] initWithObjects: [NSNumber numberWithInt:4], [NSNumber numberWithInt:5],  nil]];
    //NSLog(@"levels: %ld", [self.levels count]);
    
    [self loadNext];
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
