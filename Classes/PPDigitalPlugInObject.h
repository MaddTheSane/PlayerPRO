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

@interface PPDigitalPlugInObject : NSObject
{
	PPDigitalPlugin	**plugCode;
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

- (id)initWithBundle:(NSBundle*)toInit;

- (OSErr)callWithPcmd:(Pcmd*)myPcmd;
- (OSErr)callWithPcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug*)pi;

@end
