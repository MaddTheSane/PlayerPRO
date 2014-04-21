//
//  PPMusicListDragClass.h
//  PPMacho
//
//  Created by C.W. Betts on 12/8/13.
//
//

#import <Foundation/Foundation.h>
#import <AppKit/NSPasteboard.h>

extern NSString * const PPMLDCUTI;

@interface PPMusicListDragClass : NSObject <NSPasteboardReading, NSPasteboardWriting, NSCoding>
@property (readonly, copy) NSIndexSet *theIndexSet;

- (instancetype)initWithIndexSet:(NSIndexSet*)theSet;
+ (instancetype)dragWithIndexSet:(NSIndexSet*)theSet;

@end
