// gcc -std=c99 checkVerbForms.c libmorph.c GreekForms.c -o checkVerbForms

#include <time.h>
#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen
#include <stdbool.h> //for bool type
#include "libmorph.h"

extern Verb verbs[];
extern char *tenses[];
extern char *moods[];
extern char *voices[];

int numVerbs = 58;//58 includes unit 13

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
    vf.mood = INDICATIVE;
    int rowCount = 0;
    int bufferLen = 2048;
    char buffer[bufferLen];
    int bufferLen2 = 2048;
    char buffer2[bufferLen2];

    for (int verbi = 0; verbi < numVerbs; verbi++)
    {
        vf.verb = &verbs[verbi];
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
                                countPerSection++;
                                rowCount++;
                            }
                        }
                    }
                }
            }
        }
    }
    printf("\nTotal forms: %d", rowCount);
    return 0;
}
