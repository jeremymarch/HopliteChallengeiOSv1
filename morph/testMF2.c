#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef char UCS2;

#define COMMA ','
#define SPACE ' '
#define NEW_LINE '\n'
#define MAX_FORMS 5

bool compareWord(UCS2 *w1, int w1len, UCS2 *w2, int w2len);

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

bool compareForms(UCS2 *w1, int w1len, UCS2 *w2, int w2len)
{
    int starts[MAX_FORMS];
    int ends[MAX_FORMS];
    int numStrings = 0;
    getStartEnd(w1, w1len, starts, ends, &numStrings);

    int starts2[MAX_FORMS];
    int ends2[MAX_FORMS];
    int numStrings2 = 0;
    getStartEnd(w2, w2len, starts2, ends2, &numStrings2);

    for(int i = 0; i < numStrings; i++)
    {
        printf("s: %d, l: %d, \"%.*s\"\n", starts[i], ends[i] - starts[i], ends[i] - starts[i], &w1[starts[i]]);
    }
    for(int i = 0; i < numStrings2; i++)
    {
        printf("s: %d, l: %d, \"%.*s\"\n", starts2[i], ends2[i] - starts2[i], ends2[i] - starts2[i], &w2[starts2[i]]);
    }
    printf("num: %d\n", numStrings);

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

int main(int argc, char const *argv[]) {
    /* code */

UCS2 *s1 = " test , abc , def    ";
UCS2 *s2 = "   test ,  def ,  abc";

printf("A: %d\n", compareForms(s1, strlen(s1), s2, strlen(s2)));

    return 0;
}
