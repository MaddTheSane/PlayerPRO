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
#define kMUSICLISTKEY2 @"Music List Key2"

// GetIndString isn't supported on 64-bit Mac OS X
// This code is emulation for GetIndString.
// Code based on Mozilla's Mac Eudora importer
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

@implementation PPMusicListObject

@synthesize musicUrl;

#if 0
- (void)setMusicUrl:(NSURL *)amusicUrl
{
	if (amusicUrl == nil) {
		return;
	}

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
	
	[tempUrl release];
#endif
}
#endif

- (BOOL)isEqual:(id)object
{
	if (!object) {
		return NO;
	}
	if (self == object) {
		return YES;
	}
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
		}
		if (![object getResourceValue:&dat2 forKey:NSURLFileResourceIdentifierKey error:NULL]) {
			bothareValid = NO;
		}
		if (bothareValid) {
			theSame = [dat1 isEqual:dat2];
		}
		return theSame;
	} else return NO;
}

- (NSUInteger)hash
{
	@autoreleasepool {
		return [[[[musicUrl filePathURL] absoluteURL] path] hash];
	}
}

- (NSString *)fileName
{
	id val = nil;
	NSError *err = nil;

	if([musicUrl getResourceValue:&val forKey:NSURLLocalizedNameKey error:&err] == NO)
	{
		NSLog(@"PPMusicListObject: Could not find out if extension is hidden in file \"%@\", error: %@", [musicUrl path], [err localizedDescription]);
		return [musicUrl lastPathComponent];
	} else {
		return val;
	}
}

- (id)initWithURL:(NSURL *)aURL
{
	if (self = [super init]) {
		if (!aURL) {
			AUTORELEASEOBJNORETURN(self);
			return nil;
		}
		musicUrl = RETAINOBJ(aURL);
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
	return [NSString stringWithFormat:@"%@:%@ - %@", [musicUrl description], [musicUrl path], self.fileName];
}

@end

@interface PPMusicList ()

@property (readonly) NSMutableArray *theMusicList;

@end

@implementation PPMusicList

@synthesize theMusicList = musicList;
@synthesize lostMusicCount;

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
	[musicList sortUsingComparator:^(id rhs, id lhs) {
		@autoreleasepool {
			NSString *rhsString = [rhs fileName];
			NSString *lhsString = [lhs fileName];
			NSComparisonResult result = [rhsString localizedStandardCompare:lhsString];
			return result;
		}
	}];
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
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, ([musicList count]))];
	[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList removeAllObjects];
	[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:theIndex forKey:kMusicListKVO];
}

- (BOOL)loadMusicListFromData:(NSData *)theDat
{
	PPMusicList *preList = [NSKeyedUnarchiver unarchiveObjectWithData:theDat];
	if (!preList) {
		return NO;
	}
	
	lostMusicCount = preList.lostMusicCount;
	[self loadMusicList:preList.theMusicList];
	return YES;
}

- (void)loadMusicListFromPreferences
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSData *listData = [defaults dataForKey:PPMMusicList];
	NSAssert([self countOfMusicList] == 0, @"Music list should be empty!");
	[self loadMusicListFromData:listData];
}

- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen
{
	lostMusicCount = 0;
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
		
		NSURL *fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, together, kCFURLHFSPathStyle, false));
		BOOL validPath = [[NSFileManager defaultManager] fileExistsAtPath:[fullPath path]];
		CFRelease(together);
		NSURL *refURL = [fullPath fileReferenceURL];
		if (validPath) {
			PPMusicListObject *obj = nil;
			if (refURL) {
				obj = [[PPMusicListObject alloc] initWithURL:refURL];
			} else {
				obj = [[PPMusicListObject alloc] initWithURL:fullPath];
			}
			
			[newArray addObject:obj];
			RELEASEOBJ(obj);
		} else lostMusicCount++;
		
	}
	HUnlock( aHandle);
	DisposeHandle( aHandle);
	[self loadMusicList:newArray];
	RELEASEOBJ(newArray);

	return noErr;
}

- (BOOL)loadMusicListAtURL:(NSURL *)fromURL
{
	NSData *listData = [NSData dataWithContentsOfURL:fromURL];
	if (!listData) {
		return NO;
	}
	return [self loadMusicListFromData:listData];
}

- (id)init
{
	self = [super init];
	if (self) {
		musicList = [[NSMutableArray alloc] init];
		lostMusicCount = 0;
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

- (BOOL)addMusicURL:(NSURL *)musicToLoad
{
	PPMusicListObject *obj = nil;
	NSURL *fileRef = [musicToLoad fileReferenceURL];
	if (fileRef) {
		obj = [[PPMusicListObject alloc] initWithURL:fileRef];
	} else {
		obj = [[PPMusicListObject alloc] initWithURL:musicToLoad];
	}
	
	if (!obj) {
		return NO;
	}
	if ([musicList containsObject:obj]) {
		RELEASEOBJ(obj);
		return NO;
	}
	
	//[self willChangeValueForKey:kMusicListKVO];
	NSIndexSet *theIndex = [NSIndexSet indexSetWithIndex:[musicList count]];
	[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	[musicList addObject:obj];
	[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:theIndex forKey:kMusicListKVO];
	//[self didChangeValueForKey:kMusicListKVO];
	RELEASEOBJ(obj);
	return YES;
}

- (NSInteger)indexOfObjectSimilarToURL:(NSURL*)theURL
{
	NSInteger i;
	for (i = 0; i < [musicList count]; i++) {
		if ([[musicList objectAtIndex:i] isEqual:theURL]) {
			return i;
			break;
		}
	}
	return NSNotFound;
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
		lostMusicCount = 0;
		NSMutableArray *BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY2];
		if (!BookmarkArray) {
			BookmarkArray = [decoder decodeObjectForKey:kMUSICLISTKEY];
			if (!BookmarkArray) {
				AUTORELEASEOBJNORETURN(self);
				return nil;
			}
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
				//It seems that the URL returned from the bookmarks services is already a file reference URL.
				//NSURL *refURL = [fullURL fileReferenceURL];
				PPMusicListObject *obj = nil;
				//if (refURL) {
				//	obj = [[PPMusicListObject alloc] initWithURL:refURL];
				//} else {
				obj = [[PPMusicListObject alloc] initWithURL:fullURL];
				//}
				[musicList addObject:obj];
				RELEASEOBJ(obj);
			}
		} else {
			musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
			for (NSData *bookData in BookmarkArray) {
				BOOL isStale = NO;
				NSURL *fullURL = [NSURL URLByResolvingBookmarkData:bookData options:NSURLBookmarkResolutionWithoutUI relativeToURL:[NSURL fileURLWithPath:NSHomeDirectory()] bookmarkDataIsStale:&isStale error:nil];
#ifdef DEBUG
				if (isStale) {
					NSLog(@"Bookmark pointing to %@ is stale", [fullURL path]);
				}
#endif
				if (!fullURL) {
					lostMusicCount++;
					continue;
				}
				//It seems that the URL returned from the bookmarks services is already a file reference URL.
				//NSURL *refURL = [fullURL fileReferenceURL];
				PPMusicListObject *obj = nil;
				//if (refURL) {
				//	obj = [[PPMusicListObject alloc] initWithURL:refURL];
				//} else {
				obj = [[PPMusicListObject alloc] initWithURL:fullURL];
				//}
				[musicList addObject:obj];
				RELEASEOBJ(obj);
			}
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
		NSData *bookData = [[[musicList objectAtIndex:i] musicUrl] bookmarkDataWithOptions:NSURLBookmarkCreationPreferFileIDResolution includingResourceValuesForKeys:nil relativeToURL:[NSURL fileURLWithPath:NSHomeDirectory()] error:nil];
		if (bookData) {
			[BookmarkArray addObject:bookData];
		}
	}
	[encoder encodeObject:BookmarkArray forKey:kMUSICLISTKEY2];
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
