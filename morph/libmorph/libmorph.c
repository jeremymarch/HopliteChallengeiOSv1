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

#define DECOMPOSED_AUGMENT_CHAR GREEK_SMALL_LETTER_EPSILON

bool onlyUseCombiningDiacritics = false; //not used yet

/*
 TO DO:
 roll back word when erasing them instead of just hiding
 show buttons before keyboard disappears
 click to show principal parts or endings from decompose form.
 add definition and principal parts to top of verb detail view
 
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
void augmentStem(VerbFormC *vf, UCS2 *ucs2, int *len, bool decomposed);
void decomposePrefixes(VerbFormC *vf, UCS2 *ucs2, int *len);
void stripAugmentFromPrincipalPart(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 presentStemInitial, bool decompose);
void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int elen, bool contractedFuture, bool decompose);
bool accentRecessive(UCS2 *tempUcs2String, int *len, bool optative);

void rightShift(UCS2 *ucs2, int *len);
void leftShift(UCS2 *ucs2, int *len);
void leftShiftFromOffset(UCS2 *ucs2, int i, int *len);
void rightShiftFromOffset(UCS2 *ucs2, int offset, int *len);

char *persons[NUM_PERSONS] = { "first", "second", "third" };
char *personsabbrev[NUM_PERSONS] = { "1st", "2nd", "3rd" };
char *numbers[NUM_NUMBERS] = { "singular", "plural" };
char *numbersabbrev[NUM_NUMBERS] = { "sing.", "pl." };
char *tenses[NUM_TENSES] = { "present", "imperfect", "future", "aorist", "perfect", "pluperfect" };
char *tensesabbrev[NUM_TENSES] = { "pres.", "imp.", "fut.", "aor.", "perf.", "plup." };
char *voices[NUM_VOICES] = { "active", "middle", "passive" };
char *voicesabbrev[NUM_VOICES] = { "act.", "mid.", "pass." };
char *moods[NUM_MOODS] = { "indicative", "subjunctive", "optative", "imperative" };
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
    
    while((w1[i] == SPACE || w1[i] == COMMA) && i < w1len)
        i++;
    
    starts[0] = i;
    for ( ; i < w1len; i++)
    {
        if (w1[i] == COMMA || w1[i] == SPACE)
        {
            ends[end++] = i;
            i++;
            while((w1[i] == SPACE || w1[i] == COMMA) && i < w1len)
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
    
    UCS2 isthmi2[4] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    UCS2 deponent[] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    int ending = 0;
    /* MI */
    if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == INDICATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA)
        ending = PRESENT_ACTIVE_INDICATIVE_MI;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == INDICATIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "νυμαι")))
        ending = PERFECT_MIDPASS_IND;
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE  && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && (!preContactedEndings || word[wordLen - 3] == COMBINING_MACRON))
        ending = PRESENT_ACTIVE_SUBJ;
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && preContactedEndings)
        ending = AORIST_PASSIVE_SUBJ;

    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && (!preContactedEndings || word[wordLen - 3] == COMBINING_MACRON))
        ending = PRESENT_MIDPASS_SUBJ;
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == SUBJUNCTIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && preContactedEndings)
        ending = PRESENT_MIDPASS_SUBJ_E_CONTRACTED;
    
    
    else if (vf->tense == PRESENT && vf->voice == ACTIVE && vf->mood == OPTATIVE && word[wordLen - 2] == GREEK_SMALL_LETTER_MU && word[wordLen - 1] == GREEK_SMALL_LETTER_IOTA && word[wordLen - 3] != COMBINING_MACRON)
        ending = AORIST_PASSIVE_OPT;
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && utf8HasSuffix(vf->verb->present, "τίθημι"))
        ending = PRESENT_MIDPASS_OPT_TITHHMI; //Exception: H&Q page 347
    
    else if (vf->tense == PRESENT && (vf->voice == MIDDLE || vf->voice == PASSIVE) && vf->mood == OPTATIVE && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "σταμαι")))
        ending = PRESENT_MIDPASS_OPT;
    
    /* isthmi root aorist */
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == INDICATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU)
        ending = AORIST_ACTIVE_INDICATIVE_MI_ROOT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU && preContactedEndings)
        ending = AORIST_PASSIVE_SUBJ;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == SUBJUNCTIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU && !preContactedEndings)
        ending = PRESENT_ACTIVE_SUBJ;
    
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == OPTATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU )
        ending = AORIST_PASSIVE_OPT;
    else if (vf->tense == AORIST && vf->voice == ACTIVE && vf->mood == IMPERATIVE && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))  && word[wordLen - 1] == GREEK_SMALL_LETTER_NU )
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
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture && preContactedEndings)
        ending = PRESENT_ACTIVE_INDIC_E_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == ACTIVE && contractedFuture && !preContactedEndings)
        ending = PRESENT_ACTIVE_IND;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture && preContactedEndings)
        ending = PRESENT_MIDPASS_INDIC_E_CONTRACTED;
    else if (vf->tense == FUTURE && vf->voice == MIDDLE && contractedFuture && !preContactedEndings)
        ending = PRESENT_MIDPASS_IND;
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

/**
 * return 1 for success, 0 for failure
 */
int getForm(VerbFormC *vf, char *utf8OutputBuffer, int bufferLen, bool includeAlternateForms, bool decompose)
{
    //clear buffer
    for (int i = 0; i < bufferLen; i++)
        utf8OutputBuffer[i] = '\0';
    
    //Step 1: get principal part
    char *utf8Stems = getPrincipalPartForTense(vf->verb, vf->tense, vf->voice);
    if (!utf8Stems || utf8Stems[0] == '\0')
    {
        utf8OutputBuffer[0] = '\0'; //clear buffer
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
    //this verb has no passive (though does have an active form of perfect), is a different kind of deponent
    //maybe better to handle this somewhere else
    //see H&Q page 382
    if (utf8HasSuffix(vf->verb->present, "γίγνομαι") && vf->voice == PASSIVE)
    {
        return 0;
    }

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
    int i;
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
    UCS2 ucs2StemPlusEndingBuffer[1024];
    //buffer needs to be cleared
    for (i = 0; i < 1024; i++)
        ucs2StemPlusEndingBuffer[i] = 0;
    
    int ucs2StemPlusEndingBufferLen = 0;
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
        if ( (utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "φθάνω"))&& vf->tense == AORIST && vf->voice == MIDDLE && ucs2Stems[stemStart + stemLen -1] == GREEK_SMALL_LETTER_NU)
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
                ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = ucs2Stems[i];
                ucs2StemPlusEndingBufferLen++;
            }
            
            int stemStartInBuffer = ucs2StemPlusEndingBufferLen - stemLen; //set to the index in ucs2StemPlusEndingBuffer
            int tempStemLen = stemLen;
            
            //Step 7: remove ending from principal part
            stripEndingFromPrincipalPart(&ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, vf->tense, vf->voice);
            
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
                    if (ucs2StemPlusEndingBuffer[tempStemLen - 1] == GREEK_SMALL_LETTER_CHI || ucs2StemPlusEndingBuffer[tempStemLen - 1] == GREEK_SMALL_LETTER_PHI || ucs2StemPlusEndingBuffer[tempStemLen - 1] == GREEK_SMALL_LETTER_THETA)
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
                augmentStem(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, decompose);
            }
            else if (decompose && (vf->verb->verbclass & PREFIXED) == PREFIXED && vf->tense != AORIST)
            {
                decomposePrefixes(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen);
            }
            
            //De-augment
            if ( (vf->tense == AORIST && (vf->mood == SUBJUNCTIVE || vf->mood == OPTATIVE || vf->mood == IMPERATIVE || decompose)) || (vf->tense == FUTURE && vf->voice == PASSIVE))
            {
                UCS2 ucs2Present[(strlen(vf->verb->present) * 2) + 1];
                int ucs2PresentLen = 0;
                utf8_to_ucs2_string((const unsigned char*)vf->verb->present, ucs2Present, &ucs2PresentLen);
                stripAccent(ucs2Present, &ucs2PresentLen);
                UCS2 presentInitialLetter = ucs2Present[0];
                
                stripAugmentFromPrincipalPart(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, presentInitialLetter, decompose);
            }
            //Step 9: add the ending to the principal part
            addEnding(vf, &ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen, &ucs2Endings[endingStart], endingLen, contractedFuture, decompose);

            //Labe/ Accent EXCEPTION H&Q page 326
            //elthe/ accent exception h&q page 383
            
            //them all: λαβέ ἐλθέ εἰπέ εὑρέ ἰδέ φαθί
            
            UCS2 labe[] = { GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_BETA, GREEK_SMALL_LETTER_EPSILON } ;
            UCS2 elthe[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_LAMDA, GREEK_SMALL_LETTER_THETA, GREEK_SMALL_LETTER_EPSILON } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, labe, 4) || hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, elthe, 4)))
            {
                ucs2StemPlusEndingBuffer[3] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            }
            //exception h&q page 376, dos when compounded is paroxytone
            UCS2 apodos[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_FINAL_SIGMA } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, apodos, 6) ))
            {
                ucs2StemPlusEndingBuffer[2] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
            }
            //exception h&q page 376, dos when compounded is paroxytone
            UCS2 metados[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_FINAL_SIGMA } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == ACTIVE && (hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, metados, 7) ))
            {
                ucs2StemPlusEndingBuffer[3] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            }
            //exception h&q page 376, dou when compounded with polysyllablic prefix is paroxytone
            //Already done??
            UCS2 metadou[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == MIDDLE && (hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, metadou, 7) ))
            {
              ucs2StemPlusEndingBuffer[3] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
              ucs2StemPlusEndingBuffer[6] = GREEK_SMALL_LETTER_UPSILON;
            }
            //exception h&q page 376, dou when compounded with polysyllablic prefix is paroxytone
            UCS2 apodou[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI } ;
            if (vf->tense == AORIST && vf->mood == IMPERATIVE && vf->number == SINGULAR && vf->voice == MIDDLE && (hasPrefix(&ucs2StemPlusEndingBuffer[stemStartInBuffer], tempStemLen, apodou, 6) ))
            {
                ucs2StemPlusEndingBuffer[2] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
                ucs2StemPlusEndingBuffer[5] = GREEK_SMALL_LETTER_UPSILON;
            }
            
            //Step 10: add accent
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
            
            if (decompose && !ACCENT_DECOMPOSED_FORMS)
            {
                stripAccent(&ucs2StemPlusEndingBuffer[stemStartInBuffer], &tempStemLen);
            }
            
            ucs2StemPlusEndingBufferLen += (tempStemLen - stemLen);
            
            ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = COMMA; //comma
            ucs2StemPlusEndingBufferLen++;
            ucs2StemPlusEndingBuffer[ucs2StemPlusEndingBufferLen] = SPACE; //space
            ucs2StemPlusEndingBufferLen++;
        }
    }
    ucs2StemPlusEndingBufferLen -= 2; //remove trailing comma and space.
    
    if (vf->mood == IMPERATIVE && vf->person == FIRST && ucs2StemPlusEndingBufferLen > 0) //check len so we don't give dashes for deponents in active voice
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
    accentSyllable(ucs2String, i, len, accent, false);
    return true;
}

//we assume there is space for this
//makes a space at offset
void rightShiftFromOffset(UCS2 *ucs2, int offset, int *len)
{
    int j = offset + *len - 1;
    for ( ; j >= offset; j--)
    {
        ucs2[j + 1] = ucs2[j];
    }
    ++(*len);
}

void combiningToPrecomposed(UCS2 *ucs2String, int i, int *len)
{
    //remove macron and change back to pre-composed character
    bool roughBreathing = false;
    bool smoothBreathing = false;
    bool acute = false;
    bool circumFlex = false;
    bool iotaSubscript = false;
    unsigned int spacesToRemove = 1; //1 for the macron
    
    if (ucs2String[i + 2] == COMBINING_ROUGH_BREATHING || ucs2String[ i + 2] == COMBINING_SMOOTH_BREATHING)
    {
        if (ucs2String[i + 2] == COMBINING_ROUGH_BREATHING)
            roughBreathing = true;
        else
            smoothBreathing = true;
        spacesToRemove++;
        
        if (ucs2String[i + 3] == COMBINING_ACUTE || ucs2String[i + 3] == COMBINING_CIRCUMFLEX)
        {
            if (ucs2String[i + 3] == COMBINING_ACUTE)
                acute = true;
            else
                circumFlex = true;
            spacesToRemove++;
            
            if (ucs2String[i + 4] == COMBINING_IOTA_SUBSCRIPT)
            {
                iotaSubscript = true;
                spacesToRemove++;
            }
        }
        else if (ucs2String[i + 3] == COMBINING_IOTA_SUBSCRIPT)
        {
            iotaSubscript = true;
            spacesToRemove++;
        }
    }
    else if (ucs2String[i + 2] == COMBINING_ACUTE || ucs2String[i + 2] == COMBINING_CIRCUMFLEX)
    {
        if (ucs2String[i + 2] == COMBINING_ACUTE)
            acute = true;
        else
            circumFlex = true;
        spacesToRemove++;
        
        if (ucs2String[i + 3] == COMBINING_IOTA_SUBSCRIPT)
        {
            iotaSubscript = true;
            spacesToRemove++;
        }
    }
    else if (ucs2String[i + 2] == COMBINING_IOTA_SUBSCRIPT)
    {
        iotaSubscript = true;
        spacesToRemove++;
    }
    
    for (int j = 0; j < spacesToRemove; j++)
        leftShiftFromOffset(ucs2String, i + 1, len);// leftShift(&ucs2String[i+1], len);
    
    //make the pre-composed character
    if (!roughBreathing && !smoothBreathing && !acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
    }
    else if (roughBreathing && !smoothBreathing && !acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
    }
    else if (!roughBreathing && smoothBreathing && !acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
    }
    else if (!roughBreathing && !smoothBreathing && acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
    }
    else if (!roughBreathing && !smoothBreathing && !acute && circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
    }
    else if (!roughBreathing && !smoothBreathing && !acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
    }
    else if (roughBreathing && !smoothBreathing && acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
    }
    else if (!roughBreathing && smoothBreathing && acute && !circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
    }
    else if (roughBreathing && !smoothBreathing && !acute && circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
    }
    else if (!roughBreathing && smoothBreathing && !acute && circumFlex && !iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
    }
    else if (!roughBreathing && !smoothBreathing && acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
    }
    else if (!roughBreathing && !smoothBreathing && !acute && circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
    }
    else if (roughBreathing && !smoothBreathing && !acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
    }
    else if (!roughBreathing && smoothBreathing && !acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
    }
    else if (roughBreathing && !smoothBreathing && acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
    }
    else if (!roughBreathing && smoothBreathing && acute && !circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
    }
    else if (roughBreathing && !smoothBreathing && !acute && circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
    }
    else if (!roughBreathing && smoothBreathing && !acute && circumFlex && iotaSubscript)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
    }
}

/**
 * Fix me: iPhone does not display combining acute if alpha macron breathing and subscript.  This should only affect alpha.
 */
void accentSyllable(UCS2 *ucs2String, int i, int *len, int accent, bool toggleOff)
{
    if (*len < 1)
        return;
    //add accent to syllable
    //add cases for if has breathing or iota subscript etc.
    if (accent == ACUTE)
    {
        /* NOT USING PRE-COMPOSED MACRONS
        if ( ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_MACRON || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_MACRON || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_MACRON )
        {
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_ACUTE;
        }
        else */if ((i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON) || onlyUseCombiningDiacritics)
        {
            //remove
            if (toggleOff && ucs2String[i + 2] == COMBINING_ACUTE)
            {
                leftShift(&ucs2String[i + 2], len);
            }
            else if (toggleOff && ucs2String[i + 3] == COMBINING_ACUTE)
            {
                leftShift(&ucs2String[i + 3], len);
            }
            else
            {
                //or should we use the private use area codes for this?  looks good on iphone as three combining characters
                if (ucs2String[i + 2] == COMBINING_SMOOTH_BREATHING || ucs2String[i + 2] == COMBINING_ROUGH_BREATHING)
                    i += 1;
                
                rightShiftFromOffset(ucs2String, i + 2, len);
                ucs2String[i + 2] = COMBINING_ACUTE;
            }
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        
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
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
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
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;

        else if (toggleOff)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        }
    }
    else if (accent == CIRCUMFLEX)
    {
        //remove combining macron if a circumflex is going on that syllable.
        if ((i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON)  || onlyUseCombiningDiacritics)
        {
            if (i < *len - 1 && ucs2String[i + 1] == COMBINING_MACRON)
            {
                //leftShift(&ucs2String[i + 1], len);
                combiningToPrecomposed(ucs2String, i, len);
            }
        }
        
        /* no else here */ if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;

        else if (toggleOff)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;

            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        }
    }
    else if (accent == MACRON)
    {
        //if already has a macron...
        if (ucs2String[i + 1] == COMBINING_MACRON && !onlyUseCombiningDiacritics)
        {
            if (toggleOff)
            {
                combiningToPrecomposed(ucs2String, i, len);
            }
            else
            {
                //don't remove macron
            }
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA || ucs2String[i] == GREEK_SMALL_LETTER_IOTA || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
        {
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            
            rightShiftFromOffset(ucs2String, i + 2, len);
            rightShiftFromOffset(ucs2String, i + 2, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
            ucs2String[i + 3] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
            ucs2String[i + 3] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI || ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
        {
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
            ucs2String[i + 3] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
            ucs2String[i + 3] = COMBINING_ACUTE;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
        {
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            rightShiftFromOffset(ucs2String, i + 1, len);
            rightShiftFromOffset(ucs2String, i + 1, len);
            ucs2String[i + 1] = COMBINING_MACRON;
            ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
        }
    }
    else if (accent == ROUGH_BREATHING)
    {
        if (ucs2String[i + 1] == COMBINING_MACRON || onlyUseCombiningDiacritics)  //if macron we use combining rough breathing, except iota subscript
        {
            //remove
            if (toggleOff && ucs2String[i + 2] == COMBINING_ROUGH_BREATHING)
            {
                //leftShift(&ucs2String[i + 2], len);
                leftShiftFromOffset(ucs2String, i + 2, len);
            }
            else //add
            {
                //rightShift(&ucs2String[i + 2], len);
                if (ucs2String[i + 2] != COMBINING_SMOOTH_BREATHING)
                    rightShiftFromOffset(ucs2String, i + 2, len);
                ucs2String[i + 2] = COMBINING_ROUGH_BREATHING;
                //(*len) += 1;
            }
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_RHO)
            ucs2String[i] = GREEK_SMALL_LETTER_RHO_WITH_DASIA;
        else if (toggleOff)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_RHO_WITH_DASIA)
                ucs2String[i] = GREEK_SMALL_LETTER_RHO;
        }
    }
    else if (accent == SMOOTH_BREATHING)
    {
        if (ucs2String[i + 1] == COMBINING_MACRON || onlyUseCombiningDiacritics)  //if macron we use combining rough breathing, except iota subscript
        {
            //remove
            if (toggleOff && ucs2String[i + 2] == COMBINING_SMOOTH_BREATHING)
            {
                //leftShift(&ucs2String[i + 2], len);
                leftShiftFromOffset(ucs2String, i + 2, len);
            }
            else //add
            {
                //rightShift(&ucs2String[i + 2], len);
                if (ucs2String[i + 2] != COMBINING_ROUGH_BREATHING)
                    rightShiftFromOffset(ucs2String, i + 2, len);
                ucs2String[i + 2] = COMBINING_SMOOTH_BREATHING;
                //(*len) += 1;
            }
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;

        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (toggleOff)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMICRON_WITH_DASIA_AND_OXIA;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_UPSILON_WITH_DASIA_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        }
    }
    else if (accent == IOTA_SUBSCRIPT)
    {
        if (onlyUseCombiningDiacritics)  //ONLY USE COMBINGING iota subscript, if all combining
        {
            //FIX ME, probably WRONG INDEX for iota subscript
            //rightShift(&ucs2String[i + 2], len);
            rightShiftFromOffset(ucs2String, i + 2, len);
            ucs2String[i + 1] = COMBINING_IOTA_SUBSCRIPT;
            //(*len) += 1;
        }
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI)
            ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI;
        else if (toggleOff)
        {
            if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_PERISPOMENI;
            
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_OXIA;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA_AND_PERISPOMENI;
            else if (ucs2String[i] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI_AND_YPOGEGRAMMENI)
                ucs2String[i] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_PERISPOMENI;
        }
    }
    else if (accent == SURROUNDING_PARENTHESES)
    {
        if (ucs2String[i] == GREEK_SMALL_LETTER_NU)
        {
            rightShiftFromOffset(ucs2String, i, len);
            rightShiftFromOffset(ucs2String, i, len);
            ucs2String[i] = LEFT_PARENTHESIS;
            ucs2String[i + 1] = GREEK_SMALL_LETTER_NU;
            ucs2String[i + 2] = RIGHT_PARENTHESIS;
        }
    }
}

void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int elen, bool contractedFuture, bool decompose)
{
    if (decompose && contractedFuture)
    {
        ucs2[*len] = GREEK_SMALL_LETTER_EPSILON;
        ++(*len);
    }
    else if (!decompose && (vf->tense == PRESENT || vf->tense == IMPERFECT) && (utf8HasSuffix(vf->verb->present, "άω") || utf8HasSuffix(vf->verb->present, "άομαι")))
    {
        --(*len);
    }
    else if (!decompose && (vf->tense == PRESENT || vf->tense == IMPERFECT) && (utf8HasSuffix(vf->verb->present, "έω") || utf8HasSuffix(vf->verb->present, "έομαι")))
    {
        --(*len);
    }
    else if (!decompose && (vf->tense == PRESENT || vf->tense == IMPERFECT) && (utf8HasSuffix(vf->verb->present, "όω") || utf8HasSuffix(vf->verb->present, "όομαι")))
    {
        --(*len);
    }
    else if(decompose && ADD_EPSILON_TO_AORIST_PASSIVE_SUBJUNCTIVE_STEM && vf->tense == AORIST && vf->voice == PASSIVE && vf->mood == SUBJUNCTIVE)
    {
        ucs2[*len] = GREEK_SMALL_LETTER_EPSILON;
        ++(*len);
    }
    else if (vf->tense == PERFECT && (vf->voice == MIDDLE || vf->voice == PASSIVE))
    {
        //Labials: π, φ, β, μπ
        //γέγραμμαι, λέλειμαι, βέβλαμμαι, κέκλεμμαι, εἴλημμαι,  πέπεμμαι is separate
        if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_PHI) == CONSONANT_STEM_PERFECT_PHI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_PI) == CONSONANT_STEM_PERFECT_PI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_BETA) == CONSONANT_STEM_PERFECT_BETA)
        {
            UCS2 consonant;
            if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_PHI) == CONSONANT_STEM_PERFECT_PHI)
                consonant = GREEK_SMALL_LETTER_PHI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_PI) == CONSONANT_STEM_PERFECT_PI)
                consonant = GREEK_SMALL_LETTER_PI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_BETA) == CONSONANT_STEM_PERFECT_BETA)
                consonant = GREEK_SMALL_LETTER_BETA;
            
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_PSI;
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_PI;
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_PHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_MU_PI) == CONSONANT_STEM_PERFECT_MU_PI) //πέπεμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ending[0] = GREEK_SMALL_LETTER_PSI;
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ending[0] = GREEK_SMALL_LETTER_PHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        //Stops: γ, χ, κ
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_KAPPA) == CONSONANT_STEM_PERFECT_KAPPA || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_CHI) == CONSONANT_STEM_PERFECT_CHI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_GAMMA) == CONSONANT_STEM_PERFECT_GAMMA)
        {
            UCS2 consonant;
            if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_KAPPA) == CONSONANT_STEM_PERFECT_KAPPA)
                consonant = GREEK_SMALL_LETTER_KAPPA;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_CHI) == CONSONANT_STEM_PERFECT_CHI)
                consonant = GREEK_SMALL_LETTER_CHI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_GAMMA) == CONSONANT_STEM_PERFECT_GAMMA)
                consonant = GREEK_SMALL_LETTER_GAMMA;
            
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_XI;
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_KAPPA;
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_CHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA) == CONSONANT_STEM_PERFECT_SIGMA || ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA_2) == CONSONANT_STEM_PERFECT_SIGMA_2 &&  ucs2[(*len)-1] == GREEK_SMALL_LETTER_SIGMA)) //κεκέλευσμαι or σῴζω which is both consonant stem and not.
        {
            if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_LAMBDA) == CONSONANT_STEM_PERFECT_LAMBDA) //ἄγγελμαι
        {
            //H&Q page 273, only different in 2nd and 3rd person plural
            if (vf->person == SECOND && vf->number == PLURAL)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
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
        //Labials: π, φ, β, μπ
        //γέγραμμαι, λέλειμαι, βέβλαμμαι, κέκλεμμαι, εἴλημμαι,  πέπεμμαι is separate
        if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_PHI) == CONSONANT_STEM_PERFECT_PHI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_PI) == CONSONANT_STEM_PERFECT_PI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_BETA) == CONSONANT_STEM_PERFECT_BETA)
        {
            UCS2 consonant;
            if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_PHI) == CONSONANT_STEM_PERFECT_PHI)
                consonant = GREEK_SMALL_LETTER_PHI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_PI) == CONSONANT_STEM_PERFECT_PI)
                consonant = GREEK_SMALL_LETTER_PI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_BETA) == CONSONANT_STEM_PERFECT_BETA)
                consonant = GREEK_SMALL_LETTER_BETA;

            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_PSI;
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_PI;
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_PHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_MU_PI) == CONSONANT_STEM_PERFECT_MU_PI) //πέπεμμαι
        {
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ending[0] = GREEK_SMALL_LETTER_PSI;
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len] = GREEK_SMALL_LETTER_PI;
                    ++(*len);
                }
                else
                {
                    ending[0] = GREEK_SMALL_LETTER_PHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        //Stops: γ, χ, κ
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_KAPPA) == CONSONANT_STEM_PERFECT_KAPPA || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_CHI) == CONSONANT_STEM_PERFECT_CHI || (vf->verb->verbclass & CONSONANT_STEM_PERFECT_GAMMA) == CONSONANT_STEM_PERFECT_GAMMA)
        {
            UCS2 consonant;
            if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_KAPPA) == CONSONANT_STEM_PERFECT_KAPPA)
                consonant = GREEK_SMALL_LETTER_KAPPA;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_CHI) == CONSONANT_STEM_PERFECT_CHI)
                consonant = GREEK_SMALL_LETTER_CHI;
            else if ( (vf->verb->verbclass & CONSONANT_STEM_PERFECT_GAMMA) == CONSONANT_STEM_PERFECT_GAMMA)
                consonant = GREEK_SMALL_LETTER_GAMMA;
            if (vf->person == FIRST && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_XI;
                }
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_KAPPA;
                }
            }
            else if (vf->person == FIRST && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if ( decompose)
                {
                    ucs2[*len - 1] = consonant;
                }
                else
                {
                    --(*len);
                    ending[0] = GREEK_SMALL_LETTER_CHI;
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA) == CONSONANT_STEM_PERFECT_SIGMA || ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_SIGMA_2) == CONSONANT_STEM_PERFECT_SIGMA_2 &&  ucs2[(*len)-1] == GREEK_SMALL_LETTER_SIGMA)) //κεκέλευσμαι or σῴζω which is both consonant stem and not.
        {
            if (vf->person == SECOND && vf->number == SINGULAR)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == SECOND && vf->number == PLURAL)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
        else if ((vf->verb->verbclass & CONSONANT_STEM_PERFECT_LAMBDA) == CONSONANT_STEM_PERFECT_LAMBDA) //ἄγγελμαι
        {
            //H&Q page 273, only different in 2nd and 3rd person plural
            if (vf->person == SECOND && vf->number == PLURAL)
            {
                if (!decompose)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
            }
            else if (vf->person == THIRD && vf->number == PLURAL)
            {
                ucs2[0] = EM_DASH;
                (*len) = 1;
                return;
            }
        }
    }
    else if (vf->tense == FUTURE && vf->voice == PASSIVE) //add future passive infix hs here
    {
        if (decompose)
        {
            ucs2[*len] = SPACE;
            ++(*len);
            ucs2[*len] = HYPHEN;
            ++(*len);
            ucs2[*len] = SPACE;
            ++(*len);
        }
        
        ucs2[*len] = GREEK_SMALL_LETTER_ETA;
        ucs2[(*len) + 1] = GREEK_SMALL_LETTER_SIGMA;
        (*len) += 2; //parens required here fyi
    }
    else if (vf->tense == PRESENT && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "νυμαι"))) //mi verbs, present tense
    {
        if (vf->voice != ACTIVE || vf->number == PLURAL || vf->mood == OPTATIVE || vf->mood == IMPERATIVE || vf->mood == SUBJUNCTIVE)
        {
            if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA && ucs2[*len - 2] == GREEK_SMALL_LETTER_TAU)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
            else if (ucs2[*len - 1] == COMBINING_MACRON) //deiknumi
                --(*len);
        }
        //contract third plural indicative of isthmi
        if (vf->person == THIRD && vf->number == PLURAL && vf->mood == INDICATIVE && vf->voice == ACTIVE && utf8HasSuffix(vf->verb->present, "στημι"))
        {
            if (!decompose)
            {
                --(*len);
                leftShift(ending, &elen);
                ending[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
            }
        }
        if (vf->mood == SUBJUNCTIVE)
        {
            if (!decompose && !utf8HasSuffix(vf->verb->present, "ῡμι"))
            {
                --(*len);
            }
                // UCS2 didw[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA , GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_OMEGA };
            if ( utf8HasSuffix(vf->verb->present, "δίδωμι"))
            {
                if (!decompose)
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
            if (( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && decompose)
            {
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        if (vf->mood == OPTATIVE && !utf8HasSuffix(vf->verb->present, "ῡμι") && !utf8HasSuffix(vf->verb->present, "νυμαι"))
        {
            //exceptional alternates H&Q page 347
            if (vf->tense == PRESENT && vf->mood == OPTATIVE && vf->voice != ACTIVE && utf8HasSuffix(vf->verb->present, "τίθημι") && (vf->number == PLURAL || vf->person == THIRD))
            {
                if (ending[0] == GREEK_SMALL_LETTER_OMICRON && !decompose)// this is right
                    --(*len);
                else if (ending[0] == GREEK_SMALL_LETTER_EPSILON)
                    leftShift(ending, &elen);
            }
            else
            {
                leftShift(ending, &elen);
                if (vf->person != FIRST && vf->voice != ACTIVE)
                {
                    ending[0] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                }
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
                        if (!decompose)
                        {
                            ending[0] = GREEK_SMALL_LETTER_UPSILON;
                        }
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                        ending[1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                }
                else if ( ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")))
                {
                    if (vf->voice == ACTIVE)
                    {
                        if (!decompose)
                        {
                            --(*len);
                            ending[0] = GREEK_SMALL_LETTER_ETA;
                        }
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
                        if (!decompose)
                        {
                            ending[0] = GREEK_SMALL_LETTER_IOTA;
                        }
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                        ending[1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                }
                else if ( utf8HasSuffix(vf->verb->present, "ῡμι") || utf8HasSuffix(vf->verb->present, "νυμαι"))
                {
                    if (vf->voice == ACTIVE)
                    {
                        elen = 0;
                        ++(*len);
                        ucs2[*len - 1] = COMBINING_MACRON;
                        
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
    else if (vf->tense == IMPERFECT && (utf8HasSuffix(vf->verb->present, "μι") || utf8HasSuffix(vf->verb->present, "σταμαι") || utf8HasSuffix(vf->verb->present, "νυμαι"))) //mi verbs
    {
        if ( utf8HasSuffix(vf->verb->present, "δίδωμι") && vf->voice == ACTIVE)
        {
            //shorten stem vowel
            if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
            
            if ( vf->number == SINGULAR && vf->voice == ACTIVE)
            {
                ++(*len);
                ucs2[*len - 1] = GREEK_SMALL_LETTER_UPSILON;
                if (vf->person == FIRST || vf->person == SECOND)
                {
                    leftShift(ending, &elen);
                }
                else if (vf->person == THIRD)
                {
                    elen = 0;
                }
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
        else if ( utf8HasSuffix(vf->verb->present, "στημι") && vf->voice == ACTIVE)
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
        else if ( utf8HasSuffix(vf->verb->present, "τίθημι") && vf->voice == ACTIVE)
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
                ++(*len);
                ucs2[*len - 1] = GREEK_SMALL_LETTER_IOTA;
                ending[0] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                elen = 1;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                ++(*len);
                ucs2[*len - 1] = GREEK_SMALL_LETTER_IOTA;
                elen = 0;
            }
        }
        else if ( utf8HasSuffix(vf->verb->present, "ῡμι") && vf->voice == ACTIVE)
        {
            if (vf->number == PLURAL)
            {
                if (ucs2[*len - 1] == COMBINING_MACRON) //deiknumi
                    --(*len);
            }
            
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
                ending[0] = GREEK_SMALL_LETTER_FINAL_SIGMA;
                elen = 1;
            }
            else if (vf->person == THIRD && vf->number == SINGULAR)
            {
                elen = 0;
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
            else if (ucs2[*len - 1] == COMBINING_MACRON) //deiknumi
                --(*len);
            
            leftShift(ending, &elen);
            if (vf->person == SECOND && vf->number == SINGULAR)
            {
                ending[0] = GREEK_SMALL_LETTER_SIGMA;
                ending[1] = GREEK_SMALL_LETTER_OMICRON;
                elen = 2;
            }
        }
    }
    else if (vf->tense == AORIST && (utf8HasSuffix(vf->verb->present, "μι")  || utf8HasSuffix(vf->verb->present, "σταμαι")) && !utf8HasSuffix(vf->verb->present, "ῡμι")) //mi verbs
    {
        if (vf->voice == ACTIVE)
        {
            if (vf->mood == SUBJUNCTIVE)
            {
                if ( utf8HasSuffix(vf->verb->present, "τίθημι"))
                {
                    if (decompose)
                    {
                        (*len) -= 1;
                        ucs2[(*len) - 1] = GREEK_SMALL_LETTER_EPSILON;
                    }
                    else
                    {
                        (*len) -= 2;
                    }
                }
                else if ( utf8HasSuffix(vf->verb->present, "δίδωμι") )
                {
                    if (decompose)
                    {
                        (*len) -= 1;
                        ucs2[(*len) - 1] = GREEK_SMALL_LETTER_OMICRON;
                    }
                    else
                    {
                        (*len) -= 2;
                    }
                    if (!decompose)
                    {
                        if ((vf->person == SECOND || vf->person == THIRD) && vf->number == SINGULAR)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                        }
                        else if (vf->person == SECOND && vf->number == PLURAL)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                        }
                    }
                }
                else if ((utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && ucs2[*len -1] == GREEK_SMALL_LETTER_ETA)
                {
                    //root aorist
                    if (decompose)
                    {
                        ucs2[(*len) - 1] = GREEK_SMALL_LETTER_EPSILON; //yes epsilon
                    }
                    else
                    {
                        (*len) -= 1;
                    }
                }
            }
            else if (vf->mood == OPTATIVE)
            {
                if ( utf8HasSuffix(vf->verb->present, "τίθημι") || utf8HasSuffix(vf->verb->present, "δίδωμι") )
                {
                    (*len) -= 1;
                    if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                        ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
                    else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA )
                        ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
                    leftShift(ending, &elen);
                }
                else if ((utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && ucs2[*len -1] == GREEK_SMALL_LETTER_ETA)
                {
                    //root aorist
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
                    leftShift(ending, &elen);
                }
            }
            else if (vf->mood == IMPERATIVE)
            {
                if ( utf8HasSuffix(vf->verb->present, "τίθημι") )
                {
                    (*len) -= 1;
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
                }
                else if ( utf8HasSuffix(vf->verb->present, "δίδωμι") )
                {
                    (*len) -= 1;
                    ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
                }
                else if ( (utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && ucs2[*len -1] == GREEK_SMALL_LETTER_ETA )
                {
                    //root aorist
                    if (vf->person == THIRD && vf->number == PLURAL)
                        ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
                }
            }
            else if ( utf8HasSuffix(vf->verb->present, "τίθημι") || utf8HasSuffix(vf->verb->present, "δίδωμι") )
            {
                if (vf->number == PLURAL && vf->voice == ACTIVE && vf->mood == INDICATIVE)
                {
                    --(*len);
                    if (ucs2[*len - 1] == GREEK_SMALL_LETTER_OMEGA)
                        ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
                    else if (ucs2[*len - 1] == GREEK_SMALL_LETTER_ETA )
                        ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
                    
                    if (vf->person == THIRD)
                    {
                        rightShift(ending, &elen);
                        ending[0] = GREEK_SMALL_LETTER_SIGMA;
                    }
                    else
                    {
                        leftShift(ending, &elen);
                    }
                }
            }
        }
        else if (vf->voice == MIDDLE)
        {
            if ( utf8HasSuffix(vf->verb->present, "τίθημι"))
            {
                (*len) -= 1;
                ucs2[*len - 1] = GREEK_SMALL_LETTER_EPSILON;
                if (vf->person == SECOND && vf->number == SINGULAR && vf->mood == INDICATIVE)
                {
                    if (!decompose)
                    {
                        (*len) -= 1;
                        ending[0] = GREEK_SMALL_LETTER_OMICRON;
                        ending[1] = GREEK_SMALL_LETTER_UPSILON;
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_OMICRON;
                        elen = 1;
                    }
                }
                else if (vf->mood == SUBJUNCTIVE)
                {
                    if (!decompose)
                    {
                        (*len) -= 1;
                    }
                }
                else if (vf->mood == OPTATIVE)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                }
                else if (vf->mood == IMPERATIVE)
                {
                    if (vf->person == SECOND && vf->number == SINGULAR)
                    {
                        if (!decompose)
                        {
                            ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
                            ending[0] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
                        }
                        else
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMICRON;
                        }
                    }
                }
            }
            else if ( utf8HasSuffix(vf->verb->present, "δίδωμι") )
            {
                (*len) -= 1;
                ucs2[*len - 1] = GREEK_SMALL_LETTER_OMICRON;
                if (vf->person == SECOND && vf->number == SINGULAR && vf->mood == INDICATIVE)
                {
                    if (!decompose)
                    {
                        ending[0] = GREEK_SMALL_LETTER_UPSILON;
                    }
                    else
                    {
                        ending[0] = GREEK_SMALL_LETTER_OMICRON;
                    }
                    elen = 1;
                }
                else if (vf->mood == SUBJUNCTIVE)
                {
                    if (!decompose)
                    {
                        (*len) -= 1;
                        if (vf->person == SECOND && vf->number == SINGULAR)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI_AND_YPOGEGRAMMENI;
                        }
                        else if (vf->person == THIRD && vf->number == SINGULAR)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                        }
                        else if (vf->person == SECOND && vf->number == PLURAL)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PERISPOMENI;
                        }
                    }
                }
                else if (vf->mood == OPTATIVE)
                {
                    leftShiftFromOffset(ending, 0, &elen);
                    //accent doesn't go past syllable with iota
                    //Hardy caught this one: see his email from 02/22/2016
                    if (vf->person != FIRST)
                    {
                        ending[0] = GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI;
                    }
                }
                else if (vf->mood == IMPERATIVE)
                {
                    if (vf->person == SECOND && vf->number == SINGULAR)
                    {
                        if (decompose)
                        {
                            ending[0] = GREEK_SMALL_LETTER_OMICRON;
                        }
                        else
                        {
                            //Fix me.  See H&Q p. 376 for note on accentuation when prefixed
                            if (utf8HasSuffix(vf->verb->present, "μεταδίδωμι"))
                                ending[0] = GREEK_SMALL_LETTER_UPSILON;
                            else
                                ending[0] = GREEK_SMALL_LETTER_UPSILON_WITH_PERISPOMENI;
                        }
                        elen = 1;
                    }
                }
            }
        }
    }
    else if (vf->tense == AORIST && utf8HasSuffix(vf->verb->present, "φθάνω") &&  ucs2[*len -1] == GREEK_SMALL_LETTER_ETA )
    {
        //root aorist
        if (vf->mood == SUBJUNCTIVE)
        {
            if (decompose)
            {
                ucs2[(*len) - 1] = GREEK_SMALL_LETTER_EPSILON; //yes epsilon
            }
            else
            {
                (*len) -= 1;
            }
        }
        else if (vf->mood == OPTATIVE)
        {
            ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            leftShift(ending, &elen);
        }
        else if (vf->mood == IMPERATIVE)
        {
            if (vf->person == THIRD && vf->number == PLURAL)
                ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            else if (vf->person == SECOND && vf->number == SINGULAR)
                ending[0] = GREEK_SMALL_LETTER_TAU; //disimilation of aspirates here, right? fix me?
        }
        
    }
    else if ( ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && vf->tense == PERFECT)
    {
        if (vf->number == PLURAL)
        {
            (*len) -= 1;
            ucs2[*len -1] = GREEK_SMALL_LETTER_ALPHA;
            
            if (vf->person == THIRD)
            {
                if (!decompose)
                {
                    (*len) -= 1;
                    leftShiftFromOffset(ending, 0, &elen);
                    ending[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PERISPOMENI;
                }
            }
            else
            {
                leftShiftFromOffset(ending, 0, &elen);
            }
        }
    }
    else if ( ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")) && vf->tense == PLUPERFECT)
    {
        //H&Q p. 378
        if (vf->number == PLURAL)
        {
            (*len) -= 1;
            ucs2[*len - 1] = GREEK_SMALL_LETTER_ALPHA;
            if (vf->person == FIRST)
            {
                leftShiftFromOffset(ending, 0, &elen);
            }
            else if (vf->person == SECOND)
            {
                leftShiftFromOffset(ending, 0, &elen);
            }
            else if (vf->person == THIRD)
            {
                leftShiftFromOffset(ending, 0, &elen);
                ending[0] = GREEK_SMALL_LETTER_SIGMA;
                ending[1] = GREEK_SMALL_LETTER_ALPHA;
                ending[2] = GREEK_SMALL_LETTER_NU;
                elen = 3;
            }
        }
    }
    
    if (decompose)
    {
        ucs2[*len] = SPACE;
        ++(*len);
        ucs2[*len] = HYPHEN;
        ++(*len);
        ucs2[*len] = SPACE;
        ++(*len);
    }
    
    int i = 0;
    int j = 0;
    for (i = *len; j < elen; i++, j++)
    {
        ucs2[i] = ending[j];
        ++(*len);
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
    UCS2 presMiDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    UCS2 presNumiDeponent[5] = { GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    UCS2 presDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    UCS2 aoristDeponent[4] = { GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    
    UCS2 secondAoristDeponent[4] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };
    UCS2 secondAorist[2] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_NU };
    
    UCS2 isthmiRootAorist[2] = { GREEK_SMALL_LETTER_ETA, GREEK_SMALL_LETTER_NU };

    UCS2 futureDeponentContracted[5] = { GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_IOTA };
    
    
    if ((tense == PRESENT || tense == IMPERFECT) && hasSuffix(stem, *len, presNumiDeponent, 5)) //νυμαι
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

void decomposePrefixes(VerbFormC *vf, UCS2 *ucs2, int *len)
{
    UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON };
    UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
    UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
    UCS2 kata[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
    UCS2 metana[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
    
    if (hasPrefix(ucs2, *len, apo, 3))
    {
        rightShiftFromOffset(ucs2, 3, len);
        rightShiftFromOffset(ucs2, 3, len);
        rightShiftFromOffset(ucs2, 3, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, aph, 2))
    {
        rightShiftFromOffset(ucs2, 2, len);
        rightShiftFromOffset(ucs2, 2, len);
        rightShiftFromOffset(ucs2, 2, len);
        rightShiftFromOffset(ucs2, 2, len);
        ucs2[1] = GREEK_SMALL_LETTER_PI;
        ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
        
        if (ucs2[6] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON)
        {
            ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, kath, 3))
    {
        rightShiftFromOffset(ucs2, 3, len);
        rightShiftFromOffset(ucs2, 3, len);
        rightShiftFromOffset(ucs2, 3, len);
        rightShiftFromOffset(ucs2, 3, len);
        ucs2[2] = GREEK_SMALL_LETTER_TAU;
        ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
        
        if (ucs2[7] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[7] == GREEK_SMALL_LETTER_EPSILON)
        {
            ucs2[7] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, kata, 4))
    {
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, metan, 5))
    {
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        if (ucs2[12] == GREEK_SMALL_LETTER_IOTA || ucs2[12] == GREEK_SMALL_LETTER_EPSILON)
        {
            rightShiftFromOffset(ucs2, 4, len);
        }
        ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
        ucs2[7] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[8] = GREEK_SMALL_LETTER_NU;
        ucs2[9] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[10] = SPACE;
        ucs2[11] = HYPHEN;
        ucs2[12] = SPACE;
        if (ucs2[13] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[13] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[13] == GREEK_SMALL_LETTER_EPSILON)
        {
            ucs2[13] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, epan, 4))
    {
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        rightShiftFromOffset(ucs2, 1, len);
        if (ucs2[11] == GREEK_SMALL_LETTER_IOTA || ucs2[11] == GREEK_SMALL_LETTER_EPSILON)
        {
            rightShiftFromOffset(ucs2, 4, len);
        }
        ucs2[2] = GREEK_SMALL_LETTER_IOTA;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
        ucs2[6] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[7] = GREEK_SMALL_LETTER_NU;
        ucs2[8] = GREEK_SMALL_LETTER_ALPHA;
        ucs2[9] = SPACE;
        ucs2[10] = HYPHEN;
        ucs2[11] = SPACE;
        if (ucs2[12] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[12] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[12] == GREEK_SMALL_LETTER_EPSILON)
        {
            ucs2[12] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, meta, 4))
    {
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        rightShiftFromOffset(ucs2, 4, len);
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
    }
}

void stripAugmentFromPrincipalPart(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 presentStemInitial, bool decompose)
{
    if ((vf->verb->verbclass & PREFIXED) == PREFIXED)
    {
        UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_EPSILON };
        UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
        UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
        UCS2 kate[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_EPSILON };
        UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 mete[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_EPSILON };
        UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        
        if (hasPrefix(ucs2, *len, apo, 3))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
            }
            
            ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
        }
        else if (hasPrefix(ucs2, *len, aph, 2))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                
                ucs2[1] = GREEK_SMALL_LETTER_PI;
                ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, kath, 3))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                ucs2[2] = GREEK_SMALL_LETTER_TAU;
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    
                    ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[8] = SPACE;
                    ucs2[9] = HYPHEN;
                    ucs2[10] = SPACE;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, kate, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                //ucs2[2] = GREEK_SMALL_LETTER_TAU;
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    
                    ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[8] = SPACE;
                    ucs2[9] = HYPHEN;
                    ucs2[10] = SPACE;
                }
            }
            else
            {
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
        else if (hasPrefix(ucs2, *len, metan, 5))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                ucs2[7] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[8] = GREEK_SMALL_LETTER_NU;
                ucs2[9] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[10] = SPACE;
                ucs2[11] = HYPHEN;
                ucs2[12] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    ucs2[13] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[14] = SPACE;
                    ucs2[15] = HYPHEN;
                    ucs2[16] = SPACE;
                    if (ucs2[17] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[17] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[13] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[13] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
            }
            else
            {
                ucs2[5] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
        else if (hasPrefix(ucs2, *len, epan, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                
                ucs2[2] = GREEK_SMALL_LETTER_IOTA;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                ucs2[6] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[7] = GREEK_SMALL_LETTER_NU;
                ucs2[8] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[9] = SPACE;
                ucs2[10] = HYPHEN;
                ucs2[11] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    ucs2[12] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[13] = SPACE;
                    ucs2[14] = HYPHEN;
                    ucs2[15] = SPACE;
                    if (ucs2[16] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[16] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[12] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[12] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
            }
            else
            {
                ucs2[4] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
        else if (hasPrefix(ucs2, *len, meta, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    
                    ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[8] = SPACE;
                    ucs2[9] = HYPHEN;
                    ucs2[10] = SPACE;
                }
            }
            else
            {
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
        else if (hasPrefix(ucs2, *len, mete, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    rightShiftFromOffset(ucs2, 7, len);
                    
                    ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[8] = SPACE;
                    ucs2[9] = HYPHEN;
                    ucs2[10] = SPACE;
                }
            }
            else
            {
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
    }
    else if (vf->tense == AORIST)//removing this because of decompose option.  we already check these things before this is called. && (mood == SUBJUNCTIVE || mood == OPTATIVE || mood == IMPERATIVE))
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
    else if (vf->tense == FUTURE && vf->voice == PASSIVE)
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
    
    if (vf->tense == AORIST && vf->mood == INDICATIVE && decompose && (vf->verb->verbclass & PREFIXED) != PREFIXED)
    {
        rightShiftFromOffset(ucs2, 0, len);
        rightShiftFromOffset(ucs2, 0, len);
        rightShiftFromOffset(ucs2, 0, len);
        rightShiftFromOffset(ucs2, 0, len);
        ucs2[0] = DECOMPOSED_AUGMENT_CHAR;
        ucs2[1] = SPACE;
        ucs2[2] = HYPHEN;
        ucs2[3] = SPACE;
    }
}

//we assume accents have been stripped, but not breathings, macrons or iota subscripts
void augmentStem(VerbFormC *vf, UCS2 *ucs2, int *len, bool decompose)
{
    if (decompose && (vf->verb->verbclass & PREFIXED) != PREFIXED)
    {
        /*
         Don't add an augment on these decomposed forms which start with e, ei, or h
        
         H&Q page 326.  "In most verbs when principal part iv or v begins with e) or ei), the pluperfect is unaugmented."
         */
        if (vf->tense == PLUPERFECT && (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || (ucs2[0] == GREEK_SMALL_LETTER_EPSILON && ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI) || ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI ))
        {
            return;
        }
        
        
        if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")) || !utf8HasSuffix(vf->verb->present, "στημι"))
        {
            rightShiftFromOffset(ucs2, 0, len);
            rightShiftFromOffset(ucs2, 0, len);
            rightShiftFromOffset(ucs2, 0, len);
            rightShiftFromOffset(ucs2, 0, len);
            ucs2[0] = DECOMPOSED_AUGMENT_CHAR;
            ucs2[1] = SPACE;
            ucs2[2] = HYPHEN;
            ucs2[3] = SPACE;
        }
        return;
    }
    
    if ((vf->verb->verbclass & PREFIXED) == PREFIXED)
    {
        UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON };
        UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
        UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
        UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 epi[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_IOTA };
        UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };

        if (hasPrefix(ucs2, *len, epi, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                ucs2[7] = SPACE;
                ucs2[8] = HYPHEN;
                ucs2[9] = SPACE;
            }
            else
            {
                ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, apo, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                ucs2[7] = SPACE;
                ucs2[8] = HYPHEN;
                ucs2[9] = SPACE;
            }
            else
            {
                ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, aph, 2))
        {
            if ( decompose)
            {
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                rightShiftFromOffset(ucs2, 2, len);
                ucs2[1] = GREEK_SMALL_LETTER_PI;
                ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")))
                {
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    rightShiftFromOffset(ucs2, 6, len);
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                
                    if (ucs2[10] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[10] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[10] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[10] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[6] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
            }
            else
            {
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                if (vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι") )
                {
                    rightShiftFromOffset(ucs2, 2, len);
                    ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[3] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[2] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffset(ucs2, 3, len);
                    ucs2[3] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, kath, 3))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                rightShiftFromOffset(ucs2, 3, len);
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")))
                {
                    rightShiftFromOffset(ucs2, 3, len);
                    rightShiftFromOffset(ucs2, 3, len);
                    rightShiftFromOffset(ucs2, 3, len);
                    rightShiftFromOffset(ucs2, 3, len);
                }
                ucs2[2] = GREEK_SMALL_LETTER_TAU;
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")))
                {
                    ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[8] = SPACE;
                    ucs2[9] = HYPHEN;
                    ucs2[10] = SPACE;
                    
                    if (ucs2[11] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[11] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[11] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[11] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[7] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[7] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[7] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
            }
            else
            {
                if (ucs2[3] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffset(ucs2, 4, len);
                    ucs2[4] = COMBINING_MACRON;
                }
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                else if (vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι") )
                {
                    rightShiftFromOffset(ucs2, 3, len);
                    ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[4] = GREEK_SMALL_LETTER_IOTA;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, metan, 5))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                ucs2[7] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[8] = GREEK_SMALL_LETTER_NU;
                ucs2[9] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[10] = SPACE;
                ucs2[11] = HYPHEN;
                ucs2[12] = SPACE;
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "σταμαι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "σταμαι")))
                {
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    rightShiftFromOffset(ucs2, 13, len);
                    ucs2[13] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[14] = SPACE;
                    ucs2[15] = HYPHEN;
                    ucs2[16] = SPACE;
                    
                    if (ucs2[17] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[17] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[17] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[17] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[13] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[13] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[13] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[13] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
            }
            else
            {
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                if (vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "σταμαι") )
                {
                    rightShiftFromOffset(ucs2, 5, len);
                    ucs2[5] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[6] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[5] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffset(ucs2, 6, len);
                    ucs2[6] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, epan, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                rightShiftFromOffset(ucs2, 1, len);
                
                ucs2[2] = GREEK_SMALL_LETTER_IOTA;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                ucs2[6] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[7] = GREEK_SMALL_LETTER_NU;
                ucs2[8] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[9] = SPACE;
                ucs2[10] = HYPHEN;
                ucs2[11] = SPACE;
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "σταμαι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "σταμαι")))
                {
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    rightShiftFromOffset(ucs2, 12, len);
                    ucs2[12] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[13] = SPACE;
                    ucs2[14] = HYPHEN;
                    ucs2[15] = SPACE;
                    
                    if (ucs2[16] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[16] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[16] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[16] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[12] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[12] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                    else if (ucs2[12] == GREEK_SMALL_LETTER_EPSILON)
                    {
                        ucs2[12] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                }
            }
            else
            {
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                if (vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "σταμαι") )
                {
                    rightShiftFromOffset(ucs2, 4, len);
                    ucs2[4] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[5] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[4] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffset(ucs2, 5, len);
                    ucs2[5] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, meta, 4))
        {
            if (decompose)
            {
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                rightShiftFromOffset(ucs2, 4, len);
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                ucs2[7] = DECOMPOSED_AUGMENT_CHAR;
                ucs2[8] = SPACE;
                ucs2[9] = HYPHEN;
                ucs2[10] = SPACE;
            }
            else
            {
                ucs2[3] =  GREEK_SMALL_LETTER_EPSILON;
            }
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_ALPHA)
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
        //for histhmi pluperfect singular.  H&Q PAGE 378.
        if (vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι") )
        {
            rightShiftFromOffset(ucs2, 0, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
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
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON_WITH_PSILI)
    {
        ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
    {
        ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
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

//Moves everything over to the left, eating the first letter
void leftShiftFromOffset(UCS2 *ucs2, int i, int *len)
{
    int j = i;
    for ( ; j < *len - 1; j++)
    {
        ucs2[j] = ucs2[j + 1];
    }
    --(*len);
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
    if (*len < 2)
        return;
    
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

//FIX ME, this could probably be written more efficiently.
//only null terminate at the end and verify that it is null terminated properly
int ucs2_to_utf8_string(UCS2 *ucs2, int len, unsigned char *utf8)
{
    unsigned char *utf8Temp = utf8;
    if (len < 1)
    {
        utf8[0] = '\0';
        return 1;
    }
    
    for (int i = 0; i < len; i++)
    {
        int utf8Len;
        utf8Len = ucs2_to_utf8(ucs2[i], utf8Temp);
        if (utf8Len > -1)
            utf8Temp += utf8Len;
        else
            return 0;
        *utf8Temp = '\0';
    }
    
    return 1;
}

void utf8_to_ucs2_string(const unsigned char *utf8, UCS2 *ucs2, int *len)
{
    int temp; //because UCS2 is unsigned.
    *len = 0;
    
    for( int i = 0; *utf8 ; i++)
    {
        temp = utf8_to_ucs2 (utf8, &utf8);
        if (temp == -1)
        {
            break;
        }
        else
        {
            ucs2[i] = temp;
        }
        (*len)++;
    }
}



