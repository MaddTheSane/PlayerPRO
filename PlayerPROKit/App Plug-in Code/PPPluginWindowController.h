//
//  MADPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PPPlug.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

EXP NSString * const PPPlugInSheetDidEnd;
EXP NSString * const PPPlugReturnCode;

@interface PPPluginWindowController : NSWindowController
{
	@protected
	PPInfoPlug *infoPlug;
	BOOL isMultipleIstanceSafe; //For those that can safely handle multiple instances
	__unsafe_unretained NSWindow *parentWindow;
	dispatch_block_t plugBlock;
}

- (instancetype)initWithWindowNibName:(NSString *)windowNibName UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithWindowNibName:(NSString *)windowNibName infoPlug:(PPInfoPlug *)ip NS_DESIGNATED_INITIALIZER;

@property PPInfoPlug *infoPlug;
@property (copy) dispatch_block_t plugBlock;
- (IBAction)okOrCancel:(id)sender;
- (MADErr)runAsModal DEPRECATED_ATTRIBUTE;
- (MADErr)runAsSheet;
@end
