//
//  MADDigitalPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPPluginWindowController.h>

@interface PPDigitalPluginWindowController : PPPluginWindowController
{
	@protected
	Pcmd *thePcmd;
}
@property Pcmd *thePcmd;
@end
