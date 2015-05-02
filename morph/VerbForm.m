//
//  VerbForm.m
//  morph
//
//  Created by Jeremy on 6/30/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

/*
 ideas:
 Time trials:  tap all words whch are subjunctive, etc.
 MC for time.
 Intervals.
 
 Change to form MC mixed with id form MC
 
 ID with rollers for parameters
 */

#import "VerbForm.h"
#import "AppDelegate.h"
#import "libmorph.h"
#import "GreekUnicode.h"

@implementation VerbForm

- (NSArray *)tensesAbbrev
{
    static NSArray *_tenses;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _tenses = @[@"pres.",
                    @"aor.",
                    @"perf.",
                    @"imp.",
                    @"fut.",
                    @"plup."];
    });
    return _tenses;
}
- (NSArray *)moodsAbbrev
{
    static NSArray *_moods;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _moods = @[@"ind.",
                   @"sub.",
                   @"opt."];
        /*
         @"Infinitive",
         @"Imperative"];
         */
    });
    return _moods;
}
- (NSArray *)voicesAbbrev
{
    static NSArray *_voices;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _voices = @[@"act.",
                    @"mid.",
                    @"pass."];
    });
    return _voices;
}
- (NSArray *)personsAbbrev
{
    static NSArray *_persons;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _persons = @[@"1st",
                     @"2nd",
                     @"3rd"];
    });
    return _persons;
}
- (NSArray *)numbersAbbrev
{
    static NSArray *_numbers;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _numbers = @[@"sing.",
                     @"pl."]; //Dual
    });
    return _numbers;
}

- (NSArray *)tenses
{
    static NSArray *_tenses;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _tenses = @[@"Present",
                    @"Aorist",
                    @"Perfect",
                    @"Imperfect",
                    @"Future",
                    @"Pluperfect"];
    });
    return _tenses;
}
- (NSArray *)moods
{
    static NSArray *_moods;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _moods = @[@"Indicative",
                   @"Subjunctive",
                   @"Optative"];
        /*
                   @"Infinitive",
                   @"Imperative"];
        */
    });
    return _moods;
}
- (NSArray *)voices
{
    static NSArray *_voices;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _voices = @[@"Active",
                    @"Middle",
                    @"Passive"];
    });
    return _voices;
}
- (NSArray *)persons
{
    static NSArray *_persons;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _persons = @[@"1st",
                     @"2nd",
                     @"3rd"];
    });
    return _persons;
}
- (NSArray *)numbers
{
    static NSArray *_numbers;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _numbers = @[@"Singular",
                     @"Plural"]; //Dual
    });
    return _numbers;
}

-(NSString*)getPersonLabel:(BOOL)abbrev
{
    if (self.person < 0 || self.person > 2)
        return @"";
    
    if (abbrev)
        return [self.personsAbbrev objectAtIndex:self.person];
    else
        return [self.persons objectAtIndex:self.person];
}

-(NSString*)getNumberLabel:(BOOL)abbrev
{
    if (self.number < 0 || self.number > 1)
        return @"";

    if (abbrev)
        return [self.numbersAbbrev objectAtIndex:self.number];
    else
    return [self.numbers objectAtIndex:self.number];
}

-(NSString*)getTenseLabel:(BOOL)abbrev
{
    if (self.tense < 0 || self.tense > 5)
        return @"";
    
    if (abbrev)
        return [self.tensesAbbrev objectAtIndex:self.tense];
    else
        return [self.tenses objectAtIndex:self.tense];
}

-(NSString*)getVoiceLabel:(BOOL)abbrev
{
    if (self.voice < 0 || self.voice > 2)
        return @"";
    
    if (abbrev)
        return [self.voicesAbbrev objectAtIndex:self.voice];
    else
        return [self.voices objectAtIndex:self.voice];
}

-(NSString*)getMoodLabel:(BOOL)abbrev
{
    if (self.mood < 0 || self.mood > 2)
        return @"";
    
    if (abbrev)
        return [self.moodsAbbrev objectAtIndex:self.mood];
    else
        return [self.moods objectAtIndex:self.mood];
}

-(NSString*)getFullDescription
{
    return [NSString stringWithFormat:@"%@ %@ %@ %@ %@", (self.person >= 0) ? [self getPersonLabel:0] : @"", (self.number >= 0) ? [self getNumberLabel:0] : @"", [self getTenseLabel:0], [self getVoiceLabel:0], [self getMoodLabel:0]];
}

-(NSString*)getAbbrevDescription
{
    return [NSString stringWithFormat:@"%@ %@ %@ %@ %@", (self.person >= 0) ? [self getPersonLabel:1] : @"", (self.number >= 0) ? [self getNumberLabel:1] : @"", [self getTenseLabel:1], [self getVoiceLabel:1], [self getMoodLabel:1]];
}

-(VerbForm*) initWithVerb:(Verbs*)verb person:(NSInteger)p number:(NSInteger)n tense:(NSInteger)t voice:(NSInteger)v mood:(NSInteger)m;
{
    self = [super init];
    if (self)
    {
        self.tense = -1;
        self.verb = verb;
        if (p < 0)
        {
            [self generateForm];
        }
        else
        {
            self.person = p;
            self.number = n;
            self.tense = t;
            self.voice = v;
            self.mood = m;
        }
    }
    return self;
}

-(NSString*) getPrincipalPart
{
    NSString *stem;
    //also need to consider deponent verbs
    if (self.tense == PRESENT || self.tense == IMPERFECT)
        stem = self.verb.present;
    else if ( self.tense == FUTURE && self.voice != PASSIVE)
        stem = self.verb.future;
    else if ( self.tense == FUTURE)
        stem = self.verb.aoristpassive;
    else if (self.tense == AORIST && self.voice == PASSIVE)
        stem = self.verb.aoristpassive;
    else if (self.tense == AORIST)
        stem = self.verb.aorist;
    else if (self.voice == ACTIVE && ( self.tense == PERFECT || self.tense == PLUPERFECT))
        stem = self.verb.perfectactive;
    else
        stem = self.verb.perfectmid;
    
    return stem;
}

-(NSString*) getPrincipalPartForTense:(NSInteger)tense voice:(NSInteger)voice
{
    NSString *stem;
    //also need to consider deponent verbs
    if (tense == PRESENT || tense == IMPERFECT)
        stem = self.verb.present;
    else if ( tense == FUTURE && voice != PASSIVE)
        stem = self.verb.future;
    else if ( tense == FUTURE)
        stem = self.verb.aoristpassive;
    else if (tense == AORIST && voice == PASSIVE)
        stem = self.verb.aoristpassive;
    else if (tense == AORIST)
        stem = self.verb.aorist;
    else if (voice == ACTIVE && ( tense == PERFECT || tense == PLUPERFECT))
        stem = self.verb.perfectactive;
    else
        stem = self.verb.perfectmid;
    
    return stem;
}

-(NSString*) getEnding
{
    if ( self.mood == INFINITIVE)
    {
        if (self.tense == PRESENT && self.voice == ACTIVE )
            return @"ειν";
        if (self.tense == PRESENT && ( self.voice == MIDDLE || self.voice == PASSIVE ))
            return @"εσθαι";
        if (self.tense == AORIST && self.voice == ACTIVE )
            return @"αι";
        if (self.tense == AORIST && self.voice == PASSIVE )
            return @"ῆναι";
        if (self.tense == AORIST && self.voice == MIDDLE )
            return @"ασθαι";
        if (self.tense == PERFECT && self.voice == ACTIVE )
            return @"έναι";
        else
            return @"σθαι";
    }
    
    NSError *error = nil;
    NSManagedObjectContext *moc = [(AppDelegate*)[[UIApplication sharedApplication] delegate] managedObjectContext];
    
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"Endings" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    
    //NSPredicate *predicate = [NSPredicate predicateWithFormat:@"hq IN %@", self.levels];
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"tense == %@ AND voice CONTAINS[cd] %@ AND mood == %@", [[self tenses] objectAtIndex:self.tense], [[self voices] objectAtIndex:self.voice], [[self moods] objectAtIndex:self.mood]];
    [request setPredicate:predicate];
    
    NSInteger count = [moc countForFetchRequest:request error:&error];
    NSLog(@"Count: %ld, %@ %@ %@", count, [[self tenses] objectAtIndex:self.tense], [[self voices] objectAtIndex:self.voice], [[self moods] objectAtIndex:self.mood]);
    // Set example predicate and sort orderings...
    [request setFetchLimit:1];
    
    NSArray *array = [moc executeFetchRequest:request error:&error];
    
    if (array == nil)
    {
        // Deal with error...
        NSLog(@"Error: Def not found by id: %d.", 1);
        return @"";
    }
    
    NSString *personNumber;
    if (self.person == 0 && self.number == 0)
        personNumber = @"fs";
    else if (self.person == 1 && self.number == 0)
        personNumber = @"ss";
    else if (self.person == 2 && self.number == 0)
        personNumber = @"ts";
    else if (self.person == 0 && self.number == 1)
        personNumber = @"fp";
    else if (self.person == 1 && self.number == 1)
        personNumber = @"sp";
    else if (self.person == 2 && self.number == 1)
        personNumber = @"tp";
    
    return [[array lastObject] valueForKey:personNumber];
}

-(NSString*) getForm
{
    Verb *v = getRandomVerb();
    //NSString *temp = [NSString stringWithUTF8String: (const char*)v->present];
    VerbFormC vf;
    vf.verb = v;
    generateForm(&vf);
    char utf8[1024];
    //NSLog(@"Verb: %s %i %i %i %i %i", v->present, vf.person, vf.number, vf.tense, vf. voice, vf.mood);
    
    getForm(&vf, utf8);
 /*
    NSString *stem = [self getPrincipalPart];
    
    const char *utf8 = [stem cStringUsingEncoding:NSUTF8StringEncoding];
    const char *utf8Present = [self.verb.present cStringUsingEncoding:NSUTF8StringEncoding];
    const char *utf8Ending = [[self getEnding] cStringUsingEncoding:NSUTF8StringEncoding];
    
    //NSLog(@"here: %@, %@, %@, %li, %li, %li", stem, [self getEnding], self.verb.present, self.tense, self.voice, (long)self.mood);
    
    if (utf8)
        getFormReal(utf8, utf8Ending, utf8Present, self.person, self.number, self.tense, self.voice, self.mood);
    else
        NSLog(@"bad utf8 here");
    */
    return [NSString stringWithUTF8String: (const char*)utf8];
}

-(void) changeFormByDegrees:(NSInteger)degrees
{
    NSInteger tempPerson;
    NSInteger tempNumber;
    NSInteger tempTense;
    NSInteger tempVoice;
    NSInteger tempMood;
    
    NSInteger components[degrees];
    
    do
    {
        tempPerson = self.person;
        tempNumber = self.number;
        tempTense = self.tense;
        tempVoice = self.voice;
        tempMood = self.mood;
        
        //re-initialize components array to invalid values
        for (int i = 0; i < degrees; i++)
            components[i] = -1;
        
        for (int i = 0; i < degrees; i++)
        {
            NSInteger n = randWithMax(5);
            BOOL notAlreadyIn = true;
            for (int j = 0; j < degrees; j++)
            {
                if (n == components[j])
                {
                    notAlreadyIn = false;
                    --i;
                    break;
                }
            }
            if (notAlreadyIn)
                components[i] = n;
        }
        
        for (int i = 0; i < degrees; i++)
        {
            NSInteger v = components[i];
            
            if (v == PERSON)
            {
                tempPerson = incrementValue((int)[[self persons] count], (int)self.person);
            }
            else if (v == NUMBER)
            {
                tempNumber = incrementValue((int)[[self numbers] count], (int)self.number);
            }
            else if (v == TENSE)
            {
                tempTense = incrementValue((int)[[self tenses] count], (int)self.tense);
            }
            else if (v == VOICE)
            {
                tempVoice = incrementValue((int)[[self voices] count], (int)self.voice);
            }
            else if (v == MOOD)
            {
                tempMood = incrementValue((int)[[self moods] count], (int)self.mood);
            }
        }
    } //make sure form is valid and this verb has the required principal part
    while(!formIsValidReal((int)tempPerson, (int)tempNumber, (int)tempTense, (int)tempVoice, (int)tempMood) || [[self getPrincipalPartForTense:tempTense voice: tempVoice] isEqual: @""]);
    
    self.person = tempPerson;
    self.number = tempNumber;
    self.tense = tempTense;
    self.voice = tempVoice;
    self.mood = tempMood;
}

-(void) generateForm
{
    long iTense, iMood, iVoice, iPerson, iNumber;
    
    do
    {
        iTense = randWithMax([[self tenses] count]);
        iMood = randWithMax([[self moods] count]);
        while ( (iTense > 1 && iMood > 0 ) )
            iMood = randWithMax([[self moods] count]);
        
        iVoice = randWithMax([[self voices] count]);
        /*
         if (iMood == 1)
         {
         iPerson = -1;
         iNumber = -1;
         }
         else if (iMood == 4)
         {
         iNumber = randWithMax([[self numbers] count]);
         iPerson = randWithMax([[self persons] count]);
         while (iPerson == 0)
         iPerson = randWithMax([[self persons] count]);
         }
         else
         {
         */
        iPerson = randWithMax([[self persons] count]);
        iNumber = randWithMax([[self numbers] count]);
        //}
        
        //NSArray conj = [NSArray arrayWithObjects: [NSNumber  v], nil];
        
        //NSUInteger randomIndex = randWithMax([theArray count]);
        //NSString *form = [NSString stringWithFormat:@"%@ %@ %@ %@ %@", (iPerson > -1) ? [[self persons] objectAtIndex: iPerson] : @"", (iNumber > -1) ? [[self numbers] objectAtIndex: iNumber] : @"", [[self tenses] objectAtIndex: iTense], [[self voices] objectAtIndex: iVoice], [[self moods] objectAtIndex: iMood]];
    }
    while (!formIsValidReal(iPerson, iNumber, iTense, iVoice, iMood) || [[self getPrincipalPartForTense:iTense voice:iVoice] isEqual: @""]);
    
    self.person = iPerson;
    self.number = iNumber;
    self.tense = iTense;
    self.voice = iVoice;
    self.mood = iMood;
}

/*
 -(NSString*) getForm
 {
 NSString *stem = [self getPrincipalPart];
 //stem = [self stripAccent:stem];
 
 const char *utf8 = [stem cStringUsingEncoding:NSUTF8StringEncoding];
 const char *utf8Present = [self.verb.present cStringUsingEncoding:NSUTF8StringEncoding];
 const char *utf8Ending = [[self getEnding] cStringUsingEncoding:NSUTF8StringEncoding];
 
 NSLog(@"here: %@, %@, %@, %li, %li, %li", stem, [self getEnding], self.verb.present, self.tense, self.voice, (long)self.mood);
 
 if (utf8)
 getFormReal(utf8, utf8Ending, utf8Present, self.tense, self.voice, self.mood);
 else
 NSLog(@"bad utf8 here");
 
 int ucs2[(strlen(utf8) * 2) + 1];
 int len = 0;
 utf8_to_ucs2_string((const unsigned char*)utf8, ucs2, &len);
 
 stripAccent2(ucs2, &len);
 
 //stem = [self stripEndingFromPrincipalPart:stem];
 
 stripEndingFromPrincipalPart(ucs2, &len, (int)self.tense, (int)self.voice);
 
 
 if (self.tense == IMPERFECT || self.tense == PLUPERFECT)
 {
 //stem = [self augmentStem:stem];
 augmentStemUcs2(ucs2, &len);
 }
 
 if ( (self.tense == AORIST && (self.mood == SUBJUNCTIVE || self.mood == OPTATIVE)) || (self.tense == FUTURE && self.voice == PASSIVE))
 {
 //stem = [self stripAugmentFromPrincipalPart:stem];
 
 const char *utf82 = [self.verb.present cStringUsingEncoding:NSUTF8StringEncoding];
 int ucs22[(strlen(utf8) * 2) + 1];
 int len2 = 0;
 utf8_to_ucs2_string((const unsigned char*)utf82, ucs22, &len2);
 stripAccent2(ucs22, &len2);
 int presentInitialLetter = ucs22[0];
 
 stripAugmentFromPrincipalPart2(ucs2, &len, (int)self.tense, (int)self.voice, (int)self.mood, presentInitialLetter);
 }
 
 ucs2_to_utf8_string(ucs2, len, (unsigned char *)utf8);
 stem = [NSString stringWithUTF8String: (const char*)utf8];
 
 NSString* form;
 if (self.tense == FUTURE && self.voice == PASSIVE)
 form = [NSString stringWithFormat:@"%@ησ%@", stem, [self getEnding]];
 else
 form = [NSString stringWithFormat:@"%@%@", stem, [self getEnding]];
 
 //if ending does not already have an accent
 if (!(self.tense == AORIST && self.voice == PASSIVE && self.mood == SUBJUNCTIVE))
 form = [self accentRecessive:form];
 

return [NSString stringWithUTF8String: (const char*)utf8];
}


-(NSString*)stripEndingFromPrincipalPart:(NSString*)stem
{
    //needs to handle deponent verbs
    NSString *newStem;
    if ([stem length] < 1)
        return stem;
    
    if ((self.tense == PRESENT || self.tense == IMPERFECT) && [stem hasSuffix:@"ομαι"] == YES)
        newStem = [stem substringToIndex:[stem length] - 4];
    else if (self.tense == PRESENT || self.tense == IMPERFECT)
        newStem = [stem substringToIndex:[stem length] - 1];
    else if (self.tense == FUTURE  && self.voice != PASSIVE && [stem hasSuffix:@"ομαι"] == YES)
        newStem = [stem substringToIndex:[stem length] - 4];
    else if (self.tense == FUTURE && self.voice != PASSIVE)
        newStem = [stem substringToIndex:[stem length] - 1];
    else if (self.tense == FUTURE && self.voice == PASSIVE)
        newStem = [stem substringToIndex:[stem length] - 2];
    else if (self.tense == AORIST && self.voice != PASSIVE  && [stem hasSuffix:@"άμην"] == YES)
        newStem = [stem substringToIndex:[stem length] - 4];
    else if (self.tense == AORIST && self.voice != PASSIVE)
        newStem = [stem substringToIndex:[stem length] - 1];
    else if (self.tense == AORIST && self.voice == PASSIVE)
        newStem = [stem substringToIndex:[stem length] - 2];
    else if (self.tense == PERFECT && self.voice == ACTIVE)
        newStem = [stem substringToIndex:[stem length] - 1];
    else if (self.tense == PLUPERFECT && self.voice == ACTIVE)
        newStem = [stem substringToIndex:[stem length] - 1];
    else if (self.tense == PERFECT)
        newStem = [stem substringToIndex:[stem length] - 3];
    else if (self.tense == PLUPERFECT)
        newStem = [stem substringToIndex:[stem length] - 3];
    else
        newStem = stem;
    return newStem;
}

-(BOOL)isConsonant:(NSString*)letter
{
    NSString *consonants = @"βγδζθκλμνπρστχφψ";
    if ([consonants rangeOfString:letter].location == NSNotFound)
        return false;
    
    return true;
}

-(NSString*)accentRecessive:(NSString*)w
{
    const char *w2 = [w cStringUsingEncoding:NSUTF8StringEncoding];

    int tempUcs2String[(strlen((const char*)w2) * 2) + 10 + 1]; //10 extra in case we need room to add accents, etc.
    int len = 0;
    utf8_to_ucs2_string((const unsigned char *) w2, tempUcs2String, &len);
    
    accentRecessive(tempUcs2String, &len, false);
    
    ucs2_to_utf8_string(tempUcs2String, len, (unsigned char *)w2);
    
    return [NSString stringWithUTF8String: (const char*)w2];
}

-(NSString*)augmentStem:(NSString*)stem
{
    //add in cases for lengthened augment.
    NSString *newStem;
    if (self.tense == IMPERFECT || self.tense == PLUPERFECT)
    {
        NSString *firstLetter = [stem substringToIndex:1];
        
        if ([stem hasPrefix:@"αἰ"]) //smooth
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὐ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἰ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὐ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἰ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾠ", [stem substringFromIndex:2]];
        
        else if ([stem hasPrefix:@"αἱ"]) //rough
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὑ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἱ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὑ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἱ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾡ", [stem substringFromIndex:2]];
        
        else if ([stem hasPrefix:@"αἴ"]) //smooth acute
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὔ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἴ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὔ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἴ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾠ", [stem substringFromIndex:2]];

        else if ([stem hasPrefix:@"αἵ"]) //rough acute
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὕ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἵ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὕ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἵ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾡ", [stem substringFromIndex:2]];

        else if ([stem hasPrefix:@"αἶ"]) //smooth circumflex
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὖ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἶ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὖ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὐ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἶ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾠ", [stem substringFromIndex:2]];
        
        else if ([stem hasPrefix:@"αἷ"]) //rough circumflex
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"αὗ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εἷ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"εὗ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ηὑ", [stem substringFromIndex:2]];
        else if ([stem hasPrefix:@"οἷ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ᾡ", [stem substringFromIndex:2]];
        
        
        else if ([firstLetter isEqualToString:@"ἀ"]) //smooth
            newStem = [NSString stringWithFormat:@"%@%@", @"ἠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἐ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἰ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὀ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ὠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὐ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὐ", [stem substringFromIndex:1]];

        else if ([firstLetter isEqualToString:@"ἄ"]) //smooth
            newStem = [NSString stringWithFormat:@"%@%@", @"ἠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἔ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἴ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὄ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ὠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὔ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὐ", [stem substringFromIndex:1]];
        
        else if ([firstLetter isEqualToString:@"ἆ"]) //smooth
            newStem = [NSString stringWithFormat:@"%@%@", @"ἠ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἶ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὖ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὐ", [stem substringFromIndex:1]];
        
        else if ([firstLetter isEqualToString:@"ἁ"]) //rough
            newStem = [NSString stringWithFormat:@"%@%@", @"ἡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἑ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἱ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὁ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ὡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὑ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὑ", [stem substringFromIndex:1]];
        
        else if ([firstLetter isEqualToString:@"ἅ"]) //rough
            newStem = [NSString stringWithFormat:@"%@%@", @"ἡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἕ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἵ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὅ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ὡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὕ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὑ", [stem substringFromIndex:1]];
        
        else if ([firstLetter isEqualToString:@"ἇ"]) //rough
            newStem = [NSString stringWithFormat:@"%@%@", @"ἡ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ἷ"])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἱ", [stem substringFromIndex:1]];
        else if ([firstLetter isEqualToString:@"ὗ"]) //long iota
            newStem = [NSString stringWithFormat:@"%@%@", @"ὑ", [stem substringFromIndex:1]];
        
        else if([self isConsonant:firstLetter])
            newStem = [NSString stringWithFormat:@"%@%@", @"ἐ", stem];
        else
            newStem = stem;
    }
    else
    {
        return stem;
    }
    return newStem;
}

-(NSString*)stripAugmentFromPrincipalPart:(NSString*)stem
{
    NSString *newStem;
    if (self.tense == AORIST && (self.mood == SUBJUNCTIVE || self.mood == OPTATIVE))
    {
        if ([stem hasPrefix:@"ἐ"] || [stem hasPrefix:@"ἔ"] || [stem hasPrefix:@"έ"] || [stem hasPrefix:@"ε"])
        {
            NSLog(@"removed augment");
            newStem = [stem substringFromIndex:1];
        }
        else if ([stem hasPrefix:@"ἠ"] )
        {
            if ([self.verb.present hasPrefix:@"ἀ"])
                newStem = [NSString stringWithFormat:@"%@%@", @"ἀ", [stem substringFromIndex:1]];
            else if ([self.verb.present hasPrefix:@"ἐ"])
                newStem = [NSString stringWithFormat:@"%@%@", @"ἐ", [stem substringFromIndex:1]];
        }
        else
        {
            newStem = stem;
        }
    }
    else if (self.tense == FUTURE && self.voice == PASSIVE)
    {
        if ([stem hasPrefix:@"ἐ"] || [stem hasPrefix:@"ἔ"] || [stem hasPrefix:@"έ"] || [stem hasPrefix:@"ε"])
        {
            NSLog(@"removed augment");
            newStem = [stem substringFromIndex:1];
        }
        else if ([stem hasPrefix:@"ἠ"] )
        {
            newStem = [NSString stringWithFormat:@"%@%@", @"ἀ", [stem substringFromIndex:1]];
        }
        else
        {
            newStem = stem;
        }
    }
    else
    {
        newStem = stem;
    }
    return newStem;
}

-(NSString*) stripAccentNew:(NSString *)w
{
    const char *w2 = [w cStringUsingEncoding:NSUTF8StringEncoding];
    
    int tempUcs2String[(strlen((const char*)w2) * 2) + 10 + 1]; //10 extra in case we need room to add accents, etc.
    int len = 0;
    
    //convert to ucs2

    utf8_to_ucs2_string((const unsigned char *) w2, tempUcs2String, &len);
    stripAccent2(tempUcs2String, &len);
    
    //change back to utf8;
    //change into nsstring
    NSString *unaccentedWord;
    return unaccentedWord;
}

-(NSString*)stripAccent:(NSString*)word
{
    NSString *newString = @"";
    for (int i = 0; i < [word length]; i++)
    {
        NSString *letter = [word substringWithRange:NSMakeRange(i, 1)];
        if ([letter isEqualToString:@"ά"]) //acute
            newString = [newString stringByAppendingString:@"α"];
        else if ([letter isEqualToString:@"ά"]) //copied from db
            newString = [newString stringByAppendingString:@"α"];
        else if ([letter isEqualToString:@"έ"])
            newString = [newString stringByAppendingString:@"ε"];
        else if ([letter isEqualToString:@"έ"]) //copied from db
            newString = [newString stringByAppendingString:@"ε"];
        else if ([letter isEqualToString:@"ή"])
            newString = [newString stringByAppendingString:@"η"];
        else if ([letter isEqualToString:@"ή"]) //copied from db
            newString = [newString stringByAppendingString:@"η"];
        else if ([letter isEqualToString:@"ί"])
            newString = [newString stringByAppendingString:@"ι"];
        else if ([letter isEqualToString:@"ί"])//copied from db
            newString = [newString stringByAppendingString:@"ι"];
        else if ([letter isEqualToString:@"ό"])
            newString = [newString stringByAppendingString:@"ο"];
        else if ([letter isEqualToString:@"ό"]) //copied from db
            newString = [newString stringByAppendingString:@"ο"];
        else if ([letter isEqualToString:@"ύ"])
            newString = [newString stringByAppendingString:@"υ"];
        else if ([letter isEqualToString:@"ύ"]) //copied from db
            newString = [newString stringByAppendingString:@"υ"];
        else if ([letter isEqualToString:@"ώ"])
            newString = [newString stringByAppendingString:@"ω"];
        else if ([letter isEqualToString:@"ώ"]) //copied from db
            newString = [newString stringByAppendingString:@"ω"];
        
        else if ([letter isEqualToString:@"ᾶ"]) //circum
            newString = [newString stringByAppendingString:@"α"];
        else if ([letter isEqualToString:@"ῆ"])
            newString = [newString stringByAppendingString:@"η"];
        else if ([letter isEqualToString:@"ῖ"])
            newString = [newString stringByAppendingString:@"ι"];
        else if ([letter isEqualToString:@"ῦ"])
            newString = [newString stringByAppendingString:@"υ"];
        else if ([letter isEqualToString:@"ῶ"])
            newString = [newString stringByAppendingString:@"ω"];
        
        else if ([letter isEqualToString:@"ἄ"]) //smooth acute
            newString = [newString stringByAppendingString:@"ἀ"];
        else if ([letter isEqualToString:@"ἔ"])
            newString = [newString stringByAppendingString:@"ἐ"];
        else if ([letter isEqualToString:@"ἤ"])
            newString = [newString stringByAppendingString:@"ἠ"];
        else if ([letter isEqualToString:@"ἴ"])
            newString = [newString stringByAppendingString:@"ἰ"];
        else if ([letter isEqualToString:@"ὄ"])
            newString = [newString stringByAppendingString:@"ὀ"];
        else if ([letter isEqualToString:@"ὔ"])
            newString = [newString stringByAppendingString:@"ὐ"];
        else if ([letter isEqualToString:@"ὤ"])
            newString = [newString stringByAppendingString:@"ὠ"];

        else if ([letter isEqualToString:@"ἅ"]) //rough acute
            newString = [newString stringByAppendingString:@"ἁ"];
        else if ([letter isEqualToString:@"ἕ"])
            newString = [newString stringByAppendingString:@"ἑ"];
        else if ([letter isEqualToString:@"ἥ"])
            newString = [newString stringByAppendingString:@"ἡ"];
        else if ([letter isEqualToString:@"ἵ"])
            newString = [newString stringByAppendingString:@"ἱ"];
        else if ([letter isEqualToString:@"ὅ"])
            newString = [newString stringByAppendingString:@"ὁ"];
        else if ([letter isEqualToString:@"ὕ"])
            newString = [newString stringByAppendingString:@"ὑ"];
        else if ([letter isEqualToString:@"ὥ"])
            newString = [newString stringByAppendingString:@"ὡ"];
        
        else if ([letter isEqualToString:@"ἆ"]) //smooth circum
            newString = [newString stringByAppendingString:@"ἀ"];
        else if ([letter isEqualToString:@"ἦ"])
            newString = [newString stringByAppendingString:@"ἠ"];
        else if ([letter isEqualToString:@"ἶ"])
            newString = [newString stringByAppendingString:@"ἰ"];
        else if ([letter isEqualToString:@"ὖ"])
            newString = [newString stringByAppendingString:@"ὐ"];
        else if ([letter isEqualToString:@"ὦ"])
            newString = [newString stringByAppendingString:@"ὠ"];

        else if ([letter isEqualToString:@"ἇ"]) //rough circum
            newString = [newString stringByAppendingString:@"ἁ"];
        else if ([letter isEqualToString:@"ἧ"])
            newString = [newString stringByAppendingString:@"ἡ"];
        else if ([letter isEqualToString:@"ἷ"])
            newString = [newString stringByAppendingString:@"ἱ"];
        else if ([letter isEqualToString:@"ὗ"])
            newString = [newString stringByAppendingString:@"ὑ"];
        else if ([letter isEqualToString:@"ὧ"])
            newString = [newString stringByAppendingString:@"ὡ"];

        else if ([letter isEqualToString:@"ᾴ"]) //acute iota sub
            newString = [newString stringByAppendingString:@"ᾳ"];
        else if ([letter isEqualToString:@"ῄ"])
            newString = [newString stringByAppendingString:@"ῃ"];
        else if ([letter isEqualToString:@"ῴ"])
            newString = [newString stringByAppendingString:@"ῳ"];

        else if ([letter isEqualToString:@"ᾷ"]) //circum iota sub
            newString = [newString stringByAppendingString:@"ᾳ"];
        else if ([letter isEqualToString:@"ῇ"])
            newString = [newString stringByAppendingString:@"ῃ"];
        else if ([letter isEqualToString:@"ῷ"])
            newString = [newString stringByAppendingString:@"ῳ"];

        
        else if ([letter isEqualToString:@"ᾄ"]) //smooth acute iota sub
            newString = [newString stringByAppendingString:@"ᾀ"];
        else if ([letter isEqualToString:@"ᾔ"])
            newString = [newString stringByAppendingString:@"ῄ"];
        else if ([letter isEqualToString:@"ᾤ"])
            newString = [newString stringByAppendingString:@"ᾠ"];
        
        else if ([letter isEqualToString:@"ᾅ"]) //rough acute iota sub
            newString = [newString stringByAppendingString:@"ᾁ"];
        else if ([letter isEqualToString:@"ᾕ"])
            newString = [newString stringByAppendingString:@"ᾑ"];
        else if ([letter isEqualToString:@"ᾥ"])
            newString = [newString stringByAppendingString:@"ᾡ"];

        else if ([letter isEqualToString:@"ᾆ"]) //smooth circum iota sub
            newString = [newString stringByAppendingString:@"ᾀ"];
        else if ([letter isEqualToString:@"ᾖ"])
            newString = [newString stringByAppendingString:@"ῄ"];
        else if ([letter isEqualToString:@"ᾦ"])
            newString = [newString stringByAppendingString:@"ᾠ"];
        
        else if ([letter isEqualToString:@"ᾇ"]) //rough circum iota sub
            newString = [newString stringByAppendingString:@"ᾁ"];
        else if ([letter isEqualToString:@"ᾗ"])
            newString = [newString stringByAppendingString:@"ᾑ"];
        else if ([letter isEqualToString:@"ᾧ"])
            newString = [newString stringByAppendingString:@"ᾡ"];
        
        else if ([word characterAtIndex:i] == COMBINING_ACUTE) //combining acute
            continue;
        
        else
            newString = [newString stringByAppendingString:letter];
        //if ([self isConsonant:letter])
        //    continue;
        //unichar letter = [word characterAtIndex:i];
    }
    return newString;
}
*/

@end
