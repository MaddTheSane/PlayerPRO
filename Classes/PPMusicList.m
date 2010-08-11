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
