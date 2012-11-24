//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPMusicList.h"
#import "UserDefaultKeys.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <CoreServices/CoreServices.h>

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

/*
static NSInteger SortUsingLastPathComponent(id rhs, id lhs, void *unused)
{
	NSString *rhsString = [rhs lastPathComponent];
	NSString *lhsString = [lhs lastPathComponent];
	return [rhsString compare:lhsString];
}
 */

@implementation PPMusicListObject

- (void)setMusicUrl:(NSURL *)amusicUrl
{
	NSURL *tempUrl = musicUrl;
	NSString *tempString = fileName;
	musicUrl = [amusicUrl copy];
	fileName = [[musicUrl lastPathComponent] copy];
	
	if (tempUrl != nil) {
		[tempUrl release];
	}
	if (tempString != nil) {
		[tempString release];
	}
}

@synthesize fileName;
@synthesize musicUrl;

- (id)initWithURL:(NSURL *)aURL
{
	if (self = [super init]) {
		musicUrl = [aURL copy];
		fileName = [[musicUrl lastPathComponent] copy];
	}
	return self;
}

- (void)dealloc
{
	[musicUrl release];
	[fileName release];
	
	[super dealloc];
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@ - %@", musicUrl, fileName];
}


@end


@implementation PPMusicList

- (void)saveMusicListToPreferences
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSKeyedArchiver archivedDataWithRootObject:self] forKey:PPMMusicList];

}

- (void)saveMusicListToURL:(NSURL *)toSave
{
	NSData *theList = [NSKeyedArchiver archivedDataWithRootObject:self];
	[theList writeToURL:toSave atomically:YES];
}

- (void)sortMusicList
{
	//[musicList sortUsingFunction:SortUsingLastPathComponent context:NULL];
	[musicList sortUsingComparator:^(id rhs, id lhs) {
		NSString *rhsString = [rhs fileName];
		NSString *lhsString = [lhs fileName];
		return [rhsString compare:lhsString];
	}];
}

- (void)loadMusicList:(NSMutableArray *)newArray
{
	NSMutableArray *oldList = musicList;
	musicList = [newArray retain];
	[oldList release];
}

- (void)loadMusicListFromPreferences
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSData *listData = [defaults dataForKey:PPMMusicList];
	PPMusicList *preList = [NSKeyedUnarchiver unarchiveObjectWithData:listData];
	NSInteger i = 0;
	NSAssert([self countOfMusicList] == 0, @"Music list should be empty!");
	//[musicList removeAllObjects];

	/*for (i = 0; i < [preList countOfMusicList]; i++) {
		[self insertObject:[preList objectInMusicListAtIndex:i] inMusicListAtIndex:i];
	}*/
	
	
	NSMutableArray *musicArray = [NSMutableArray array];
	
	for (i = 0; i < [preList countOfMusicList]; i++) {
		[musicArray insertObject:[preList objectInMusicListAtIndex:i] atIndex:i];
	}
	
	[self loadMusicList:musicArray];
	 
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

		PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:(id)tempURLRef];
		
		[newArray addObject:obj];
		[obj release];
		CFRelease(tempURLRef);
	}
	HUnlock( aHandle);
	DisposeHandle( aHandle);
	CloseResFile(refNum);
	[self loadMusicList:newArray];
	return noErr;
}

- (void)loadMusicListAtURL:(NSURL *)fromURL
{
	NSString *filename = [fromURL path];
	NSError *err = nil;
	NSString *uti = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
	NSInteger i = 0;
	if ([uti isEqualToString:@"net.sourceforge.playerpro.stcfmusiclist"]) {
		[self loadOldMusicListAtURL:fromURL];
		return;
	}
	NSData *listData = [NSData dataWithContentsOfURL:fromURL];
	PPMusicList *preList= [NSKeyedUnarchiver unarchiveObjectWithData:listData];
	//[musicList removeAllObjects];
	NSMutableArray *newArray = [NSMutableArray array];
	for (i = 0; i < [preList countOfMusicList]; i++) {
		[newArray insertObject:[preList objectInMusicListAtIndex:i] atIndex:i];
	}
	[self loadMusicList:newArray];
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
	PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:musicToLoad];
	[musicList addObject:obj];
	[obj release];
}

- (void)removeObjectAtIndex:(NSUInteger)object
{
	[musicList removeObjectAtIndex:object];
}

- (NSURL*)URLAtIndex:(NSUInteger)index
{
	return [[musicList objectAtIndex:index] musicUrl];
}

#pragma mark Archiving

- (id)initWithCoder:(NSCoder *)decoder {
	
	if ((self = [super init])) 
	{
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
		NSInteger i = 0;
		musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
		for (i = 0; i < [BookmarkArray count]; i++) {
			NSURL *fullURL = [NSURL URLByResolvingBookmarkData:[BookmarkArray objectAtIndex:i] options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:NULL error:nil];
			PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:fullURL];
			[musicList insertObject:obj atIndex:i];
			[obj release];
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder {
	NSMutableArray *BookmarkArray = [NSMutableArray arrayWithCapacity:[musicList count]];
	NSInteger i = 0;
	for (i = 0; i < [musicList count]; i++)
	{
		[BookmarkArray insertObject:[[[musicList objectAtIndex:i] musicUrl] bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:nil error:nil] atIndex:i];
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
