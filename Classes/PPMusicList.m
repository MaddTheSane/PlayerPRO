//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"

#define kMUSICLISTKEY @"MusicListKey"

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
	[musicList addObject:[[[PPAlias alloc] initWithURL:musicToLoad] autorelease]];
}

-(void)removeObjectAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

-(NSURL*)URLAtIndex:(NSUInteger)index
{
	return [[musicList objectAtIndex:index] url];
}

#pragma mark Archiving

-(id)initWithCoder:(NSCoder *)decoder {
	
	if ((self = [super init])) 
	{
		musicList = [[decoder decodeObjectForKey:kMUSICLISTKEY] retain];
	}
	return self;
}

-(void)encodeWithCoder:(NSCoder *)encoder {
	[encoder encodeObject:musicList forKey:kMUSICLISTKEY];
}


@end
