//
//  PPAPPLImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import "PPAPPLImporter.h"
#include "APPL.h"

@interface PPAPPLImporter ()

@end

static inline MADErr TESTMADk(MADSpec* MADPtr)
{
	OSType madType = MADPtr->MAD;
	MADBE32(&madType);
	if (madType == 'MADK')
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static const OSType MADTypes[] = {'MADK', 'MADI', 'MADF', 'MADG', 'MADH'};

@implementation PPAPPLImporter

- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler
{
	for (int i = 0; i < sizeof(MADTypes) / sizeof(MADTypes[0]); i++) {
		OSType currentType = MADTypes[i];
	}
}

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr
{
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	Handle			myRes;
	BOOL			canOpen = NO;
	
	if (!CFURLGetFSRef((__bridge CFURLRef)theURL, &fileRef))
		return NO;
	iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
	if (iFileRefI == -1) {
		return NO;
	} else {
		UseResFile(iFileRefI);
		
		if (Count1Resources('MADK') > 0) {
			myRes = Get1IndResource('MADK', 1);
			DetachResource(myRes);
			
			HLock(myRes);
			canOpen = TESTMADk((MADSpec*)*myRes) == MADNoErr;
			HUnlock(myRes);
			DisposeHandle(myRes);
		} else if (Count1Resources('MADI') > 0) {
			myRes = Get1IndResource('MADI', 1);
			DetachResource(myRes);
			
			HLock(myRes);
			canOpen = TESTMADI(*myRes) == MADNoErr;
			HUnlock(myRes);
			DisposeHandle(myRes);
			
		} else if (Count1Resources('MADH') > 0) {
			myRes = Get1IndResource('MADH', 1);
			DetachResource(myRes);
			
			HLock(myRes);
			canOpen = TESTMADH(*myRes) == MADNoErr;
			HUnlock(myRes);
			DisposeHandle(myRes);
			
		} else if (Count1Resources('MADF') > 0) {
			myRes = Get1IndResource('MADF', 1);
			DetachResource(myRes);
			
			HLock(myRes);
			canOpen = TestMADFGFile(*myRes) == MADNoErr;
			HUnlock(myRes);
			DisposeHandle(myRes);
			
		} else if (Count1Resources('MADG') > 0) {
			myRes = Get1IndResource('MADG', 1);
			DetachResource(myRes);
			
			HLock(myRes);
			canOpen = TestMADFGFile(*myRes) == MADNoErr;
			HUnlock(myRes);
			DisposeHandle(myRes);
			
		}
		
		CloseResFile(iFileRefI);
	}
	
	return canOpen;
}

- (IBAction)importMusicObject:(id)sender
{
	
}

- (IBAction)cancelImport:(id)sender
{
	
}

@end
