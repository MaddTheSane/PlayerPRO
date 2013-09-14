//
//  MADPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PPPlug.h>

extern const NSString * const PPPlugInSheetDidEnd;

@interface PPPluginWindowController : NSWindowController
{
	@protected
	PPInfoPlug *infoPlug;
	BOOL isMultipleIstanceSafe;//For those that can safely handle multiple instances
	__unsafe_unretained NSWindow *parentWindow;
	dispatch_block_t plugBlock;
}

@property PPInfoPlug *infoPlug;
- (IBAction)okOrCancel:(id)sender;
- (OSErr)runAsModal;

@end
