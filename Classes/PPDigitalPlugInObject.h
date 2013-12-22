//
//  PPDigitalPlugInObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#import "PPPlugInObject.h"

@interface PPDigitalPlugInObject : PPPlugInObject
{
	PPDigitalPlugin	**plugCode;
}

- (instancetype)initWithBundle:(NSBundle*)toInit;

- (OSErr)callWithPcmd:(Pcmd*)myPcmd;
- (OSErr)callWithPcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug*)pi;

@end
