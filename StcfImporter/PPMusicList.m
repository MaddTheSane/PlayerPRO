//
//  PPMusicList.m
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPTFMusicList.h"
#include <CoreServices/CoreServices.h>
#include <PlayerPROCore/FileUtils.h>

#define kMusicListKey3 @"Music List Key 3"
#define kMusicListLocation3 @"Music Key Location 3"

static NSString * const kMusicListKVO = @"musicList";

@interface PPMusicListObject ()
@property (retain, readwrite, setter = setTheMusicUrl:) NSURL *musicUrl;
@end

@implementation PPMusicListObject
@synthesize musicUrl;
@synthesize fileSize = _fileSize;
- (unsigned long long)fileSize
{
	if (_fileSize == 0) {
		NSFileManager *fm = [NSFileManager defaultManager];
		NSDictionary *theparam = [fm attributesOfItemAtPath:[musicUrl path] error:NULL];
		if (!theparam) {
			return 0;
		}
		_fileSize = [theparam fileSize];
	}
	
	return _fileSize;
}

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
		id dat1 = nil, dat2 = nil;
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
	if (!aURL) {
		return nil;
	}
	if (self = [super init]) {
		if ([aURL isFileReferenceURL]) {
			self.musicUrl = aURL;
		} else {
			NSURL *tmpURL = [aURL fileReferenceURL];
			self.musicUrl = tmpURL ? tmpURL : aURL;
		}
	}
	return self;
}

- (void)dealloc
{
	self.musicUrl = nil;
	
	[super dealloc];
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"%@ : %@ - %@", [musicUrl description], [musicUrl path], self.fileName];
}

- (id)copyWithZone:(NSZone *)zone
{
	return self; // this class is immutable
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	return self = [self initWithURL:[aDecoder decodeObject]];
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:musicUrl];
}

@end

@interface PPMusicList ()
@property (strong, readonly) NSMutableArray *theMusicList;
@end

@implementation PPMusicList

@synthesize theMusicList = musicList;
@synthesize lostMusicCount;
@synthesize selectedMusic;

- (NSString *)description
{
	return [NSString stringWithFormat:@"Size: %ld, selection: %ld, Contents: %@", (long)[musicList count], (long)selectedMusic, [musicList description]];
}

- (BOOL)saveApplicationMusicList
{
	NSFileManager *manager = [NSFileManager defaultManager];
	NSURL *PPPPath = [[[manager URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:YES error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Player"];
	if (![PPPPath checkResourceIsReachableAndReturnError:NULL]) {
		//Just making sure...
		[manager createDirectoryAtURL:PPPPath withIntermediateDirectories:YES attributes:nil error:NULL];
	}

	return [self saveMusicListToURL:[PPPPath URLByAppendingPathComponent:@"Player List" isDirectory:NO]];
}

- (BOOL)loadApplicationMusicList
{
	NSAssert([self countOfMusicList] == 0, @"Music list should be empty!");
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSData *listData = [defaults dataForKey:@"PlayerPRO Music List"];
	if (listData) {
		[self loadMusicListFromData:listData];
		[defaults removeObjectForKey:@"PlayerPRO Music List"];
		[self saveApplicationMusicList];
		//Technically we did succeed...
		return YES;
	}
	
	NSFileManager *manager = [NSFileManager defaultManager];
	NSURL *PPPPath = [[[manager URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:YES error:NULL] URLByAppendingPathComponent:@"PlayerPRO"] URLByAppendingPathComponent:@"Player"];
	if (![PPPPath checkResourceIsReachableAndReturnError:NULL]) {
		//Set up our directory for later use...
		[manager createDirectoryAtURL:PPPPath withIntermediateDirectories:YES attributes:nil error:NULL];
		//...Then say we failed
		return NO;
	}
	return [self loadMusicListAtURL:[PPPPath URLByAppendingPathComponent:@"Player List" isDirectory:NO]];
}

- (BOOL)saveMusicListToURL:(NSURL *)toSave
{
	NSData *theList = [NSKeyedArchiver archivedDataWithRootObject:self];
	return [theList writeToURL:toSave atomically:YES];
}

- (void)sortMusicListByName
{
	[self sortMusicListWithOptions:(NSSortConcurrent | NSSortStable) usingComparator:^NSComparisonResult(id obj1, id obj2) {
		NSString *rhsString = [obj1 fileName];
		NSString *lhsString = [obj2 fileName];
		NSComparisonResult result = [rhsString localizedStandardCompare:lhsString];
		return result;
	}];
}

- (void)sortMusicListWithOptions:(NSSortOptions)opts usingComparator:(NSComparator)comp
{
	[self willChangeValueForKey:kMusicListKVO];
	[musicList sortWithOptions:(NSSortConcurrent | opts) usingComparator:comp];
	[self didChangeValueForKey:kMusicListKVO];
}

- (void)sortMusicListUsingComparator:(NSComparator)comp
{
	[self sortMusicListWithOptions:NSSortStable usingComparator:comp];
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
	theNo = *((UInt16*)(*aHandle));
	PPBE16(&theNo);
	
	HUnlock(aHandle);
	DisposeHandle(aHandle);
	theNo /= 2;
	
	HLock(locHand);
	short location = **((short**)locHand);
	PPBE16(&location);
	HUnlock(locHand);
	DisposeHandle(locHand);
	
	NSMutableArray *newArray = [[NSMutableArray alloc] initWithCapacity:theNo];
	
	for(i = 0; i < theNo * 2; i += 2) {
		StringPtr aStr, aStr2;
		GetIndString(aStr, 128, i);
		GetIndString(aStr2, 128, i + 1);
		if (!aStr || !aStr2) {
			break;
		}
		NSString *CFaStr, *CFaStr2;
		CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman));
		CFaStr2 = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, kCFStringEncodingMacRoman));

		NSString *together = [@[CFaStr, CFaStr2] componentsJoinedByString:@":"];
		CFaStr = CFaStr2 = nil;
		
		NSURL *fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)together, kCFURLHFSPathStyle, false));
		together = nil;
		if ([fullPath checkResourceIsReachableAndReturnError:NULL]) {
			PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:fullPath];
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
	
	self.selectedMusic = (location >= [newArray count]) ? location : -1;
	
	[self loadMusicList:newArray];

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

- (instancetype)init
{
	if (self = [super init]) {
		musicList = [[NSMutableArray alloc] init];
		lostMusicCount = 0;
		selectedMusic = -1;
	}
	return self;
}

- (void)dealloc
{
	
	
	[super dealloc];
}

- (BOOL)addMusicURL:(NSURL *)musicToLoad
{
	PPMusicListObject *obj = [[PPMusicListObject alloc] initWithURL:musicToLoad];
	
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
	return [[musicList objectAtIndex:index] musicUrl];
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
	//Do nothing for this version
	return nil;
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
