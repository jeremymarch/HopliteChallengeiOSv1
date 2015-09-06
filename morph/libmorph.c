//
//  libmorph.c
//  morph
//
//  Created by Jeremy on 4/15/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen()
#include <stdbool.h> //for bool type
#include "libmorph.h"
#include "GreekForms.h"
#include "GreekUnicode.h"

/*
 TO DO:
 verb prefixes
 check the randomVerb units code
 
 *remember, don't copy and paste unicode files into android studio, copy and paste file in finder
 */

bool letterIsAccented(UCS2 letter);
void stripAccent(UCS2 *word, int *len);
char *getEnding(VerbFormC *vf, UCS2 *ending, int endingLen, bool contractedFuture);
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, unsigned char tense, unsigned char voice);
void augmentStem(VerbFormC *vf, UCS2 *ucs2, int *len);
void stripAugmentFromPrincipalPart(UCS2 *ucs2, int *len, unsigned char tense, unsigned char voice, unsigned char mood, UCS2 presentStemInitial);
void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int elen);
bool accentRecessive(UCS2 *tempUcs2String, int *len, bool optative);
bool accentWord(UCS2 *ucs2String, int *len, int syllableToAccent, int accent);
bool isContractedVerb(VerbFormC *vf, UCS2 *ucs2, int *len);

void rightShift(UCS2 *ucs2, int *len);
void leftShift(UCS2 *ucs2, int *len);

char *persons[NUM_PERSONS] = { "first", "second", "third" };
char *personsabbrev[NUM_PERSONS] = { "1st", "2nd", "3rd" };
char *numbers[NUM_NUMBERS] = { "singular", "plural" };
char *numbersabbrev[NUM_NUMBERS] = { "sing.", "pl." };
char *tenses[NUM_TENSES] = { "present", "imperfect", "future", "aorist", "perfect", "pluperfect" };
char *tensesabbrev[NUM_TENSES] = { "pres.", "imp.", "fut.", "aor.", "perf.", "plup." };
char *voices[NUM_VOICES] = { "active", "middle", "passive" };
char *voicesabbrev[NUM_VOICES] = { "act.", "mid.", "pass." };
char *moods[NUM_MOODS] = { "indicative", "subjunctive", "optative", "imperative", "infinitive" };
char *moodsabbrev[NUM_MOODS] = { "ind.", "subj.", "opt.", "imper." };

void endingGetDescription(int e, char *buffer, int bufferLen)
{
    char present[] = "Present";
    char aorist[] = "Aorist";
    char perfect[] = "Perfect";
    
    snprintf(buffer, bufferLen, "%s %s %s", "Present", "Active", "Subjunctive");
}

/*
void VFToShort(VerbFormC *vf, unsigned short *s)
{
    
}

void shortToVF(unsigned short s, VerbFormC *vf)
{
    if ( (s & PRESENT_) == PRESENT_)
        vf->tense = PRESENT;
    else if (s & FUTURE_) == FUTURE_)
        vf->tense = FUTURE;
}
*/
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

void getPrincipalParts(Verb *v, char *buffer, int len)
{
    snprintf(buffer, len, "%s; %s; %s; %s; %s; %s", v->present, v->future, v->aorist, v->perf, v->perfmid, v->aoristpass);
}

void getFullDescription (VerbFormC *vf, char *buffer, int len)
{
    snprintf(buffer, len, "%s %s %s %s %s", persons[vf->person], numbers[vf->number], tenses[vf->tense], voices[vf->voice], moods[vf->mood]);
}

void getAbbrevDescription (VerbFormC *vf, char *buffer, int len)
{
    snprintf(buffer, len, "%s %s %s %s %s", personsabbrev[vf->person], numbersabbrev[vf->number], tensesabbrev[vf->tense], voicesabbrev[vf->voice], moodsabbrev[vf->mood]);
}

bool wordIsAccented(UCS2 *letter, int len)
{
    for (int i = (len - 1); i >= 0; i--) //faster to start at end
    {
        if (letterIsAccented(letter[i]))
            return true;
    }
    return false;
}

bool letterIsAccented(UCS2 letter)
{
    switch (letter)
    {
        case COMBINING_ACUTE:
            /* ALPHA */
        case GREEK_SMALL_LETTER_ALPHA_WITH_TONOS:
        case GREEK_SMALL_LETTER_ALPHA_WITH_OXIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_VARIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_VARIA:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
            /* EPSILON */
        case GREEK_SMALL_LETTER_EPSILON_WITH_TONOS:
        case GREEK_SMALL_LETTER_EPSILON_WITH_OXIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_VARIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_VARIA:
            /* ETA */
        case GREEK_SMALL_LETTER_ETA_WITH_TONOS:
        case GREEK_SMALL_LETTER_ETA_WITH_OXIA:
        case GREEK_SMALL_LETTER_ETA_WITH_VARIA:
        case GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_VARIA:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
            /* IOTA */
        case GREEK_SMALL_LETTER_IOTA_WITH_TONOS:
        case GREEK_SMALL_LETTER_IOTA_WITH_OXIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_VARIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI:
        case GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_VARIA:
        case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI:
            /* OMICRON */
        case GREEK_SMALL_LETTER_OMICRON_WITH_TONOS:
        case GREEK_SMALL_LETTER_OMICRON_WITH_OXIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_VARIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_VARIA:
            /* UPSILON */
        case GREEK_SMALL_LETTER_UPSILON_WITH_TONOS:
        case GREEK_SMALL_LETTER_UPSILON_WITH_OXIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_VARIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI:
        case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_VARIA:
        case GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI:
            /* OMEGA */
        case GREEK_SMALL_LETTER_OMEGA_WITH_TONOS:
        case GREEK_SMALL_LETTER_OMEGA_WITH_OXIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_VARIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_VARIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_VARIA:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_VARIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI:
            return true;
    }
    return false;
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
    
    getForm(new, tempBuffer, 2048, false); //put the changed form on the buffer so no duplicates
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
        
        getForm(&vf, tempBuffer, 2048, false);
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

char *getEnding(VerbFormC *vf, UCS2 *word, int wordLen, bool contractedFuture)
{
    UCS2 secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    UCS2 secondAorist2[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    int ending = 0;
    /* MI */
    if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PRESENT_ACTIVE_INDICATIVE_MI;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PERFECT_MIDPASS_IND;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = AORIST_PASSIVE_OPT;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PRESENT_MIDPASS_OPT;
    
    /* /MI */
    /* CONTRACTED FUTURES */
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture)
        ending = PRESENT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED;
    /* CONTRACTED PRESENT AND IMPERFECT */
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_INDIC_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_INDIC_A_CONTRACTED;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_ACTIVE_INDIC_A_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_MIDPASS_INDIC_A_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_SUBJ_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_SUBJ_A_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA)
        ending = PRESENT_ACTIVE_OPT_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_OPT_A_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_SUBJ_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_OPT_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_OPT_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_INDIC_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_INDIC_O_CONTRACTED;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_ACTIVE_INDIC_O_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = IMPERFECT_MIDPASS_INDIC_O_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_SUBJ_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_SUBJ_O_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_OPT_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_OPT_O_CONTRACTED;
    
    /* CONTRACTED IMPERATIVES */
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_IMPERATIVE_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_IMPERATIVE_A_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_IMPERATIVE_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_IMPERATIVE_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_ACTIVE_IMPERATIVE_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA)
        ending = PRESENT_MIDPASS_IMPERATIVE_O_CONTRACTED;
    /* /CONTRACTED IMPERATIVES */
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE)
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
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE && hasSuffix(word, wordLen, secondAorist, 2))
        ending = PRESENT_ACTIVE_IMPERATIVE;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == IMPERATIVE && (hasSuffix(word, wordLen, secondAorist, 2) || hasSuffix(word, wordLen, secondAorist2, 4)))
        ending = PRESENT_MIDPASS_IMPERATIVE;
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
        return NULL; //or return ""?
    
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
        return NULL; //or return ""?
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

char * getPrincipalPartForTense(Verb *verb, unsigned char tense, unsigned char voice)
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
    unsigned char tempPerson;
    unsigned char tempNumber;
    unsigned char tempTense;
    unsigned char tempVoice;
    unsigned char tempMood;
    
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
    while(!formIsValidReal(tempPerson, tempNumber, tempTense, tempVoice, tempMood) || getPrincipalPartForTense(verbform->verb, tempTense, tempVoice)[0] == '\0');
    
    verbform->person = tempPerson;
    verbform->number = tempNumber;
    verbform->tense = tempTense;
    verbform->voice = tempVoice;
    verbform->mood = tempMood;
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

/**
 * return 1 for success, 0 for failure
 */
int getForm(VerbFormC *vf, char *utf8OutputBuffer, int bufferLen, bool includeAlternateForms)
{
    //clear buffer
    for (int i = 0; i < bufferLen; i++)
        utf8OutputBuffer[i] = '\0';
    
    char *utf8Stems = getPrincipalPartForTense(vf->verb, vf->tense, vf->voice);
    if (!utf8Stems || utf8Stems[0] == '\0')
    {
        utf8OutputBuffer[0] = '\0'; //clear buffer
        return 0;
    }
    
    UCS2 ucs2Stems[(strlen(utf8Stems) * 3) + 1];
    int ucs2StemsLen = 0;
    utf8_to_ucs2_string((const unsigned char*)utf8Stems, ucs2Stems, &ucs2StemsLen);
    
    //find out how many stems, then how many endings.  loop through stems adding each ending in an inner loop.
    //accent each inside the loop
    
    int stemStarts[5] = { 0,0,0,0,0 };  //we leave space for up to five alternate stems
    int numStems = 1;
    int i;
    for (i = 0; i < ucs2StemsLen; i++)
    {
        if (ucs2Stems[i] == 0x0020)//space, 002C == comma
        {
            stemStarts[numStems] = i + 1;
            numStems++;
        }
    }
    stemStarts[numStems] = i + 2; //to get length of last stem. plus 2 to simulate a comma and space
    
    int stem;
    int ending;
    int stemStart = 0;
    int stemLen = 0;
    int endingStart = 0;
    int endingLen = 0;
    UCS2 ucs2StemPlusEndingBuffer[1024];
    //buffer needs to be cleared
    for (i = 0; i < 1024; i++)
        ucs2StemPlusEndingBuffer[i] = 0;
    
    int ucs2StemPlusEndingBufferLen = 0;

    for (stem = 0; stem < numStems; stem++)
    {
        bool contractedFuture = false;
        stemStart = stemStarts[stem];
        stemLen = stemStarts[stem + 1] - stemStarts[stem] - 2;
        
        if (vf->tense == FUTURE && ucs2Stems[stemLen - 1] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
        {
            contractedFuture = true;
        }
        
        stripAccent(&ucs2Stems[stemStart], &stemLen);
        
        //eliminate FUTURE PASSIVE blaphthhsomai here
        //NB: accent is already stripped by now
        UCS2 blaph[8] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_BETA, GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_PHI, GREEK_SMALL_LETTER_THETA, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
        if (vf->tense == FUTURE && vf->voice == PASSIVE && stemLen == 8 && hasPrefix(&ucs2Stems[stemStart], 8, blaph, 8))
        {
            continue;
        }
        
        if ( vf->voice == ACTIVE && isDeponent(vf, &ucs2Stems[stemStart], stemLen) )
        {
            continue;
        }
        
        //This needs to be in the stems loop.  What if the stems require different endings?
        char *utf8Ending = getEnding(vf, ucs2Stems, ucs2StemsLen, contractedFuture); //get ending here before stripping from pp, so know if 2nd aorist
        if (!utf8Ending)
            return 0;
        
        UCS2 ucs2Endings[(strlen(utf8Ending) * 3) + 1];
        int ucs2EndingsLen = 0;
        utf8_to_ucs2_string((const unsigned char*)utf8Ending, ucs2Endings, &ucs2EndingsLen);
        
        int endingStarts[5] = { 0,0,0,0,0 };  //we leave space for up to five alternate endings
        int numEndings = 1;
        for (i = 0; i < ucs2EndingsLen; i++)
        {
            if (ucs2Endings[i] == 0x0020)//space, 002C == comma
            {
                endingStarts[numEndings] = i + 1;
                numEndings++;
            }
        }
        endingStarts[numEndings] = i + 2; //to get length of last stem. plus 2 to simulate a comma and space
        
        for (ending = 0; ending < numEndings; ending++)
        {
            endingStart = endingStarts[ending];
            endingLen = endingStarts[ending + 1] - endingStarts[ending] - 2;
            
            //add stem to temp buffer
            for (i = stemStart; i < (stemStart + stemLen); i++)
            {
                ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = ucs2Stems[i];
                ucs2StemPlusEndingBufferLen++;
            }
            
            int stemStartInBuffer = ucs2StemPlusEndingBufferLen - stemLen; //set to the index in ucs2StemPlusEndingBuffer
            int tempStemLen = stemLen;
            
            stripEndingFromPrincipalPart(&ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, vf->tense, vf->voice);
            
            if (vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
            {
                augmentStem(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen);
            }
            
            //De-augment
            if ( (vf->tense == AORIST && (vf->mood == SUBJUNCTIVE || vf->mood == OPTATIVE || vf->mood == IMPERATIVE)) || (vf->tense == FUTURE && vf->voice == PASSIVE))
            {
                UCS2 ucs2Present[(strlen(vf->verb->present) * 2) + 1];
                int ucs2PresentLen = 0;
                utf8_to_ucs2_string((const unsigned char*)vf->verb->present, ucs2Present, &ucs2PresentLen);
                stripAccent(ucs2Present, &ucs2PresentLen);
                UCS2 presentInitialLetter = ucs2Present[0];
                
                stripAugmentFromPrincipalPart(&ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, vf->tense, vf->voice, vf->mood, presentInitialLetter);
            }
            
            addEnding(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, &ucs2Endings[endingStart], endingLen);
            
            //Labe/ Accent EXCEPTION H&Q page 326
            UCS2 labe[] = { GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_BETA, GREEK_SMALL_LETTER_EPSILON } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, labe, 4))
            {
                ucs2StemPlusEndingBuffer[3] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            }
            
            //add accent, if word does not already have one
            if (!wordIsAccented(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen))
            {
                bool isOpt = false;
                if (vf->mood == OPTATIVE)
                    isOpt = true;
                else
                    isOpt = false;
                
                accentRecessive(&ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, isOpt);
            }
            
            ucs2StemPlusEndingBufferLen += (tempStemLen - stemLen);
            
            ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = 0x002C; //comma
            ucs2StemPlusEndingBufferLen++;
            ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = 0x0020; //space
            ucs2StemPlusEndingBufferLen++;
        }
    }
    ucs2StemPlusEndingBufferLen -= 2; //remove trailing comma and space.
    
    if (vf->mood == IMPERATIVE && vf->person == FIRST)
    {
        ucs2StemPlusEndingBuffer[0] = EM_DASH;
        ucs2StemPlusEndingBufferLen = 1;
    }
    
    ucs2_to_utf8_string(ucs2StemPlusEndingBuffer, ucs2StemPlusEndingBufferLen, (unsigned char *)utf8OutputBuffer);
    
    if (utf8OutputBuffer[0] == '\0')
        return 0;
    else
        return 1;
}

bool utf8HasSuffix(char *s, char *suffix)
{
    unsigned long len = strlen(s);
    unsigned long suffixLen = strlen(suffix);
    
    if (suffixLen > len)
        return false;
    
    long j = len - 1;
    for (long i = suffixLen - 1; i >= 0; i--, j--)
    {
        if (suffix[i] != s[j])
            return false;
    }
    return true;
}

//page 316 in h&q
int deponentType(Verb *v)
{
    if ( utf8HasSuffix(v->present, "ομαι") && utf8HasSuffix(v->future, "ομαι") && utf8HasSuffix(v->aorist, "άμην") && v->perf[0] == '\0' && utf8HasSuffix(v->perfmid, "μαι") && v->aoristpass[0] == '\0')
    {
        return MIDDLE_DEPONENT;
    }
    else if ( utf8HasSuffix(v->present, "ομαι") && utf8HasSuffix(v->future, "ομαι") && v->aorist[0] == '\0' && v->perf[0] == '\0' && utf8HasSuffix(v->perfmid, "μαι") && v->aoristpass[0] != '\0')
    {
        return PASSIVE_DEPONENT;
    }
    else if (utf8HasSuffix(v->present, "ομαι") || utf8HasSuffix(v->future, "ομαι") || utf8HasSuffix(v->aorist, "άμην") )
    {
        return PARTIAL_DEPONENT;
    }
    else
    {
        return NOT_DEPONENT;
    }
}

bool isDeponent(VerbFormC *vf, UCS2 *stem, int stemLen)
{
    UCS2 ending[] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };

    UCS2 ending2[] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    if ((vf->tense == PRESENT || vf->tense == IMPERFECT || vf->tense == FUTURE) && hasSuffix(stem, stemLen, ending, 4))
        return true;
    
    if (vf->tense == AORIST && vf->voice != PASSIVE && hasSuffix(stem, stemLen, ending2, 4))
        return true;
    
    return false;
}

bool accentRecessive(UCS2 *tempUcs2String, int *len, bool isOptative)
{
    //find syllable
    int i = 0;
    int vowelsAndDiphthongs = 0;
    int longUltima = false;
    int longPenult = false;
    int consonants = 0;
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
                    if (consonants == 0 && tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA && tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA && !isOptative)
                    {
                        longUltima = false;
                    }
                    else if (consonants == 0 && tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA && tempUcs2String[i - 1] == GREEK_SMALL_LETTER_OMICRON && !isOptative)
                    {
                        longUltima = false;
                    }
                    else
                    {
                        longUltima = true;
                    }
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
        else
        {
            consonants++;
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
    
    //find which character to accent, == i
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
        if ( ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_MACRON || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_MACRON || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_MACRON )
        {
            //change to shift right?
            for (int j = *len - 1; j > i; j--)
                ucs2String[j + 1] = ucs2String[j];
            ucs2String[i + 1] = COMBINING_ACUTE;
            (*len)++;
        }
        else if (i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON )
        {
            //or should we use the private codes for this?  looks good on iphone as three combining characters
            if (ucs2String[i + 2] == COMBINING_SMOOTH_BREATHING || ucs2String[i + 2] == COMBINING_ROUGH_BREATHING)
                i++;
            
            //change to shift right?
            for (int j = *len - 1; j > i + 1; j--)
                ucs2String[j + 1] = ucs2String[j];
            ucs2String[i + 2] = COMBINING_ACUTE;
            (*len)++;
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
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
    }
    else if (accent == CIRCUMFLEX)
    {
        //remove combining macron if a circumflex is going on that syllable.
        if (i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON )
        {
            //change to shift left?
            for (int j = i + 1; j < *len; j++)
                ucs2String[j] = ucs2String[j + 1];
            (*len)--;
        }
        
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

void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int elen)
{
    if ((vf->tense == PRESENT || vf->tense == IMPERFECT) /* && ucs2[*len] == GREEK_SMALL_LETTER_OMEGA */ && ucs2[*len - 1] == GREEK_SMALL_LETTER_ALPHA)
    {
        --(*len);
    }
    else if ((vf->tense == PRESENT || vf->tense == IMPERFECT) /* && ucs2[*len] == GREEK_SMALL_LETTER_OMEGA */ && ucs2[*len - 1] == GREEK_SMALL_LETTER_EPSILON)
    {
        --(*len);
    }
    else if ((vf->tense == PRESENT || vf->tense == IMPERFECT) /* && ucs2[*len] == GREEK_SMALL_LETTER_OMEGA */ && ucs2[*len - 1] == GREEK_SMALL_LETTER_OMICRON)
    {
        --(*len);
    }
    else if (vf->tense == PERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE))
    {
        if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_1) == CONSONANT_STEM_PERFECT_1) //γέγραμμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_2) == CONSONANT_STEM_PERFECT_2) //πέπεμμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_3) == CONSONANT_STEM_PERFECT_3) //πεφύλαγμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_4) == CONSONANT_STEM_PERFECT_4 || ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_5) == CONSONANT_STEM_PERFECT_5 &&  ucs2[(*len)-1] == GREEK_SMALL_LETTER_SIGMA)) //κεκέλευσμαι or σῴζω which is both consonant stem and not.
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_6) == CONSONANT_STEM_PERFECT_6) //ἄγγελμαι
        {
            //H&Q page 273, only different in 2nd and 3rd person plural
            if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
    }
    else if (vf->tense == PLUPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE))
    {
        if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_1) == CONSONANT_STEM_PERFECT_1) //γέγραμμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_2) == CONSONANT_STEM_PERFECT_2) //πέπεμμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_3) == CONSONANT_STEM_PERFECT_3) //πεφύλαγμαι
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_4) == CONSONANT_STEM_PERFECT_4 || ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_5) == CONSONANT_STEM_PERFECT_5 &&  ucs2[(*len)-1] == GREEK_SMALL_LETTER_SIGMA)) //κεκέλευσμαι or σῴζω which is both consonant stem and not.
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
                ucs2[(*len)] = GREEK_SMALL_LETTER_OMICRON;
                (*len) += 1;
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
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_6) == CONSONANT_STEM_PERFECT_6) //ἄγγελμαι
        {
            //H&Q page 273, only different in 2nd and 3rd person plural
            if (vf->person == SECOND && vf->number == PLURAL)
            {
                ucs2[(*len)] = GREEK_SMALL_LETTER_THETA;
                ucs2[(*len)+1] = GREEK_SMALL_LETTER_EPSILON;
                (*len) += 2;
                return;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
    }
    else if ((vf->tense == PRESENT || vf->tense == IMPERFECT) && isContractedVerb(vf, ucs2, len)) //CONTRACTED
    {
        //if ();
    }
    else if (vf->tense == FUTURE && vf->voice == PASSIVE) //add future passive infix hs here
    {
        ucs2[*len] = GREEK_SMALL_LETTER_ETA;
        ucs2[(*len) + 1] = GREEK_SMALL_LETTER_SIGMA;
        (*len) += 2; //parens required here fyi
    }
    else if (vf->tense == PRESENT && utf8HasSuffix(vf->verb->present, "μι")) //mi verbs
    {
        //shorten stem vowel
        if (vf->voice != ACTIVE || vf->number == PLURAL || vf->mood == OPTATIVE || vf->mood == IMPERATIVE)
        {
            if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA && ucs2[*len - 2] == GREEK_SMALL_LETTER_TAU)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
        }
        //contract third plural indicative of isthmi
        if (vf->person == THIRD && vf->number == PLURAL && vf->mood == INDICATIVE && vf->voice == ACTIVE && utf8HasSuffix(vf->verb->present, "ἵστημι"))
        {
            --(*len);
            leftShift(ending, &elen);
            ending[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
        }
        if (vf->mood == SUBJUNCTIVE)
        {
            --(*len);
           // UCS2 didw[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA , GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMEGA };
            if ( utf8HasSuffix(vf->verb->present, "δίδωμι"))
            {
                if ( vf->person == SECOND && vf->number == SINGULAR)
                    ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                else if ( vf->person == THIRD && vf->number == SINGULAR && vf->voice == ACTIVE)
                    ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                else if ( vf->person == THIRD && vf->number == SINGULAR && vf->voice != ACTIVE)
                    ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                else if ( vf->person == SECOND && vf->number == PLURAL)
                    ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
            }
        }
        if (vf->mood == OPTATIVE)
        {
            leftShift(ending, &elen);
            if (vf->person != FIRST && vf->voice != ACTIVE)
            {
                ending[0] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
            }
        }
        if (vf->mood == IMPERATIVE)
        {
            if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( utf8HasSuffix(vf->verb->present, "δίδωμι"))
                {
                    if (vf->voice == ACTIVE)
                    {
                        ending[0] = GREEK_SMALL_LETTER_UPSILON;
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                        ending[1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                }
                else if ( utf8HasSuffix(vf->verb->present, "ἵστημι"))
                {
                    if (vf->voice == ACTIVE)
                    {
                        --(*len);
                        ending[0] = GREEK_SMALL_LETTER_ETA;
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                        ending[1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                }
                else if ( utf8HasSuffix(vf->verb->present, "τίθημι"))
                {
                    if (vf->voice == ACTIVE)
                    {
                        ending[0] = GREEK_SMALL_LETTER_IOTA;
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                        ending[1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                }
            }
            else
            {
                leftShift(ending, &elen);
            }
            
        }
    }
    else if (vf->tense == IMPERFECT && utf8HasSuffix(vf->verb->present, "μι")) //mi verbs
    {
        if ( utf8HasSuffix(vf->verb->present, "δίδωμι") && vf->voice == ACTIVE)
        {
            //shorten stem vowel
            if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
            
            if ( vf->number == SINGULAR && vf->voice == ACTIVE)
            {
                ending[0] = GREEK_SMALL_LETTER_UPSILON;
                if (vf->person == THIRD)
                    elen -= 3;
            }
            else if (vf->number == PLURAL && vf->voice == ACTIVE)
            {
                leftShift(ending, &elen);
                if (vf->person == THIRD)
                {
                    ending[0] = GREEK_SMALL_LETTER_SIGMA;
                    ending[1] = GREEK_SMALL_LETTER_ALPHA;
                    ending[2] = GREEK_SMALL_LETTER_NU;
                    elen += 2;
                }
            }
        }
        if ( utf8HasSuffix(vf->verb->present, "ἵστημι") && vf->voice == ACTIVE)
        {
            //shorten stem vowel
            if (vf->number == PLURAL)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            
            leftShift(ending, &elen);
            if (vf->person == THIRD && vf->number == SINGULAR)
            {
                elen -= 3;
            }
            if (vf->person == THIRD && vf->number == PLURAL)
            {
                ending[0] = GREEK_SMALL_LETTER_SIGMA;
                ending[1] = GREEK_SMALL_LETTER_ALPHA;
                ending[2] = GREEK_SMALL_LETTER_NU;
                elen += 2;
            }
        }
        if ( utf8HasSuffix(vf->verb->present, "τίθημι") && vf->voice == ACTIVE)
        {
            if (vf->person != FIRST || vf->number == PLURAL)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
            
            if (vf->person == THIRD && vf->number == PLURAL)
            {
                leftShift(ending, &elen);
                ending[0] = GREEK_SMALL_LETTER_SIGMA;
                ending[1] = GREEK_SMALL_LETTER_ALPHA;
                ending[2] = GREEK_SMALL_LETTER_NU;
                elen += 2;
            }
            else if (vf->person == FIRST || vf->number == PLURAL)
            {
                leftShift(ending, &elen);
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ending[0] = GREEK_SMALL_LETTER_IOTA;
                ending[1] = GREEK_SMALL_LETTER_FINAL_SIGMA;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ending[0] = GREEK_SMALL_LETTER_IOTA;
                elen -= 3;
            }
        }
        else if (vf->voice != ACTIVE)
        {
            //shorten stem vowel
            if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA && ucs2[*len - 2] == GREEK_SMALL_LETTER_TAU )
                ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA )
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
            
            leftShift(ending, &elen);
            if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ending[0] = GREEK_SMALL_LETTER_SIGMA;
                ending[1] = GREEK_SMALL_LETTER_OMICRON;
                elen = 2;
            }
        }
    }
    
    int i = 0;
    int j = 0;
    for (i = *len; j < elen; i++, j++)
    {
        ucs2[i] = ending[j];
        ++(*len);
    }
}

bool isContractedVerb(VerbFormC *vf, UCS2 *ucs2, int *len)
{
    UCS2 oStem[2] = { GREEK_SMALL_LETTER_OMICRON_WITH_OXIA, GREEK_SMALL_LETTER_OMEGA };
    UCS2 aStem[2] = { GREEK_SMALL_LETTER_ALPHA_WITH_OXIA, GREEK_SMALL_LETTER_OMEGA };
    UCS2 eStem[2] = { GREEK_SMALL_LETTER_EPSILON_WITH_OXIA, GREEK_SMALL_LETTER_OMEGA };
    
    if (hasSuffix(ucs2, *len, aStem, 2))
    {
        printf("here\n");
    
        return true;
    }
    else
    {
        //printf("nope\n");
    }
    return false;
}

void stripAugmentFromPrincipalPart(UCS2 *ucs2, int *len, unsigned char tense, unsigned char voice, unsigned char mood, UCS2 presentStemInitial)
{
    if (tense == AORIST && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE))
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
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, unsigned char tense, unsigned char voice)
{
    UCS2 presMi[2] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_IOTA };
    
    UCS2 presDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    UCS2 aoristDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    UCS2 secondAoristDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    UCS2 secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };

    if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presMi, 2)) //μι
        *len -= 2;
    else if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presDeponent, 4)) //ομαι
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
void augmentStem(VerbFormC *vf, UCS2 *ucs2, int *len)
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
        if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI && vf->tense != PLUPERFECT)
        {
            /*
             H&Q page 326.  "In most verbs when principal part iv or v begins with e) or ei), the pluperfect is unaugmented."
             */
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
        /* //probably don't need this, since it's either a diphthong or will have psili or dasia
        else if ( vf->tense != PLUPERFECT ) //because ei) in pluperfect will fall through to this, so need this if
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA;
        }
         */
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI && vf->tense != PLUPERFECT)
    {
        /*
         H&Q page 326.  "In most verbs when principal part iv or v begins with e) or ei), the pluperfect is unaugmented."
         */
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
        ucs2[2] = COMBINING_SMOOTH_BREATHING;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
    {
        rightShift(ucs2, len);
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_ROUGH_BREATHING;
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
    } /* FIX ME, is this supposed to be omicron with psili /dasia? */
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
        ucs2[2] = COMBINING_SMOOTH_BREATHING;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
    {
        rightShift(ucs2, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_ROUGH_BREATHING;
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
        case GREEK_SMALL_LETTER_ALPHA_WITH_MACRON:
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
        case GREEK_SMALL_LETTER_IOTA_WITH_MACRON:
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
        case GREEK_SMALL_LETTER_UPSILON_WITH_MACRON:
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

bool formIsValidReal(unsigned char person, unsigned char number, unsigned char tense, unsigned char voice, unsigned char mood)
{
    if ( tense == IMPERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE) )
        return false;
    if ( tense == FUTURE && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE) )
        return false;
    if ( tense == PERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE) )
        return false;
    if ( tense == PLUPERFECT && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE) )
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



