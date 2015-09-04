//
//  libmorph.h
//  morph
//
//  Created by Jeremy on 4/15/15.
//  Copyright (c) 2015 Jeremy March. All rights reserved.
//

#ifndef __morph__libmorph__
#define __morph__libmorph__

#include <stdio.h>
#include <stdbool.h>

#define NUM_VOWELS 14
#define NUM_CONSONANTS 35

typedef unsigned short UCS2;

#define NUM_VERBS 113//34//25 //


//verb classes
enum {
    REGULAR = 0,
    CONSONANT_STEM_PERFECT_1 = 1, // γέγραμμαι
    CONSONANT_STEM_PERFECT_2 = 2, // πέπεμμαι
    CONSONANT_STEM_PERFECT_3 = 4, // πεφύλαγμαι
    CONSONANT_STEM_PERFECT_4 = 8, // κεκέλευσμαι
    CONSONANT_STEM_PERFECT_5 = 16, // σῴζω one part is consonant stem, the other not, so check.
    CONSONANT_STEM_PERFECT_6 = 32, // ἤγγελμαι
    PREFIXED = 64,
    CONTRACTED_FUTURE = 128,
    MI_VERB = 256
};

enum {
    NO_ACCENT = 0,
    ACUTE,
    CIRCUMFLEX,
    GRAVE
};

enum {
    ULTIMA = 0,
    PENULT,
    ANTEPENULT
};

enum {
    FIRST = 0,
    SECOND,
    THIRD,
    NUM_PERSONS
};

enum {
    SINGULAR = 0,
    PLURAL,
    NUM_NUMBERS
};

enum {
    PRESENT = 0,
    IMPERFECT,
    FUTURE,
    AORIST,
    PERFECT,
    PLUPERFECT,
    NUM_TENSES
};

enum {
    ACTIVE = 0,
    MIDDLE,
    PASSIVE,
    NUM_VOICES
};

#define NUM_MOODS  4

enum {
    INDICATIVE = 0,
    SUBJUNCTIVE,
    OPTATIVE,
    IMPERATIVE,
    INFINITIVE,
    PARTICIPLE
};

enum {
    PERSON = 0,
    NUMBER,
    TENSE,
    VOICE,
    MOOD
};

enum {
    NOT_DEPONENT = 0,
    MIDDLE_DEPONENT,
    PASSIVE_DEPONENT,
    PARTIAL_DEPONENT
};

#define UNICODE_SURROGATE_PAIR    -1
#define UNICODE_BAD_INPUT         -2

typedef struct v {
    unsigned int verbid; //an index in the verbs static array
    unsigned int verbclass; //a bitfield
    unsigned char hq;
    char *present;
    char *future;
    char *aorist;
    char *perf;
    char *perfmid;
    char *aoristpass;
    //char *def;
    //char *more;
} Verb;

typedef struct vf {
    unsigned char person;
    unsigned char number;
    unsigned char tense;
    unsigned char voice;
    unsigned char mood;
    Verb *verb;
} VerbFormC;

typedef struct e {
    unsigned int id;
    unsigned char hq;
    unsigned char tense;
    unsigned char voice;
    unsigned char mood;
    char *fs;
    char *ss;
    char *ts;
    char *fp;
    char *sp;
    char *tp;
    char *description;
} Ending;

enum {
    PRESENT_ACTIVE_IND = 0,
    IMPERFECT_ACTIVE_IND,
    AORIST_ACTIVE_IND,
    PERFECT_ACTIVE_IND,
    PLUPERFECT_ACTIVE_IND,
    FUTURE_ACTIVE_IND,
    
    PRESENT_ACTIVE_SUBJ,
    AORIST_ACTIVE_SUBJ,
    PRESENT_ACTIVE_OPT,
    AORIST_ACTIVE_OPT,
    PRESENT_MIDPASS_IND,
    IMPERFECT_MIDPASS_IND,
    AORIST_PASSIVE_IND,
    AORIST_MID_IND,
    AORIST_PASSIVE_SUBJ,
    AORIST_PASSIVE_OPT,
    
    AORIST_MIDDLE_SUBJ,
    AORIST_MIDDLE_OPT,
    PERFECT_MIDPASS_IND,
    PLUPERFECT_MIDPASS_IND,
    PRESENT_MIDPASS_SUBJ,
    PRESENT_MIDPASS_OPT,
    
    PRESENT_ACTIVE_IMPERATIVE,
    PRESENT_MIDPASS_IMPERATIVE,
    AORIST_ACTIVE_IMPERATIVE,
    AORIST_MIDDLE_IMPERATIVE,
    AORIST_PASSIVE_IMPERATIVE,
    FUTURE_MIDPASS_IND,
    
    PRESENT_ACTIVE_INDIC_A_CONTRACTED,
    PRESENT_MIDPASS_INDIC_A_CONTRACTED,
    IMPERFECT_ACTIVE_INDIC_A_CONTRACTED,
    IMPERFECT_MIDPASS_INDIC_A_CONTRACTED,
    PRESENT_ACTIVE_SUBJ_A_CONTRACTED,
    PRESENT_MIDPASS_SUBJ_A_CONTRACTED,
    PRESENT_ACTIVE_OPT_A_CONTRACTED,
    PRESENT_MIDPASS_OPT_A_CONTRACTED,
    
    PRESENT_ACTIVE_INDIC_E_CONTRACTED,
    PRESENT_MIDPASS_INDIC_E_CONTRACTED,
    IMPERFECT_ACTIVE_INDIC_E_CONTRACTED,
    IMPERFECT_MIDPASS_INDIC_E_CONTRACTED,
    PRESENT_ACTIVE_SUBJ_E_CONTRACTED,
    PRESENT_MIDPASS_SUBJ_E_CONTRACTED,
    PRESENT_ACTIVE_OPT_E_CONTRACTED,
    PRESENT_MIDPASS_OPT_E_CONTRACTED,
    
    PRESENT_ACTIVE_INDIC_O_CONTRACTED,
    PRESENT_MIDPASS_INDIC_O_CONTRACTED,
    IMPERFECT_ACTIVE_INDIC_O_CONTRACTED,
    IMPERFECT_MIDPASS_INDIC_O_CONTRACTED,
    PRESENT_ACTIVE_SUBJ_O_CONTRACTED,
    PRESENT_MIDPASS_SUBJ_O_CONTRACTED,
    PRESENT_ACTIVE_OPT_O_CONTRACTED,
    PRESENT_MIDPASS_OPT_O_CONTRACTED,
    PRESENT_ACTIVE_IMPERATIVE_A_CONTRACTED,
    PRESENT_MIDPASS_IMPERATIVE_A_CONTRACTED,
    PRESENT_ACTIVE_IMPERATIVE_E_CONTRACTED,
    PRESENT_MIDPASS_IMPERATIVE_E_CONTRACTED,
    PRESENT_ACTIVE_IMPERATIVE_O_CONTRACTED,
    PRESENT_MIDPASS_IMPERATIVE_O_CONTRACTED,
    
    PRESENT_ACTIVE_INDICATIVE_MI, 
    
    NUM_ENDINGS
};

int getForm(VerbFormC *vf, char *buffer, int bufferLen, bool includeAlternateForms);

Verb *getRandomVerb(int *units, int numUnits);
Ending *getRandomEnding(int *units, int numUnits);
void getRandomEndingAsString(int *units, int numUnits, char *buffer, int bufferLen);

void changeFormByDegrees(VerbFormC *verbform, int degrees);
void generateForm(VerbFormC *verbform);
void getDistractorsForChange(VerbFormC *orig, VerbFormC *new, int numDistractors, char *buffer);

void getFullDescription (VerbFormC *vf, char *buffer, int len);
void getAbbrevDescription (VerbFormC *vf, char *buffer, int len);

void getPrincipalParts(Verb *v, char *buffer, int len);

char *getPrincipalPartForTense(Verb *verb, unsigned char tense, unsigned char voice);

int deponentType(Verb *v);
bool isDeponent(VerbFormC *vf, UCS2 *stem, int stemLen);

bool isVowel(UCS2 l);
bool isLong(UCS2 l);
bool isSecondVowelOfDiphthong(UCS2 *tempUcs2String, int len, int i);
bool isConsonant(UCS2 l);

bool formIsValidReal(unsigned char person, unsigned char number, unsigned char tense, unsigned char voice, unsigned char mood);

int ucs2_to_utf8 (UCS2 ucs2, unsigned char * utf8);
UCS2 utf8_to_ucs2 (const unsigned char * input, const unsigned char ** end_ptr);

void utf8_to_ucs2_string(const unsigned char *utf8, UCS2 *ucs2, int *len);
int ucs2_to_utf8_string(UCS2 *ucs2, int len, unsigned char *utf8);

bool utf8HasSuffix(char *s, char *suffix);
bool hasPrefix(UCS2 *stem, int len, UCS2 *prefix, int preflen);
bool hasSuffix(UCS2 *stem, int len, UCS2 *suffix, int sufflen);

long randWithMax(unsigned int max);

int incrementValue(int theArrayCount, int start);

#endif /* defined(__morph__libmorph__) */
