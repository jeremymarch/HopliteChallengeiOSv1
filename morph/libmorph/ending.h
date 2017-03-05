//
//  ending.h
//  Hoplite Challenge
//
//  Created by Jeremy March on 3/4/17.
//  Copyright © 2017 Jeremy March. All rights reserved.
//

#ifndef ending_h
#define ending_h

#include <stdio.h>
#include <stdbool.h>
#include "utilities.h"
#include "libmorph.h"
#include "GreekUnicode.h"

void addEnding(VerbFormC *vf, UCS2 *ucs2, int *len, UCS2 *ending, int elen, bool contractedFuture, bool decompose);

#endif /* ending_h */
