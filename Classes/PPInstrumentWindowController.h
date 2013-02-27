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
@class InstrumentInfoController;
@class PPFilterPlugHandler;

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
	InstrumentInfoController *instrumentInfo;
	PPFilterPlugHandler *filterHandler;
	
	NSUndoManager *undoManager;
}

@property (readwrite, retain) PPInstrumentImporter *importer;
@property (readwrite, nonatomic) MADMusic **curMusic;
@property (readwrite) MADDriverRec **theDriver;
@property (readwrite, retain) NSUndoManager *undoManager;
@property (readwrite, retain) PPFilterPlugHandler *filterHandler;

- (BOOL)importSampleFromURL:(NSURL *)sampURL;
- (BOOL)importSampleFromURL:(NSURL *)sampURL error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr;


- (void)writeInstrumentsBackToMusic;
- (void)writeInstrumentAtIndexBackToMusic:(short)idx;
- (void)writeSampleAtIndex:(short)sampIdx withInstrumentAtIndexBackToMusic:(short)insIdx;

//Toolbar functions
- (IBAction)exportInstrument:(id)sender;
- (IBAction)importInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)playInstrument:(id)sender;
- (IBAction)showInstrumentInfo:(id)sender;

- (IBAction)deleteSample:(id)sender;

- (IBAction)playSample:(id)sender;

- (void)replaceObjectInInstrumentsAtIndex:(NSUInteger)index withObject:(id)object;

@end
