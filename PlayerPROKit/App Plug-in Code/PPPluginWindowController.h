//
//  MADPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PPPlug.h>

extern NSString * const PPPlugInSheetDidEnd;
extern NSString * const PPPlugReturnCode;

@interface PPPluginWindowController : NSWindowController
{
	@protected
	PPInfoPlug *infoPlug;
	BOOL isMultipleIstanceSafe; //For those that can safely handle multiple instances
	__unsafe_unretained NSWindow *parentWindow;
	dispatch_block_t plugBlock;
}

- (instancetype)initWithWindowNibName:(NSString *)windowNibName UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithWindowNibName:(NSString *)windowNibName infoPlug:(PPInfoPlug *)ip;

@property PPInfoPlug *infoPlug;
@property (copy) dispatch_block_t plugBlock;
- (IBAction)okOrCancel:(id)sender;
- (OSErr)runAsModal;
- (OSErr)runAsSheet;
@end
