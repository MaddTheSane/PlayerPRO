//
//  PPMusicList.h
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#if !TARGET_OS_IPHONE
#import <Cocoa/Cocoa.h>
#define UNAVAILABLE_IPHONE
#else 
#define UNAVAILABLE_IPHONE UNAVAILABLE_ATTRIBUTE
#endif

@interface PPMusicListObject : NSObject <NSCopying>
@property (strong, readonly) NSURL *musicUrl;
@property (unsafe_unretained, readonly) NSString *fileName;
#if !TARGET_OS_IPHONE
@property (unsafe_unretained, readonly) NSImage *fileIcon;
#endif

- (instancetype)initWithURL:(NSURL *)aURL;
@end

@interface PPMusicList : NSObject <NSCoding, NSFastEnumeration>
@property (readonly) NSUInteger lostMusicCount;
@property NSInteger selectedMusic;

- (instancetype)init;

- (void)sortMusicList DEPRECATED_ATTRIBUTE;
- (void)sortMusicListByName;

- (void)loadMusicListFromPreferences;
- (void)saveMusicListToPreferences;
- (BOOL)saveMusicListToURL:(NSURL *)toSave;
- (BOOL)loadMusicListAtURL:(NSURL *)fromURL;
- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen UNAVAILABLE_IPHONE;

- (NSURL*)URLAtIndex:(NSUInteger)index;
- (BOOL)addMusicURL:(NSURL *)musicToLoad;
- (void)removeObjectAtIndex:(NSUInteger)object;
- (void)clearMusicList;

- (NSInteger)indexOfObjectSimilarToURL:(NSURL*)theURL;

- (void)removeObjectsInMusicListAtIndexes:(NSIndexSet *)set;
- (NSArray*)arrayOfObjectsInMusicListAtIndexes:(NSIndexSet*)theSet;
- (void)insertObjects:(NSArray*)anObj inMusicListAtIndex:(NSUInteger)idx;

//KVC functions
- (NSUInteger)countOfMusicList;
- (id)objectInMusicListAtIndex:(NSUInteger)idx;
- (void)insertObject:(id)anObj inMusicListAtIndex:(NSUInteger)idx;
- (void)removeObjectInMusicListAtIndex:(NSUInteger)object;
- (void)replaceObjectInMusicListAtIndex:(NSUInteger)index withObject:(id)anObject;
@end
