// gcc -std=c99 ucs2test.c libmorph.c GreekForms.c -o ucs2test

#include <stdlib.h> // For random(), RAND_MAX
#include <string.h>  //for strlen
#include <stdbool.h> //for bool type
#include "GreekUnicode.h"
#include "libmorph.h"

void printUCS2(UCS2 *u, int len)
{
    printf("UCS2 Length: %d\n", len);
    for(int i = 0; i < len; i++)
        printf("UCS2 %d: %0x\n", i, u[i]);
}

void printUtf8(char *u, int len)
{
    printf("UTF8 Length: %d\n", len);
    for(int i = 0; i < len; i++)
        printf("UTF8 %d: %0x\n", i, u[i]);
}

int main(int argc, char const *argv[]) {

    char buffer[1024];
    UCS2 u[] = { GREEK_SMALL_LETTER_ALPHA, COMBINING_MACRON };
    int uLen = 2;

    UCS2 ucs22[1024];
    int ucs2Len2 = 0;

    ucs2_to_utf8_string(u, 2, buffer);

    printUtf8(buffer, strlen(buffer));

    utf8_to_ucs2_string((const unsigned char *)buffer, ucs22, &ucs2Len2);
    printf("before\n");
    printUCS2(ucs22, ucs2Len2);


    accentSyllable2(ucs22, 0, &ucs2Len2, MACRON);

    ucs2_to_utf8_string(ucs22, ucs2Len2, buffer);
    printf("after\n");
    printUCS2(ucs22, ucs2Len2);

    printf("Buffer: %s\n", buffer);

    return 0;
}
