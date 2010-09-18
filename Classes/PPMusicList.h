//
//  PPMusicList.h
//  PPMacho
//
//  Created by C.W. Betts on 8/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class NDAlias;

@interface PPMusicList : NSObject <NSCoding> {
	NSMutableArray *musicList;
}
-(NSURL*)URLAtIndex:(NSUInteger)index;

@end
