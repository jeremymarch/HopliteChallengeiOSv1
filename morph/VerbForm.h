//
//  VerbForm.h
//  morph
//
//  Created by Jeremy on 6/30/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Verbs.h"

@interface VerbForm : NSObject

-(void) generateForm;
-(VerbForm*) initWithVerb:(Verbs*)verb person:(NSInteger)p number:(NSInteger)n tense:(NSInteger)t voice:(NSInteger)v mood:(NSInteger)m;
-(NSString*) getPrincipalPart;
-(NSString*)stripEndingFromPrincipalPart:(NSString*)stem;
-(NSString*) getEnding;
-(NSString*) getForm;

-(NSString*)getPersonLabel:(BOOL)abbrev;
-(NSString*)getNumberLabel:(BOOL)abbrev;
-(NSString*)getTenseLabel:(BOOL)abbrev;
-(NSString*)getVoiceLabel:(BOOL)abbrev;
-(NSString*)getMoodLabel:(BOOL)abbrev;
-(NSString*)getFullDescription;
-(NSString*)getAbbrevDescription;
-(void) changeFormByDegrees:(NSInteger)degrees;
-(NSString*)stripAugmentFromPrincipalPart:(NSString*)stem;
-(NSString*)augmentStem:(NSString*)stem;

-(NSString*)accentRecessive:(NSString*)word;

@property NSInteger person;
@property NSInteger number;
@property NSInteger tense;
@property NSInteger voice;
@property NSInteger mood;
@property Verbs* verb;

@end
