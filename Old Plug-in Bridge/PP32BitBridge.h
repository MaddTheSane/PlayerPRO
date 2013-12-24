//
//  PP32BitBridge.h
//  PPMacho
//
//  Created by C.W. Betts on 12/21/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCoreTemp/PPDefs.h>
#include <PlayerPROCoreTemp/MAD.h>
#include <PlayerPROCoreTemp/RDriver.h>
#include <PlayerPROCoreTemp/PPPlug.h>
#include <PlayerPROCoreTemp/PPPrivate.h>
#include <PlayerPROCoreTemp/MADDriver.h>

@interface PP32BitBridge : NSObject
{
	MADLibrary *the32BitLibrary;
}
- (id)init;
- (BOOL)addPlugInAtPath:(NSURL*)plugPath;
- (OSErr)getMusicData:(out NSData **)theOutData fromMusicAtFileURL:(NSURL*)theURL withPlugInType:(char*)theType;
- (BOOL)canLoadFileAtURL:(NSURL*)theURL withPlugInType:(char*)theType;
- (OSErr)getMusicInformation:(out NSData **)theOutData fromMusicAtFileURL:(NSURL*)theURL withPlugInType:(char*)theType;

@end
