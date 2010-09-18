//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"
#import "NDAlias/NDAlias.h"

#define kMUSICLISTKEY @"Music List Key1"

@implementation PPMusicList

-(id)init
{
	self = [super init];
	if (self) {
		musicList = [[NSMutableArray alloc] init];
	}
	return self;
}

-(void)dealloc
{
	[musicList release];
	
	[super dealloc];
}

-(void)addMusicURL:(NSURL *)musicToLoad
{
	[musicList addObject:[NDAlias aliasWithURL:musicToLoad]];
}

-(void)removeObjectAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

-(NSURL*)URLAtIndex:(NSUInteger)index
{
	return [[musicList objectAtIndex:index] URL];
}

#pragma mark Archiving

-(id)initWithCoder:(NSCoder *)decoder {
	
	if ((self = [super init])) 
	{
		if ([decoder allowsKeyedCoding]) {
			musicList = [[decoder decodeObjectForKey:kMUSICLISTKEY] retain];
		} else {
			musicList = [[decoder decodeObject] retain];
		}
	}
	return self;
}

-(void)encodeWithCoder:(NSCoder *)encoder {
	if ([encoder allowsKeyedCoding]) {
		[encoder encodeObject:musicList forKey:kMUSICLISTKEY];
	} else {
		[encoder encodeObject:musicList];
	}
}


@end
