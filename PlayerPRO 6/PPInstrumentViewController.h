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

@class PPInstrumentPlugHandler;
@class InstrumentInfoController;
@class PPFilterPlugHandler;
@class PPDocument;

@interface PPInstrumentViewController : NSViewController <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	InstrumentInfoController *instrumentInfo;
}
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
@property (weak) IBOutlet NSOutlineView *instrumentOutline;

@property (weak) PPInstrumentPlugHandler *importer;
@property (weak) PPFilterPlugHandler *filterHandler;
@property (weak) IBOutlet PPDocument *currentDocument;

- (BOOL)importSampleFromURL:(NSURL *)sampURL;
- (BOOL)importSampleFromURL:(NSURL *)sampURL error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr;
- (OSErr)exportInstrumentListToURL:(NSURL*)outURL;

//Toolbar functions
- (IBAction)exportInstrument:(id)sender;
- (IBAction)importInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)playInstrument:(id)sender;
- (IBAction)showInstrumentInfo:(id)sender;
- (IBAction)toggleInfo:(id)sender;

- (IBAction)deleteSample:(id)sender;
- (IBAction)playSample:(id)sender;

- (void)replaceObjectInInstrumentsAtIndex:(NSUInteger)index withObject:(id)object;

@end
