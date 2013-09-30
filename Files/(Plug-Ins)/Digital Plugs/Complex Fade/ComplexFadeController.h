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

@interface ComplexFadeController : PPDigitalPluginWindowController
@property (weak) IBOutlet NSMatrix *fadeTypeMatrix;
@property (weak) IBOutlet NSFormCell *stepCell;
@property (weak) IBOutlet NSTextField *textValues;




- (IBAction)changeFadeType:(id)sender;

@end
