//
//  augment.c
//  Hoplite Challenge
//
//  Created by Jeremy March on 3/4/17.
//  Copyright © 2017 Jeremy March. All rights reserved.
//

#include "augment.h"

//we assume accents have been stripped, but not breathings, macrons or iota subscripts
void augmentStem(VerbFormC *vf, UCS2 *ucs2, int *len, bool decompose)
{
    if (decompose && (vf->verb->verbclass & PREFIXED) != PREFIXED)
    {
        /*
         Don't add an augment on these decomposed forms which start with e, ei, or h
         
         H&Q page 326.  "In most verbs when principal part iv or v begins with e) or ei), the pluperfect is unaugmented."
         */
        if (vf->tense == PLUPERFECT && (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || (ucs2[0] == GREEK_SMALL_LETTER_EPSILON && ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI) || ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI || ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA || ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI || ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI ))
        {
            return;
        }
        
        if (vf->tense == PLUPERFECT && ucs2[0] == GREEK_SMALL_LETTER_ETA && ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
        {
            return;
        }
        
        if (utf8HasSuffix(vf->verb->present, "εἶμι"))
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
            return; //don't do anything with this weird augmenting
        }
        //long iota doesn't get further augment (e.g. ihmi)
        if (ucs2[0] == GREEK_SMALL_LETTER_IOTA && ucs2[1] == COMBINING_MACRON)
        {
            return;
        }
        
        if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")) || !utf8HasSuffix(vf->verb->present, "στημι"))
        {
            //pluperfect of elau/nw herehere
            if (ucs2[0] == '-' && ucs2[1] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                return;
            
            if (ucs2[1] == GREEK_SMALL_LETTER_EPSILON && ucs2[2] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
                return;
            
            if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
                return;
            
            rightShiftFromOffsetSteps(ucs2, 0, 4, len);
            ucs2[0] = DECOMPOSED_AUGMENT_CHAR;
            ucs2[1] = SPACE;
            ucs2[2] = HYPHEN;
            ucs2[3] = SPACE;
        }
        return;
    }
    
    if ((vf->verb->verbclass & PREFIXED) == PREFIXED)
    {
        UCS2 ek[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_KAPPA };
        UCS2 ana[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 sum[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU };
        UCS2 sun[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_NU };
        UCS2 dia[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_ALPHA };
        //UCS2 di[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA };
        UCS2 ape[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_EPSILON };
        UCS2 apol[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_LAMDA };
        UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON };
        UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
        UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
        UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 epi[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_IOTA };
        UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        
        if (hasPrefix(ucs2, *len, ek, 2))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 2, 3, len);
                ucs2[2] = SPACE;
                ucs2[3] = HYPHEN;
                ucs2[4] = SPACE;
                if (vf->tense == AORIST || vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                    
                    ucs2[5] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[6] = SPACE;
                    ucs2[7] = HYPHEN;
                    ucs2[8] = SPACE;
                }
            }
            else if (vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
            {
                rightShiftFromOffsetSteps(ucs2, 2, 1, len);
                ucs2[1] = GREEK_SMALL_LETTER_XI;
                ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, ana, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                if (vf->tense == AORIST || vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
            }
            else if (vf->tense == IMPERFECT || vf->tense == PLUPERFECT)
            {
                //rightShiftFromOffsetSteps(ucs2, 3, 1, len);
                ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, sum, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                ucs2[2] = GREEK_SMALL_LETTER_NU;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                if (vf->tense == AORIST || vf->tense == IMPERFECT)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
            }
            else if (vf->tense == IMPERFECT)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 1, len);
                ucs2[2] = GREEK_SMALL_LETTER_NU;
                ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, sun, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                ucs2[2] = GREEK_SMALL_LETTER_NU;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                if ((vf->tense == AORIST || vf->tense == IMPERFECT) && !(ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON))
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
                else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON)
                {
                    rightShiftFromOffsetSteps(ucs2, 7, 1, len);
                    ucs2[8] = COMBINING_ROUGH_BREATHING;
                }
                else if (vf->tense == PLUPERFECT && ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
                {
                    ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                }
            }
            else if (vf->tense == IMPERFECT)
            {
                if (!(ucs2[3] == GREEK_SMALL_LETTER_IOTA && ucs2[4] == COMBINING_MACRON))
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 1, len);
                    ucs2[2] = GREEK_SMALL_LETTER_NU;
                    ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, dia, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 7, len);
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
        else if (hasPrefix(ucs2, *len, epi, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 7, len);
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
        else if (hasPrefix(ucs2, *len, ape, 3) && vf->tense == PLUPERFECT && decompose)
        {
            rightShiftFromOffsetSteps(ucs2, 2, 4, len);
            ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
            ucs2[3] = SPACE;
            ucs2[4] = HYPHEN;
            ucs2[5] = SPACE;
        }
        else if (hasPrefix(ucs2, *len, apol, 4))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 2, 8, len);
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
                ucs2[2] = GREEK_SMALL_LETTER_OMEGA;
            }
        }
        else if (hasPrefix(ucs2, *len, apo, 3))
        {
            if ( decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 7, len);
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
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 2, 4, len);
                
                ucs2[1] = GREEK_SMALL_LETTER_PI;
                ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")) || (!utf8HasSuffix(vf->verb->present, "στημι") && !utf8HasSuffix(vf->verb->present, "ἀφῑ́ημι") && !(utf8HasSuffix(vf->verb->present, "ἀφικνέομαι") && vf->tense == PLUPERFECT)))
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                    
                    if (ucs2[10] == GREEK_SMALL_LETTER_IOTA && ucs2[11] == COMBINING_MACRON)
                    {
                        rightShiftFromOffsetSteps(ucs2, 11, 1, len);
                        ucs2[12] = COMBINING_ROUGH_BREATHING;
                    }
                    else if (ucs2[10] == GREEK_SMALL_LETTER_IOTA)
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
                    if (ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON)
                    {
                        rightShiftFromOffsetSteps(ucs2, 7, 1, len);
                        ucs2[8] = COMBINING_ROUGH_BREATHING;
                    }
                    else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA)
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
                if (vf->tense == PLUPERFECT && (vf->number == SINGULAR || vf->voice != ACTIVE) && utf8HasSuffix(vf->verb->present, "στημι") )
                {
                    rightShiftFromOffsetSteps(ucs2, 2, 1, len);
                    ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[3] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[2] == GREEK_SMALL_LETTER_IOTA && ucs2[3] != COMBINING_MACRON)
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 1, len);
                    ucs2[3] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, kath, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 4, len);
                
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "στημι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "στημι")))
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 4, len);
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
                    rightShiftFromOffsetSteps(ucs2, 4, 1, len);
                    ucs2[4] = COMBINING_MACRON;
                }
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                else if (vf->tense == PLUPERFECT && (vf->number == SINGULAR || vf->voice != ACTIVE) && utf8HasSuffix(vf->verb->present, "στημι") )
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 1, len);
                    ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[4] = GREEK_SMALL_LETTER_IOTA;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, metan, 5))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 4, 8, len);
                
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
                    rightShiftFromOffsetSteps(ucs2, 13, 4, len);
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
                if (vf->tense == PLUPERFECT && (vf->number == SINGULAR || vf->voice != ACTIVE) && utf8HasSuffix(vf->verb->present, "σταμαι") )
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 1, len);
                    ucs2[5] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[6] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[5] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 1, len);
                    ucs2[6] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, epan, 4))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 1, 8, len);
                
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
                //isthmi only augments pluperfect in the singular
                if ((vf->tense == PLUPERFECT && vf->number == SINGULAR && utf8HasSuffix(vf->verb->present, "σταμαι")) || (vf->tense != PLUPERFECT && utf8HasSuffix(vf->verb->present, "σταμαι")))
                {
                    rightShiftFromOffsetSteps(ucs2, 12, 4, len);
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
                //isthmi only augments pluperfect in the singular
                //for histhmi pluperfect singular.  H&Q PAGE 378.
                if (vf->tense == PLUPERFECT && (vf->number == SINGULAR || vf->voice != ACTIVE) && utf8HasSuffix(vf->verb->present, "σταμαι") )
                {
                    rightShiftFromOffsetSteps(ucs2, 4, 1, len);
                    ucs2[4] = GREEK_SMALL_LETTER_EPSILON;
                    ucs2[5] = GREEK_SMALL_LETTER_IOTA;
                }
                else if (ucs2[4] == GREEK_SMALL_LETTER_IOTA)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 1, len);
                    ucs2[5] = COMBINING_MACRON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, meta, 4))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 4, 7, len);
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
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
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
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
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
        if (utf8HasSuffix(vf->verb->present, "ἔχω"))
        {
            rightShiftFromOffsetSteps(ucs2, 1, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        }
        else
        {
            ucs2[0] = GREEK_SMALL_LETTER_ETA_WITH_PSILI;
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
    {
        //for histhmi pluperfect singular.  H&Q PAGE 378.
        if (vf->tense == PLUPERFECT && (vf->number == SINGULAR || vf->voice != ACTIVE) && ( utf8HasSuffix(vf->verb->present, "στημι") || utf8HasSuffix(vf->verb->present, "σταμαι")))
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (vf->tense == IMPERFECT)
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA && ucs2[1] != COMBINING_MACRON)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 1, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 2, len);
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_SMOOTH_BREATHING;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 2, len);
        
        ucs2[0] = GREEK_SMALL_LETTER_IOTA;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_ROUGH_BREATHING;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON)
    {
        if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_PSILI_AND_YPOGEGRAMMENI;
        }
        else if (ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
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
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA && vf->tense == IMPERFECT && utf8HasSuffix(vf->verb->present, "ὁράω") )
    {
        //h&q page 449
        rightShiftFromOffsetSteps(ucs2, 0, 1, len);
        ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        ucs2[1] = GREEK_SMALL_LETTER_OMEGA;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_OMICRON_WITH_DASIA)
    {
        ucs2[0] = GREEK_SMALL_LETTER_OMEGA_WITH_DASIA;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 1, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON_WITH_PSILI)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 2, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_SMOOTH_BREATHING;
    }
    else if (ucs2[0] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA)
    {
        rightShiftFromOffsetSteps(ucs2, 0, 1, len);
        ucs2[0] = GREEK_SMALL_LETTER_UPSILON;
        ucs2[1] = COMBINING_MACRON;
        ucs2[2] = COMBINING_ROUGH_BREATHING;
    }
    else if (isConsonant(ucs2[0]))
    {
        rightShiftFromOffsetSteps(ucs2, 0, 1, len);
        ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
    }
}

void stripAugmentFromPrincipalPart(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 presentStemInitial, bool decompose)
{
    UCS2 err[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_RHO, GREEK_SMALL_LETTER_RHO };
    if ((vf->verb->verbclass & PREFIXED) == PREFIXED)
    {
        UCS2 ex[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_XI };
        UCS2 ane[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_EPSILON };
        UCS2 sum[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU };
        UCS2 sun[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_NU };
        UCS2 dia[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_ALPHA };
        UCS2 dih[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_ETA };
        //UCS2 di[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA };
        UCS2 apw[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMEGA };
        UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_EPSILON };
        UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
        UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
        UCS2 kate[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_EPSILON };
        UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
        UCS2 mete[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_EPSILON };
        UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
        UCS2 ep[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI };
        
        if (hasPrefix(ucs2, *len, ex, 2))
        {
            if (decompose)
            {
                if (vf->tense == AORIST)
                {
                    rightShiftFromOffsetSteps(ucs2, 2, 3, len);
                    
                    ucs2[1] = GREEK_SMALL_LETTER_KAPPA;
                    ucs2[2] = SPACE;
                    ucs2[3] = HYPHEN;
                    ucs2[4] = SPACE;
                    if (vf->mood == INDICATIVE)
                    {
                        rightShiftFromOffsetSteps(ucs2, 4, 3, len);
                        
                        ucs2[5] = DECOMPOSED_AUGMENT_CHAR;
                        ucs2[6] = SPACE;
                        ucs2[7] = HYPHEN;
                        ucs2[8] = SPACE;
                    }
                    else
                    {
                        leftShiftFromOffsetSteps(ucs2, 5, 1, len);
                    }
                }
            }
            else if ((vf->tense == AORIST && vf->mood != INDICATIVE) || vf->tense == FUTURE)
            {
                ucs2[1] = GREEK_SMALL_LETTER_KAPPA;
                leftShiftFromOffsetSteps(ucs2, 2, 1, len);
            }
        }
        else if (hasPrefix(ucs2, *len, ane, 3))
        {
            if (decompose)
            {
                if (vf->tense == AORIST)
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                    ucs2[2] = GREEK_SMALL_LETTER_ALPHA;
                    ucs2[3] = SPACE;
                    ucs2[4] = HYPHEN;
                    ucs2[5] = SPACE;
                    if (vf->mood == INDICATIVE)
                    {
                        rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                        
                        ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                        ucs2[7] = SPACE;
                        ucs2[8] = HYPHEN;
                        ucs2[9] = SPACE;
                    }
                    else if (vf->tense == AORIST || vf->tense == FUTURE)
                    {
                        //ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                    }
                }
                else
                {
                    ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                }
            }
            else if ((vf->tense == AORIST && vf->mood != INDICATIVE) || vf->tense == FUTURE)
            {
                ucs2[2] = GREEK_SMALL_LETTER_ALPHA;
            }
        }
        else if (hasPrefix(ucs2, *len, sum, 3))
        {
            if (decompose)
            {
                if (vf->tense == AORIST)
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                    ucs2[2] = GREEK_SMALL_LETTER_NU;
                    ucs2[3] = SPACE;
                    ucs2[4] = HYPHEN;
                    ucs2[5] = SPACE;
                    if (vf->mood == INDICATIVE)
                    {
                        rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                        
                        ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                        ucs2[7] = SPACE;
                        ucs2[8] = HYPHEN;
                        ucs2[9] = SPACE;
                        ucs2[10] = GREEK_SMALL_LETTER_EPSILON;
                    }
                    else if (vf->tense == AORIST || vf->tense == FUTURE)
                    {
                        ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                    }
                }
                else
                {
                    ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                }
            }
            else if ((vf->tense == AORIST && vf->mood != INDICATIVE) || vf->tense == FUTURE)
            {
                ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, sun, 3))
        {
            if (decompose)
            {
                if (vf->tense == AORIST)
                {
                    rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                    ucs2[2] = GREEK_SMALL_LETTER_NU;
                    ucs2[3] = SPACE;
                    ucs2[4] = HYPHEN;
                    ucs2[5] = SPACE;
                    if (vf->mood == INDICATIVE && !(ucs2[6] == GREEK_SMALL_LETTER_ETA && ucs2[7] == GREEK_SMALL_LETTER_KAPPA && (vf->number == SINGULAR || vf->voice == MIDDLE)))
                    {
                        rightShiftFromOffsetSteps(ucs2, 5, 4, len);
                        
                        ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                        ucs2[7] = SPACE;
                        ucs2[8] = HYPHEN;
                        ucs2[9] = SPACE;
                        ucs2[10] = GREEK_SMALL_LETTER_EPSILON;
                        
                        if (ucs2[10] == GREEK_SMALL_LETTER_EPSILON && ucs2[11] == GREEK_SMALL_LETTER_IOTA && vf->voice == PASSIVE)
                        {
                            leftShiftFromOffsetSteps(ucs2, 11, 1, len);
                            ucs2[10] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                        }
                    }
                    else if (vf->tense == AORIST || vf->tense == FUTURE)
                    {
                        if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
                        {
                            leftShiftFromOffsetSteps(ucs2, 7, 1, len);
                            ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                        }
                        else
                        {
                            ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                        }
                    }
                }
                else
                {
                    if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
                    {
                        leftShiftFromOffsetSteps(ucs2, 7, 1, len);
                        ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                    else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
                    {
                        leftShiftFromOffsetSteps(ucs2, 7, 1, len);
                        ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                    }
                    else
                    {
                        ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                    }
                }
            }
            else if ((vf->tense == AORIST && vf->mood != INDICATIVE) || vf->tense == FUTURE)
            {
                if (ucs2[3] == GREEK_SMALL_LETTER_EPSILON && ucs2[4] == GREEK_SMALL_LETTER_IOTA)
                {
                    leftShiftFromOffsetSteps(ucs2, 4, 1, len);
                }
                else
                {
                    ucs2[3] = GREEK_SMALL_LETTER_EPSILON;
                }
            }
        }
        else if (hasPrefix(ucs2, *len, dih, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                
                ucs2[2] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 5, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                    ucs2[10] = GREEK_SMALL_LETTER_EPSILON;
                }
                else if (vf->tense == AORIST || vf->tense == FUTURE)
                {
                    rightShiftFromOffsetSteps(ucs2, 5, 1, len);
                    ucs2[6] = GREEK_SMALL_LETTER_EPSILON;
                }
            }
            else
            {
                ucs2[2] = GREEK_SMALL_LETTER_EPSILON;
            }
        }
        else if (hasPrefix(ucs2, *len, dia, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                }
            }
            
            ucs2[2] = GREEK_SMALL_LETTER_ALPHA;
        }
        else if (hasPrefix(ucs2, *len, apw, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 2, 4, len);
                
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                    ucs2[10] = GREEK_SMALL_LETTER_OMICRON;
                }
                else
                {
                    ucs2[6] = GREEK_SMALL_LETTER_OMICRON;
                }
            }
            
            ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
        }
        else if (hasPrefix(ucs2, *len, apo, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    
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
                rightShiftFromOffsetSteps(ucs2, 2, 4, len);
                
                ucs2[1] = GREEK_SMALL_LETTER_PI;
                ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE && !(ucs2[6] == GREEK_SMALL_LETTER_ETA && ucs2[7] == GREEK_SMALL_LETTER_KAPPA && (vf->number == SINGULAR || vf->voice != ACTIVE)))
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                    if (ucs2[10] == GREEK_SMALL_LETTER_EPSILON && ucs2[11] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[10] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                        leftShiftFromOffsetSteps(ucs2, 11, 1, len);
                    }
                    else if (ucs2[10] == GREEK_SMALL_LETTER_IOTA && ucs2[11] == COMBINING_MACRON)
                    {//ikneomai
                        leftShiftFromOffsetSteps(ucs2, 11, 1, len);
                        ucs2[10] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                        
                    }
                }
                else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
                {
                    leftShiftFromOffsetSteps(ucs2, 6, 1, len);
                    ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
                }
                else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON)
                {   //ikneomai
                    leftShiftFromOffsetSteps(ucs2, 7, 1, len);
                    ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                }
                else if (!(ucs2[6] == GREEK_SMALL_LETTER_ETA && ucs2[7] == GREEK_SMALL_LETTER_KAPPA))
                {
                    leftShiftFromOffsetSteps(ucs2, 6, 1, len);
                }
            }
            else
            {
                if (ucs2[2] == GREEK_SMALL_LETTER_EPSILON && ucs2[3] == GREEK_SMALL_LETTER_IOTA)
                {
                    leftShiftFromOffsetSteps(ucs2, 3, 1, len);
                }
                else if (ucs2[2] == GREEK_SMALL_LETTER_IOTA && ucs2[3] == COMBINING_MACRON)
                {   //ikneomai
                    leftShiftFromOffsetSteps(ucs2, 3, 1, len);
                }
            }
        }
        else if (hasPrefix(ucs2, *len, kath, 3))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 4, len);
                ucs2[2] = GREEK_SMALL_LETTER_TAU;
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 7, 4, len);
                    
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
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                
                //ucs2[2] = GREEK_SMALL_LETTER_TAU;
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 7, 4, len);
                    
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
                rightShiftFromOffsetSteps(ucs2, 4, 7, len);
                
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
                    rightShiftFromOffsetSteps(ucs2, 13, 4, len);
                    
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
                rightShiftFromOffsetSteps(ucs2, 1, 7, len);
                
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
                    rightShiftFromOffsetSteps(ucs2, 12, 4, len);
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
        else if (hasPrefix(ucs2, *len, ep, 2))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 1, 3, len);
                
                ucs2[2] = GREEK_SMALL_LETTER_IOTA;
                ucs2[3] = SPACE;
                ucs2[4] = HYPHEN;
                ucs2[5] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 6, 4, len);
                    ucs2[6] = DECOMPOSED_AUGMENT_CHAR;
                    ucs2[7] = SPACE;
                    ucs2[8] = HYPHEN;
                    ucs2[9] = SPACE;
                    if (ucs2[10] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[10] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
                else
                {
                    if (ucs2[6] == GREEK_SMALL_LETTER_IOTA)
                    {
                        ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
                    }
                }
            }
            else
            {
                ucs2[2] = GREEK_SMALL_LETTER_IOTA;
            }
        }
        else if (hasPrefix(ucs2, *len, meta, 4))
        {
            if (decompose)
            {
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 7, 4, len);
                    
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
                rightShiftFromOffsetSteps(ucs2, 3, 3, len);
                ucs2[3] = GREEK_SMALL_LETTER_ALPHA;
                ucs2[4] = SPACE;
                ucs2[5] = HYPHEN;
                ucs2[6] = SPACE;
                
                if (vf->tense == AORIST && vf->mood == INDICATIVE)
                {
                    rightShiftFromOffsetSteps(ucs2, 7, 4, len);
                    
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
        if (ucs2[0] == '-')
        {
            if (ucs2[1] == GREEK_SMALL_LETTER_EPSILON && ucs2[2] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA) //ei(
            {
                leftShiftFromOffsetSteps(ucs2, 1, 1, len);
                ucs2[1] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            }
            else if (ucs2[1] != GREEK_SMALL_LETTER_ETA_WITH_DASIA)
            {
                leftShiftFromOffsetSteps(ucs2, 1, 1, len);
            }
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON && ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA) //ei(
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON && ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_PSILI && utf8HasSuffix(vf->verb->aorist, "εἶδον")) //ei)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
        }
        else if (hasPrefix(ucs2, *len, err, 3)) //err
        {
            leftShiftFromOffsetSteps(ucs2, 0, 2, len);
            ucs2[0] = GREEK_SMALL_LETTER_RHO_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA) //strip
        {
            //NSLog(@"removed augment");
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA && ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA) //hu)
        {
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_PHI)//for phero
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            else
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI; //for erhsomai, doesn't have present to check
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            else if (presentStemInitial == GREEK_SMALL_LETTER_PHI)//for phero
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI) //hi(
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ALPHA;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI) //hi)
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ALPHA;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
        {
            ucs2[0] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
        }
        
    }
    else if (vf->tense == FUTURE && vf->voice == PASSIVE)
    {
        if (ucs2[0] == '-')
        {
            if (ucs2[1] == GREEK_SMALL_LETTER_EPSILON && ucs2[2] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA) //ei(
            {
                leftShiftFromOffsetSteps(ucs2, 1, 1, len);
                ucs2[1] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            }
            else
            {
                leftShiftFromOffsetSteps(ucs2, 1, 1, len);
            }
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON && ucs2[1] == GREEK_SMALL_LETTER_IOTA_WITH_DASIA)
        {
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
        else if (hasPrefix(ucs2, *len, err, 3)) //err
        {
            leftShiftFromOffsetSteps(ucs2, 0, 2, len);
            ucs2[0] = GREEK_SMALL_LETTER_RHO_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI || ucs2[0] == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
        {
            //NSLog(@"removed augment");
            leftShiftFromOffsetSteps(ucs2, 0, 1, len);
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA && ucs2[1] == GREEK_SMALL_LETTER_UPSILON_WITH_DASIA) //hu)
        {
            ucs2[0] = GREEK_SMALL_LETTER_EPSILON;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_PSILI)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
            else if (presentStemInitial == GREEK_SMALL_LETTER_PHI)//for phero
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_PSILI;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA )
        {
            if (presentStemInitial == GREEK_SMALL_LETTER_ALPHA_WITH_DASIA)
                ucs2[0] = GREEK_SMALL_LETTER_ALPHA_WITH_DASIA;
            else if (presentStemInitial == GREEK_SMALL_LETTER_EPSILON_WITH_DASIA)
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
            else if (presentStemInitial == GREEK_SMALL_LETTER_PHI)//for phero
                ucs2[0] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA_AND_YPOGEGRAMMENI)
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ALPHA;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_PSILI_AND_YPOGEGRAMMENI)
        {
            rightShiftFromOffsetSteps(ucs2, 0, 1, len);
            ucs2[0] = GREEK_SMALL_LETTER_ALPHA;
            ucs2[1] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        }
        else if (ucs2[0] == GREEK_SMALL_LETTER_OMEGA_WITH_PSILI)
        {
            ucs2[0] = GREEK_SMALL_LETTER_OMICRON_WITH_PSILI;
        }
    }
    
    if (vf->tense == AORIST && vf->mood == INDICATIVE && decompose && (vf->verb->verbclass & PREFIXED) != PREFIXED)
    {
        if (ucs2[0] == '-')
        {
            if (utf8HasSuffix(vf->verb->aorist, "-ἧκα") && ((vf->voice == ACTIVE && vf->number == SINGULAR) || vf->voice == MIDDLE))
                return;
            
            rightShiftFromOffsetSteps(ucs2, 1, 5, len);
            ucs2[1] = SPACE;
            ucs2[2] = DECOMPOSED_AUGMENT_CHAR;
            ucs2[3] = SPACE;
            ucs2[4] = HYPHEN;
            ucs2[5] = SPACE;
        }
        else
        {
            if (ucs2[0] == GREEK_SMALL_LETTER_ETA_WITH_DASIA)
                return;
            
            rightShiftFromOffsetSteps(ucs2, 0, 4, len);
            ucs2[0] = DECOMPOSED_AUGMENT_CHAR;
            ucs2[1] = SPACE;
            ucs2[2] = HYPHEN;
            ucs2[3] = SPACE;
        }
    }
}

void decomposePrefixes(VerbFormC *vf, UCS2 *ucs2, int *len)
{
    UCS2 ek[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_KAPPA };
    UCS2 ana[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 sum[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_MU };
    UCS2 sun[] = { GREEK_SMALL_LETTER_SIGMA, GREEK_SMALL_LETTER_UPSILON, GREEK_SMALL_LETTER_NU };
    UCS2 dia[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_ALPHA };
    UCS2 dio[] = { GREEK_SMALL_LETTER_DELTA, GREEK_SMALL_LETTER_IOTA, GREEK_SMALL_LETTER_OMICRON }; //fix me hack so this doesn't work on future passive
    UCS2 ape[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_EPSILON };
    UCS2 apol[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON, GREEK_SMALL_LETTER_LAMDA };
    UCS2 apo[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_OMICRON };
    UCS2 aph[] = { GREEK_SMALL_LETTER_ALPHA_WITH_PSILI, GREEK_SMALL_LETTER_PHI };
    UCS2 kath[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_THETA };
    UCS2 kata[] = { GREEK_SMALL_LETTER_KAPPA, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 meta[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 metan[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
    //UCS2 metana[] = { GREEK_SMALL_LETTER_MU, GREEK_SMALL_LETTER_EPSILON, GREEK_SMALL_LETTER_TAU, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU, GREEK_SMALL_LETTER_ALPHA };
    UCS2 epan[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_ALPHA, GREEK_SMALL_LETTER_NU };
    UCS2 epi[] = { GREEK_SMALL_LETTER_EPSILON_WITH_PSILI, GREEK_SMALL_LETTER_PI, GREEK_SMALL_LETTER_IOTA };
    
    if (hasPrefix(ucs2, *len, ek, 2))
    {
        rightShiftFromOffsetSteps(ucs2, 2, 3, len);
        ucs2[2] = SPACE;
        ucs2[3] = HYPHEN;
        ucs2[4] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, ana, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, sum, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[2] = GREEK_SMALL_LETTER_NU;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, sun, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[2] = GREEK_SMALL_LETTER_NU;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
        if (ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON)
        {
            rightShiftFromOffsetSteps(ucs2, 7, 1, len);
            ucs2[8] = COMBINING_ROUGH_BREATHING;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA && ucs2[8] == GREEK_SMALL_LETTER_SIGMA)
        {
            ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_PSILI;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (vf->tense == FUTURE && vf->voice != PASSIVE && utf8HasSuffix(vf->verb->present, "ῑ́ημι"))
        {
            //rightShiftFromOffsetSteps(ucs2, 8, 1, len);
            ucs2[6] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, dia, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, dio, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 1, 4, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
        ucs2[2] = GREEK_SMALL_LETTER_ALPHA;
    }
    else if (hasPrefix(ucs2, *len, ape, 3) && vf->tense == PERFECT)
    {
        rightShiftFromOffsetSteps(ucs2, 2, 4, len);
        ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, apol, 4))
    {
        rightShiftFromOffsetSteps(ucs2, 2, 4, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, apo, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, epi, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 3, len);
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, aph, 2))
    {
        rightShiftFromOffsetSteps(ucs2, 2, 4, len);
        ucs2[1] = GREEK_SMALL_LETTER_PI;
        ucs2[2] = GREEK_SMALL_LETTER_OMICRON;
        ucs2[3] = SPACE;
        ucs2[4] = HYPHEN;
        ucs2[5] = SPACE;
        
        if (vf->tense == FUTURE && vf->voice != PASSIVE && utf8HasSuffix(vf->verb->present, "ῑ́ημι"))
        {
            //rightShiftFromOffsetSteps(ucs2, 8, 1, len);
            ucs2[6] = GREEK_SMALL_LETTER_ETA_WITH_DASIA;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA_WITH_PERISPOMENI)
        {
            ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA_AND_PERISPOMENI;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA && ucs2[7] == COMBINING_MACRON)
        {
            rightShiftFromOffsetSteps(ucs2, 8, 1, len);
            ucs2[8] = COMBINING_ROUGH_BREATHING;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_IOTA)
        {
            ucs2[6] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA && vf->tense == PERFECT)
        {
            ucs2[7] = GREEK_SMALL_LETTER_IOTA_WITH_DASIA;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON && ucs2[7] == GREEK_SMALL_LETTER_IOTA)
        {
            leftShiftFromOffsetSteps(ucs2, 7, 1, len);
            ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
        else if (ucs2[6] == GREEK_SMALL_LETTER_EPSILON)
        {
            ucs2[6] = GREEK_SMALL_LETTER_EPSILON_WITH_DASIA;
        }
    }
    else if (hasPrefix(ucs2, *len, kath, 3))
    {
        rightShiftFromOffsetSteps(ucs2, 3, 4, len);
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
        rightShiftFromOffsetSteps(ucs2, 4, 3, len);
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
    }
    else if (hasPrefix(ucs2, *len, metan, 5))
    {
        rightShiftFromOffsetSteps(ucs2, 4, 7, len);
        if (ucs2[12] == GREEK_SMALL_LETTER_IOTA || ucs2[12] == GREEK_SMALL_LETTER_EPSILON)
        {
            rightShiftFromOffsetSteps(ucs2, 4, 1, len);
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
        rightShiftFromOffsetSteps(ucs2, 1, 7, len);
        if (ucs2[11] == GREEK_SMALL_LETTER_IOTA || ucs2[11] == GREEK_SMALL_LETTER_EPSILON)
        {
            rightShiftFromOffsetSteps(ucs2, 4, 1, len);
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
        rightShiftFromOffsetSteps(ucs2, 4, 3, len);
        ucs2[4] = SPACE;
        ucs2[5] = HYPHEN;
        ucs2[6] = SPACE;
    }
}
