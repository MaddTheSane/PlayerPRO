//
//  PPSmallPianoView.h
//  PPMacho
//
//  Created by C.W. Betts on 3/16/14.
//
//

#import <Cocoa/Cocoa.h>

@protocol PPSmallPianoDelegate;

IB_DESIGNABLE
@interface PPSmallPianoView : NSView
@property (weak) id<PPSmallPianoDelegate> delegate;
@property (copy) IBInspectable NSColor *selectedColor;
@end

@protocol PPSmallPianoDelegate <NSObject>

@end
