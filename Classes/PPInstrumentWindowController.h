//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>

@class PPInstrumentImporter;

@interface PPInstrumentWindowController : NSWindowController <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	PPInstrumentImporter *importer;
	MADMusic **curMusic;
	MADDriverRec **theDriver;
	NSMutableArray *instruments;
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

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite, nonatomic) MADMusic **curMusic;
@property (readwrite) MADDriverRec **theDriver;

- (BOOL)importSampleFromURL:(NSURL *)sampURL;
- (BOOL)importSampleFromURL:(NSURL *)sampURL error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;

- (void)writeInstrumentsBackToMusic;
- (void)writeInstrumentAtIndexBackToMusic:(short)idx;
- (void)writeSampleAtIndex:(short)sampIdx withInstrumentAtIndexBackToMusic:(short)insIdx;

- (IBAction)exportInstrument:(id)sender;
- (IBAction)importInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)playInstrument:(id)sender;

- (IBAction)deleteSample:(id)sender;

- (IBAction)playSample:(id)sender;

@end
