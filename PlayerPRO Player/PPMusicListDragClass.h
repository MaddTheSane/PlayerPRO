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

@interface PPMusicListDragClass : NSObject <NSPasteboardReading, NSPasteboardWriting>
@property (readonly, strong) NSIndexSet *theIndexSet;

- (id)initWithIndexSet:(NSIndexSet*)theSet;
@end
