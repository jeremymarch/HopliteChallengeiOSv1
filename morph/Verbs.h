//
//  Verbs.h
//  morph
//
//  Created by Jeremy on 11/29/14.
//  Copyright (c) 2014 Jeremy March. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface Verbs : NSManagedObject

@property (nonatomic, retain) NSString * aorist;
@property (nonatomic, retain) NSString * aoristpassive;
@property (nonatomic, retain) NSString * def;
@property (nonatomic, retain) NSString * future;
@property (nonatomic, retain) NSNumber * hq;
@property (nonatomic, retain) NSNumber * id;
@property (nonatomic, retain) NSString * more;
@property (nonatomic, retain) NSString * perfectactive;
@property (nonatomic, retain) NSString * perfectmid;
@property (nonatomic, retain) NSString * present;
@property (nonatomic, retain) NSNumber * verbclass;

@end
