//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"
#include "RDriver.h"
#import "NDAlias/NDAlias.h"
#import "NDAlias/NSURL+NDCarbonUtilities.h"

#define kMUSICLISTKEY @"Music List Key1"
static inline void pStrcat(register unsigned char *s1, register unsigned char *s2)
{
	register unsigned char *p;
	register short len, i;
	
	if (*s1+*s2<=255) 
	{
		p = *s1 + s1 + 1;
		*s1 += (len = *s2++);
	}
	else 
	{
		*s1 = 255;
		p = s1 + 256 - (len = *s2++);
	}
	for (i=len; i; --i) *p++ = *s2++;
}

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

-(OSErr)openOldMusicList:(NSURL *)toOpen {
	UInt16 unused1;
	OSType unused2, fType;
	[toOpen finderInfoFlags:&unused1 type:&fType creator:&unused2];
	if(fType == 'STCf') {
		ResFileRefNum refNum;
		Handle aHandle;
		FSRef theRef;
		Str255 aStr, aStr2;
		short theNo, i;
		HFSUniStr255 ResForkName;
		FSGetResourceForkName(&ResForkName);
		[toOpen getFSRef:&theRef];
		FSOpenResourceFile(&theRef, ResForkName.length, ResForkName.unicode, fsRdPerm, &refNum);
		if (ResError()) {
			return ResError();
		}
		UseResFile(refNum);
		aHandle = Get1Resource( 'STR#', 128);
		if( aHandle == NULL)
		{
			CloseResFile( refNum);
			return ResError();
		}
		DetachResource( aHandle);
		
					
		HLock( aHandle);
		theNo = *((short*)(*aHandle));		// number of musics...
		HUnlock( aHandle);
		
		theNo /= 2;
		
		for(i = 1; i > theNo * 2;i += 2) {
			GetIndString( aStr, 128, i);
			GetIndString(aStr2, 128, i+1);
			pStrcat(aStr, "\p:");
			pStrcat(aStr, aStr2);
			P2CStr(aStr);
			[self addMusicURL:[NSURL URLWithFileSystemPathHFSStyle:[NSString stringWithUTF8String:aStr]]];
		}
		DisposeHandle( aHandle);
		CloseResFile(refNum);
			
	} else return MADIncompatibleFile;
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
