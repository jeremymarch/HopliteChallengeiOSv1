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
#include "utilities.h"
#include "libmorph.h"
#include "GreekForms.h"
#include "GreekUnicode.h"
#include "accent.h"
#include "augment.h"
#include "ending.h"

bool onlyUseCombiningDiacritics = false; //not used yet

/*
 TO DO:
 click to show principal parts or endings from decompose form.
 
 Settings:
 disable animation (or select from different animation modes)
 disable sound
 include dual
 ask principal parts
 ask verb endings
 white on black color scheme
 
 *remember, don't copy and paste unicode files into android studio, copy and paste file in finder

 */

bool letterIsAccented(UCS2 letter);
void stripAccent(UCS2 *word, int *len);
char *getEnding(VerbFormC *vf, UCS2 *ending, int endingLen, bool contractedFuture, bool preContactedEndings);
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, unsigned char tense, unsigned char voice);

bool accentRecessive(VerbFormC *vf, UCS2 *tempUcs2String, int *len);

char *persons[NUM_PERSONS] = { "first", "second", "third" };
char *personsabbrev[NUM_PERSONS] = { "1st", "2nd", "3rd" };
char *numbers[NUM_NUMBERS] = { "singular", "plural" };
char *numbersabbrev[NUM_NUMBERS] = { "sing.", "pl." };
char *tenses[NUM_TENSES] = { "Present", "Imperfect", "Future", "Aorist", "Perfect", "Pluperfect" };
char *tensesabbrev[NUM_TENSES] = { "pres.", "imp.", "fut.", "aor.", "perf.", "plup." };
char *voices[NUM_VOICES] = { "Active", "Middle", "Passive" };
char *voicesabbrev[NUM_VOICES] = { "act.", "mid.", "pass." };
char *moods[NUM_MOODS] = { "Indicative", "Subjunctive", "Optative", "Imperative" };
char *moodsabbrev[NUM_MOODS] = { "ind.", "subj.", "opt.", "imper." };
/*
void endingGetDescription(int e, char *buffer, int bufferLen)
{
    char present[] = "Present";
    char aorist[] = "Aorist";
    char perfect[] = "Perfect";
    
    snprintf(buffer, bufferLen, "%s %s %s", "Present", "Active", "Subjunctive");
}
*/
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

void getStartEnd(UCS2 *w1, int w1len, int *starts, int *ends, int *numStrings)
{
    int start = 0;
    int end = 0;
    int i = 0;
    
    while((w1[i] == SPACE || w1[i] == COMMA ||w1[i] == NEWLINE) && i < w1len)
        i++;
    
    starts[0] = i;
    for ( ; i < w1len; i++)
    {
        if (w1[i] == COMMA || w1[i] == SPACE ||w1[i] == NEWLINE)
        {
            ends[end++] = i;
            i++;
            while((w1[i] == SPACE || w1[i] == COMMA ||w1[i] == NEWLINE) && i < w1len)
                i++;
            if (i < w1len)
            {
                starts[++start] = i;
            }
        }
    }
    ends[end++] = w1len;
    *numStrings = ++start;
}

bool compareFormsCheckMF(UCS2 *expected, int expectedLen, UCS2 *given, int givenLen, bool MFPressed)
{
    bool expectedHasMF = false;
    for (int i = 0; i < expectedLen; i++)
    {
        if (expected[i] == COMMA)
        {
            expectedHasMF = true;
            break;
        }
    }
    if (MFPressed && !expectedHasMF)
        return false;
    else if (!MFPressed && expectedHasMF)
        return false;
    else
        return compareForms(expected, expectedLen, given, givenLen);
}

bool compareForms(UCS2 *w1, int w1len, UCS2 *w2, int w2len)
{
    int starts[MAX_MULTIPLE_FORMS];
    int ends[MAX_MULTIPLE_FORMS];
    int numStrings = 0;
    getStartEnd(w1, w1len, starts, ends, &numStrings);
    
    int starts2[MAX_MULTIPLE_FORMS];
    int ends2[MAX_MULTIPLE_FORMS];
    int numStrings2 = 0;
    getStartEnd(w2, w2len, starts2, ends2, &numStrings2);
    /*
    for(int i = 0; i < numStrings; i++)
    {
        printf("s: %d, l: %d, \"%.*s\"\n", starts[i], ends[i] - starts[i], ends[i] - starts[i], &w1[starts[i]]);
    }
    for(int i = 0; i < numStrings2; i++)
    {
        printf("s: %d, l: %d, \"%.*s\"\n", starts2[i], ends2[i] - starts2[i], ends2[i] - starts2[i], &w2[starts2[i]]);
    }
    printf("num: %d\n", numStrings);
    */
    if (numStrings != numStrings2)
        return false;
    
    bool hasMatch = false;
    for (int i = 0; i < numStrings; i++)
    {
        hasMatch = false;
        for (int j = 0; j < numStrings2; j++)
        {
            if ( compareWord(&w1[starts[i]], ends[i] - starts[i], &w2[starts2[j]], ends2[j] - starts2[j]) )
            {
                hasMatch = true;
                break;
            }
        }
        if (!hasMatch)
        {
            return false;
        }
    }
    return true;
}

bool compareWord(UCS2 *w1, int w1len, UCS2 *w2, int w2len)
{
    /*
     //for testing
    for (int i = 0; i < w1len; i++)
    {
        fprintf(stderr, "%04X ", w1[i]);
    }
    fprintf(stderr, "\n");
    
    for (int i = 0; i < w2len; i++)
    {
        fprintf(stderr, "%04X ", w2[i]);
    }
    fprintf(stderr, "\n");
    */
    
    if (w1len != w2len)
        return false;
    
    for (int i = 0; i < w1len; i++)
    {
        if (w1[i] != w2[i])
            return false;
    }
    return true;
}

void getPrincipalParts(Verb *v, char *buffer, int len)
{
    snprintf(buffer, len, "%s; %s; %s; %s; %s; %s", v->present, v->future, v->aorist, v->perf, v->perfmid, v->aoristpass);
}

void getFullDescription (VerbFormC *vf, char *buffer, int len)
{
    snprintf(buffer, len, "%s %s %s %s %s", persons[vf->person], numbers[vf->number], tenses[vf->tense], voices[vf->voice], moods[vf->mood]);
}

void getAbbrevDescription2 (VerbFormD *vf, char *buffer, int len)
{
    VerbFormC vfc;
    vfc.person = vf->person;
    vfc.number = vf->number;
    vfc.tense = vf->tense;
    vfc.voice = vf->voice;
    vfc.mood = vf->mood;
    getAbbrevDescription (&vfc, buffer, len);
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

char *getEnding(VerbFormC *vf, UCS2 *word, int wordLen, bool contractedFuture, bool preContactedEndings)
{
    UCS2 secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    UCS2 secondAorist2[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    //UCS2 isthmi2[4] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    //UCS2 deponent[] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    int ending = 0;

    //H&Q page 503, 607, 676
    if ((utf8HasSuffix(vf->verb->present, "δύναμαι") || utf8HasSuffix(vf->verb->present, "ἐπίσταμαι")) && vf->tense == PRESENT && vf->mood == SUBJUNCTIVE)
    {
        ending = PRESENT_MIDPASS_SUBJ;
    }
    else if (utf8HasSuffix(vf->verb->present, "κεῖμαι"))
    {
        if (vf->tense == PRESENT  && vf->mood == INDICATIVE)
            ending = PERFECT_MIDPASS_IND;
        else if (vf->tense == IMPERFECT  && vf->mood == INDICATIVE)
            ending = PLUPERFECT_MIDPASS_IND;
        else if (vf->mood == SUBJUNCTIVE)
            ending = PRESENT_MIDPASS_SUBJ;
        else if (vf->mood == OPTATIVE)
            ending = PRESENT_MIDPASS_OPT;
        else if (vf->mood == IMPERATIVE)
            ending  = AORIST_MIDDLE_IMPERATIVES_MI;
        else if (vf->tense == FUTURE)
            ending = PRESENT_MIDPASS_IND;
    }
    ////echw h&q page 504
    else if (utf8HasSuffix(vf->verb->present, "ἔχω") && vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE)
        ending = AORIST_PASSIVE_SUBJ;
    else if (utf8HasSuffix(vf->verb->present, "ἔχω") && vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && vf->number == SINGULAR)
        ending = AORIST_PASSIVE_OPT;
    /* MI */
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PRESENT_ACTIVE_INDICATIVE_MI;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "αμαι") || utf8HasSuffix(vf->verb->present, "νυμαι")))
        ending = PERFECT_MIDPASS_IND;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE  && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && (!preContactedEndings || word[wordLen - 3] == COMBINING_MACRON || utf8HasSuffix(vf->verb->present, "εἶμι")))
        ending = PRESENT_ACTIVE_SUBJ;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && preContactedEndings)
        ending = AORIST_PASSIVE_SUBJ;
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && (!preContactedEndings || word[wordLen - 3] == COMBINING_MACRON))
        ending = PRESENT_MIDPASS_SUBJ;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "αμαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    
    else if (utf8HasSuffix(vf->verb->present, "εἶμι") && vf->mood == OPTATIVE)
        ending = PRESENT_ACTIVE_OPT;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && word[wordLen - 3] != COMBINING_MACRON)
        ending = AORIST_PASSIVE_OPT;
    
    else if (((vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE)) || (vf->tense == AORIST && vf->voice == MIDDLE)) && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "τίθημι"))
        ending = PRESENT_MIDPASS_OPT_TITHHMI; //Exception: H&Q page 347
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "ῑ̔́ημι"))
        ending = PRESENT_MIDPASS_OPT_TITHHMI; //Exception: H&Q page 347
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "ῑ́ημι"))
        ending = PRESENT_MIDPASS_OPT_TITHHMI; //Exception: H&Q page 347
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "αμαι")))
        ending = PRESENT_MIDPASS_OPT;
    
    /* isthmi root aorist */
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == INDICATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU)
        ending = AORIST_ACTIVE_INDICATIVE_MI_ROOT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU && preContactedEndings)
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU && !preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ;
    
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU )
        ending = AORIST_PASSIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU )
        ending = AORIST_ACTIVE_IMPERATIVES_MI_ROOT;
    /* isthmi root aorist has no middle voice */
    
    
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_ACTIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_ACTIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_ACTIVE_IMPERATIVE;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == INDICATIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_MID_IND;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_MIDDLE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_MIDDLE_OPT;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == IMPERATIVE && utf8HasSuffix(vf->verb->present, "στημι") )
        ending = AORIST_MIDDLE_IMPERATIVE;
    
    
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι") && preContactedEndings)
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι") && !preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι"))
        ending = AORIST_PASSIVE_OPT;
    
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == INDICATIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι"))
        ending = PLUPERFECT_MIDPASS_IND;
    
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι")  && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == SUBJUNCTIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι")  && !preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ;
    
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι"))
        ending = AORIST_ACTIVE_IMPERATIVES_MI;
    
    else if (vf->tense == AORIST && vf->voice == MIDDLE && vf->mood == IMPERATIVE && utf8HasSuffix(vf->verb->present, "μι") && !utf8HasSuffix(vf->verb->present, "ῡμι"))
        ending = AORIST_MIDDLE_IMPERATIVES_MI;
    /* end MI verbs */
    
    /* CONTRACTED FUTURES */
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture && preContactedEndings && (vf->verb->verbclass & CONTRACTED_FUTURE_ALPHA) != CONTRACTED_FUTURE_ALPHA)
        ending = PRESENT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture && !preContactedEndings)
        ending = PRESENT_ACTIVE_IND;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture && preContactedEndings && (vf->verb->verbclass & CONTRACTED_FUTURE_ALPHA) != CONTRACTED_FUTURE_ALPHA)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture && !preContactedEndings)
        ending = PRESENT_MIDPASS_IND;
    /* ALPHA CONTRACTED */
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture && preContactedEndings && (vf->verb->verbclass & CONTRACTED_FUTURE_ALPHA) == CONTRACTED_FUTURE_ALPHA)
        ending = PRESENT_ACTIVE_INDIC_A_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture && preContactedEndings && (vf->verb->verbclass & CONTRACTED_FUTURE_ALPHA) == CONTRACTED_FUTURE_ALPHA)
        ending = PRESENT_MIDPASS_INDIC_A_CONTRACTED;

    /* CONTRACTED PRESENT AND IMPERFECT */
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE  && (word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA || word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON|| word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON) && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && !preContactedEndings)
        ending = IMPERFECT_ACTIVE_CONTRACTED_DECOMPOSED;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_INDIC_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_INDIC_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "έομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED; //fix me, is this a typo? aw and eomai?
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = IMPERFECT_ACTIVE_INDIC_A_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")) && preContactedEndings)
        ending = IMPERFECT_MIDPASS_INDIC_A_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "έομαι")) && preContactedEndings)
        ending = IMPERFECT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "έομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && preContactedEndings)
        ending = PRESENT_ACTIVE_OPT_A_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && !preContactedEndings)
        ending = PRESENT_ACTIVE_OPTATIVE_CONTRACTED_NOT_PRECONTRACTED;
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_OPT_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "έομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_OPT_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = IMPERFECT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = IMPERFECT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_OPT_E_CONTRACTED;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && !preContactedEndings)
        ending = PRESENT_ACTIVE_OPTATIVE_CONTRACTED_NOT_PRECONTRACTED;
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_OPT_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_INDIC_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_INDIC_O_CONTRACTED;
    else if (vf->tense == IMPERFECT && vf->voice == ACTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = IMPERFECT_ACTIVE_INDIC_O_CONTRACTED;
    else if (vf->tense == IMPERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = IMPERFECT_MIDPASS_INDIC_O_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_O_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_OPT_O_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && !preContactedEndings)
        ending = PRESENT_ACTIVE_OPTATIVE_CONTRACTED_NOT_PRECONTRACTED;
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_OPT_O_CONTRACTED;
    
    /* CONTRACTED IMPERATIVES */
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_ALPHA && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_IMPERATIVE_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_IMPERATIVE_A_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && (utf8HasSuffix(vf->verb->present, "έω") || utf8HasSuffix(vf->verb->present, "έομαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_IMPERATIVE_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_IMPERATIVE_E_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_EPSILON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_MIDPASS_IMPERATIVE_E_CONTRACTED;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
        ending = PRESENT_ACTIVE_IMPERATIVE_O_CONTRACTED;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == IMPERATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_OMICRON && word[wordLen - 1] == GREEK_SMALL_LETTER_OMEGA && preContactedEndings)
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
        ending = SECOND_AORIST_MIDDLE_IMPERATIVE; //remember irreg accent on 2nd sing!
/* end SECOND AORIST */
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
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == SUBJUNCTIVE && (preContactedEndings || !ADD_EPSILON_TO_AORIST_PASSIVE_SUBJUNCTIVE_STEM))
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == SUBJUNCTIVE && !preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ;
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
        return NULL;
    /*
    if ( vf->mood == INFINITIVE)
    {
        if (vf->tense == PRESENT && vf->voice == ACTIVE )
            return "ειν";
        if (vf->tense == PRESENT && ( vf->voice == MIDDLE || vf->voice == PASSIVE ))
            return "εσθαι";
        if (vf->tense == AORIST && vf->voice == ACTIVE )
            return "αι";
        if (vf->tense == AORIST && vf->voice == PASSIVE )
            return "ῆναι";
        if (vf->tense == AORIST && vf->voice == MIDDLE )
            return "ασθαι";
        if (vf->tense == PERFECT && vf->voice == ACTIVE )
            return "έναι";
        else
            return "σθαι";
    }
    */
}

char *getPrincipalPartForTense(Verb *verb, unsigned char tense, unsigned char voice)
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

int getEimi(VerbFormC *vf, UCS2 *buffer, int *bufferLen)
{
    //only present/imperfect active for eimi/
    if ( (vf->tense == PRESENT || vf->tense == IMPERFECT) && vf->voice != ACTIVE )
    {
        return 0;
    }
    if (vf->tense == PRESENT)
    {
        if (vf->mood == INDICATIVE)
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                buffer[2] = GREEK_SMALL_LETTER_MU;
                buffer[3] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                *bufferLen = 4;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
                *bufferLen = 2;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[4] = LEFT_PARENTHESIS;
                buffer[5] = GREEK_SMALL_LETTER_NU;
                buffer[6] = RIGHT_PARENTHESIS;
                *bufferLen = 7;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_MU;
                buffer[3] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
                buffer[4] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 5;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
                *bufferLen = 4;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                buffer[2] = GREEK_SMALL_LETTER_SIGMA;
                buffer[3] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[4] = LEFT_PARENTHESIS;
                buffer[5] = GREEK_SMALL_LETTER_NU;
                buffer[6] = RIGHT_PARENTHESIS;
                *bufferLen = 7;
            }
        }
        else if (vf->mood == SUBJUNCTIVE)
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
                *bufferLen = 1;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
                buffer[1] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                *bufferLen = 2;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
                *bufferLen = 1;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
                buffer[1] = GREEK_SMALL_LETTER_MU;
                buffer[2] = GREEK_SMALL_LETTER_EPSILON;
                buffer[3] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 4;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
                buffer[1] = GREEK_SMALL_LETTER_TAU;
                buffer[2] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen = 3;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_IOTA;
                buffer[3] = LEFT_PARENTHESIS;
                buffer[4] = GREEK_SMALL_LETTER_NU;
                buffer[5] = RIGHT_PARENTHESIS;
                *bufferLen = 6;
            }
        }
        else if (vf->mood == OPTATIVE)
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[2] = GREEK_SMALL_LETTER_ETA;
                buffer[3] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 4;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[2] = GREEK_SMALL_LETTER_ETA;
                buffer[3] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                *bufferLen = 4;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[2] = GREEK_SMALL_LETTER_ETA;
                *bufferLen = 3;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
                buffer[2] = GREEK_SMALL_LETTER_MU;
                buffer[3] = GREEK_SMALL_LETTER_EPSILON;
                buffer[4] = GREEK_SMALL_LETTER_NU;
                buffer[5] = COMMA;
                buffer[6] = SPACE;
                buffer[7] = GREEK_SMALL_LETTER_EPSILON;
                buffer[8] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[9] = GREEK_SMALL_LETTER_ETA;
                buffer[10] = GREEK_SMALL_LETTER_MU;
                buffer[11] = GREEK_SMALL_LETTER_EPSILON;
                buffer[12] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 13;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_EPSILON;
                buffer[4] = COMMA;
                buffer[5] = SPACE;
                buffer[6] = GREEK_SMALL_LETTER_EPSILON;
                buffer[7] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[8] = GREEK_SMALL_LETTER_ETA;
                buffer[9] = GREEK_SMALL_LETTER_TAU;
                buffer[10] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen = 11;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
                buffer[2] = GREEK_SMALL_LETTER_EPSILON;
                buffer[3] = GREEK_SMALL_LETTER_NU;
                buffer[4] = COMMA;
                buffer[5] = SPACE;
                buffer[6] = GREEK_SMALL_LETTER_EPSILON;
                buffer[7] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[8] = GREEK_SMALL_LETTER_ETA;
                buffer[9] = GREEK_SMALL_LETTER_SIGMA;
                buffer[10] = GREEK_SMALL_LETTER_ALPHA;
                buffer[11] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 12;
            }
        }
        else if (vf->mood == IMPERATIVE)
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                return 0;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_THETA;
                buffer[3] = GREEK_SMALL_LETTER_IOTA;
                *bufferLen = 4;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_OMEGA;
                *bufferLen = 4;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                return 0;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen = 4;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
                buffer[1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[2] = GREEK_SMALL_LETTER_TAU;
                buffer[3] = GREEK_SMALL_LETTER_OMEGA;
                buffer[4] = GREEK_SMALL_LETTER_NU;
                buffer[5] = COMMA;
                buffer[6] = SPACE;
                buffer[7] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA;
                buffer[8] = GREEK_SMALL_LETTER_NU;
                buffer[9] = GREEK_SMALL_LETTER_TAU;
                buffer[10] = GREEK_SMALL_LETTER_OMEGA;
                buffer[11] = GREEK_SMALL_LETTER_NU;
                *bufferLen = 12;
            }
        }
    }
    else if (vf->tense == IMPERFECT)
    {
        if (vf->person == FIRST && vf->number == SINGULAR)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = COMMA;
            buffer[2] = SPACE;
            buffer[3] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[4] = GREEK_SMALL_LETTER_NU;
            *bufferLen = 5;
        }
        else if (vf->person == SECOND && vf->number == SINGULAR)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = GREEK_SMALL_LETTER_SIGMA;
            buffer[2] = GREEK_SMALL_LETTER_THETA;
            buffer[3] = GREEK_SMALL_LETTER_ALPHA;
            *bufferLen = 4;
        }
        else if (vf->person == THIRD && vf->number == SINGULAR)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = GREEK_SMALL_LETTER_NU;
            *bufferLen = 2;
        }
        else if (vf->person == FIRST && vf->number == PLURAL)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = GREEK_SMALL_LETTER_MU;
            buffer[2] = GREEK_SMALL_LETTER_EPSILON;
            buffer[3] = GREEK_SMALL_LETTER_NU;
            *bufferLen = 4;
        }
        else if (vf->person == SECOND && vf->number == PLURAL)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = GREEK_SMALL_LETTER_TAU;
            buffer[2] = GREEK_SMALL_LETTER_EPSILON;
            *bufferLen = 3;
        }
        else if (vf->person == THIRD && vf->number == PLURAL)
        {
            buffer[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            buffer[1] = GREEK_SMALL_LETTER_SIGMA;
            buffer[2] = GREEK_SMALL_LETTER_ALPHA;
            buffer[3] = GREEK_SMALL_LETTER_NU;
            *bufferLen = 4;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int getDei(VerbFormC *vf, UCS2 *buffer, int *bufferLen, bool decompose)
{
    if (vf->person == THIRD && vf->number == SINGULAR && vf->voice == ACTIVE)
    {
        buffer[0] = GREEK_SMALL_LETTER_DELTA;
        buffer[1] = GREEK_SMALL_LETTER_EPSILON;
        *bufferLen = 2;
        if (decompose)
        {
            buffer[*bufferLen] = SPACE;
            buffer[*bufferLen+1] = HYPHEN;
            buffer[*bufferLen+2] = SPACE;
            *bufferLen += 3;
        }
        if (vf->tense == PRESENT)
        {
            if (vf->mood == INDICATIVE)
            {
                if (decompose)
                {
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                    *bufferLen += 2;
                }
                else
                {
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                    *bufferLen += 1;
                }
            }
            else if (vf->mood == SUBJUNCTIVE)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (vf->mood == OPTATIVE)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_OMICRON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                *bufferLen += 2;
            }
            else
            {
                return 0;
            }
        }
        else if (vf->tense == IMPERFECT)
        {
            rightShiftFromOffsetSteps(buffer, 0, 1, bufferLen);
            if (decompose)
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
            else
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            
            if (decompose)
            {
                rightShiftFromOffsetSteps(buffer, 1, 3, bufferLen);
                buffer[1] = SPACE;
                buffer[2] = HYPHEN;
                buffer[3] = SPACE;
            }
            if (decompose)
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            else
                buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA;
            *bufferLen += 1;
        }
        else if (vf->tense == FUTURE)
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(buffer, 2, 2, bufferLen);
                buffer[2] = GREEK_SMALL_LETTER_ETA;
                buffer[3] = GREEK_SMALL_LETTER_SIGMA;
            }
            else
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
                *bufferLen += 2;
            }
            buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            *bufferLen += 2;
        }
        else if (vf->tense == AORIST && vf->mood == INDICATIVE)
        {
            rightShiftFromOffsetSteps(buffer, 0, 1, bufferLen);
            if (decompose)
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
            else
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            
            if (decompose)
            {
                rightShiftFromOffsetSteps(buffer, 1, 3, bufferLen);
                buffer[1] = SPACE;
                buffer[2] = HYPHEN;
                buffer[3] = SPACE;

                rightShiftFromOffsetSteps(buffer, 6, 2, bufferLen);
                buffer[6] = GREEK_SMALL_LETTER_ETA;
                buffer[7] = GREEK_SMALL_LETTER_SIGMA;
            }
            else
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
                *bufferLen += 2;
            }
            buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = LEFT_PARENTHESIS;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
            buffer[*bufferLen+3] = RIGHT_PARENTHESIS;
            *bufferLen += 4;
        }
        else
        {
            return 0;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int getXrh(VerbFormC *vf, UCS2 *buffer, int *bufferLen, bool decompose)
{
    if (vf->person == THIRD && vf->number == SINGULAR && vf->voice == ACTIVE)
    {
        buffer[0] = GREEK_SMALL_LETTER_CHI;
        buffer[1] = GREEK_SMALL_LETTER_RHO;
        buffer[2] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
        *bufferLen = 3;
        if (decompose && !(vf->tense == PRESENT && vf->mood == INDICATIVE))
        {
            buffer[*bufferLen] = SPACE;
            buffer[*bufferLen+1] = HYPHEN;
            buffer[*bufferLen+2] = SPACE;
            *bufferLen += 3;
        }
        if (vf->tense == PRESENT)
        {
            if (vf->mood == INDICATIVE)
            {

            }
            else if (vf->mood == SUBJUNCTIVE)
            {
                if (!decompose)
                {
                    buffer[*bufferLen - 1] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                }
                else
                {
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
                    *bufferLen += 1;
                }
            }
            else if (vf->mood == OPTATIVE)
            {
                if (!decompose)
                {
                    buffer[*bufferLen - 1] = GREEK_SMALL_LETTER_EPSILON;
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_ETA;
                    *bufferLen += 2;
                }
                else
                {
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                    *bufferLen += 3;
                }
            }
            else
            {
                return 0;
            }
        }
        else if (vf->tense == IMPERFECT)
        {
            rightShiftFromOffsetSteps(buffer, 0, 1, bufferLen);
            if (decompose)
                buffer[0] = GREEK_SMALL_LETTER_EPSILON;
            else
                buffer[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            
            if (decompose)
            {
                rightShiftFromOffsetSteps(buffer, 1, 3, bufferLen);
                buffer[1] = SPACE;
                buffer[2] = HYPHEN;
                buffer[3] = SPACE;
            }
            if (decompose)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 2;
            }
            else
            {
                buffer[*bufferLen - 1] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
                buffer[*bufferLen] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+1] = COMMA;
                buffer[*bufferLen+2] = SPACE;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_CHI;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_RHO;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
                buffer[*bufferLen+6] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 7;
            }
            
        }
        else if (vf->tense == FUTURE)
        {
            if (decompose)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_ALPHA;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_IOTA;
                *bufferLen += 5;
            }
            else
            {
                buffer[*bufferLen-1] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
                buffer[*bufferLen] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ALPHA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_IOTA;
                *bufferLen += 4;
            }
        }
        else
        {
            return 0;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int getOida(VerbFormC *vf, UCS2 *buffer, int *bufferLen, bool decompose)
{
    bool hasPrefix = false;
    if (vf->voice != ACTIVE)
        return 0;
 
    *bufferLen = 0;
    
    if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
    {
        buffer[0] = GREEK_SMALL_LETTER_SIGMA;
        buffer[1] = GREEK_SMALL_LETTER_UPSILON;
        buffer[2] = GREEK_SMALL_LETTER_NU;
        *bufferLen = 3;
        hasPrefix = true;
        
        if (decompose)
        {
            buffer[*bufferLen] = SPACE;
            buffer[*bufferLen+1] = HYPHEN;
            buffer[*bufferLen+2] = SPACE;
            *bufferLen += 3;
        }
    }
    
    if (vf->tense == PERFECT)
    {
        if (vf->mood == INDICATIVE)
        {
            if (vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_OMICRON;
                if (!hasPrefix || decompose)
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
                else
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                if (vf->person == SECOND)
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_SIGMA;
                else
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
                *bufferLen += 3;
            }
            else
            {
                if (!hasPrefix || decompose)
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                else
                    buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
                *bufferLen += 2;
            }
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ALPHA;
                *bufferLen += 1;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_THETA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_ALPHA;
                *bufferLen += 2;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = LEFT_PARENTHESIS;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+3] = RIGHT_PARENTHESIS;
                *bufferLen += 4;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_MU;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen += 2;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ALPHA;
                buffer[*bufferLen+1] = COMBINING_MACRON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_IOTA;
                buffer[*bufferLen+4] = LEFT_PARENTHESIS;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+6] = RIGHT_PARENTHESIS;
                *bufferLen += 7;
            }
        }
        else if (vf->mood == SUBJUNCTIVE)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            if (!hasPrefix || decompose)
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 3;
            if (decompose)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                *bufferLen += 4;
            }
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                *bufferLen += 1;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                *bufferLen += 2;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_MU;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen +3] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 4;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen += 3;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_IOTA;
                buffer[*bufferLen+3] = LEFT_PARENTHESIS;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+5] = RIGHT_PARENTHESIS;
                *bufferLen += 6;
            }
        }
        else if (vf->mood == OPTATIVE)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            if (!hasPrefix | decompose)
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 3;
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 4;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                *bufferLen += 4;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                *bufferLen += 3;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_MU;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+5] = COMMA;
                buffer[*bufferLen+6] = SPACE;
                *bufferLen += 7;
                if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
                {
                    buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                    *bufferLen += 3;
                    
                    if (decompose)
                    {
                        buffer[*bufferLen] = SPACE;
                        buffer[*bufferLen+1] = HYPHEN;
                        buffer[*bufferLen+2] = SPACE;
                        *bufferLen += 3;
                    }
                }
                
                
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                if (hasPrefix && !decompose)
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                else
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
                *bufferLen += 3;
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
                
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_MU;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 6;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+4] = COMMA;
                buffer[*bufferLen+5] = SPACE;
                *bufferLen += 6;
                if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
                {
                    buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                    *bufferLen += 3;
                    
                    if (decompose)
                    {
                        buffer[*bufferLen] = SPACE;
                        buffer[*bufferLen+1] = HYPHEN;
                        buffer[*bufferLen+2] = SPACE;
                        *bufferLen += 3;
                    }
                }
                
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                if (hasPrefix && !decompose)
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                else
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
                *bufferLen += 3;
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
                
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen += 5;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_NU;
                buffer[*bufferLen+4] = COMMA;
                buffer[*bufferLen+5] = SPACE;
                *bufferLen += 6;
                if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
                {
                    buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                    buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                    *bufferLen += 3;
                    
                    if (decompose)
                    {
                        buffer[*bufferLen] = SPACE;
                        buffer[*bufferLen+1] = HYPHEN;
                        buffer[*bufferLen+2] = SPACE;
                        *bufferLen += 3;
                    }
                }
                
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_EPSILON;
                if (hasPrefix && !decompose)
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                else
                    buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_DELTA;
                *bufferLen += 3;
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
                
                buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_ETA;
                buffer[*bufferLen+3] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_ALPHA;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 6;
            }
        }
        else if (vf->mood == IMPERATIVE)
        {
            if (!hasPrefix || decompose)
                buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else
                buffer[*bufferLen] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
            *bufferLen += 2;
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
                
            }
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                return 0;
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {

                buffer[*bufferLen] = GREEK_SMALL_LETTER_THETA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
                *bufferLen += 2;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_OMEGA;
                *bufferLen += 2;
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                return 0;
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_EPSILON;
                *bufferLen += 2;
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                buffer[*bufferLen] = GREEK_SMALL_LETTER_TAU;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_OMEGA;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
            }
        }
    }
    else if (vf->tense == PLUPERFECT)
    {
        if (!decompose)
        {
            if (!hasPrefix && vf->number == SINGULAR)
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
            else if ( vf->number == SINGULAR )
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (!hasPrefix && vf->number == PLURAL)
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if ( vf->number == PLURAL)
                buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            
            if (vf->number == SINGULAR)
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_DELTA;
            else
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
            *bufferLen += 2;
        }
        else
        {
            buffer[*bufferLen] = DECOMPOSED_AUGMENT_CHAR;
            buffer[*bufferLen+1] = SPACE;
            buffer[*bufferLen+2] = HYPHEN;
            buffer[*bufferLen+3] = SPACE;
            buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            
            if (vf->number == SINGULAR)
                buffer[*bufferLen+6] = GREEK_SMALL_LETTER_DELTA;
            else
                buffer[*bufferLen+6] = GREEK_SMALL_LETTER_SIGMA;
            *bufferLen += 7;
        }
        
        if (decompose)
        {
            buffer[*bufferLen] = SPACE;
            buffer[*bufferLen+1] = HYPHEN;
            buffer[*bufferLen+2] = SPACE;
            *bufferLen += 3;
        }
        if (vf->person == FIRST && vf->number == SINGULAR)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA;
            buffer[*bufferLen+1] = COMMA;
            buffer[*bufferLen+2] = SPACE;
            *bufferLen += 3;
            
            if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
                
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
            }
            
            if (hasPrefix && !decompose)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (decompose)
            {
                buffer[*bufferLen+0] = DECOMPOSED_AUGMENT_CHAR;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                *bufferLen += 6;
            }
            else
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            buffer[*bufferLen+0] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 1;
            
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
            *bufferLen += 3;
        }
        else if (vf->person == SECOND && vf->number == SINGULAR)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_ETA;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_SIGMA;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_THETA;
            buffer[*bufferLen+3] = GREEK_SMALL_LETTER_ALPHA;
            buffer[*bufferLen+4] = COMMA;
            buffer[*bufferLen+5] = SPACE;
            *bufferLen += 6;
            
            if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
                
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
            }
            
            if (hasPrefix && !decompose)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (decompose)
            {
                buffer[*bufferLen+0] = DECOMPOSED_AUGMENT_CHAR;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                *bufferLen += 6;
            }
            else
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            buffer[*bufferLen+0] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 1;
            
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_FINAL_SIGMA;
            *bufferLen += 3;
        }
        else if (vf->person == THIRD && vf->number == SINGULAR)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_IOTA;
            buffer[*bufferLen+2] = LEFT_PARENTHESIS;
            buffer[*bufferLen+3] = GREEK_SMALL_LETTER_NU;
            buffer[*bufferLen+4] = RIGHT_PARENTHESIS;
            *bufferLen += 5;
        }
        else if (vf->person == FIRST && vf->number == PLURAL)
        {

            buffer[*bufferLen] = GREEK_SMALL_LETTER_MU;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
            buffer[*bufferLen+3] = COMMA;
            buffer[*bufferLen+4] = SPACE;
            *bufferLen += 5;
            if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
                
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
            }
            
            if (hasPrefix && !decompose)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (decompose)
            {
                buffer[*bufferLen+0] = DECOMPOSED_AUGMENT_CHAR;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                *bufferLen += 6;
            }
            else
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            buffer[*bufferLen+0] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 1;
            
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_MU;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+3] = GREEK_SMALL_LETTER_NU;
            *bufferLen += 4;
        }
        else if (vf->person == SECOND && vf->number == PLURAL)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_TAU;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+2] = COMMA;
            buffer[*bufferLen+3] = SPACE;
            
            *bufferLen += 4;
            if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
                
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
            }
            
            if (hasPrefix && !decompose)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (decompose)
            {
                buffer[*bufferLen+0] = DECOMPOSED_AUGMENT_CHAR;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                *bufferLen += 6;
            }
            else
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 1;
            
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_TAU;
            buffer[*bufferLen+2] = GREEK_SMALL_LETTER_EPSILON;
            *bufferLen += 3;
        }
        else if (vf->person == THIRD && vf->number == PLURAL)
        {
            buffer[*bufferLen] = GREEK_SMALL_LETTER_ALPHA;
            buffer[*bufferLen+1] = GREEK_SMALL_LETTER_NU;
            buffer[*bufferLen+2] = COMMA;
            buffer[*bufferLen+3] = SPACE;
            
            *bufferLen += 4;
            if (utf8HasSuffix(vf->verb->present, "σύνοιδα"))
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_SIGMA;
                buffer[*bufferLen+1] = GREEK_SMALL_LETTER_UPSILON;
                buffer[*bufferLen+2] = GREEK_SMALL_LETTER_NU;
                *bufferLen += 3;
                
                if (decompose)
                {
                    buffer[*bufferLen] = SPACE;
                    buffer[*bufferLen+1] = HYPHEN;
                    buffer[*bufferLen+2] = SPACE;
                    *bufferLen += 3;
                }
            }
            
            if (hasPrefix && !decompose)
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            else if (decompose)
            {
                buffer[*bufferLen+0] = DECOMPOSED_AUGMENT_CHAR;
                buffer[*bufferLen+1] = SPACE;
                buffer[*bufferLen+2] = HYPHEN;
                buffer[*bufferLen+3] = SPACE;
                buffer[*bufferLen+4] = GREEK_SMALL_LETTER_EPSILON;
                buffer[*bufferLen+5] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
                *bufferLen += 6;
            }
            else
            {
                buffer[*bufferLen+0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
                *bufferLen += 1;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_DELTA;
            *bufferLen += 1;
            
            if (decompose)
            {
                buffer[*bufferLen] = SPACE;
                buffer[*bufferLen+1] = HYPHEN;
                buffer[*bufferLen+2] = SPACE;
                *bufferLen += 3;
            }
            
            buffer[*bufferLen] = GREEK_SMALL_LETTER_EPSILON;
            buffer[*bufferLen + 1] = GREEK_SMALL_LETTER_SIGMA;
            buffer[*bufferLen + 2] = GREEK_SMALL_LETTER_ALPHA;
            buffer[*bufferLen + 3] = GREEK_SMALL_LETTER_NU;
            *bufferLen += 4;
        }
    }
    else
    {
        return 0;
    }
    
    return 1;
}

int getForm(VerbFormC *vf, char *utf8OutputBuffer, int bufferLen, bool includeAlternateForms, bool decompose)
{
    utf8OutputBuffer[0] = '\0'; //clear buffer
    
    int bufferLen2 = 1024;
    UCS2 ucs2Buffer[bufferLen2];
    
    int ret = getFormUCS2(vf, ucs2Buffer, &bufferLen2, includeAlternateForms, decompose);
    if (ret != 0)
    {
        ucs2_to_utf8_string(ucs2Buffer, bufferLen2, (unsigned char*)utf8OutputBuffer);
    }
    return ret;
}

int getForm2(VerbFormD *vf, char *utf8OutputBuffer, int bufferLen, bool includeAlternateForms, bool decompose)
{
    VerbFormC vfc;
    vfc.person = vf->person;
    vfc.number = vf->number;
    vfc.tense = vf->tense;
    vfc.voice = vf->voice;
    vfc.mood = vf->mood;
    vfc.verb = &verbs[vf->verbid];
    
    return getForm(&vfc, utf8OutputBuffer, bufferLen, includeAlternateForms, decompose);
}

/**
 * return 1 for success, 0 for failure
 */
int getFormUCS2(VerbFormC *vf, UCS2 *ucs2Buffer, int *bufferLen, bool includeAlternateForms, bool decompose)
{
    //clear buffer
    int i;
    //UCS2 ucs2Buffer[1024];
    //buffer needs to be cleared
    for (i = 0; i < *bufferLen; i++)
        ucs2Buffer[i] = 0;
    
    int ucs2StemPlusEndingBufferLen = 0;
    
    if (vf->mood == IMPERATIVE && vf->person == FIRST)
        return 0;
    
    //eimi/
    if ( vf->tense != FUTURE && utf8HasSuffix(vf->verb->present, "εἰμί"))
    {
        int ret = getEimi(vf, ucs2Buffer, &ucs2StemPlusEndingBufferLen);
        *bufferLen = ucs2StemPlusEndingBufferLen;
        return ret;
    }
    else if ((utf8HasSuffix(vf->verb->present, "οἶδα") || utf8HasSuffix(vf->verb->present, "οιδα")) && vf->tense != FUTURE)
    {
        int ret = getOida(vf, ucs2Buffer, &ucs2StemPlusEndingBufferLen, decompose);
        if (ret)
        {
            if (!wordIsAccented(&ucs2Buffer[0], ucs2StemPlusEndingBufferLen))
            {
                accentRecessive(vf, &ucs2Buffer[0], &ucs2StemPlusEndingBufferLen);
            }
            
            if (decompose && !ACCENT_DECOMPOSED_FORMS)
            {
                stripAccent(&ucs2Buffer[0], &ucs2StemPlusEndingBufferLen);
            }
        }
        *bufferLen = ucs2StemPlusEndingBufferLen;
        return ret;
    }
    else if (utf8HasSuffix(vf->verb->present, "δεῖ"))
    {
        int ret = getDei(vf, ucs2Buffer, &ucs2StemPlusEndingBufferLen, decompose);
        if (ret)
        {
            if (!wordIsAccented(&ucs2Buffer[0], ucs2StemPlusEndingBufferLen))
            {
                accentRecessive(vf, &ucs2Buffer[0], &ucs2StemPlusEndingBufferLen);
            }
            
            if (decompose && !ACCENT_DECOMPOSED_FORMS)
            {
                stripAccent(&ucs2Buffer[0], &ucs2StemPlusEndingBufferLen);
            }
        }
        *bufferLen = ucs2StemPlusEndingBufferLen;
        return ret;
    }
    else if (utf8HasSuffix(vf->verb->present, "χρή"))
    {
        int ret = getXrh(vf, ucs2Buffer, &ucs2StemPlusEndingBufferLen, decompose);
        if (ret)
        {
            if (!wordIsAccented(&ucs2Buffer[0], ucs2StemPlusEndingBufferLen))
            {
                accentRecessive(vf, &ucs2Buffer[0], &ucs2StemPlusEndingBufferLen);
            }
        }
        *bufferLen = ucs2StemPlusEndingBufferLen;
        return ret;
    }
    
    //Step 1: get principal part
    char *utf8Stems = getPrincipalPartForTense(vf->verb, vf->tense, vf->voice);
    if (!utf8Stems || utf8Stems[0] == '\0')
    {
        return 0;
    }
    
    //middle deponents do not have a passive voice.  H&Q page 316
    if (deponentType(vf->verb) == MIDDLE_DEPONENT && vf->voice == PASSIVE)
    {
        return 0;
    }
    if (deponentType(vf->verb) == PASSIVE_DEPONENT && vf->voice == PASSIVE && (vf->tense == PRESENT || vf->tense == IMPERFECT || vf->tense == PERFECT || vf->tense == PLUPERFECT)) //aorist or future are ok
    {
        return 0;
    }
    
    //Test special voice stuff here
    //this verb has no passive (though does have an active form of perfect), is a different kind of deponent
    //maybe better to handle this somewhere else
    //see H&Q page 382
    if (utf8HasSuffix(vf->verb->present, "γίγνομαι") && vf->voice == PASSIVE)
    {
        return 0;
    }
    else if (utf8HasSuffix(vf->verb->present, "φημί") && vf->voice != ACTIVE)
    {
        return 0;
    }
    else if (utf8HasSuffix(vf->verb->present, "εἶμι") && vf->voice != ACTIVE)
    {
        return 0;
    }
    else if (utf8HasSuffix(vf->verb->present, "κεῖμαι") && vf->voice != MIDDLE)
    {
        return 0;
    }
    
    /*
     13 erxomai      - should be partial deponent, no passive.  present only used in the indicative in attic greek.
        This part should be done in verbseq because I want to show all forms in paradigm even if not used in attic
     
     13 metanistamai - should be partial deponent, no passive?
     14 epanistamai  - should be partial deponent, no passive?
     14 epideiknumai - ?
     
     15 epomai       - should be middle deponent, no passive
     
     *16 bainw (has passive see Montanari p 371
     *16 anabainw - these do have passives
     *16 gignwskw is ok, there is a passive be known
     
     16 piptw, no passive
     17 epistamai   - aorist passive has middle meaning, so no passive
     *17 exw         - does have a passive meaning
     18 apothnhskw  - no passive
     *18 apokteinw   - can have a passive meaning
     *18 mellw       - seems to rarely show up in the passive meaning to be delayed
     19 apollumi    - shouldn't have a passive
     */
     /*
     if (vf->voice == PASSIVE && (
      utf8HasSuffix(v->present, "ἔρχομαι") ||
     utf8HasSuffix(v->present, "μετανίσταμαι") ||
     utf8HasSuffix(v->present, "ἐπανίσταμαι") ||
     utf8HasSuffix(v->present, "ἐπιδείκνυμαι") ||
     utf8HasSuffix(v->present, "ἕπομαι") ||
      // utf8HasSuffix(v->present, "ἐπίσταμαι") ||
      utf8HasSuffix(v->present, "ἐκπίπτω") ||
      utf8HasSuffix(v->present, "πίπτω") ||
      utf8HasSuffix(v->present, "ἀποθνῄσκω") ||
      utf8HasSuffix(v->present, "ἀπόλλῡμι")

     ))
     {
        return 0;
     }
     */
    
    
    /*
    if (vf->mood == IMPERATIVE && vf->person == FIRST)
    {
        return 0;
    }
    */
    UCS2 ucs2Stems[(strlen(utf8Stems) * 3) + 1];
    int ucs2StemsLen = 0;
    utf8_to_ucs2_string((const unsigned char*)utf8Stems, ucs2Stems, &ucs2StemsLen);
    
    //convert any tonos into oxia, just in case.
    tonosToOxia(ucs2Stems, ucs2StemsLen);
    
    //find out how many stems, then how many endings.  loop through stems adding each ending in an inner loop.
    //accent each inside the loop
    
    int stemStarts[5] = { 0,0,0,0,0 };  //we leave space for up to five alternate stems
    int numStems = 1;
    for (i = 0; i < ucs2StemsLen; i++)
    {
        if (ucs2Stems[i] == SPACE)//space, 002C == comma
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

    //Step 2: for each stem of this principal part...
    for (stem = 0; stem < numStems; stem++)
    {
        stemStart = stemStarts[stem];
        stemLen = stemStarts[stem + 1] - stemStarts[stem] - 2;
     
        //Step 3: check for contracted future before stripping accent, has to be done for each stem
        bool contractedFuture = false;
        UCS2 contractedMiddleDeponentFuture[] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
        
        if (vf->tense == FUTURE && (ucs2Stems[stemLen - 1] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI || hasSuffix(&ucs2Stems[stemStart], stemLen, contractedMiddleDeponentFuture, 5) ))
        {
            contractedFuture = true;
        }
        
        //isthmi root aorist has no middle voice
        if ( (utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω") || utf8HasSuffix(vf->verb->present, "βαίνω") || utf8HasSuffix(vf->verb->present, "γιγνώσκω"))&& vf->tense == AORIST && vf->voice == MIDDLE && ucs2Stems[stemStart + stemLen -1] == GREEK_SMALL_LETTER_NU)
        {
            continue;
        }
        else if (utf8HasSuffix(vf->verb->present, "ἀπόλλῡμι") && vf->voice == MIDDLE && vf->tense == AORIST && stem == 0)
        {
            continue;
        }
        
        //Step 4: strip accent from principal part
        stripAccent(&ucs2Stems[stemStart], &stemLen);
        
        //weed out active forms if aorist deponent
        UCS2 aorDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
        if (vf->tense == AORIST && vf->voice == ACTIVE && hasSuffix(&ucs2Stems[stemStart], stemLen, aorDeponent, 4))
        {
            if (numStems < 2)
                return 0;
            else
                continue;
        }
        
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
        
        //for swzw perfect and pluperfect mid/passive with multiple stems in second person sing and pl,
        //the forms end up being the same, so just use one
        //fix me, what about 3rd plural?
        if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA_2) == CONSONANT_STEM_PERFECT_SIGMA_2 && (vf->tense == PERFECT || vf->tense == PLUPERFECT) && (vf->voice == PASSIVE || vf->voice == MIDDLE) && vf->person == SECOND)
        {
            if (stem > 0)
            {
                continue;
            }
        } //skip 3rd plural also for now. fix me later?
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA_2) == CONSONANT_STEM_PERFECT_SIGMA_2 && (vf->tense == PERFECT || vf->tense == PLUPERFECT) && (vf->voice == PASSIVE || vf->voice == MIDDLE) && vf->person == THIRD && vf->number == PLURAL)
        {
            if (stem == 0)
            {
                continue;
            }
        }
        
        //Step 5: get appropriate ending for this stem
        //This needs to be in the stems loop.  What if the stems require different endings?
        char *utf8Ending = getEnding(vf, &ucs2Stems[stemStart], stemLen, contractedFuture, !decompose); //get ending here before stripping from pp, so know if 2nd aorist
        if (!utf8Ending)
            return 0;
        
        UCS2 ucs2Endings[(strlen(utf8Ending) * 3) + 1];
        int ucs2EndingsLen = 0;
        utf8_to_ucs2_string((const unsigned char*)utf8Ending, ucs2Endings, &ucs2EndingsLen);
        
        //convert any tonos into oxia, just in case.
        tonosToOxia(ucs2Endings, ucs2EndingsLen);
        
        //phhmi/
        if (vf->person == SECOND && vf->number == SINGULAR && vf->tense == IMPERFECT && vf->voice == ACTIVE && vf->mood == INDICATIVE && utf8HasSuffix(vf->verb->present, "φημί"))
        {
            ucs2Endings[0] = GREEK_SMALL_LETTER_ETA;
            ucs2Endings[1] = GREEK_SMALL_LETTER_SIGMA;
            ucs2Endings[2] = GREEK_SMALL_LETTER_THETA;
            ucs2Endings[3] = GREEK_SMALL_LETTER_ALPHA;
            ucs2Endings[4] = COMMA;
            ucs2Endings[5] = SPACE;
            ucs2Endings[6] = GREEK_SMALL_LETTER_ETA;
            ucs2Endings[7] = GREEK_SMALL_LETTER_FINAL_SIGMA;
            ucs2EndingsLen = 8;
        }
        else if (vf->person == SECOND && vf->number == SINGULAR && vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && (utf8HasSuffix(vf->verb->present, "ῑ̔́ημι") || utf8HasSuffix(vf->verb->present, "ῑ́ημι")))
        {
            ucs2Endings[ucs2EndingsLen] = COMMA;
            ucs2Endings[ucs2EndingsLen + 1] = SPACE;
            ucs2Endings[ucs2EndingsLen + 2] = GREEK_SMALL_LETTER_EPSILON;
            ucs2Endings[ucs2EndingsLen + 3] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
            ucs2Endings[ucs2EndingsLen + 4] = GREEK_SMALL_LETTER_FINAL_SIGMA;
            ucs2EndingsLen += 5;
        }
        else if (utf8HasSuffix(vf->verb->present, "εἶμι") && ((vf->tense == IMPERFECT && vf->number == SINGULAR && vf->person != THIRD) || (vf->tense == IMPERFECT && vf->number == PLURAL && vf->person == THIRD) || (vf->mood == OPTATIVE && vf->person == FIRST && vf->number == SINGULAR)))
        {
            if (vf->tense == IMPERFECT)
            {
                if (vf->person == FIRST)
                {
                    ucs2Endings[ucs2EndingsLen] = COMMA;
                    ucs2Endings[ucs2EndingsLen + 1] = SPACE;
                    ucs2Endings[ucs2EndingsLen + 2] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2Endings[ucs2EndingsLen + 3] = GREEK_SMALL_LETTER_IOTA;
                    ucs2Endings[ucs2EndingsLen + 4] = GREEK_SMALL_LETTER_NU;
                    ucs2EndingsLen += 5;
                }
                else if (vf->person == SECOND) // the epsilong character here is being overwritten by the ifrst alternate which is changed in addending at the other //heheh
                {
                    ucs2Endings[ucs2EndingsLen] = COMMA;
                    ucs2Endings[ucs2EndingsLen + 1] = SPACE;
                    ucs2Endings[ucs2EndingsLen + 2] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2Endings[ucs2EndingsLen + 3] = GREEK_SMALL_LETTER_IOTA;
                    ucs2Endings[ucs2EndingsLen + 4] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                    ucs2EndingsLen += 5;
                }
                else if (vf->person == THIRD)
                {
                    ucs2Endings[ucs2EndingsLen] = COMMA;
                    ucs2Endings[ucs2EndingsLen + 1] = SPACE;
                    ucs2Endings[ucs2EndingsLen + 2] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2Endings[ucs2EndingsLen + 3] = GREEK_SMALL_LETTER_SIGMA;
                    ucs2Endings[ucs2EndingsLen + 4] = GREEK_SMALL_LETTER_ALPHA;
                    ucs2Endings[ucs2EndingsLen + 5] = GREEK_SMALL_LETTER_NU;
                    ucs2EndingsLen += 6;
                }
            }
            else if (vf->mood == OPTATIVE)
            {
                ucs2Endings[ucs2EndingsLen] = COMMA;
                ucs2Endings[ucs2EndingsLen + 1] = SPACE;
                ucs2Endings[ucs2EndingsLen + 2] = GREEK_SMALL_LETTER_OMICRON;
                ucs2Endings[ucs2EndingsLen + 3] = GREEK_SMALL_LETTER_IOTA;
                ucs2Endings[ucs2EndingsLen + 4] = GREEK_SMALL_LETTER_ETA;
                ucs2Endings[ucs2EndingsLen + 5] = GREEK_SMALL_LETTER_NU;
                ucs2EndingsLen += 6;
            }
        }
        int endingStarts[5] = { 0,0,0,0,0 };  //we leave space for up to five alternate endings
        int numEndings = 1;
        for (i = 0; i < ucs2EndingsLen; i++)
        {
            if (ucs2Endings[i] == SPACE)//space, 002C == comma
            {
                endingStarts[numEndings] = i + 1;
                numEndings++;
            }
        }
        endingStarts[numEndings] = i + 2; //to get length of last stem. plus 2 to simulate a comma and space
        //Step 6: for each alternative ending...
        for (ending = 0; ending < numEndings; ending++)
        {
            endingStart = endingStarts[ending];
            endingLen = endingStarts[ending + 1] - endingStarts[ending] - 2;
            
            //add stem to temp buffer
            for (i = stemStart; i < (stemStart + stemLen); i++)
            {
                ucs2Buffer[ucs2StemPlusEndingBufferLen] = ucs2Stems[i];
                ucs2StemPlusEndingBufferLen++;
            }
            
            int stemStartInBuffer = ucs2StemPlusEndingBufferLen - stemLen; //set to the index in ucs2Buffer
            int tempStemLen = stemLen;
            
            //Step 7: remove ending from principal part
            stripEndingFromPrincipalPart(&ucs2Buffer[stemStartInBuffer], &tempStemLen, vf->tense, vf->voice);
            
            //only use one of the aorist passive imperative endings for second singular
            if (vf->mood == IMPERATIVE && vf->tense == AORIST && vf->voice == PASSIVE && vf->number == SINGULAR && vf->person == SECOND)
            {
                if (ending > 0)
                {
                    //we only add one of these endings and we decide which one in addEnding.
                    ucs2StemPlusEndingBufferLen -= tempStemLen + 2; //two more for the comma + space
                    continue;
                }
                else
                {
                    //decide which imperative ending
                    if (ucs2Buffer[tempStemLen - 1] == GREEK_SMALL_LETTER_CHI || ucs2Buffer[tempStemLen - 1] == GREEK_SMALL_LETTER_PHI || ucs2Buffer[tempStemLen - 1] == GREEK_SMALL_LETTER_THETA)
                    {
                        ucs2Endings[endingStart + 1] = GREEK_SMALL_LETTER_TAU;
                    }
                    else
                    {
                        ucs2Endings[endingStart + 1] = GREEK_SMALL_LETTER_THETA;
                    }
                }
            }
            //Step 8: add or remove augment
            if (vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
            {
                augmentStem(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen, decompose);
            }
            else if (decompose && (vf->verb->verbclass & PREFIXED) == PREFIXED && vf->tense != AORIST)
            {
                decomposePrefixes(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen);
            }
            
            //De-augment
            if ( (vf->tense == AORIST && (vf->mood == SUBJUNCTIVE || vf->mood == OPTATIVE || vf->mood == IMPERATIVE || decompose)) || (vf->tense == FUTURE && vf->voice == PASSIVE))
            {
                UCS2 ucs2Present[(strlen(vf->verb->present) * 2) + 1];
                int ucs2PresentLen = 0;
                utf8_to_ucs2_string((const unsigned char*)vf->verb->present, ucs2Present, &ucs2PresentLen);
                stripAccent(ucs2Present, &ucs2PresentLen);
                UCS2 presentInitialLetter = ucs2Present[0];
                
                stripAugmentFromPrincipalPart(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen, presentInitialLetter, decompose);
            }
            //Step 9: add the ending to the principal part
            addEnding(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen, &ucs2Endings[endingStart], endingLen, contractedFuture, decompose);

            //Labe/ Accent EXCEPTION H&Q page 326
            //elthe/ accent exception h&q page 383
            //ide/ h&q page 449
            //them all: λαβέ ἐλθέ εἰπέ εὑρέ ἰδέ φαθί
            //remember, this exception is only when verb is not prefixed.
            
            UCS2 ide[] = { GREEK_SMALL_LETTER_IOTA_WITH_PSILI, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_EPSILON } ;
            UCS2 labe[] = { GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_BETA, GREEK_SMALL_LETTER_EPSILON } ;
            UCS2 elthe[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_THETA, GREEK_SMALL_LETTER_EPSILON } ;
            UCS2 eipe[] = { GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_IOTA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_EPSILON } ;
            UCS2 eure[] = { GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_UPSILON_WITH_DASIA, GREEK_SMALL_LETTER_RHO, GREEK_SMALL_LETTER_EPSILON } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, ide, 3)))
            {
                ucs2Buffer[2] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            }
            else if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, labe, 4) || hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, elthe, 4) || hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, eipe, 4) || hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, eure, 4)))
            {
                ucs2Buffer[3] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            }
            //exception h&q page 376, dos when compounded is paroxytone
            UCS2 apodos[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_FINAL_SIGMA } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, apodos, 6) ))
            {
                ucs2Buffer[2] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
            }
            //exception h&q page 376, dos when compounded is paroxytone
            UCS2 metados[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_FINAL_SIGMA } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, metados, 7) ))
            {
                ucs2Buffer[3] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            }
            //exception h&q page 376, dou when compounded with polysyllablic prefix is paroxytone
            //Already done??
            UCS2 metadou[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == MIDDLE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, metadou, 7) ))
            {
              ucs2Buffer[3] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
              ucs2Buffer[6] = GREEK_SMALL_LETTER_UPSILON;
            }
            //exception h&q page 376, dou when compounded with polysyllablic prefix is paroxytone
            UCS2 apodou[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == MIDDLE && (hasPrefix(&ucs2Buffer[stemStartInBuffer], tempStemLen, apodou, 6) ))
            {
                ucs2Buffer[2] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
                ucs2Buffer[5] = GREEK_SMALL_LETTER_UPSILON;
            }
            
            if (vf->tense == PRESENT && vf->mood == INDICATIVE && vf->voice == ACTIVE && utf8HasSuffix(vf->verb->present, "φημί"))
            {
                accentWord(&ucs2Buffer[stemStartInBuffer], &tempStemLen, ULTIMA, ACUTE);
            }
            
            //Step 10: add accent
            //add accent, if word does not already have one
            if (!wordIsAccented(&ucs2Buffer[stemStartInBuffer], tempStemLen))
            {
                accentRecessive(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen);
            }
            
            //do contraction here
            //contractEnding(vf, &ucs2Buffer[stemStartInBuffer], &tempStemLen, &ucs2Endings[endingStart], endingLen);
            
            if (decompose && !ACCENT_DECOMPOSED_FORMS)
            {
                stripAccent(&ucs2Buffer[stemStartInBuffer], &tempStemLen);
            }
            
            ucs2StemPlusEndingBufferLen += (tempStemLen - stemLen);
            
            ucs2Buffer[ucs2StemPlusEndingBufferLen] = COMMA; //comma
            ucs2StemPlusEndingBufferLen++;
            ucs2Buffer[ucs2StemPlusEndingBufferLen] = SPACE; //space
            ucs2StemPlusEndingBufferLen++;
        }
    }
    ucs2StemPlusEndingBufferLen -= 2; //remove trailing comma and space.
    
    if (vf->mood == IMPERATIVE && vf->person == FIRST && ucs2StemPlusEndingBufferLen > 0) //check len so we don't give dashes for deponents in active voice
    {
        ucs2Buffer[0] = EM_DASH;
        ucs2StemPlusEndingBufferLen = 1;
    }
    
    //add eu alternates here here H&Q p. 552
    if (utf8HasSuffix(vf->verb->present, "εὑρίσκω"))
    {
        if ((vf->tense == AORIST || vf->tense == IMPERFECT || vf->tense == PERFECT || vf->tense == PLUPERFECT) && vf->mood == INDICATIVE)// ucs2Buffer[0] == GREEK_SMALL_LETTER_ETA)
        {
            ucs2Buffer[ucs2StemPlusEndingBufferLen] = COMMA; //comma
            ucs2StemPlusEndingBufferLen++;
            ucs2Buffer[ucs2StemPlusEndingBufferLen] = SPACE; //space
            ucs2StemPlusEndingBufferLen++;
            ucs2Buffer[ucs2StemPlusEndingBufferLen] = GREEK_SMALL_LETTER_EPSILON;
            ucs2StemPlusEndingBufferLen++;
            
            int j = 1;
            int i = 1;
            if (decompose && vf->tense != PERFECT && vf->tense != PLUPERFECT)
                j = 5;
            
            for (; j < (ucs2StemPlusEndingBufferLen - 3); i++, j++)
            {
                ucs2Buffer[ucs2StemPlusEndingBufferLen+i-1] = ucs2Buffer[j];
            }
            ucs2StemPlusEndingBufferLen += (j-1);
        }
    }
    
    *bufferLen = ucs2StemPlusEndingBufferLen;
    if (ucs2StemPlusEndingBufferLen < 1)
        return 0;
    else
        return 1;
}
/*
//contraction for present and imperfect
void contractEnding(VerbFormC *vf, UCS2 *buffer, int *len, UCS2 *ending, int endingLen)
{
    if ((vf->tense == PRESENT || vf->tense == IMPERFECT) && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι") || utf8HasSuffix(vf->verb->present, "έω") || utf8HasSuffix(vf->verb->present, "έομαι") || utf8HasSuffix(vf->verb->present, "όω") || utf8HasSuffix(vf->verb->present, "όομαι")))
    {
        //1st sing and plural, 3rd pl
        if (buffer[*len - endingLen - 1] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA && ending[0] == GREEK_SMALL_LETTER_OMEGA)
        {
            leftShiftFromOffsetSteps(buffer, *len - endingLen - 1, 1, len);
            buffer[*len - 1] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
        }
        else if (buffer[*len - endingLen - 1] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA && ending[0] == GREEK_SMALL_LETTER_EPSILON && endingLen > 1 && ending[1] == GREEK_SMALL_LETTER_IOTA)
        {
            leftShiftFromOffsetSteps(buffer, *len - endingLen, 2, len);
            buffer[*len - 2] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        }
        
    }
}
*/
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

int deponentType2(int verbid)
{
    Verb *v = &verbs[verbid];
    return deponentType(v);
}

//page 316 in h&q
int deponentType(Verb *v)
{
    if (utf8HasSuffix(v->present, "γίγνομαι"))
    {
        return DEPONENT_GIGNOMAI; //see H&Q page 382. fix me, there may be a better way to do this without separate case
    }
    else if ( utf8HasSuffix(v->present, "μαι") && utf8HasSuffix(v->future, "μαι") && utf8HasSuffix(v->aorist, "μην") && v->perf[0] == '\0' && utf8HasSuffix(v->perfmid, "μαι") && v->aoristpass[0] == '\0')
    {
        return MIDDLE_DEPONENT;
    }
    else if ( utf8HasSuffix(v->present, "μαι") && utf8HasSuffix(v->future, "μαι") && v->aorist[0] == '\0' && v->perf[0] == '\0' && utf8HasSuffix(v->perfmid, "μαι") && v->aoristpass[0] != '\0')
    {
        return PASSIVE_DEPONENT;
    }
    else if (utf8HasSuffix(v->present, "μαι") || utf8HasSuffix(v->future, "μαι") || utf8HasSuffix(v->aorist, "μην") )
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
    UCS2 ending[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };

    UCS2 ending2[] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    if ((vf->tense == PRESENT || vf->tense == IMPERFECT || vf->tense == FUTURE) && hasSuffix(stem, stemLen, ending, 3))
        return true;
    
    if (vf->tense == AORIST && vf->voice != PASSIVE && hasSuffix(stem, stemLen, ending2, 4))
        return true;
    
    return false;
}

bool accentRecessive(VerbFormC *vf, UCS2 *tempUcs2String, int *len)
{
    if (*len < 1 || tempUcs2String[0] == EM_DASH)
    {
        return true;
    }
    
    //find syllable
    int i = 0;
    int vowelsAndDiphthongs = 0;
    int longUltima = false;
    int longPenult = false;
    int consonants = 0;
    bool isOptative = (vf->mood == OPTATIVE) ? true : false;
    int accentableRegionStart = 0; //might be different if prefixed and aorist indic or perf/plup
    
    /*
     For prefixes, find where the prefix ends and don't look past that character
     */
    if ((vf->verb->verbclass & PREFIXED) == PREFIXED && !utf8HasSuffix(vf->verb->present, "σύνοιδα") && ((vf->tense == AORIST && vf->mood == INDICATIVE) || vf->tense == PERFECT || vf->tense == PLUPERFECT))
    {
        UCS2 ek[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_KAPPA };
        UCS2 ana[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 sum[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU };
        UCS2 sun[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_NU };
        UCS2 dia[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_ALPHA };
        //UCS2 dio[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_OMICRON }; //fix me hack so this doesn't work on future passive
        
        UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON };
        UCS2 ap[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI };
        UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
        UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
        UCS2 kata[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 metana[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 epi[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_IOTA };
        
        if (hasPrefix(tempUcs2String, *len, ek, 2))
            accentableRegionStart = 2;
        else if (hasPrefix(tempUcs2String, *len, ana, 3))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, sum, 3))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, sun, 3))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, dia, 3))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, apo, 3) && !utf8HasSuffix(vf->verb->present, "ἀπόλλῡμι"))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, ap, 2))
            accentableRegionStart = 2;
        else if (hasPrefix(tempUcs2String, *len, aph, 2))
            accentableRegionStart = 2;
        else if (hasPrefix(tempUcs2String, *len, kath, 3))
            accentableRegionStart = 3;
        else if (hasPrefix(tempUcs2String, *len, kata, 4))
            accentableRegionStart = 4;
        else if (hasPrefix(tempUcs2String, *len, metana, 6))
            accentableRegionStart = 6;
        else if (hasPrefix(tempUcs2String, *len, metan, 5))
            accentableRegionStart = 5;
        else if (hasPrefix(tempUcs2String, *len, meta, 4))
            accentableRegionStart = 4;
        else if (hasPrefix(tempUcs2String, *len, epan, 4))
            accentableRegionStart = 4;
        else if (hasPrefix(tempUcs2String, *len, epi, 3))
            accentableRegionStart = 3;
        else
            accentableRegionStart = 0;
    }
    
    for ( i = *len - 1; i >= accentableRegionStart; i--) //start at end of word and work left
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
    accentSyllable(ucs2String, i, len, accent, false, PRECOMPOSED_HC_MODE);
    return true;
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
                break; /*
            case GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI: //fix me, this needs to be done for the other cases below
                rightShiftFromOffset(Stepsword, i, 1, len);
                word[i] = GREEK_SMALL_LETTER_ALPHA;
                word[i+1] = COMBINING_MACRON;
                break; */
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
            //case GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_IOTA;
                break;
                
            case GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI: //to fix decomposed macron on krine
                if (i > 0 && word[i-1] != GREEK_SMALL_LETTER_EPSILON && word[i-1] != GREEK_SMALL_LETTER_OMICRON && word[i-1] != GREEK_SMALL_LETTER_ALPHA && word[i-1] != SPACE)
                {
                    rightShiftFromOffsetSteps(word, i, 1, len);
                    word[i] = GREEK_SMALL_LETTER_IOTA;
                    word[i+1] = COMBINING_MACRON;
                }
                else
                {
                    word[i] = GREEK_SMALL_LETTER_IOTA;
                }
                break;
                
            case GREEK_SMALL_LETTER_OMICRON_WITH_OXIA:
            case GREEK_SMALL_LETTER_OMICRON_WITH_TONOS:
                word[i] = GREEK_SMALL_LETTER_OMICRON;
                break;
            case GREEK_SMALL_LETTER_UPSILON_WITH_OXIA:
            case GREEK_SMALL_LETTER_UPSILON_WITH_TONOS:
            case GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI:
                word[i] = GREEK_SMALL_LETTER_UPSILON;
                break; /*
            case GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI:
                rightShiftFromOffsetSteps(word, i, 1, len);
                word[i] = GREEK_SMALL_LETTER_UPSILON;
                word[i+1] = COMBINING_MACRON;
                break; */
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
            case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI:
                if (i > 0 && word[i-1] != GREEK_SMALL_LETTER_EPSILON && word[i-1] != GREEK_SMALL_LETTER_OMICRON && word[i-1] != GREEK_SMALL_LETTER_ALPHA)
                {
                rightShiftFromOffsetSteps(word, i, 2, len);
                word[i] = GREEK_SMALL_LETTER_IOTA;
                word[i+1] = COMBINING_MACRON;
                word[i+2] = COMBINING_ROUGH_BREATHING;
                }
                else
                {
                    word[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                }
                break;
            case GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA:
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

void tonosToOxia(UCS2 *word, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (word[i] == GREEK_SMALL_LETTER_ALPHA_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_EPSILON_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_ETA_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_IOTA_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_OMICRON_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_UPSILON_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
        else if (word[i] == GREEK_SMALL_LETTER_OMEGA_WITH_TONOS)
            word[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
    }
}

//accents should be stripped before calling this
void stripEndingFromPrincipalPart(UCS2 *stem, int *len, unsigned char tense, unsigned char voice)
{
    UCS2 keimai[6] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA};
    
    UCS2 presMi[2] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_IOTA };
    UCS2 presMiDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    UCS2 presNumiDeponent[5] = { GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    UCS2 presDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    UCS2 aoristDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    UCS2 secondAoristDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    UCS2 secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    
    UCS2 isthmiRootAorist[2] = { GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };

    UCS2 futureDeponentContracted[5] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    if (hasSuffix(stem, *len, keimai, 6))
        *len -= 3;
    else if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presNumiDeponent, 5)) //νυμαι
        *len -= 3;
    else if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presMiDeponent, 4)) //μαι
        *len -= 3;
    else if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presMi, 2)) //μι
        *len -= 2;
    else if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presDeponent, 4)) //ομαι
        *len -= 4;
    else if (tense == PRESENT || tense == IMPERFECT)
        *len -= 1;
    else if (tense == FUTURE  && voice != PASSIVE && hasSuffix(stem, *len, futureDeponentContracted, 5)) //ουμαι
        *len -= 5;
    else if (tense == FUTURE  && voice != PASSIVE && hasSuffix(stem, *len, presDeponent, 4)) //ομαι
        *len -= 4;
    else if (tense == FUTURE && voice != PASSIVE)
        *len -= 1;
    else if (tense == FUTURE && voice == PASSIVE)
        *len -= 2;
    else if (tense == AORIST && voice != PASSIVE  && hasSuffix(stem, *len, aoristDeponent, 4)) //άμην
        *len -= 4;
    else if (tense == AORIST && voice == ACTIVE && hasSuffix(stem, *len, isthmiRootAorist, 2)) //ην
        *len -= 1;
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
        case GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI:
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI:
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
        case GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI:
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
        case GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA:
        case GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI:
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
    if ((tempUcs2String[i] == GREEK_SMALL_LETTER_UPSILON || tempUcs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA || tempUcs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI ) && i != 0 && (tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_EPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ETA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_OMICRON))
    {
        return true;
    }
    else if ( (tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA || tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA || tempUcs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI ) && i != 0 && (tempUcs2String[i - 1] == GREEK_SMALL_LETTER_ALPHA || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_EPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_UPSILON || tempUcs2String[i - 1] == GREEK_SMALL_LETTER_OMICRON))
    {
        return true;
    }
    
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

