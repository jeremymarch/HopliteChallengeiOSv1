//
//  libmorph.c
//  morph
//
//  Created by Jeremy on 4/15/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen
#include <stdbool.h> //for bool type
#include "libmorph.h"
#include "GreekUnicode.h"

Verb verbs[];

void stripAccent(UCS2 *word, int *len);
char *getEnding(VerbFormC *vf, UCS2 *ending, int endingLen);
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, int tense, int voice);
void augmentStem(UCS2 *ucs2, int *len);
void stripAugmentFromPrincipalPart(UCS2 *ucs2, int *len, int tense, int voice, int mood, int presentStemInitial);
void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int *elen);
bool accentRecessive(UCS2 *tempUcs2String, int *len, int optative);
bool accentWord(UCS2 *ucs2String, int *len, int syllableToAccent, int accent);

void rightShift(UCS2 *ucs2, int *len);
void leftShift(UCS2 *ucs2, int *len);

char *persons[NUM_PERSONS] = { "first", "second", "third" };
char *personsabbrev[NUM_PERSONS] = { "1st", "2nd", "3rd" };
char *numbers[NUM_NUMBERS] = { "singular", "plural" };
char *numbersabbrev[NUM_NUMBERS] = { "sing.", "pl." };
char *tenses[NUM_TENSES] = { "present", "aorist", "perfect", "imperfect", "future", "pluperfect" };
char *tensesabbrev[NUM_TENSES] = { "pres.", "aor.", "perf.", "imp.", "fut.", "plup." };
char *voices[NUM_VOICES] = { "active", "middle", "passive" };
char *voicesabbrev[NUM_VOICES] = { "act.", "mid.", "pass." };
char *moods[NUM_MOODS] = { "indicative", "subjunctive", "optative" };
char *moodsabbrev[NUM_MOODS] = { "ind.", "subj.", "opt." };

Ending endings[NUM_ENDINGS] = {
    { 0, 2, 0, 0, 0, "ω", "εις", "ει", "ομεν", "ετε", "ουσι(ν)" },
    { 0, 2, 0, 0, 0, "ον", "ες", "ε(ν)", "ομεν", "ετε", "ον" },
    { 0, 2, 0, 0, 0, "α", "ας", "ε(ν)", "αμεν", "ατε", "αν" },
    { 0, 3, 0, 0, 0, "α", "ας", "ε(ν)", "αμεν", "ατε", "ᾱσι(ν)" },
    { 0, 3, 0, 0, 0, "η", "ης", "ει(ν)", "εμεν", "ετε", "εσαν" },
    { 0, 2, 0, 0, 0, "ω", "εις", "ει", "ομεν", "ετε", "ουσι(ν)" },
    { 0, 3, 0, 0, 0, "ω", "ῃς", "ῃ", "ωμεν", "ητε", "ωσι(ν)" },
    { 0, 3, 0, 0, 0, "ω", "ῃς", "ῃ", "ωμεν", "ητε", "ωσι(ν)" },
    { 0, 3, 0, 0, 0, "οιμι", "οις", "οι", "οιμεν", "οιτε", "οιεν" },
    { 0, 3, 0, 0, 0, "αιμι", "αις, ειας", "αι, ειε(ν)", "αιμεν", "αιτε", "αιεν, ειαν" },
    { 0, 5, 0, 0, 0, "ομαι", "ει, ῃ", "εται", "ομεθα", "εσθε", "ονται" },
    { 0, 5, 0, 0, 0, "ομην", "ου", "ετο", "ομεθα", "εσθε", "οντο" },
    { 0, 5, 0, 0, 0, "ην", "ης", "η", "ημεν", "ητε", "ησαν" },
    { 0, 7, 0, 0, 0, "αμην", "ω", "ατο", "αμεθα", "ασθε", "αντο" },
    { 0, 5, 0, 0, 0, "ῶ", "ῇς", "ῇ", "ῶμεν", "ῆτε", "ῶσι(ν)" },
    { 0, 5, 0, 0, 0, "ειην", "ειης", "ειη", "εῖμεν, ειημεν", "εῖτε, ειητε", "εῖεν, ειησαν" },
    { 0, 7, 0, 0, 0, "ωμαι", "ῃ", "ηται", "ωμεθα", "ησθε", "ωνται" },
    { 0, 7, 0, 0, 0, "αιμην", "αιο", "αιτο", "αιμεθα", "αισθε", "αιντο" },
    { 0, 5, 0, 0, 0, "μαι", "σαι", "ται", "μεθα", "σθε", "νται" },
    { 0, 5, 0, 0, 0, "μην", "σο", "το", "μεθα", "σθε", "ντο" },
    { 0, 5, 0, 0, 0, "ωμαι", "ῃ", "ηται", "ωμεθα", "ησθε", "ωνται" },
    { 0, 5, 0, 0, 0, "οιμην", "οιο", "οιτο", "οιμεθα", "οισθε", "οιντο" },
    { 0, 11, 0, 0, 0, "", "ε", "ετω",   "", "ετε", "οντων" },
    { 0, 11, 0, 0, 0, "", "ου", "εσθω", "", "εσθε", "εσθων" },
    { 0, 11, 0, 0, 0, "", "ον", "ατω",  "", "ατε", "αντων" },
    { 0, 11, 0, 0, 0, "", "αι", "ασθω", "", "ασθε", "ασθων" },
    { 0, 11, 0, 0, 0, "", "ητι, ηθι", "ητω", "", "ητε", "εντων" },
    { 0, 5, 0, 0, 0, "ομαι", "ει, ῃ", "εται", "ομεθα", "εσθε", "ονται" }
};

Verb *getRandomVerb()
{
    return &verbs[randWithMax(NUM_VERBS)];
}

void getFullDescription (VerbFormC *vf, char *buffer, int len)
{
    snprintf(buffer, len, "%s %s %s %s %s", persons[vf->person], numbers[vf->number], tenses[vf->tense], voices[vf->voice], moods[vf->mood]);
}

void getAbbrevDescription (VerbFormC *vf, char *buffer, int len)
{
    snprintf(buffer, len, "%s %s %s %s %s", personsabbrev[vf->person], numbersabbrev[vf->number], tensesabbrev[vf->tense], voicesabbrev[vf->voice], moodsabbrev[vf->mood]);
}

char *getEnding(VerbFormC *vf, UCS2 *word, int wordLen)
{
    int secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    int secondAorist2[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    int ending = 0;
    if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = PRESENT_ACTIVE_IND;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = IMPERFECT_ACTIVE_IND;
/* SECOND AORIST */
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == INDICATIVE && hasSuffix(word, wordLen, secondAorist, 2))
        ending = IMPERFECT_ACTIVE_IND;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == INDICATIVE && (hasSuffix(word, wordLen, secondAorist, 2) || hasSuffix(word, wordLen, secondAorist2, 4)))
        ending = IMPERFECT_MIDPASS_IND;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && hasSuffix(word, wordLen, secondAorist, 2))
        ending = PRESENT_ACTIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == OPTATIVE && (hasSuffix(word, wordLen, secondAorist, 2) || hasSuffix(word, wordLen, secondAorist2, 4)))
        ending = PRESENT_MIDPASS_OPT;
/* SECOND AORIST */
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = AORIST_ACTIVE_IND;
    else if (vf->tense == PERFECT && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = PERFECT_ACTIVE_IND;
    else if (vf->tense == PLUPERFECT && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = PLUPERFECT_ACTIVE_IND;
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && vf->mood == INDICATIVE)
        ending = FUTURE_ACTIVE_IND;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE)
        ending = PRESENT_ACTIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE)
        ending = AORIST_ACTIVE_SUBJ;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE)
        ending = PRESENT_ACTIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE)
        ending = AORIST_ACTIVE_OPT;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE)
        ending = PRESENT_MIDPASS_IND;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE)
        ending = IMPERFECT_MIDPASS_IND;
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == INDICATIVE)
        ending = AORIST_PASSIVE_IND;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == INDICATIVE)
        ending = AORIST_MID_IND;
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == SUBJUNCTIVE)
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == OPTATIVE)
        ending = AORIST_PASSIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == SUBJUNCTIVE)
        ending = AORIST_MIDDLE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == OPTATIVE)
        ending = AORIST_MIDDLE_OPT;
    else if (vf->tense == PERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE)
        ending = PERFECT_MIDPASS_IND;
    else if (vf->tense == PLUPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE)
        ending = PLUPERFECT_MIDPASS_IND;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE)
        ending = PRESENT_MIDPASS_SUBJ;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE)
        ending = PRESENT_MIDPASS_OPT;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE)
        ending = PRESENT_ACTIVE_IMPERATIVE;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE)
        ending = PRESENT_MIDPASS_IMPERATIVE;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE)
        ending = AORIST_ACTIVE_IMPERATIVE;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == IMPERATIVE)
        ending = AORIST_MIDDLE_IMPERATIVE;
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == IMPERATIVE)
        ending = AORIST_PASSIVE_IMPERATIVE;
    else if (vf->tense == FUTURE && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE)
        ending = FUTURE_MIDPASS_IND;
    else
        return NULL;
    
    if (vf->person == FIRST && vf->number == SINGULAR)
        return endings[ending].fs;
    else if (vf->person == SECOND && vf->number == SINGULAR)
        return endings[ending].ss;
    else if (vf->person == THIRD && vf->number == SINGULAR)
        return endings[ending].ts;
    else if (vf->person == FIRST && vf->number == PLURAL)
        return endings[ending].fp;
    else if (vf->person == SECOND && vf->number == PLURAL)
        return endings[ending].sp;
    else if (vf->person == THIRD && vf->number == PLURAL)
        return endings[ending].tp;
    else
        return NULL;
    /*
    if ( mood == INFINITIVE)
    {
        if (tense == PRESENT && voice == ACTIVE )
            return @"ειν";
        if (tense == PRESENT && ( voice == MIDDLE || voice == PASSIVE ))
            return @"εσθαι";
        if (tense == AORIST && voice == ACTIVE )
            return @"αι";
        if (tense == AORIST && voice == PASSIVE )
            return @"ῆναι";
        if (tense == AORIST && voice == MIDDLE )
            return @"ασθαι";
        if (tense == PERFECT && voice == ACTIVE )
            return @"έναι";
        else
            return @"σθαι";
    }
    */
}

char * getPrincipalPartForTense(Verb *verb, int tense, int voice)
{
    //also need to consider deponent verbs
    if (tense == PRESENT || tense == IMPERFECT)
        return verb->present;
    else if ( tense == FUTURE && voice != PASSIVE)
        return verb->future;
    else if ( tense == FUTURE)
        return verb->aoristpass;
    else if (tense == AORIST && voice == PASSIVE)
        return verb->aoristpass;
    else if (tense == AORIST)
        return verb->aorist;
    else if (voice == ACTIVE && ( tense == PERFECT || tense == PLUPERFECT))
        return verb->perf;
    else
        return verb->perfmid;
}

void changeFormByDegrees(VerbFormC *verbform, int degrees)
{
    int tempPerson;
    int tempNumber;
    int tempTense;
    int tempVoice;
    int tempMood;
    
    int components[degrees];
    
    do
    {
        tempPerson = verbform->person;
        tempNumber = verbform->number;
        tempTense = verbform->tense;
        tempVoice = verbform->voice;
        tempMood = verbform->mood;
        
        //re-initialize components array to invalid values
        for (int i = 0; i < degrees; i++)
            components[i] = -1;
        
        for (int i = 0; i < degrees; i++)
        {
            int n = (int)randWithMax(5);
            bool notAlreadyIn = true;
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
            int v = components[i];
            
            if (v == PERSON)
            {
                tempPerson = incrementValue(NUM_PERSONS, verbform->person);
            }
            else if (v == NUMBER)
            {
                tempNumber = incrementValue(NUM_NUMBERS, verbform->number);
            }
            else if (v == TENSE)
            {
                tempTense = incrementValue(NUM_TENSES, verbform->tense);
            }
            else if (v == VOICE)
            {
                tempVoice = incrementValue(NUM_VOICES, verbform->voice);
            }
            else if (v == MOOD)
            {
                tempMood = incrementValue(NUM_MOODS, verbform->mood);
            }
        }
    } //make sure form is valid and this verb has the required principal part
    while(!formIsValidReal((int)tempPerson, (int)tempNumber, (int)tempTense, (int)tempVoice, (int)tempMood) || getPrincipalPartForTense(verbform->verb, tempTense, tempVoice)[0] == '\0');
    
    verbform->person = tempPerson;
    verbform->number = tempNumber;
    verbform->tense = tempTense;
    verbform->voice = tempVoice;
    verbform->mood = tempMood;
}

void generateForm(VerbFormC *verbform)
{
    int iTense, iMood, iVoice, iPerson, iNumber;
    
    do
    {
        iTense = (int)randWithMax(NUM_TENSES);
        iMood = (int)randWithMax(NUM_MOODS);
        while ( (iTense > 1 && iMood > 0 ) )
            iMood = (int)randWithMax(NUM_MOODS);
        
        iVoice = (int)randWithMax(NUM_VOICES);
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
        iPerson = (int)randWithMax(NUM_PERSONS);
        iNumber = (int)randWithMax(NUM_NUMBERS);
        //}
        
        //NSArray conj = [NSArray arrayWithObjects: [NSNumber  v], nil];
        
        //NSUInteger randomIndex = randWithMax([theArray count]);
        //NSString *form = [NSString stringWithFormat:@"%@ %@ %@ %@ %@", (iPerson > -1) ? [[self persons] objectAtIndex: iPerson] : @"", (iNumber > -1) ? [[self numbers] objectAtIndex: iNumber] : @"", [[self tenses] objectAtIndex: iTense], [[self voices] objectAtIndex: iVoice], [[self moods] objectAtIndex: iMood]];
    }
    while (!formIsValidReal(iPerson, iNumber, iTense, iVoice, iMood) || getPrincipalPartForTense(verbform->verb, iTense, iVoice)[0] == '\0');
    
    verbform->person = iPerson;
    verbform->number = iNumber;
    verbform->tense = iTense;
    verbform->voice = iVoice;
    verbform->mood = iMood;
}

int incrementValue(int theArrayCount, int start)
{
    long n = randWithMax((theArrayCount - 1));
    for (int i = 0; i < n + 1; i++)
    {
        if (start < theArrayCount - 1)
            start++;
        else
            start = 0;
    }
    return start;
}

//From: http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
long randWithMax(unsigned int max)
{
    //return arc4random() % max;
    return arc4random_uniform(max);
    
    unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;
    
    long x;
    do {
        x = random();
    }
    // This is carefully written not to overflow
    while (num_rand - defect <= (unsigned long)x);
    
    // Truncated division is intentional
    return x/bin_size;
}

int getForm(VerbFormC *vf, char *buffer)
{
    char *utf8Stem = getPrincipalPartForTense(vf->verb, vf->tense, vf->voice);
    
    UCS2 ucs2[(strlen(utf8Stem) * 3) + 1];
    int len = 0;
    utf8_to_ucs2_string((const unsigned char*)utf8Stem, ucs2, &len);
    
    stripAccent(ucs2, &len);
    char *utf8Ending = getEnding(vf, ucs2, len);
    stripEndingFromPrincipalPart(ucs2, &len, (int)vf->tense, (int)vf->voice);
    
    if (vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
    {
        augmentStem(ucs2, &len);
    }
    
    if ( (vf->tense == AORIST && (vf->mood == SUBJUNCTIVE || vf->mood == OPTATIVE)) || (vf->tense == FUTURE && vf->voice == PASSIVE))
    {
        UCS2 ucs22[(strlen(vf->verb->present) * 2) + 1];
        int len2 = 0;
        utf8_to_ucs2_string((const unsigned char*)vf->verb->present, ucs22, &len2);
        stripAccent(ucs22, &len2);
        int presentInitialLetter = ucs22[0];
        
        stripAugmentFromPrincipalPart(ucs2, &len, (int)vf->tense, (int)vf->voice, (int)vf->mood, presentInitialLetter);
    }
    

    UCS2 ending[(strlen(utf8Ending) * 3) + 1];
    int elen = 0;
    utf8_to_ucs2_string((const unsigned char*)utf8Ending, ending, &elen);
    
    addEnding(vf, ucs2, &len, ending, &elen);
    
    //if ending does not already have an accent
    if (!(vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == SUBJUNCTIVE))
        accentRecessive(ucs2, &len, 0);
    
    ucs2_to_utf8_string(ucs2, len, (unsigned char *)buffer);
    
    return 1;
}

bool accentRecessive(UCS2 *tempUcs2String, int *len, int optative)
{
    //find syllable
    int i = 0;
    int vowelsAndDiphthongs = 0;
    int longUltima = false;
    int longPenult = false;
    for ( i = *len - 1; i >= 0 ; i--) //start at end of word and work left
    {
        if (isVowel(tempUcs2String[i]))
        {
            ++vowelsAndDiphthongs; //i.e. syllables
            
            if (isLong(tempUcs2String[i]))
            {
                if (vowelsAndDiphthongs == 1)
                    longUltima = true;
                else if (vowelsAndDiphthongs == 2)
                    longPenult = true;
            }
            else if (isSecondVowelOfDiphthong(tempUcs2String, *len, i))
            {
                if (vowelsAndDiphthongs == 1)
                {
                    //treat αι as short except for optative ending
                    if (tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA && tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA && !optative)                        longUltima = false;
                    else
                        longUltima = true;
                }
                else if (vowelsAndDiphthongs == 2)
                {
                    longPenult = true;
                }
                --i; //move past first vowel of diphthong so we don't count is as a separate syllable
            }
            else if (i < *len - 1 && tempUcs2String[i + 1] == COMBINING_MACRON)
            {
                if (vowelsAndDiphthongs == 1)
                    longUltima = true;
                else if (vowelsAndDiphthongs == 2)
                    longPenult = true;
            }
        }
    }
    
    //NSLog(@"syl: %d, %d, %d", vowelsAndDiphthongs, longPenult, longUltima);
    if (vowelsAndDiphthongs == 1)
    {
        //acute on ultima
        accentWord(tempUcs2String, len, ULTIMA, ACUTE);
    }
    else if (vowelsAndDiphthongs == 2 && longPenult && !longUltima)
    {
        //circumflex on penult
        accentWord(tempUcs2String, len, PENULT, CIRCUMFLEX);
    }
    else if (vowelsAndDiphthongs > 2 && !longUltima)
    {
        //acute on the antepenult
        accentWord(tempUcs2String, len, ANTEPENULT, ACUTE);
    }
    else
    {
        //acute on the penult
        accentWord(tempUcs2String, len, PENULT, ACUTE);
    }

    return true;
}

//return false if not enough syllables
bool accentWord(UCS2 *ucs2String, int *len, int syllableToAccent, int accent)
{
    //find syllable
    int i = 0;
    int vowelsAndDiphthongs = 0;
    for ( i = *len - 1; i >= 0 ; i--)
    {
        if (isVowel(ucs2String[i]))
        {
            ++vowelsAndDiphthongs;
            if ((syllableToAccent == ULTIMA && vowelsAndDiphthongs == 1) || (syllableToAccent == PENULT && vowelsAndDiphthongs == 2) || (syllableToAccent == ANTEPENULT && vowelsAndDiphthongs == 3))
            {
                break;
            }
            
            if (isSecondVowelOfDiphthong(ucs2String, *len, i))
                --i;
        }
    }
    
    //add accent to syllable
    //add cases for if has breathing or iota subscript etc.
    if (accent == ACUTE)
    {
        if (i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON)
        {
            for (int j = *len - 1; j > i + 1; j--)
                ucs2String[j + 1] = ucs2String[j];
            
            ucs2String[i + 2] = COMBINING_ACUTE;
            (*len)++;
            //NSLog(@"macron");
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
    }
    else if (accent == CIRCUMFLEX)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
    }
    return true;
}

void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int *elen)
{
    if (vf->tense == PERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE))
    {
        if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_1) //γέγραμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_PSI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_PI;
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len-1] = GREEK_SMALL_LETTER_PHI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_2) //πέπεμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PSI;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PI;
                ucs2[*len+1] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 4;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PHI;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 3;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_3) //πεφύλαγμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_XI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_KAPPA;
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len-1] = GREEK_SMALL_LETTER_CHI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_4) //κεκέλευσμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 3;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
    }
    else if (vf->tense == PLUPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE))
    {
        if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_1) //γέγραμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_NU;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_PSI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 1;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_PI;
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 2;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len-1] = GREEK_SMALL_LETTER_PHI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_2) //πέπεμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_NU;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PSI;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PI;
                ucs2[*len+1] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 3;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_PHI;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 3;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_3) //πεφύλαγμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_NU;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_XI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 1;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_KAPPA;
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 2;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[*len-1] = GREEK_SMALL_LETTER_CHI;
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
        else if (vf->verb->verbclass == CONSONANT_STEM_PERFECT_4) //κεκέλευσμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_ETA;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_NU;
                (*len) += 3;
                return;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_IOTA;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_TAU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 2;
                return;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                ucs2[*len] = GREEK_SMALL_LETTER_MU;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                ucs2[(*len)+2] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+3] = GREEK_SMALL_LETTER_ALPHA;
                (*len) += 4;
                return;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = 0x2014; //EM DASH
                (*len) = 1;
                return;
            }
        }
    }
    else if (vf->tense == FUTURE && vf->voice == PASSIVE)
    {
        ucs2[*len] = GREEK_SMALL_LETTER_ETA;
        ucs2[(*len) + 1] = GREEK_SMALL_LETTER_SIGMA;
        (*len) += 2; //parens required here fyi
    }
    
    int i = 0;
    int j = 0;
    for (i = *len; j < *elen; i++, j++)
    {
        ucs2[i] = ending[j];
        ++(*len);
    }
}

void stripAugmentFromPrincipalPart(UCS2 *ucs2, int *len, int tense, int voice, int mood, UCS2 presentStemInitial)
{
    if (tense == AORIST && (mood == SUBJUNCTIVE || mood == OPTATIVE))
    {
        if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
        {
            //NSLog(@"removed augment");
            leftShift(ucs2, len);
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
        }
    }
    else if (tense == FUTURE && voice == PASSIVE)
    {
        if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
        {
            //NSLog(@"removed augment");
            leftShift(ucs2, len);
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
        }
    }
}

void stripAccent(UCS2 *word, int *len)
{
    int i = 0;
    for ( ; i < *len; i++)
    {
        switch (word[i])
        {
            case GREEK_SMALL_LETTER_ALPHA_WITH_OXIA:
            case GREEK_SMALL_LETTER_ALPHA_WITH_TONOS:
            case GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA;
                break;
            case GREEK_SMALL_LETTER_EPSILON_WITH_OXIA:
            case GREEK_SMALL_LETTER_EPSILON_WITH_TONOS:
                word[i] = GREEK_SMALL_LETTER_EPSILON;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_OXIA:
            case GREEK_SMALL_LETTER_ETA_WITH_TONOS:
            case GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ETA;
                break;
            case GREEK_SMALL_LETTER_IOTA_WITH_OXIA:
            case GREEK_SMALL_LETTER_IOTA_WITH_TONOS:
            case GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_IOTA;
                break;
            case GREEK_SMALL_LETTER_OMICRON_WITH_OXIA:
            case GREEK_SMALL_LETTER_OMICRON_WITH_TONOS:
                word[i] = GREEK_SMALL_LETTER_OMICRON;
                break;
            case GREEK_SMALL_LETTER_UPSILON_WITH_OXIA:
            case GREEK_SMALL_LETTER_UPSILON_WITH_TONOS:
            case GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_UPSILON;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_OXIA:
            case GREEK_SMALL_LETTER_OMEGA_WITH_TONOS:
            case GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA:
            case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA:
            case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA:
                word[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA:
                word[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA:
            case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA:
            case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA:
            case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA:
            case GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA:
                word[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA:
                word[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA:
            case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA:
            case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA:
            case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA:
            case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
                break;
            case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
            case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
                word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
                break;
            case COMBINING_ACUTE:
                //case COMBINING_MACRON:
                //remove combining accent, shift other characters over 1
                //change this to use leftShift function.  add later when I can test it out.
                for (int j = i; j < *len; j++)
                {
                    word[j] = word[j + 1];
                }
                --(*len);
                break;
            default:
                break;
        }
    }
}

//accents should be stripped before calling this
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, int tense, int voice)
{
    int presDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    int aoristDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    int secondAoristDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    int secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    
    if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presDeponent, 4)) //ομαι
        *len -= 4;
    else if (tense == PRESENT || tense == IMPERFECT)
        *len -= 1;
    else if (tense == FUTURE  && voice != PASSIVE && hasSuffix(stem, *len, presDeponent, 4)) //ομαι
        *len -= 4;
    else if (tense == FUTURE && voice != PASSIVE)
        *len -= 1;
    else if (tense == FUTURE && voice == PASSIVE)
        *len -= 2;
    else if (tense == AORIST && voice != PASSIVE  && hasSuffix(stem, *len, aoristDeponent, 4)) //άμην
        *len -= 4;
    else if (tense == AORIST && voice != PASSIVE && hasSuffix(stem, *len, secondAorist, 2))
        *len -= 2;
    else if (tense == AORIST && voice != PASSIVE && hasSuffix(stem, *len, secondAoristDeponent, 4))
        *len -= 4;
    else if (tense == AORIST && voice != PASSIVE)
        *len -= 1;
    else if (tense == AORIST && voice == PASSIVE)
        *len -= 2;
    else if (tense == PERFECT && voice == ACTIVE)
        *len -= 1;
    else if (tense == PLUPERFECT && voice == ACTIVE)
        *len -= 1;
    else if (tense == PERFECT)
        *len -= 3;
    else if (tense == PLUPERFECT)
        *len -= 3;
}

//we assume accents have been stripped, but not breathings, macrons or iota subscripts
void augmentStem(UCS2 *ucs2, int *len)
{
    if (ucs2[0] == GREEK_SMALL_LETTER_ALPHA)
    {
        if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
        else
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON)
    {
        if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
        else
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA)
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
    {
        rightShift(ucs2, len);
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = GREEK_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = GREEK_DASIA;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON)
    {
        if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShift(ucs2, len);
            ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
        }
        else
        {
            ucs2[0] = GREEK_SMALL_LETTER_OMEGA;
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
    {
        ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
    }
    
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON)
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
    {
        rightShift(ucs2, len);
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = GREEK_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = GREEK_DASIA;
    }
    else if (isConsonant(ucs2[0]))
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
    }
}

bool isVowel(UCS2 l)
{
    switch (l)
    {
        case GREEK_SMALL_LETTER_ALPHA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_ALPHA:
        case GREEK_CAPITAL_LETTER_ALPHA_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_ALPHA_WITH_PSILI:
        case GREEK_SMALL_LETTER_EPSILON:
        case GREEK_SMALL_LETTER_EPSILON_WITH_DASIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_EPSILON:
        case GREEK_CAPITAL_LETTER_EPSILON_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_EPSILON_WITH_PSILI:
        case GREEK_SMALL_LETTER_ETA:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI:
        case GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI:
        case GREEK_CAPITAL_LETTER_ETA:
        case GREEK_CAPITAL_LETTER_ETA_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_ETA_WITH_PSILI:
        case GREEK_SMALL_LETTER_IOTA:
        case GREEK_SMALL_LETTER_IOTA_WITH_DASIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_IOTA:
        case GREEK_CAPITAL_LETTER_IOTA_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_IOTA_WITH_PSILI:
        case GREEK_SMALL_LETTER_OMICRON:
        case GREEK_SMALL_LETTER_OMICRON_WITH_DASIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_OMICRON:
        case GREEK_CAPITAL_LETTER_OMICRON_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_OMICRON_WITH_PSILI:
        case GREEK_SMALL_LETTER_UPSILON:
        case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_UPSILON:
        case GREEK_CAPITAL_LETTER_UPSILON_WITH_DASIA:
        case GREEK_SMALL_LETTER_OMEGA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI:
        case GREEK_CAPITAL_LETTER_OMEGA:
        case GREEK_CAPITAL_LETTER_OMEGA_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_OMEGA_WITH_PSILI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI:
            return true;
        default:
            return false;
    }
}

//combining macrons need to be handled in the loop
bool isLong(UCS2 l)
{
    switch (l)
    {
        case GREEK_SMALL_LETTER_ALPHA_WITH_MACRON:
        case GREEK_SMALL_LETTER_ETA:
        case GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA:
        case GREEK_CAPITAL_LETTER_ETA:
        case GREEK_SMALL_LETTER_IOTA_WITH_MACRON:
        case GREEK_SMALL_LETTER_UPSILON_WITH_MACRON:
        case GREEK_SMALL_LETTER_OMEGA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI:
        case GREEK_CAPITAL_LETTER_OMEGA:
            return true;
        default:
            return false;
    }
}

bool isSecondVowelOfDiphthong(UCS2 *tempUcs2String, int len, int i)
{
    //need to add for case tempUcs2String[i] vowel has diaeresis or if has combining macron
    if (tempUcs2String[i] == GREEK_SMALL_LETTER_UPSILON && i != 1 && (tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_EPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ETA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_OMICRON))
        return true;
    else if (tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA && i != 1 && (tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_EPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_UPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_OMICRON))
        return true;
    
    return false;
}

bool isConsonant(UCS2 l)
{
    switch (l)
    {
        case GREEK_SMALL_LETTER_BETA:
        case GREEK_CAPITAL_LETTER_BETA:
        case GREEK_SMALL_LETTER_GAMMA:
        case GREEK_CAPITAL_LETTER_GAMMA:
        case GREEK_SMALL_LETTER_DELTA:
        case GREEK_CAPITAL_LETTER_DELTA:
        case GREEK_SMALL_LETTER_ZETA:
        case GREEK_CAPITAL_LETTER_ZETA:
        case GREEK_SMALL_LETTER_THETA:
        case GREEK_CAPITAL_LETTER_THETA:
        case GREEK_SMALL_LETTER_KAPPA:
        case GREEK_CAPITAL_LETTER_KAPPA:
        case GREEK_SMALL_LETTER_LAMDA:
        case GREEK_CAPITAL_LETTER_LAMDA:
        case GREEK_SMALL_LETTER_MU:
        case GREEK_CAPITAL_LETTER_MU:
        case GREEK_SMALL_LETTER_NU:
        case GREEK_CAPITAL_LETTER_NU:
        case GREEK_SMALL_LETTER_XI:
        case GREEK_CAPITAL_LETTER_XI:
        case GREEK_SMALL_LETTER_PI:
        case GREEK_CAPITAL_LETTER_PI:
        case GREEK_SMALL_LETTER_RHO:
        case GREEK_CAPITAL_LETTER_RHO:
        case GREEK_SMALL_LETTER_SIGMA:
        case GREEK_CAPITAL_LETTER_SIGMA:
        case GREEK_SMALL_LETTER_FINAL_SIGMA:
        case GREEK_SMALL_LETTER_TAU:
        case GREEK_CAPITAL_LETTER_TAU:
        case GREEK_SMALL_LETTER_PHI:
        case GREEK_CAPITAL_LETTER_PHI:
        case GREEK_SMALL_LETTER_CHI:
        case GREEK_CAPITAL_LETTER_CHI:
        case GREEK_SMALL_LETTER_PSI:
        case GREEK_CAPITAL_LETTER_PSI:
            return true;
        default:
            return false;
    }
}

bool formIsValidReal(int person, int number, int tense, int voice, int mood)
{
    if ( tense == IMPERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE) )
        return false;
    if ( tense == FUTURE && (mood == SUBJUNCTIVE || mood == OPTATIVE) )
        return false;
    if ( tense == PERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE) )
        return false;
    if ( tense == PLUPERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE) )
        return false;
    
    return true;
}

//stem and suffix are ucs2 strings
bool hasSuffix(UCS2 *stem, int len, UCS2 *suffix, int sufflen)
{
    int i = len - 1;
    int j = sufflen - 1;
    
    if (sufflen > len)
        return false;
    
    for ( ; j >= 0; i--, j--)
    {
        if (stem[i] != suffix[j])
            return false;
    }
    
    return true;
}

//stem and prefix are ucs2 strings
bool hasPrefix(UCS2 *stem, int len, UCS2 *prefix, int preflen)
{
    int i = 0;
    int j = 0;
    
    if (preflen > len)
        return false;
    
    for ( ; j < preflen; i++, j++)
    {
        if (stem[i] != prefix[j])
            return false;
    }
    
    return true;
}

//Moves everything over to the left, eating the first letter
void leftShift(UCS2 *ucs2, int *len)
{
    int j = 0;
    for ( ; j < *len; j++)
    {
        ucs2[j] = ucs2[j + 1];
    }
    --(*len);
}

//we assume there is space for this
void rightShift(UCS2 *ucs2, int *len)
{
    int j = *len - 1;
    for ( ; j >= 0; j--)
    {
        ucs2[j + 1] = ucs2[j];
    }
    ++(*len);
}

/* Input: a Unicode code point, "ucs2".
 
 Output: UTF-8 characters in buffer "utf8".
 
 Return value: the number of bytes written into "utf8", or -1 if
 there was an error.
 
 This adds a zero byte to the end of the string. It assumes that the
 buffer "utf8" has at least four bytes of space to write to. */
// from http://www.lemoda.net/c/ucs2-to-utf8/
int ucs2_to_utf8 (UCS2 ucs2, unsigned char * utf8)
{
    if (ucs2 < 0x80) {
        utf8[0] = ucs2;
        utf8[1] = '\0';
        return 1;
    }
    if (ucs2 >= 0x80  && ucs2 < 0x800) {
        utf8[0] = (ucs2 >> 6)   | 0xC0;
        utf8[1] = (ucs2 & 0x3F) | 0x80;
        utf8[2] = '\0';
        return 2;
    }
    if (ucs2 >= 0x800 && ucs2 < 0xFFFF) {
        if (ucs2 >= 0xD800 && ucs2 <= 0xDFFF) {
            /* Ill-formed. */
            return UNICODE_SURROGATE_PAIR;
        }
        utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
        utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
        utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
        utf8[3] = '\0';
        return 3;
    }
    if (ucs2 >= 0x10000 && ucs2 < 0x10FFFF) {
        /* http://tidy.sourceforge.net/cgi-bin/lxr/source/src/utf8.c#L380 */
        utf8[0] = 0xF0 | (ucs2 >> 18);
        utf8[1] = 0x80 | ((ucs2 >> 12) & 0x3F);
        utf8[2] = 0x80 | ((ucs2 >> 6) & 0x3F);
        utf8[3] = 0x80 | ((ucs2 & 0x3F));
        utf8[4] = '\0';
        return 4;
    }
    return UNICODE_BAD_INPUT;
}

/* Convert a UTF-8 encoded character in "input" into a number. This
 function returns the unicode value of the UTF-8 character if
 successful, and -1 if not successful. "end_ptr" is set to the next
 character after the read character on success. "end_ptr" is set to
 the start of input on failure. "end_ptr" may not be null. */

UCS2 utf8_to_ucs2 (const unsigned char * input, const unsigned char ** end_ptr)
{
    *end_ptr = input;
    if (input[0] == 0)
        return -1;
    if (input[0] < 0x80) {
        * end_ptr = input + 1;
        return input[0];
    }
    if ((input[0] & 0xE0) == 0xE0) {
        if (input[1] == 0 || input[2] == 0)
            return -1;
        * end_ptr = input + 3;
        return
        (input[0] & 0x0F)<<12 |
        (input[1] & 0x3F)<<6  |
        (input[2] & 0x3F);
    }
    if ((input[0] & 0xC0) == 0xC0) {
        if (input[1] == 0)
            return -1;
        * end_ptr = input + 2;
        return
        (input[0] & 0x1F)<<6  |
        (input[1] & 0x3F);
    }
    return -1;
}

int ucs2_to_utf8_string(UCS2 *ucs2, int len, unsigned char *utf8)
{
    unsigned char *utf8Temp = utf8;
    for (int i = 0; i < len; i++)
    {
        int utf8Len;
        utf8Len = ucs2_to_utf8(ucs2[i], utf8Temp);
        if (utf8Len > -1)
            utf8Temp += utf8Len;
        else
            return 0;
    }
    return 1;
}

void utf8_to_ucs2_string(const unsigned char *utf8, UCS2 *ucs2, int *len)
{
    for( int i = 0; *utf8 ; i++)
    {
        ucs2[i] = utf8_to_ucs2 (utf8, &utf8);
        (*len)++;
    }
}



