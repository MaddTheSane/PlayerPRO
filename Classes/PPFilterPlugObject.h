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
#import "PPPlugInObject.h"

@interface PPFilterPlugObject : PPPlugInObject
{
	PPFiltersPlugin	**plugData;
}

- (instancetype)initWithBundle:(NSBundle *)aBund;

- (OSErr)callPluginWithData:(sData *)theData selectionStart:(long) SelectionStart selectionEnd:(long) SelectionEnd plugInInfo:(PPInfoPlug *)thePPInfoPlug stereoMode:(short)stereoMode;

@end
