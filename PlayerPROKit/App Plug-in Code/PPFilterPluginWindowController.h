//
//  PPFilterPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/24/13.
//
//

#import <PlayerPROKit/PPPlugIns.h>
#import <PlayerPROKit/PPPluginWindowController.h>

@interface PPFilterPluginWindowController : PPPluginWindowController
{
	@protected
	sData *theData;
	long selectionStart;
	long selectionEnd;
	BOOL stereoMode;
}

@property sData *theData;
@property long selectionStart;
@property long selectionEnd;
@property BOOL stereoMode;
@end
