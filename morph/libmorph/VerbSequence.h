//
//  VerbSequence.h
//  Hoplite Challenge
//
//  Created by Jeremy on 2/1/16.
//  Copyright © 2016 Jeremy March. All rights reserved.
//

#ifndef VerbSequence_h
#define VerbSequence_h

#include <stdio.h>
#include "libmorph.h"
#include "GreekForms.h"

enum {
    VERB_SEQ_CHANGE = 1,
    VERB_SEQ_CHANGE_NEW,
    VERB_SEQ_PP,
    VERB_SEQ_ENDING
};

typedef struct vfr VerbFormRecord;
struct vfr {
    time_t time;
    int elapsedTime;
    int verb;
    unsigned char person;
    unsigned char number;
    unsigned char tense;
    unsigned char voice;
    unsigned char mood;
    bool correct;
    char answer[130]; //needs to be more than longest answer: Longest Form: 2,1,3,0,2, v: καταστήσαιεν, καταστήσειαν, κατασταῖεν, κατασταίησαν, l: 102
};

//for mmap on ios see: https://github.com/mekjaer/ios-mmap/blob/master/ios-mmap-example/ios-mmap-example/AppDelegate.m
typedef struct da {
    unsigned int numRecords;
    unsigned int head;
    VerbFormRecord vr[2000];
} DataFormat;

typedef struct vso {
    bool startOnFirstSing;
    unsigned char repsPerVerb;
    unsigned char degreesToChange;
    unsigned char numUnits;
    bool askEndings;
    bool askPrincipalParts;
    bool isHCGame; //else is practice
    int practiceVerbID; //to just practice on one verb
    int units[20];
    
} VerbSeqOptions;

bool compareFormsCheckMFRecordResult(UCS2 *expected, int expectedLen, UCS2 *given, int givenLen, bool MFPressed, const char *elapsedTime, int *score, int *lives);
void closeDataFile();
void syncDataFile();

bool dbInit(const char *path);
void VerbSeqInit(const char *path);
int nextVerbSeq(int *seq, VerbFormC *vf1, VerbFormC *vf2, VerbSeqOptions *vso);
int nextVerbSeq2(int *seq, VerbFormD *vf1, VerbFormD *vf2, VerbSeqOptions *vso1);
void resetVerbSeq();
void changeFormByDegrees(VerbFormC *verbform, int degrees);
void generateForm(VerbFormC *verbform);
void getDistractorsForChange(VerbFormC *orig, VerbFormC *new, int numDistractors, char *buffer);
bool isValidFormForUnit(VerbFormC *vf, int unit);

Verb *getRandomVerb(int *units, int numUnits);
Verb *getRandomVerbFromUnit(int *units, int numUnits);

Ending *getRandomEnding(int *units, int numUnits);
void getRandomEndingAsString(int *units, int numUnits, char *buffer, int bufferLen);


#endif /* VerbSequence_h */


