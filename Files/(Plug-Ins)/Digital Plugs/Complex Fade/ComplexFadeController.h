//
//  ComplexFadeController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>

typedef NS_ENUM(NSInteger, FadeTypeEnum) {
	fadeInstrument = 1,
	fadeNote,
	fadeArgument,
	fadeVolume
};

@interface ComplexFadeController : PPDigitalPluginWindowController
@property (weak) IBOutlet NSMatrix *fadeTypeMatrix;
@property (weak) IBOutlet NSFormCell *stepCell;
@property (weak) IBOutlet NSTextField *textValues;
@property FadeTypeEnum fadeType;

- (IBAction)changeFadeType:(id)sender;

@end
