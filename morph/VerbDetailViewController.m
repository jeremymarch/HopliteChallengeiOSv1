//
//  VerbDetailViewController.m
//  morph
//
//  Created by Jeremy on 7/23/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#import "VerbDetailViewController.h"
#import "GreekForms.h"
#import "HCColors.h"

@interface VerbDetailViewController ()

@end

@implementation VerbDetailViewController

@synthesize verbIndex;

@dynamic view;
//http://travisjeffery.com/b/2012/12/overriding-uiviewcontrollers-view-property-done-right/
- (void)loadView {

    self.view = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 0, [[UIScreen mainScreen] bounds].size.width, [[UIScreen mainScreen] bounds].size.height)];
    self.view.backgroundColor = [UIColor whiteColor];
    self.view.scrollEnabled = YES;
    //self.view.pagingEnabled = YES; //don't want this
    self.view.showsVerticalScrollIndicator = YES;
    self.view.showsHorizontalScrollIndicator = YES;
    //this is required
    self.view.contentSize = CGSizeMake(self.view.bounds.size.width, self.view.bounds.size.height);
}

- (void)handlePinch:(UIPinchGestureRecognizer *)pinchGestureRecognizer
{
    //handle pinch...
    if (pinchGestureRecognizer.scale > 1)
    {
        if (!self.expanded)
        {
            self.expanded = YES;
            [self printVerbs];
        }
        //NSLog(@"pinch out2");
    }
    else
    {
        if (self.expanded)
        {
            self.expanded = NO;
            [self printVerbs];
        }
        //NSLog(@"pinch in2");
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];

    [self.navigationController setNavigationBarHidden:NO];
    
    self.view.userInteractionEnabled = YES;
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]
                                       initWithTarget:self action:@selector(handlePinch:)];
    pinch.delegate = self;
    //pinch.tag = self;
    [self.view addGestureRecognizer:pinch];
    
    self.expanded = false;

    [self printVerbs];
}

- (NSString *) getPPString: (Verb *)verb
{
    NSString *pres = [NSString stringWithUTF8String: verb->present];
    NSString *fut = [NSString stringWithUTF8String: verb->future];
    NSString *aor = [NSString stringWithUTF8String: verb->aorist];
    NSString *perf = [NSString stringWithUTF8String: verb->perf];
    NSString *perfmid = [NSString stringWithUTF8String: verb->perfmid];
    NSString *aorpass = [NSString stringWithUTF8String: verb->aoristpass];

    pres = [pres stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    fut = [fut stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    aor = [aor stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    perf = [perf stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    perfmid = [perfmid stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    aorpass = [aorpass stringByReplacingOccurrencesOfString:@"," withString:@" or"];
    
    NSString *dash = @"—";
    
    if ([pres length] == 0)
        pres = dash;
    if ([fut length] == 0)
        fut = dash;
    if ([aor length] == 0)
        aor = dash;
    if ([perf length] == 0)
        perf = dash;
    if ([perfmid length] == 0)
        perfmid = dash;
    if ([aorpass length] == 0)
        aorpass = dash;
    
    return [NSString stringWithFormat:@"%@, %@, %@, %@, %@, %@", pres, fut, aor, perf, perfmid, aorpass];
}

-(void) printVerbs
{
    [[self.view subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
    
    VerbFormC vf;
    vf.mood = INDICATIVE;
    int rowCount = 0;
    int bufferLen = 2048;
    char buffer[bufferLen];
    int labelHeight = 28;
    int verticalPadding = 4;
    int leftPadding = 12;
    bool twoCols = true;
    int yOffset = 0;
    
    vf.verb = &verbs[self.verbIndex];
    NSString *title = [NSString stringWithUTF8String: (strlen(vf.verb->present) > 0) ? vf.verb->present : vf.verb->future ];
    [self.navigationController.navigationBar setTitleTextAttributes:
     @{NSForegroundColorAttributeName:[UIColor blackColor],
       NSFontAttributeName:[UIFont fontWithName:@"NewAthenaUnicode" size:24]}];
    self.title = title;
    
    UILabel *l = [[UILabel alloc] initWithFrame:CGRectMake(0, yOffset, self.view.frame.size.width, labelHeight)];
    yOffset += (labelHeight + verticalPadding);
    l.text = [NSString stringWithFormat:@"  Principal Parts"];
    l.autoresizingMask = (UIViewAutoresizingFlexibleWidth |
                          UIViewAutoresizingFlexibleLeftMargin |
                          UIViewAutoresizingFlexibleRightMargin);
    
    l.font = [UIFont fontWithName:@"HelveticaNeue" size:18.0];
    l.textColor = [UIColor whiteColor];

    l.backgroundColor = [UIColor HCDarkBlue];
    [self.view addSubview:l];
    rowCount++;
    
    //principal part label
    l = [[UILabel alloc] init];

    l.numberOfLines = 0;
    
    l.text = [self getPPString:vf.verb];
    l.font = [UIFont fontWithName:@"NewAthenaUnicode" size:24.0];
    [self.view addSubview:l];
    
    //http://stackoverflow.com/questions/27374612/how-do-i-calculate-the-uilabel-height-dynamically
    CGFloat maxLabelWidth = self.view.bounds.size.width - (leftPadding * 2);
    CGSize neededSize = [l sizeThatFits:CGSizeMake(maxLabelWidth, CGFLOAT_MAX)];
    [l setFrame:CGRectMake(leftPadding, yOffset, self.view.frame.size.width - (leftPadding * 2), neededSize.height + (verticalPadding * 2))];
    yOffset += neededSize.height + (verticalPadding * 2);
    
    //countPerSection++;
    rowCount += 4;
    bool isOida = false;
    if (utf8HasSuffix(vf.verb->present, "οἶδα") || utf8HasSuffix(vf.verb->present, "σύνοιδα") )
    {
        isOida = true;
    }
    
    for (int g1 = 0; g1 < NUM_TENSES; g1++)
    {
        vf.tense = g1;
        for (int v = 0; v < NUM_VOICES; v++)
        {
            for (int m = 0; m < NUM_MOODS; m++)
            {
                if (!isOida && m != INDICATIVE && (g1 == PERFECT || g1 == PLUPERFECT || g1 == IMPERFECT || g1 == FUTURE))
                    continue;
                else if (isOida && m != INDICATIVE && (g1 == PLUPERFECT || g1 == IMPERFECT || g1 == FUTURE))
                    continue;
                
                NSString *s;
                if (v == ACTIVE || g1 == AORIST || g1 == FUTURE)
                {
                    s = [NSString stringWithFormat:@"  %@ %@ %@", [NSString stringWithUTF8String: tenses[g1]], [NSString stringWithUTF8String: voices[v]], [NSString stringWithUTF8String: moods[m]]];
                }
                else if (v == MIDDLE)
                {
                    //FIX ME, is this right?? how do we label these.
                    //yes it's correct, middle deponents do not have a passive voice.  H&Q page 316
                    if ( deponentType(vf.verb) == MIDDLE_DEPONENT || deponentType(vf.verb) == PASSIVE_DEPONENT || deponentType(vf.verb) == DEPONENT_GIGNOMAI || utf8HasSuffix(vf.verb->present, "κεῖμαι"))
                    {
                        s = [NSString stringWithFormat:@"  %@ %@ %@", [NSString stringWithUTF8String: tenses[g1]], @"Middle", [NSString stringWithUTF8String: moods[m]]];
                    }
                    else
                    {
                        s = [NSString stringWithFormat:@"  %@ %@ %@", [NSString stringWithUTF8String: tenses[g1]], @"Middle/Passive", [NSString stringWithUTF8String: moods[m]]];
                    }
                }
                else
                {
                    continue; //skip passive if middle+passive are the same
                }
                
                UILabel *l = [[UILabel alloc] initWithFrame:CGRectMake(0, yOffset, self.view.frame.size.width, labelHeight)];
                yOffset += (labelHeight + verticalPadding);
                
                l.autoresizingMask = (UIViewAutoresizingFlexibleWidth |
                                                          UIViewAutoresizingFlexibleLeftMargin |
                                                          UIViewAutoresizingFlexibleRightMargin);
                
                l.text = s;
                l.font = [UIFont fontWithName:@"HelveticaNeue" size:18.0];
                l.textColor = [UIColor whiteColor];
                l.backgroundColor = [UIColor HCDarkBlue];
                
                [self.view addSubview:l];
                rowCount++;
                
                vf.voice = v;
                int countPerSection = 0;
                for (int h = 0; h < NUM_NUMBERS; h++)
                {
                    for (int i = 0; i < NUM_PERSONS; i++)
                    {
                        vf.number = h;
                        vf.person = i;
                        vf.mood = m;
                        
                        if (getForm(&vf, buffer, bufferLen, true, self.expanded))
                        {
                            UILabel *l = [[UILabel alloc] init];
                            l.font = [UIFont fontWithName:@"NewAthenaUnicode" size:24.0];
                            NSString *s2 = [NSString stringWithUTF8String: buffer];
                            if (0)
                            {
                                l.text = s2;
                                neededSize.height = labelHeight;
                                [l setFrame:CGRectMake(leftPadding, yOffset, self.view.frame.size.width - (leftPadding * 2), neededSize.height + verticalPadding)];
                                yOffset += neededSize.height + verticalPadding;
                            }
                            else if (0)
                            {
                                s2 = [s2 stringByReplacingOccurrencesOfString:@", " withString:@",\n\t "];
                                NSMutableAttributedString *as = [[NSMutableAttributedString alloc] initWithString:[NSString stringWithFormat:@"%d%@: %@", (i+1), (h == 0) ? @"s" : @"p", s2]];
                                
                                [as addAttribute:NSFontAttributeName
                                           value:[UIFont fontWithName:@"HelveticaNeue" size:20.0]
                                           range:NSMakeRange(0, 3)];
                                
                                [as addAttribute:NSForegroundColorAttributeName
                                           value:[UIColor grayColor]
                                           range:NSMakeRange(0, 3)];
                                
                                l.attributedText = as;
                                
                                l.numberOfLines = 0;
                                neededSize = [l sizeThatFits:CGSizeMake(maxLabelWidth, CGFLOAT_MAX)];
                                [l setFrame:CGRectMake(leftPadding, yOffset, self.view.frame.size.width - (leftPadding * 2), neededSize.height + verticalPadding)];
                                yOffset += neededSize.height + verticalPadding;
                            }
                            else
                            {
                                UILabel *l2 = [[UILabel alloc] init];
                                l2.font = [UIFont fontWithName:@"HelveticaNeue" size:20.0];
                                l2.textColor = [UIColor grayColor];
                                l2.text = [NSString stringWithFormat:@"%d%@:", (i+1), (h == 0) ? @"s" : @"p"];
                                l2.textAlignment = NSTextAlignmentRight;
                                
                                
                                s2 = [s2 stringByReplacingOccurrencesOfString:@", " withString:@"\n"];
                                
                                l.text = s2;
                                l.numberOfLines = 0;
                                neededSize = [l sizeThatFits:CGSizeMake(maxLabelWidth, CGFLOAT_MAX)];
                                [l setFrame:CGRectMake(leftPadding + 40, yOffset, self.view.frame.size.width - (leftPadding * 2), neededSize.height + verticalPadding)];
                                
                                
                                CGSize needed2 = [l2 sizeThatFits:CGSizeMake(maxLabelWidth, CGFLOAT_MAX)];
                                [l2 setFrame:CGRectMake(leftPadding, yOffset, 30, needed2.height)];
                                
                                yOffset += neededSize.height + verticalPadding;
                                [self.view addSubview:l2];
                            }
                            
                            [self.view addSubview:l];
                            countPerSection++;
                            rowCount++;
                        }
                    }
                }
                //if no forms in a section, remove its label
                if (countPerSection == 0)
                {
                    //remove label
                    [l removeFromSuperview];
                    yOffset -= (labelHeight + verticalPadding);
                    rowCount--;
                }
            }
            //rowCount++;
        }
    }
    //set height of scrollview
    self.view.contentSize = CGSizeMake([[UIScreen mainScreen] bounds].size.width, yOffset);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)setVerbIndex2:(NSInteger)verbIndex2
{
    self.verbIndex = verbIndex2;
}

-(NSInteger)verbIndex2
{
    return self.verbIndex;
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
