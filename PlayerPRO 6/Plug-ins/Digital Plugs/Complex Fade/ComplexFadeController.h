//
//  ComplexFadeController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>
//TODO: Make a note formatter
//@class CompFadeNoteFormatter;

typedef NS_ENUM(NSInteger, FadeTypeEnum) {
	fadeInstrument = 1,
	fadeNote,
	fadeArgument,
	fadeVolume
};

@interface ComplexFadeController : NSWindowController
@property (weak) IBOutlet NSFormCell *stepCell;
@property (weak) IBOutlet NSFormCell *fromCell;
@property (weak) IBOutlet NSFormCell *toCell;

@property (weak) IBOutlet NSMatrix *fadeTypeMatrix;
@property (weak) IBOutlet NSTextField *valueInfo;
//@property (strong) IBOutlet CompFadeNoteFormatter *noteFormatter;
@property FadeTypeEnum fadeType;

- (IBAction)okOrCancel:(id)sender;
- (IBAction)changeFadeType:(id)sender;

@end
