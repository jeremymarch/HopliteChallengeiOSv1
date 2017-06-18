// gcc -std=c99 morphtest.c libmorph.c GreekForms.c -o morphtest

#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen
#include <stdbool.h> //for bool type
#include "libmorph.h"

extern Verb verbs[];


int main(int argc, char **argv)
{
	int bufferLen = 2048;
    char buffer[bufferLen];
	VerbFormC vf, vf2;

    vf.verb = &verbs[104];
	vf.person = SECOND;
	vf.number = SINGULAR;
	vf.tense = PRESENT;
	vf.voice = ACTIVE;
	vf.mood = IMPERATIVE;

	vf2.verb = vf.verb;
	vf2.person = vf.person;
	vf2.number = vf.number;
	vf2.tense = vf.tense;
	vf2.voice = vf.voice;
	vf2.mood = vf.mood;
	//changeFormByDegrees(&vf2, 2);
	getForm(&vf2, buffer, bufferLen, true, false);
	printf("Verb: %s\n", buffer);
/*
	getDistractorsForChange(&vf, &vf2, 3, buffer);
	printf("Verb: %s\n", buffer);

   int units[20] = { 1,2,3,4,5,6,7,8,9,10,11 };
    int numUnits = 2;
	//while (1)
	//{
	    Verb *v = getRandomVerb(units, numUnits);
	    vf.verb = v;//&verbs[25];
	    generateForm(&vf);
	    //vf.tense = PRESENT;

	    getAbbrevDescription(&vf, buffer, bufferLen);
	    printf("\n\n%s\n", buffer);

	    getForm(&vf, buffer, bufferLen, true);
	    printf("%s\n\n", buffer);

	    changeFormByDegrees(&vf, 2);
	    //generateForm(&vf);

	    getAbbrevDescription(&vf, buffer, bufferLen);
	    printf("%s\n", buffer);

	    getForm(&vf, buffer, bufferLen, true);
	    printf("%s\n\n\n\n", buffer);
		*/
	//}
}
