//
//  PPPlugInCommon.h
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#ifndef PPMacho_PPPlugInCommon_h
#define PPMacho_PPPlugInCommon_h

void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID);
NSArray *DefaultPlugInLocations();
OSErr inMADPlaySoundData(MADDriverRec *theRec, Ptr soundPtr, long size, SInt32 channel, SInt32 note, SInt32 amplitude, long loopBeg, long loopSize, unsigned int rate, Boolean stereo);

#endif
