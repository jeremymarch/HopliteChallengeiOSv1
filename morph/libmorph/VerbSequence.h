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

typedef struct vso {
    bool startOnFirstSing;
    unsigned char repsPerVerb;
    unsigned char degreesToChange;
    unsigned char numUnits;
    bool askEndings;
    bool askPrincipalParts;
    int units[20];
} VerbSeqOptions;

void init(char *path);
int nextVerbSeq(int *seq, VerbFormC *vf1, VerbFormC *vf2, VerbSeqOptions *vso);
void resetVerbSeq();
void changeFormByDegrees(VerbFormC *verbform, int degrees);
void generateForm(VerbFormC *verbform);
void getDistractorsForChange(VerbFormC *orig, VerbFormC *new, int numDistractors, char *buffer);
bool isValidFormForUnit(VerbFormC *vf, int unit);

Verb *getRandomVerb(int *units, int numUnits);
Ending *getRandomEnding(int *units, int numUnits);
void getRandomEndingAsString(int *units, int numUnits, char *buffer, int bufferLen);


#endif /* VerbSequence_h */


