//
//  PPInstrumentPluginWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/26/13.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPPlugIns.h>
#import <PlayerPROKit/PPPluginWindowController.h>

@interface PPInstrumentPluginWindowController : PPPluginWindowController
{
	@protected
	OSType		order;						// Order to execute
	InstrData	*instrumentHeader;					// Ptr on instrument header
	sData		**sample;					// Ptr on samples data
	short		*sampleID;					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
	// If sampleID == -1 : add sample else replace selected sample.
	NSURL		*alienFileURL;			// IN/OUT file
}
@property			OSType		order;
@property			InstrData	*instrumentHeader;
@property			sData		**sample;
@property			short		*sampleID;
@property (strong)	NSURL		*alienFileURL;

@end
