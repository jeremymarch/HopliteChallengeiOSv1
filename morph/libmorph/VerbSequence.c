//
//  VerbSequence.c
//  Hoplite Challenge
//
//  Created by Jeremy on 2/1/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//
#include <stdlib.h> // For random(), RAND_MAX
#include "VerbSequence.h"

void randomAlternative(char *s, int *offset);

static int verbSeq = 99999; //start more than repsPerVerb so we reset

void resetVerbSeq()
{
    verbSeq = 99999;
}

long lastInitialDegreesToChange = 0;
int nextVerbSeq(int *seq, VerbFormC *vf1, VerbFormC *vf2, VerbSeqOptions *vso)
{
    static Verb *v;
    static Verb *lastV = NULL;
    static VerbFormC lastVF;
    
    int bufferLen = 2048;
    char buffer[bufferLen];
    long degreesToChange = vso->degreesToChange;
    
    if (verbSeq >= vso->repsPerVerb)
    {
        do //so we don't ask the same verb twice in a row
        {
            v = getRandomVerb(vso->units, vso->numUnits);
        } while (v == lastV);
        lastV = v;
        
        verbSeq = 1;
    }
    else
    {
        verbSeq++;
    }
    *seq = verbSeq;
    vf1->verb = v;
    
    int highestUnit = 0;
    for (int i = 0; i < vso->numUnits; i++)
    {
        if (vso->units[i] > highestUnit)
            highestUnit = vso->units[i];
    }
    
    if (vso->startOnFirstSing && verbSeq == 1)
    {
        vf1->person = FIRST;
        vf1->number = SINGULAR;
        vf1->tense = PRESENT;
        vf1->voice = ACTIVE;
        vf1->mood = INDICATIVE;
        
        //doesn't work if verb is deponent
        if (!getForm(vf1, buffer, bufferLen, false, false))
        {
            vf1->voice = MIDDLE;
            getForm(vf1, buffer, bufferLen, false, false);
        }
    }
    else
    {
        /*
         do
         {
         generateForm(vf1);
         
         } while (!getForm(vf1, buffer, bufferLen, false, false) || !isValidFormForUnit(vf1, highestUnit) || !strncmp(buffer, "—", 1));
         */
        vf1->person = lastVF.person;
        vf1->number = lastVF.number;
        vf1->tense = lastVF.tense;
        vf1->voice = lastVF.voice;
        vf1->mood = lastVF.mood;
        vf1->verb = lastVF.verb;
        
        //we assume this is valid since it was the resulting form from last seq.
        //getForm(vf1, buffer, bufferLen, false, false);
    }

    do
    {
        if (verbSeq == 1)
        {
            int limit = 1000;
            do
            {
                degreesToChange = randWithMax(4) + 2; //2-5
                limit--;
            } while (degreesToChange == lastInitialDegreesToChange && limit > 0); //for variety
            if (limit == 0)
            {
                degreesToChange = 2;
            }
            
            lastInitialDegreesToChange = degreesToChange;
        }
        
        //these need to be in the loop, so we're always starting from the same place
        vf2->person = vf1->person;
        vf2->number = vf1->number;
        vf2->tense = vf1->tense;
        vf2->voice = vf1->voice;
        vf2->mood = vf1->mood;
        vf2->verb = vf1->verb;
        
        changeFormByDegrees(vf2, degreesToChange);
    } while (!getForm(vf2, buffer, bufferLen, true, false) || !isValidFormForUnit(vf2, highestUnit) || !strncmp(buffer, "—", 1));
    
    lastVF.person = vf2->person;
    lastVF.number = vf2->number;
    lastVF.tense = vf2->tense;
    lastVF.voice = vf2->voice;
    lastVF.mood = vf2->mood;
    lastVF.verb = vf2->verb;
    
    
    //temp
    if(0)//verbSeq == 2)
    {
        return VERB_SEQ_PP;
    }
    
    return VERB_SEQ_CHANGE;
}

void changeFormByDegrees(VerbFormC *vf, int degrees)
{
    unsigned char tempPerson;
    unsigned char tempNumber;
    unsigned char tempTense;
    unsigned char tempVoice;
    unsigned char tempMood;
    
    int components[degrees];
    
    do
    {
        tempPerson = vf->person;
        tempNumber = vf->number;
        tempTense = vf->tense;
        tempVoice = vf->voice;
        tempMood = vf->mood;
        
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
                tempPerson = incrementValue(NUM_PERSONS, vf->person);
            }
            else if (v == NUMBER)
            {
                tempNumber = incrementValue(NUM_NUMBERS, vf->number);
            }
            else if (v == TENSE)
            {
                tempTense = incrementValue(NUM_TENSES, vf->tense);
            }
            else if (v == VOICE)
            {
                tempVoice = incrementValue(NUM_VOICES, vf->voice);
            }
            else if (v == MOOD)
            {
                tempMood = incrementValue(NUM_MOODS, vf->mood);
            }
        }
    } //make sure form is valid and this verb has the required principal part,
    //and make sure we're not changing from mid to pass or vice versa unless the tense is aorist or future
    while (!formIsValidReal(tempPerson, tempNumber, tempTense, tempVoice, tempMood) || getPrincipalPartForTense(vf->verb, tempTense, tempVoice)[0] == '\0' );
    
    /*
     || ((tempTense != AORIST && tempTense != FUTURE) && ((vf->voice == MIDDLE && tempVoice == PASSIVE) || (vf->voice == PASSIVE && tempVoice == MIDDLE)))
     */
    
    
    
    
    vf->person = tempPerson;
    vf->number = tempNumber;
    vf->tense = tempTense;
    vf->voice = tempVoice;
    vf->mood = tempMood;
}

//unit is the highest unit we're up to
bool isValidFormForUnit(VerbFormC *vf, int unit)
{
    if (unit <= 2)
    {
        if (vf->tense == PERFECT || vf->tense == PLUPERFECT || vf->voice != ACTIVE || vf->mood != INDICATIVE)
            return false;
    }
    else if (unit <= 4)
    {
        if (vf->voice != ACTIVE || vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 5)
    {
        if (vf->voice == MIDDLE || vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 7)
    {
        if (vf->mood == IMPERATIVE)
            return false;
    }
    else if (unit <= 11)
    {
        return true;
    }
    else if (unit <= 12)
    {
        if ((utf8HasSuffix(vf->verb->present, "μι") && vf->tense == AORIST) || (utf8HasSuffix(vf->verb->present, "στημι") && (vf->tense == AORIST || vf->tense == PERFECT || vf->tense == PLUPERFECT)))
            return false;
    }
    
    return true;
}

//sort with weights going from smallest to largest, they will be ints who add up to 100
//http://stackoverflow.com/questions/8529665/changing-probability-of-getting-a-random-number
int chooseRandomFromArrayWithWeighting(int *values, int len, int *weights)
{
    //or make weights a double? and out of 1?
    long rand = randWithMax(100);
    for (int i = 0; i < len; i++)
    {
        if (rand < weights[i])
            return values[i];
    }
    return 0; //shouldn't need this
}

void generateForm(VerbFormC *verbform)
{
    unsigned char iTense, iMood, iVoice, iPerson, iNumber;
    
    do
    {
        iTense = (unsigned char)randWithMax(NUM_TENSES);
        iMood = (unsigned char)randWithMax(NUM_MOODS);
        while ( iTense != PRESENT && iTense != AORIST && iMood != INDICATIVE )
            iMood = (unsigned char)randWithMax(NUM_MOODS);
        
        iVoice = (unsigned char)randWithMax(NUM_VOICES);
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
        iPerson = (unsigned char)randWithMax(NUM_PERSONS);
        iNumber = (unsigned char)randWithMax(NUM_NUMBERS);
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
    /*
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
     */
}

//problem if match and distractor have are alternate forms of each other.
void getDistractorsForChange(VerbFormC *orig, VerbFormC *new, int numDistractors, char *buffer)
{
    VerbFormC vf;
    int i = 0;
    int n = 0;
    int starts[numDistractors + 1];
    int numStarts = 0;
    for (i = 0; i < numDistractors + 1; i++)
    {
        starts[i] = 0;
    }
    
    i = 0;
    char tempBuffer[2048];
    int offset = 0;
    
    getForm(new, tempBuffer, 2048, false, false); //put the changed form on the buffer so no duplicates
    randomAlternative(tempBuffer, &offset);
    strncpy(&buffer[n], &tempBuffer[offset], strlen(&tempBuffer[offset]));
    n += strlen(&tempBuffer[offset]);
    strncpy(&buffer[n], "; ", 2);
    n += 2;
    
    numStarts++;
    
    do
    {
        vf.verb = new->verb;
        vf.person = new->person;
        vf.number = new->number;
        vf.tense = new->tense;
        vf.voice = new->voice;
        vf.mood = new->mood;
        
        changeFormByDegrees(&vf, 1);
        
        getForm(&vf, tempBuffer, 2048, false, false);
        offset = 0;
        randomAlternative(tempBuffer, &offset);
        
        int j = 0;
        int noMatches = 1;
        for (j = 0; j < numStarts; j++)
        {
            if (memcmp(&tempBuffer[offset], &buffer[ starts[j] ], strlen(&tempBuffer[offset])) == 0 || memcmp(&tempBuffer[offset], "—", 1) == 0)
            {
                //printf("HEREREREREEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
                noMatches = 0;
                break;
            }
        }
        
        if (noMatches == 1)
        {
            //reduce alternate forms to just one
            strncpy(&buffer[n], &tempBuffer[offset], strlen(&tempBuffer[offset]));
            starts[numStarts] = n;
            numStarts++;
            
            //printf("%s\n", tempBuffer);
            n += strlen(&tempBuffer[offset]);
            strncpy(&buffer[n], "; ", 2);
            n += 2;
            
            i++;
        }
    } while (i < numDistractors);
    buffer[n - 2] = '\0';
}

void randomAlternative(char *s, int *offset)
{
    int starts[5] = { 0,0,0,0,0 };
    int numStarts = 1;
    unsigned long lenS = strlen(s);
    
    for (int i = 0; i < lenS; i++)
    {
        if (s[i] == ',')
        {
            starts[numStarts] = i + 2;
            numStarts++;
        }
    }
    long random = randWithMax(numStarts);
    *offset = starts[random];
    
    if (random < numStarts - 1)
        s[starts[random + 1] - 2] = '\0';
}


Verb *getRandomVerb(int *units, int numUnits)
{
    int u, v;
    int verbsToChooseFrom[NUM_VERBS];
    int numVerbsToChooseFrom = 0;
    for (v = 0; v < NUM_VERBS; v++)
    {
        for (u = 0; u < numUnits; u++)
        {
            if (verbs[v].hq == units[u])
            {
                verbsToChooseFrom[numVerbsToChooseFrom] = v;
                numVerbsToChooseFrom++;
                break;
            }
        }
    }
    int verb = (int)randWithMax(numVerbsToChooseFrom);
    return &verbs[ verbsToChooseFrom[verb] ];
}

Ending *getRandomEnding(int *units, int numUnits)
{
    int u, e;
    int endingsToChooseFrom[NUM_ENDINGS];
    int numEndingsToChooseFrom = 0;
    for (e = 0; e < NUM_ENDINGS; e++)
    {
        for (u = 0; u < numUnits; u++)
        {
            if (endings[e].hq == units[u])
            {
                endingsToChooseFrom[numEndingsToChooseFrom] = e;
                numEndingsToChooseFrom++;
                break;
            }
        }
    }
    int ending = (int)randWithMax(numEndingsToChooseFrom);
    return &endings[ endingsToChooseFrom[ending] ];
}

void getRandomEndingAsString(int *units, int numUnits, char *buffer, int bufferLen)
{
    //char description[512];
    Ending *e = getRandomEnding(units, numUnits);
    
    //endingGetDescription(1, description, 512);
    
    snprintf(buffer, bufferLen, "%s; %s; %s; %s; %s; %s; %s", e->description, e->fs, e->ss, e->ts, e->fp, e->sp, e->tp);
}
