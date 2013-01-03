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
#import "ARCBridge.h"

#define kMUSICLISTKEY @"Music List Key1"

static NSString * const kMusicListKVO = @"musicList";

// GetIndString isn't supported on 64-bit Mac OS X
// This code is emulation for GetIndString.
// Code taken from Mozilla's Mac Eudora importer
static StringPtr GetStringFromHandle(Handle aResource, ResourceIndex aId)
{
	long handSize = GetHandleSize(aResource);
	long curSize = 2;
	
	if (!aResource)
		return NULL;
	UInt8 *data = *(UInt8**)aResource;
	UInt16 count = *(UInt16*)data;
	PPBE16(&count);
	
	// First 2 bytes are the count of strings that this resource has.
	if (count < aId)
		return NULL;
	// skip count
	data += 2;
	
	// looking for data.  data is in order
	while (--aId >= 0)
	{
		short toAdd = (*(UInt8*)data) + 1;
		curSize += toAdd;
		if (curSize >= handSize) {
			return NULL;
		}
		data = data + toAdd;
	}
	
	return data;
}

/*
static NSInteger SortUsingFileName(id rhs, id lhs, void *unused)
{
	NSString *rhsString = [rhs fileName];
	NSString *lhsString = [lhs fileName];
	return [rhsString localizedStandardCompare:lhsString];
}
 */

@implementation PPMusicListObject

@synthesize musicUrl;

- (void)setMusicUrl:(NSURL *)amusicUrl
{
#if __has_feature(objc_arc)
	musicUrl = amusicUrl;
	
	[self willChangeValueForKey:@"fileName"];
	//fileName = [[musicUrl lastPathComponent] copy];
	[self didChangeValueForKey:@"fileName"];
#else
	NSURL *tempUrl = musicUrl;
	musicUrl = [amusicUrl retain];
	
	[self willChangeValueForKey:@"fileName"];
	//fileName = [[musicUrl lastPathComponent] copy];
	[self didChangeValueForKey:@"fileName"];
	
	if (tempUrl != nil) {
		[tempUrl release];
	}
#endif
}

- (NSString *)fileName
{
	id val = nil;
	NSError *err = nil;
	if([musicUrl getResourceValue:&val forKey:NSURLHasHiddenExtensionKey error:&err] == NO)
	{
		NSLog(@"PPMusicListObject: Could not find out if extension is hidden, error: %@", [err localizedDescription]);
		return [musicUrl lastPathComponent];
	} else {
		if ([val boolValue]) {
			return [[musicUrl lastPathComponent] stringByDeletingPathExtension];
		} else {
			return [musicUrl lastPathComponent];
		}
	}
}

- (id)initWithURL:(NSURL *)aURL
{
	if (self = [super init]) {
		[self willChangeValueForKey:@"fileName"];
		[self willChangeValueForKey:@"musicUrl"];
		musicUrl = RETAINOBJ(aURL);
		[self didChangeValueForKey:@"fileName"];
		[self didChangeValueForKey:@"musicUrl"];
	}
	return self;
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[musicUrl release];
	
	[super dealloc];
}
#endif

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@ - %@", [musicUrl description], self.fileName];
}

@end

@implementation PPMusicList

- (NSString *)description
{
	return [NSString stringWithFormat:@"Size: %ld Contents: %@", (long)[musicList count], [musicList description]];
}

- (void)saveMusicListToPreferences
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:[NSKeyedArchiver archivedDataWithRootObject:self] forKey:PPMMusicList];
}

- (BOOL)saveMusicListToURL:(NSURL *)toSave
{
	NSData *theList = [NSKeyedArchiver archivedDataWithRootObject:self];
	return [theList writeToURL:toSave atomically:YES];
}

- (void)sortMusicList
{
	//[musicList sortUsingFunction:SortUsingFileName context:NULL];
	[self willChangeValueForKey:kMusicListKVO];
	@autoreleasepool {
		[musicList sortUsingComparator:^(id rhs, id lhs) {
			NSString *rhsString = [rhs fileName];
			NSString *lhsString = [lhs fileName];
			NSComparisonResult result = [rhsString localizedStandardCompare:lhsString];
			return result;
		}];
	}
	[self didChangeValueForKey:kMusicListKVO];
}

- (void)loadMusicList:(NSMutableArray *)newArray
{
#if __has_feature(objc_arc)
	[self willChangeValueForKey:kMusicListKVO];
	musicList = newArray;
	[self didChangeValueForKey:kMusicListKVO];
#else
	NSMutableArray *oldList = musicList;
	[self willChangeValueForKey:kMusicListKVO];
	musicList = [newArray retain];
	[self didChangeValueForKey:kMusicListKVO];
	[oldList release];
#endif
}

- (void)clearMusicList
{
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, ([musicList count] - 1))];
	[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList removeAllObjects];
	[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
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
	
	
	NSMutableArray *musicArray = [[NSMutableArray alloc] init];
	
	for (i = 0; i < [preList countOfMusicList]; i++) {
		[musicArray insertObject:[preList objectInMusicListAtIndex:i] atIndex:i];
	}
	
	[self loadMusicList:musicArray];
	RELEASEOBJ(musicArray);
}

- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen
{
	ResFileRefNum refNum;
	Handle aHandle;
	FSRef theRef;
	StringPtr aStr, aStr2;
	UInt16 theNo, i;
	CFURLGetFSRef(BRIDGE(CFURLRef, toOpen), &theRef);
	refNum = FSOpenResFile(&theRef, fsRdPerm);
	OSErr resErr = ResError();
	if (resErr) {
		return resErr;
	}
	UseResFile(refNum);
	aHandle = Get1Resource( 'STR#', 128);
	if( aHandle == NULL)
	{
		CloseResFile( refNum);
		return ResError();
	}
	DetachResource( aHandle);
	CloseResFile(refNum);
	
	HLock( aHandle);
	theNo = *((UInt16*)(*aHandle));          // number of musics...
	PPBE16(&theNo);
	
	theNo /= 2;
	
	NSMutableArray *newArray = [[NSMutableArray alloc] init];
	
	for(i = 0; i < theNo * 2; i += 2) {
		aStr = GetStringFromHandle(aHandle, i);
		aStr2 = GetStringFromHandle(aHandle, i + 1);
		if (!aStr || !aStr2) {
			break;
		}
		CFStringRef CFaStr = NULL, CFaStr2 = NULL;
		CFaStr = CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman);
		CFaStr2 = CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, kCFStringEncodingMacRoman);

		CFMutableArrayRef CFaStrArray = CFArrayCreateMutable(kCFAllocatorDefault, 2, &kCFTypeArrayCallBacks);
		
		CFArrayAppendValue(CFaStrArray, CFaStr);
		CFRelease(CFaStr);
		CFArrayAppendValue(CFaStrArray, CFaStr2);
		CFRelease(CFaStr2);

		CFStringRef together = CFStringCreateByCombiningStrings(kCFAllocatorDefault, CFaStrArray, CFSTR(":"));
		CFRelease(CFaStrArray);
		//NSString *together = [NSString stringWithFormat:@"%@:%@", BRIDGE(NSString*, CFaStr), BRIDGE(NSString*, CFaStr2)];
		NSURL *fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, together, kCFURLHFSPathStyle, false));
		CFRelease(together);
		NSURL *refURL = [fullPath fileReferenceURL];
		
		PPMusicListObject *obj = nil;
		if (refURL) {
			obj = [[PPMusicListObject alloc] initWithURL:refURL];
		} else {
			obj = [[PPMusicListObject alloc] initWithURL:fullPath];
		}
		
		[newArray addObject:obj];
		RELEASEOBJ(obj);
	}
	HUnlock( aHandle);
	DisposeHandle( aHandle);
	[self loadMusicList:newArray];
	RELEASEOBJ(newArray);

	return noErr;
}

- (BOOL)loadMusicListAtURL:(NSURL *)fromURL
{
	NSInteger i = 0;
	NSData *listData = [NSData dataWithContentsOfURL:fromURL];
	if (!listData) {
		return NO;
	}
	PPMusicList *preList = [NSKeyedUnarchiver unarchiveObjectWithData:listData];
	if (!preList) {
		return NO;
	}
	//[musicList removeAllObjects];
	NSMutableArray *newArray = [[NSMutableArray alloc] init];
	for (i = 0; i < [preList countOfMusicList]; i++) {
		[newArray insertObject:[preList objectInMusicListAtIndex:i] atIndex:i];
	}
	[self loadMusicList:newArray];
	RELEASEOBJ(newArray);
	return YES;
}

- (id)init
{
	self = [super init];
	if (self) {
		musicList = [[NSMutableArray alloc] init];
	}
	return self;
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[musicList release];
	
	[super dealloc];
}
#endif

- (void)addMusicURL:(NSURL *)musicToLoad
{
	PPMusicListObject *obj = nil;
	NSURL *fileRef = [musicToLoad fileReferenceURL];
	if (fileRef) {
		obj = [[PPMusicListObject alloc] initWithURL:fileRef];
	} else {
		obj = [[PPMusicListObject alloc] initWithURL:musicToLoad];
	}
	//[self willChangeValueForKey:kMusicListKVO];
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndex:[musicList count]];
	[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList addObject:obj];
	[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	//[self didChangeValueForKey:kMusicListKVO];
	RELEASEOBJ(obj);
}

- (void)removeObjectAtIndex:(NSUInteger)object
{
	[self removeObjectInMusicListAtIndex:object];
}

- (NSURL*)URLAtIndex:(NSUInteger)index
{
	return [[musicList objectAtIndex:index] musicUrl];
}

#pragma mark Archiving

- (id)initWithCoder:(NSCoder *)decoder
{
	if ((self = [super init])) 
	{
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
		if (!BookmarkArray) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		NSInteger i = 0;
		musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
		for (i = 0; i < [BookmarkArray count]; i++) {
			BOOL isStale = NO;
			NSURL *fullURL = [NSURL URLByResolvingBookmarkData:[BookmarkArray objectAtIndex:i] options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:&isStale error:nil];
#ifdef DEBUG
			if (isStale) {
				NSLog(@"Bookmark %@ is stale", [fullURL path]);
			}
#endif
			NSURL *refURL = [fullURL fileReferenceURL];
			PPMusicListObject *obj = nil;
			if (refURL) {
				obj = [[PPMusicListObject alloc] initWithURL:refURL];
			} else {
				obj = [[PPMusicListObject alloc] initWithURL:fullURL];
			}
			[musicList insertObject:obj atIndex:i];
			RELEASEOBJ(obj);
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
	NSMutableArray *BookmarkArray = [NSMutableArray arrayWithCapacity:[musicList count]];
	NSInteger i = 0;
	for (i = 0; i < [musicList count]; i++)
	{
		NSData *bookData = [[[musicList objectAtIndex:i] musicUrl] bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:nil error:nil];
		if (bookData) {
			[BookmarkArray addObject:bookData];
		}
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
