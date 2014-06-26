//
//  Cocoa_ExampleAppDelegate.h
//  Cocoa Example
//
//  Created by C.W. Betts on 11/29/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>

@interface Cocoa_ExampleAppDelegate : NSObject <NSApplicationDelegate> {
	NSWindow *window;
	IBOutlet NSImageView *fileIcon;
	
	IBOutlet NSTextField *fileName;
	MADDriverRec	*MADDriver;
	MADMusic		*Music;
	MADLibrary		*MADLib;
}

@property (assign) IBOutlet NSWindow *window;

@end
