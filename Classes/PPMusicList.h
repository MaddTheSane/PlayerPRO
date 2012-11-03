//
//  PPMusicList.h
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PPMusicList : NSObject <NSCoding, NSTableViewDataSource> {
	NSMutableArray *musicList;
}
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
