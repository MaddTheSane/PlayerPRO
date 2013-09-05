//
//  PPMusicList.h
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PPMusicListObject : NSObject <NSCopying>
{
	NSURL *musicUrl;
	//NSString *fileName;
}

@property (retain, readonly) NSURL *musicUrl;
@property (unsafe_unretained, readonly) NSString *fileName;

- (id)initWithURL:(NSURL *)aURL;

@end

@interface PPMusicList : NSObject <NSCoding, NSFastEnumeration> {
	NSMutableArray	*musicList;
	NSInteger		selectedMusic;
	NSUInteger		lostMusicCount;
}

@property (readonly) NSUInteger lostMusicCount;
@property (readwrite) NSInteger selectedMusic;

- (void)sortMusicList;

- (void)loadMusicListFromPreferences;
- (void)saveMusicListToPreferences;
- (BOOL)saveMusicListToURL:(NSURL *)toSave;
- (BOOL)loadMusicListAtURL:(NSURL *)fromURL;
- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen;

- (NSURL*)URLAtIndex:(NSUInteger)index;
- (BOOL)addMusicURL:(NSURL *)musicToLoad;
- (void)removeObjectAtIndex:(NSUInteger)object;
- (void)clearMusicList;

- (NSInteger)indexOfObjectSimilarToURL:(NSURL*)theURL;

- (void)removeObjectsInMusicListAtIndexes:(NSIndexSet *)set;

//KVC functions
- (NSUInteger)countOfMusicList;
- (id)objectInMusicListAtIndex:(NSUInteger)idx;
- (void)insertObject:(id)anObj inMusicListAtIndex:(NSUInteger)idx;
- (void)removeObjectInMusicListAtIndex:(NSUInteger)object;
- (void)replaceObjectInMusicListAtIndex:(NSUInteger)index withObject:(id)anObject;
@end