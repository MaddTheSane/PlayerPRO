/*
 *  PPPrivate.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 6/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROCORE_PPPRIVATE__
#define __PLAYERPROCORE_PPPRIVATE__

#include "RDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MAC_H
#pragma mark Core Audio Functions
OSErr initCoreAudio( MADDriverRec *inMADDriver);
OSErr closeCoreAudio( MADDriverRec *inMADDriver);
void StopChannelCA(MADDriverRec *inMADDriver);
void PlayChannelCA(MADDriverRec *inMADDriver);

CFMutableArrayRef GetDefaultPluginFolderLocations();

const CFStringRef kMadPlugMenuNameKey;
const CFStringRef kMadPlugAuthorNameKey;
const CFStringRef kMadPlugUTITypesKey;
const CFStringRef kMadPlugModeKey;
const CFStringRef kMadPlugTypeKey;
#endif

#ifdef _ESOUND
OSErr initESD( MADDriverRec *inMADDriver);
OSErr closeESD( MADDriverRec *inMADDriver);
void StopChannelESD(MADDriverRec *inMADDriver);
void PlayChannelESD(MADDriverRec *inMADDriver);
#endif

#ifdef _OSSSOUND
OSErr initOSS( MADDriverRec *inMADDriver);
OSErr closeOSS( MADDriverRec *inMADDriver);
void StopChannelOSS(MADDriverRec *inMADDriver);
void PlayChannelOSS(MADDriverRec *inMADDriver);
#endif

#ifdef LINUX
OSErr initALSA( MADDriverRec *inMADDriver);
OSErr closeALSA( MADDriverRec *inMADDriver);
void StopChannelALSA(MADDriverRec *inMADDriver);
void PlayChannelALSA(MADDriverRec *inMADDriver);
#endif

#ifdef WIN32
void DirectSoundClose( MADDriverRec* driver);
Boolean DirectSoundInit( MADDriverRec* driver);
Boolean W95_Init( MADDriverRec* driver);
void W95_Exit( MADDriverRec* driver);
#endif
	
OSErr	CallImportPlug( 	MADLibrary		*inMADDriver,
					   short			PlugNo,			// CODE du plug
					   OSType			order,
					   char			*AlienFile,
					   MADMusic		*theNewMAD,
					   PPInfoRec		*info);


OSErr	PPTestFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile);
OSErr	PPInfoFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, PPInfoRec	*InfoRec);
OSErr	PPExportFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, MADMusic	*theNewMAD);
OSErr	PPImportFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, MADMusic	**theNewMAD);
OSErr	PPIdentifyFile( MADLibrary		*inMADDriver, char *kindFile, char	*AlienFile);

OSType	GetPPPlugType( MADLibrary		*inMADDriver, short ID, OSType type);
void	MInitImportPlug( MADLibrary		*inMADDriver, char*);
#ifdef _MAC_H
void	MADInitImportPlug( MADLibrary	*inMADDriver, char *PluginFolder);
#endif
	
void	CloseImportPlug( MADLibrary		*inMADDriver);
OSErr	MADLoadMADFileCString( MADMusic **, Ptr fName);
OSErr	CheckMADFile( char *AlienFile);

	
#ifdef __cplusplus
}
#endif


#endif
