//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>

@class AppDelegate;

@interface PPInstrumentWindowController : NSWindowController <NSOutlineViewDataSource, NSOutlineViewDelegate>
@property (weak) IBOutlet NSDrawer *infoDrawer;
@property (weak) IBOutlet NSTextField *instrumentSize;
@property (weak) IBOutlet NSTextField *instrumentLoopStart;
@property (weak) IBOutlet NSTextField *instrumentLoopSize;
@property (weak) IBOutlet NSTextField *instrumentVolume;
@property (weak) IBOutlet NSTextField *instrumentRate;
@property (weak) IBOutlet NSTextField *instrumentNote;
@property (weak) IBOutlet NSTextField *instrumentBits;
@property (weak) IBOutlet NSTextField *instrumentMode;
@property (weak) IBOutlet NSImageView *waveFormImage;
@property (weak) IBOutlet NSOutlineView *instrumentView;

- (IBAction)toggleInfo:(id)sender;

@end
