// gcc -std=c99 checkVerbForms.c libmorph.c GreekForms.c accent.c utilities.c -o checkVerbForms

#include <time.h>
#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen
#include <stdbool.h> //for bool type
#include "libmorph.h"

extern Verb verbs[];
extern char *tenses[];
extern char *moods[];
extern char *voices[];

int numVerbs = 114;//114 is all 20 units

void printCurrentTime()
{
    time_t current_time;
    char* c_time_string;
    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to compute the current time.\n");
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
    }

    /* Print to stdout. ctime() has already added a terminating newline character. */
    (void) printf("%sTimestamp: %ld\n", c_time_string, current_time);
}

int main(int argc, char **argv)
{
    printCurrentTime();

    VerbFormC vf;
    VerbFormC longestVF;
    VerbFormC longestVFDecomposed;
    vf.mood = INDICATIVE;
    int rowCount = 0;
    int totalNumForms = 0;
    int bufferLen = 50;
    char buffer[bufferLen];
    int bufferLen2 = 137;
    char buffer2[bufferLen2];

    int MFZero = 0;
    int MFOne = 0;
    int MFTwo = 0;
    int MFThree = 0;
    int MFFour = 0;
    int MFFive = 0;
    int MFMore = 0;
    int longestForm = 0;
    int longestFormDecomposed = 0;

//numVerbs = 1;
    for (int verbi = 0; verbi < numVerbs; verbi++)
    {
        vf.verb = &verbs[verbi];
        char *depo;
        char *depMid = " (Middle Deponent)";
        char *depPass = " (Passive Deponent)";
        char *depPart = " (Partial Deponent)";
        char *depGig = " (Deponent gignomai)";
        if (deponentType(vf.verb) == MIDDLE_DEPONENT)
            depo = depMid;
        else if (deponentType(vf.verb) == PASSIVE_DEPONENT)
            depo = depMid;
        else if (deponentType(vf.verb) == PARTIAL_DEPONENT)
            depo = depPart;
        else if (deponentType(vf.verb) == DEPONENT_GIGNOMAI)
            depo = depGig;
        else
            depo = NULL;

        printf("\nVerb %d. %s%s\n", verbi, (strlen(verbs[verbi].present) > 0) ? verbs[verbi].present : verbs[verbi].future, (depo) ? depo : "");
        for (int g1 = 0; g1 < NUM_TENSES; g1++)
        {
            vf.tense = g1;
            for (int v = 0; v < NUM_VOICES; v++)
            {
                for (int m = 0; m < NUM_MOODS; m++)
                {
                    if (m != INDICATIVE && (g1 == PERFECT || g1 == PLUPERFECT || g1 == IMPERFECT || g1 == FUTURE))
                        continue;

                    vf.voice = v;
                    int countPerSection = 0;
                    for (int h = 0; h < NUM_NUMBERS; h++)
                    {
                        for (int i = 0; i < NUM_PERSONS; i++)
                        {
                            vf.number = h;
                            vf.person = i;
                            vf.mood = m;
                            if (getForm(&vf, buffer, bufferLen, true, false) && getForm(&vf, buffer2, bufferLen2, true, true))
                            {
                                if (countPerSection == 0)
                                {
                                    if (v == ACTIVE || g1 == AORIST || g1 == FUTURE)
                                    {
                                        printf("\n%s %s %s\n", tenses[g1], voices[v], moods[m]);
                                    }
                                    else if (v == MIDDLE)
                                    {
                                        //FIX ME, is this right?? how do we label these?
                                        if ( deponentType(vf.verb) == MIDDLE_DEPONENT || deponentType(vf.verb) == PASSIVE_DEPONENT)
                                        {
                                            printf("\n%s %s %s\n", tenses[g1], "middle", moods[m]);
                                        }
                                        else
                                        {
                                            printf("\n%s %s %s\n", tenses[g1], "middle/passive", moods[m]);
                                        }
                                    }
                                    else
                                    {
                                        continue; //skip passive if middle+passive are the same
                                    }
                                }

                                printf("%s ; %s\n", buffer, buffer2);
                                int thisNumForms = 0;
                                if (strncmp(buffer, "—", 2))
                                {
                                    thisNumForms++;
                                    totalNumForms++;
                                }

                                int utf8BufferLen = strlen(buffer);
                                //find the longest forms
                                if (longestForm < utf8BufferLen)
                                {
                                    longestForm = utf8BufferLen;
                                    longestVF.verb = vf.verb;
                                    longestVF.person = vf.person;
                                    longestVF.number = vf.number;
                                    longestVF.tense = vf.tense;
                                    longestVF.voice = vf.voice;
                                    longestVF.mood = vf.mood;
                                }

                                int utf8BufferLenDecomposed = strlen(buffer2);
                                //find the longest forms
                                if (longestFormDecomposed < utf8BufferLenDecomposed)
                                {
                                    longestFormDecomposed = utf8BufferLenDecomposed;
                                    longestVFDecomposed.verb = vf.verb;
                                    longestVFDecomposed.person = vf.person;
                                    longestVFDecomposed.number = vf.number;
                                    longestVFDecomposed.tense = vf.tense;
                                    longestVFDecomposed.voice = vf.voice;
                                    longestVFDecomposed.mood = vf.mood;
                                }

                                for (int z = 0; z < utf8BufferLen; z++)
                                {
                                    if (buffer[z] == ',')
                                    {
                                        thisNumForms++;
                                        totalNumForms++;
                                    }
                                }
                                switch(thisNumForms)
                                {
                                    case 0:
                                        MFZero++;
                                        break;
                                    case 1:
                                        MFOne++;
                                        break;
                                    case 2:
                                        MFTwo++;
                                        break;
                                    case 3:
                                        MFThree++;
                                        break;
                                    case 4:
                                        MFFour++;
                                        break;
                                    case 5:
                                        MFFive++;
                                        break;
                                    default:
                                        MFMore++;
                                        break;
                                }

                                countPerSection++;
                                rowCount++;
                            }
                        }
                    }
                }
            }
        }
    }

    printf("\nTotal rows including -: %d\n", rowCount);
    printf("Total forms including alternates, minus -: %d\n\n", totalNumForms);
    printf("Zero Forms: %d\n", MFZero);
    printf("One Form: %d\n", MFOne);
    printf("Two Forms: %d\n", MFTwo);
    printf("Three Forms: %d\n", MFThree);
    printf("Four Forms: %d\n", MFFour);
    printf("Five Forms: %d\n", MFFive);
    printf("More Forms: %d\n", MFMore);
    printf("Total Forms: %d = %d\n", (MFOne + (MFTwo*2) + (MFThree*3) + (MFFour*4) + (MFFive*5) + MFMore), totalNumForms);

    if (getForm(&longestVF, buffer, bufferLen, true, false))
    {
        printf("\nLongest Form: %d,%d,%d,%d,%d, v: %s, l: %d\n", longestVF.person, longestVF.number, longestVF.tense, longestVF.voice, longestVF.mood, buffer, strlen(buffer));
    }
    if (getForm(&longestVFDecomposed, buffer2, bufferLen, true, true))
    {
        printf("\nLongest Decomposed Form: %d,%d,%d,%d,%d, v: %s, l: %d\n", longestVFDecomposed.person, longestVFDecomposed.number, longestVFDecomposed.tense, longestVFDecomposed.voice, longestVFDecomposed.mood, buffer2, strlen(buffer2));
    }
    return 0;
}
