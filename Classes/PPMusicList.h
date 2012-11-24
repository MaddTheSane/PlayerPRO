//
//  PPMusicList.h
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PPMusicListObject : NSObject
{
	NSURL *musicUrl;
	NSString *fileName;
}

@property (copy, nonatomic) NSURL *musicUrl;
@property (readonly) NSString *fileName;

- (id)initWithURL:(NSURL *)aURL;

@end

@interface PPMusicList : NSObject <NSCoding> {
	NSMutableArray *musicList;
}
- (void)sortMusicList;

- (void)loadMusicListFromPreferences;
- (void)saveMusicListToPreferences;
- (void)saveMusicListToURL:(NSURL *)toSave;
- (void)loadMusicListAtURL:(NSURL *)fromURL;
- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen;

- (NSURL*)URLAtIndex:(NSUInteger)index;
- (void)loadMusicListAtURL:(NSURL *)fromURL;
- (void)addMusicURL:(NSURL *)musicToLoad;
- (void)removeObjectAtIndex:(NSUInteger)object;

//KVC functions
- (NSUInteger)countOfMusicList;
- (id)objectInMusicListAtIndex:(NSUInteger)idx;
- (void)insertObject:(id)anObj inMusicListAtIndex:(NSUInteger)idx;
- (void)removeObjectInMusicListAtIndex:(NSUInteger)object;
- (void)replaceObjectInMusicListAtIndex:(NSUInteger)index withObject:(id)anObject;
@end
