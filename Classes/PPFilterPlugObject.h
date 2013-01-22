//
//  PPFilterPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@interface PPFilterPlugObject : NSObject
{
	PPFiltersPlugin	**plugData;
	NSString		*menuName;
	NSString		*authorString;
	NSBundle		*file;
	OSType			type;
	UInt32			version;
}

@property (readonly) NSString *menuName;
@property (readonly) NSString *authorString;
@property (readonly) NSBundle *file;
@property (readonly) OSType type;
@property (readonly) UInt32 version;

- (id)initWithBundle:(NSBundle *)aBund;

- (OSErr)callPluginWithData:(sData *)theData selectionStart:(long) SelectionStart selectionEnd:(long) SelectionEnd plugInInfo:(PPInfoPlug *)thePPInfoPlug stereoMode:(short)stereoMode;

@end
