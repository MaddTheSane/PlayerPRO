//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <Carbon/Carbon.h>

#define kMUSICLISTKEY @"Music List Key1"

// GetIndString isn't supported on 64-bit Mac OS X
// This code is emulation for GetIndString.
// Code taken from Mozilla's Mac Eudora importer
static StringPtr GetStringFromHandle(Handle aResource, ResourceIndex aId)
{
	if (!aResource)
		return NULL;
	UInt8 *data = *(UInt8**)aResource;
	UInt16 count = *(UInt16*)data;
	PPBE16(&count);
	
	// First 2 bytes are the count of string that this resource has.
	if (count < aId)
		return NULL;
	// skip count
	data += 2;
	
	// looking for data.  data is in order
	while (--aId > 0)
		data = data + (*(UInt8*)data) + 1;
	
	return data;
}

@implementation PPMusicList

- (void)loadMusicList:(NSMutableArray *)newArray
{
	NSMutableArray *oldList = musicList;
	musicList = [newArray retain];
	[oldList release];
}

-(OSErr)loadOldMusicListAtURL:(NSURL *)toOpen {
	NSMutableArray *newArray = [NSMutableArray array];
	ResFileRefNum refNum;
	Handle aHandle;
	FSRef theRef;
	StringPtr aStr, aStr2;
	UInt16 theNo, i;
	CFURLGetFSRef((CFURLRef)toOpen, &theRef);
	refNum = FSOpenResFile(&theRef, fsRdPerm);
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
	theNo = *((UInt16*)(*aHandle));          // number of musics...
	PPBE16(&theNo);
	
	theNo /= 2;
	
	for(i = 0; i > theNo * 2;i += 2) {
		aStr = GetStringFromHandle(aHandle, i);
		aStr2 = GetStringFromHandle(aHandle, i+1);
		CFStringRef CFaStr = NULL, CFaStr2 = NULL;
		CFaStr = CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman);
		CFaStr2 = CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, kCFStringEncodingMacRoman);

		NSString *together = [NSString stringWithFormat:@"%@:%@", CFaStr, CFaStr2];
		CFRelease(CFaStr);
		CFRelease(CFaStr2);
		CFURLRef tempURLRef = NULL;
		tempURLRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)together, kCFURLHFSPathStyle, false);
		//GetIndString( aStr, 128, i);
		//GetIndString(aStr2, 128, i+1);
		//pStrcat(aStr, "\p:");
		//pStrcat(aStr, aStr2);
		//P2CStr(aStr);
		//[self addMusicURL:[NSURL URLWithFileSystemPathHFSStyle:[NSString stringWithUTF8String:aStr]]];
		[newArray addObject:(id)tempURLRef];
		CFRelease(tempURLRef);
	}
	HUnlock( aHandle);
	DisposeHandle( aHandle);
	CloseResFile(refNum);
	[self loadMusicList:newArray];
}

- (void)loadMusicListAtURL:(NSURL *)fromURL
{
	
}

- (id)init
{
	self = [super init];
	if (self) {
		musicList = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[musicList release];
	
	[super dealloc];
}

- (void)addMusicURL:(NSURL *)musicToLoad
{
	[musicList addObject:musicToLoad];
}

- (void)removeObjectAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

- (NSURL*)URLAtIndex:(NSUInteger)index
{
	return [musicList objectAtIndex:index];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [musicList count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSTableCellView *view = [[NSTableCellView alloc] init];
	NSTextField *field = view.textField;
	[field setStringValue:[[musicList objectAtIndex:row] lastPathComponent]];
	//[field setAllowsEditingTextAttributes:NO];
	[field setEditable:NO];
	return [view autorelease];
}

#pragma mark Archiving

- (id)initWithCoder:(NSCoder *)decoder {
	
	if ((self = [super init])) 
	{
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
		NSInteger i = 0;
		musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
		for (i = 0; i < [BookmarkArray count]; i++) {
			[musicList insertObject:[NSURL URLByResolvingBookmarkData:[BookmarkArray objectAtIndex:i] options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:NULL error:nil]  atIndex:i];
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder {
	NSMutableArray *BookmarkArray = [NSMutableArray arrayWithCapacity:[musicList count]];
	NSInteger i = 0;
	for (i = 0; i < [musicList count]; i++)
	{
		[BookmarkArray insertObject:[[musicList objectAtIndex:i] bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:nil error:nil] atIndex:i];
	}
	[encoder encodeObject:BookmarkArray forKey:kMUSICLISTKEY];
}

#pragma mark Key-valued Coding

- (NSUInteger)countOfMusicList
{
	return [musicList count];
}

- (id)objectInMusicListAtIndex:(NSUInteger)idx
{
	return [musicList objectAtIndex:idx];
}

- (void)insertObject:(id)anObj inMusicListAtIndex:(NSUInteger)idx
{
	[musicList insertObject:anObj atIndex:idx];
}

- (void)removeObjectInMusicListAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

- (void)replaceObjectInMusicListAtIndex:(NSUInteger)index withObject:(id)anObject
{
	[musicList replaceObjectAtIndex:index withObject:anObject];
}


@end
