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
#if !TARGET_OS_IPHONE
#include <CoreServices/CoreServices.h>
#endif

#define kMUSICLISTKEY @"Music List Key1"
#define kMUSICLISTKEY2 @"Music List Key2"
#define kMusicListLocation2 @"Music Key Location2"

#define kMusicListKey3 @"Music List Key 3"
#define kMusicListLocation3 @"Music Key Location 3"

#if !TARGET_OS_IPHONE
// GetIndString isn't supported on 64-bit Mac OS X
// This code is emulation for GetIndString.
// Code based on Mozilla's Mac Eudora importer

// Ssh, we're using deprecated functions.
// We're using them because there is no easy replacement
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
static StringPtr GetStringFromHandle(Handle aResource, ResourceIndex aId)
{
	Size handSize = GetHandleSize(aResource);
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
#pragma clang diagnostic pop
#endif

static inline NSURL *PPHomeURL()
{
	static NSURL *homeURL;
	if (homeURL == nil) {
		homeURL = [NSURL fileURLWithPath:NSHomeDirectory() isDirectory:YES];
	}
	return homeURL;
}

static inline NSURL *GenerateFileReferenceURLFromURLIfPossible(NSURL *otherURL)
{
	if ([otherURL isFileReferenceURL])
		return otherURL;
	
	NSURL *tmpURL = [otherURL fileReferenceURL];
	return tmpURL ? tmpURL : otherURL;
}

@interface PPMusicListObject ()
@property (strong, readwrite, setter = setTheMusicUrl:) NSURL *musicUrl;
@end

@implementation PPMusicListObject
@synthesize musicUrl;

- (BOOL)isEqual:(id)object
{
	if (!object)
		return NO;
	
	if (self == object)
		return YES;
	
	if ([object isKindOfClass:[PPMusicListObject class]]) {
		id dat1, dat2;
		BOOL bothareValid = YES;
		BOOL theSame = NO;
		if (![musicUrl getResourceValue:&dat1 forKey:NSURLFileResourceIdentifierKey error:NULL]) {
			bothareValid = NO;
		}
		if (![[object musicUrl] getResourceValue:&dat2 forKey:NSURLFileResourceIdentifierKey error:NULL]) {
			bothareValid = NO;
		}
		if (bothareValid) {
			theSame = [dat1 isEqual:dat2];
		}
		return theSame;
	} else if ([object isKindOfClass:[NSURL class]]) {
		id dat1, dat2;
		BOOL bothareValid = YES;
		BOOL theSame = NO;
		if (![musicUrl getResourceValue:&dat1 forKey:NSURLFileResourceIdentifierKey error:NULL]) {
			bothareValid = NO;
		} else if (![object getResourceValue:&dat2 forKey:NSURLFileResourceIdentifierKey error:NULL]) {
			bothareValid = NO;
		}
		if (bothareValid) {
			theSame = [dat1 isEqual:dat2];
		}
		return theSame;
	} else
		return NO;
}

- (NSUInteger)hash
{
	return [[[[musicUrl filePathURL] absoluteURL] path] hash];
}

- (NSString *)fileName
{
	NSString *val;
	NSError *err;

	if ([musicUrl getResourceValue:&val forKey:NSURLLocalizedNameKey error:&err] == NO) {
		NSLog(@"PPMusicListObject: Could not find out if extension is hidden in file \"%@\", error: %@", [musicUrl path], [err localizedDescription]);
		return [musicUrl lastPathComponent];
	} else {
		return val;
	}
}

#if !TARGET_OS_IPHONE
- (NSImage *)fileIcon
{
	NSImage *image = [[NSWorkspace sharedWorkspace] iconForFile:[musicUrl path]];
	[image setSize:NSMakeSize(16, 16)];
	return image;
}
#endif

- (instancetype)initWithURL:(NSURL *)aURL
{
	if (self = [super init]) {
		if (!aURL) {
			return nil;
		}
		self.musicUrl = aURL;
	}
	return self;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@:%@ - %@", [musicUrl description], [musicUrl path], self.fileName];
}

- (id)copyWithZone:(NSZone *)zone
{
	return self; // this class is immutable
}

@end

@interface PPMusicList ()
@property (readonly) NSMutableArray *theMusicList;
@end

@implementation PPMusicList

@synthesize theMusicList = musicList;
@synthesize lostMusicCount;
@synthesize selectedMusic;

- (NSString *)description
{
	return [NSString stringWithFormat:@"Size: %ld, selection: %ld, Contents: %@", (long)[musicList count], (long)selectedMusic, [musicList description]];
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

- (void)sortMusicListByName
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortWithOptions:(NSSortConcurrent | NSSortStable) usingComparator:^(id rhs, id lhs) {
		@autoreleasepool {
			NSString *rhsString = [rhs fileName];
			NSString *lhsString = [lhs fileName];
			NSComparisonResult result = [rhsString localizedStandardCompare:lhsString];
			return result;
		}
	}];
	[self didChangeValueForKey:kMusicListKVO];
}

- (void)sortMusicList
{
	[self sortMusicListByName];
}

- (void)loadMusicList:(NSArray *)newArray
{
	[self willChangeValueForKey:kMusicListKVO];
	musicList = [newArray mutableCopy];
	[self didChangeValueForKey:kMusicListKVO];
}

- (void)clearMusicList
{
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [musicList count])];
	[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList removeAllObjects];
	[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
}

- (BOOL)loadMusicListFromData:(NSData *)theDat
{
	PPMusicList *preList = [NSKeyedUnarchiver unarchiveObjectWithData:theDat];
	if (!preList)
		return NO;
	
	lostMusicCount = preList.lostMusicCount;
	[self loadMusicList:preList.theMusicList];
	self.selectedMusic = preList.selectedMusic;
	return YES;
}

- (void)loadMusicListFromPreferences
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSData *listData = [defaults dataForKey:PPMMusicList];
	NSAssert([self countOfMusicList] == 0, @"Music list should be empty!");
	[self loadMusicListFromData:listData];
}


#if !TARGET_OS_IPHONE
// Ssh, we're using deprecated functions.
// We're using them because there is no easy replacement
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen
{
	lostMusicCount = 0;
	ResFileRefNum refNum;
	Handle aHandle, locHand;
	FSRef theRef;
	UInt16 theNo, i;
	CFURLGetFSRef((__bridge CFURLRef)toOpen, &theRef);
	refNum = FSOpenResFile(&theRef, fsRdPerm);
	OSErr resErr = ResError();
	if (resErr) {
		return resErr;
	}
	UseResFile(refNum);
	aHandle = Get1Resource('STR#', 128);
	if (aHandle == NULL)
	{
		CloseResFile(refNum);
		return ResError();
	}
	locHand = Get1Resource('selc', 128);
	if (locHand == NULL) {
		CloseResFile(refNum);
		return ResError();
	}
	DetachResource(aHandle);
	DetachResource(locHand);
	CloseResFile(refNum);
	
	HLock(aHandle);
	theNo = *((UInt16*)(*aHandle));          // number of musics...
	PPBE16(&theNo);
	
	theNo /= 2;
	
	HLock(locHand);
	short location = **((short**)locHand);
	PPBE16(&location);
	HUnlock(locHand);
	DisposeHandle(locHand);
	
	NSMutableArray *newArray = [[NSMutableArray alloc] initWithCapacity:theNo];
	
	for(i = 0; i < theNo * 2; i += 2) {
		StringPtr aStr, aStr2;
		aStr = GetStringFromHandle(aHandle, i);
		aStr2 = GetStringFromHandle(aHandle, i + 1);
		if (!aStr || !aStr2) {
			break;
		}
		NSString *CFaStr = nil, *CFaStr2 = nil;
		CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman));
		CFaStr2 = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, kCFStringEncodingMacRoman));

		NSString *together = [@[CFaStr, CFaStr2] componentsJoinedByString:@":"];
		CFaStr = CFaStr2 = nil;
		
		NSURL *fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef) together, kCFURLHFSPathStyle, false));
		together = nil;
		BOOL validPath = [[NSFileManager defaultManager] fileExistsAtPath:[fullPath path]];
		if (validPath) {
			PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:GenerateFileReferenceURLFromURLIfPossible(fullPath)];
			[newArray addObject:obj];
		} else {
			if (location != -1 && location == (i / 2)) {
				location = -1;
			} else if (location != -1 && location > (i / 2)) {
				location--;
			}
			lostMusicCount++;
		}
	}
	HUnlock(aHandle);
	DisposeHandle(aHandle);
	
	self.selectedMusic = (location >= [newArray count]) ? location : -1;
	
	[self loadMusicList:newArray];

	return noErr;
}
#pragma clang diagnostic pop
#endif

- (BOOL)loadMusicListAtURL:(NSURL *)fromURL
{
	NSData *listData = [NSData dataWithContentsOfURL:fromURL];
	if (!listData) {
		return NO;
	}
	return [self loadMusicListFromData:listData];
}

- (instancetype)init
{
	if (self = [super init]) {
		musicList = [[NSMutableArray alloc] init];
		lostMusicCount = 0;
		selectedMusic = -1;
	}
	return self;
}

- (BOOL)addMusicURL:(NSURL *)musicToLoad
{
	PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:GenerateFileReferenceURLFromURLIfPossible(musicToLoad)];
	
	if (!obj)
		return NO;
	
	if ([musicList containsObject:obj])
		return NO;
	
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndex:[musicList count]];
	[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList addObject:obj];
	[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	return YES;
}

- (NSInteger)indexOfObjectSimilarToURL:(NSURL*)theURL
{
	return [musicList indexOfObject:theURL];
}

- (void)removeObjectAtIndex:(NSUInteger)object
{
	[self removeObjectInMusicListAtIndex:object];
}

- (NSURL*)URLAtIndex:(NSUInteger)index
{
	return [musicList[index] musicUrl];
}

- (void)insertObjects:(NSArray*)anObj inMusicListAtIndex:(NSUInteger)idx
{
	NSIndexSet *theIndexSet = [[NSIndexSet alloc] initWithIndexesInRange:NSMakeRange(idx, [anObj count])];
	[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndexSet forKey:kMusicListKVO];
	[musicList insertObjects:anObj atIndexes:theIndexSet];
	[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndexSet forKey:kMusicListKVO];
}

#pragma mark Fast Enumeration

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [musicList countByEnumeratingWithState:state objects:buffer count:len];
}

#pragma mark Archiving

- (id)initWithCoder:(NSCoder *)decoder
{
	if ((self = [super init]))  {
		lostMusicCount = 0;
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMusicListKey3];
		if (!BookmarkArray) {
			BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY2];
			if (!BookmarkArray) {
				BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
				if (!BookmarkArray)
					return nil;
				
				musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				for (NSData *bookData in BookmarkArray) {
					BOOL isStale = NO;
					NSURL *fullURL = [NSURL URLByResolvingBookmarkData:bookData options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:&isStale error:nil];
#ifdef DEBUG
					if (isStale) {
						NSLog(@"Bookmark pointing to %@ is stale", [fullURL path]);
					}
#endif
					if (!fullURL) {
						lostMusicCount++;
						continue;
					}
					PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:GenerateFileReferenceURLFromURLIfPossible(fullURL)];
					[musicList addObject:obj];
				}
				selectedMusic = -1;
			} else {
				NSNumber *curSel;
				if ((curSel = [decoder decodeObjectForKey:kMusicListLocation2])) {
					selectedMusic = [curSel integerValue];
				} else
					selectedMusic = -1;
				musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				for (NSData *bookData in BookmarkArray) {
					BOOL isStale = NO;
					NSURL *fullURL = [NSURL URLByResolvingBookmarkData:bookData options:NSURLBookmarkResolutionWithoutUI relativeToURL:PPHomeURL() bookmarkDataIsStale:&isStale error:nil];
#ifdef DEBUG
					if (isStale) {
						NSLog(@"Bookmark pointing to %@ is stale.", [fullURL path]);
					}
#endif
					if (!fullURL) {
						if (selectedMusic == -1) {
							//Do nothing
						} else if (selectedMusic == [musicList count] + 1) {
							selectedMusic = -1;
						} else if (selectedMusic > [musicList count] + 1) {
							selectedMusic--;
						}
						lostMusicCount++;
						continue;
					}
					PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:GenerateFileReferenceURLFromURLIfPossible(fullURL)];
					[musicList addObject:obj];
				}
			}
		} else {
			selectedMusic = [decoder decodeIntegerForKey:kMusicListLocation3];
			musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
			for (NSURL *bookURL in BookmarkArray) {
				if (![bookURL checkResourceIsReachableAndReturnError:NULL]) {
					if (selectedMusic == -1) {
						//Do nothing
					} else if (selectedMusic == [musicList count] + 1) {
						selectedMusic = -1;
					} else if (selectedMusic > [musicList count] + 1) {
						selectedMusic--;
					}
					lostMusicCount++;
					continue;
				}
				PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:GenerateFileReferenceURLFromURLIfPossible(bookURL)];
				[musicList addObject:obj];
			}
			
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
	NSMutableArray *BookmarkArray = [[NSMutableArray alloc] initWithCapacity:[musicList count]];
	for (PPMusicListObject *obj in musicList) {
		NSURL *bookData = obj.musicUrl;
		if (bookData)
			[BookmarkArray addObject:bookData];
	}
	//TODO: check for failed data initialization, and decrement changedIndex to match.
	[encoder encodeInteger:selectedMusic forKey:kMusicListLocation3];
	[encoder encodeObject:BookmarkArray forKey:kMusicListKey3];
}

#pragma mark Key-valued Coding

- (void)addMusicListObject:(PPMusicListObject*)obj
{
	if (![musicList containsObject:obj]) {
		[musicList addObject:obj];
	}
}

- (NSUInteger)countOfMusicList
{
	return [musicList count];
}

- (NSArray*)arrayOfObjectsInMusicListAtIndexes:(NSIndexSet*)theSet
{
	return [musicList objectsAtIndexes:theSet];
}

- (void)removeObjectsInMusicListAtIndexes:(NSIndexSet *)set
{
	if ([set containsIndex:selectedMusic]) {
		self.selectedMusic = -1;
	}
	[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:set forKey:kMusicListKVO];
	[musicList removeObjectsAtIndexes:set];
	[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:set forKey:kMusicListKVO];
}

- (id)objectInMusicListAtIndex:(NSUInteger)idx
{
	return musicList[idx];
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
	musicList[index] = anObject;
}

@end
