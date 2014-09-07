//
//  FadeNoteController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface FadeNoteController : NSWindowController
@property (weak) IBOutlet NSComboBox *fromBox;
@property (weak) IBOutlet NSComboBox *toBox;
@property (copy) NSString* initialFrom;
@property (copy) NSString* initialTo;
@end
