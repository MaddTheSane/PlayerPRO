//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class AppDelegate;

@interface PPInstrumentWindowController : NSWindowController <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	MADMusic **curMusic;
	MADDriverRec **theDriver;
	NSMutableArray *instruments;
	IBOutlet NSDrawer *infoDrawer;
	IBOutlet NSTextField *instrumentSize;
	IBOutlet NSTextField *instrumentLoopStart;
	IBOutlet NSTextField *instrumentLoopSize;
	IBOutlet NSTextField *instrumentVolume;
	IBOutlet NSTextField *instrumentRate;
	IBOutlet NSTextField *instrumentNote;
	IBOutlet NSTextField *instrumentBits;
	IBOutlet NSTextField *instrumentMode;

	IBOutlet NSImageView *waveFormImage;
	IBOutlet NSOutlineView *instrumentView;
}

@property (weak) AppDelegate *appDel;

@property (readwrite, nonatomic) MADMusic **curMusic;
@property (readwrite) MADDriverRec **theDriver;

- (IBAction)toggleInfo:(id)sender;

@end
