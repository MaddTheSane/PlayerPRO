/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#include <CoreServices/CoreServices.h>
#include "APPL.h"

static inline OSErr TESTMADk(MADSpec* MADPtr)
{
	OSType madType = MADPtr->MAD;
	MADBE32(&madType);
	if (madType == 'MADK')
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr INFOMADkApp(MADSpec* MADPtr, MADInfoRec *info)
{
	//short	i;
	
	strlcpy(info->internalFileName, MADPtr->name, sizeof(MADPtr->name));
	
	strcpy(info->formatDescription, "MADK Resource (APPL)");
	
	info->totalPatterns		= MADPtr->numPat;
	info->partitionLength	= MADPtr->numPointers;
	
	info->totalTracks		= MADPtr->numChn;
	info->signature			= MADPtr->MAD;
	MADBE32(&info->signature);
	
	info->totalInstruments	= MADPtr->numInstru;
	
	return MADNoErr;
}

/*****************/
/* MAIN FUNCTION */
/*****************/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
extern OSErr PPImpExpMain(OSType order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init)
{
	//TODO: learn how to get and parse resource forks using POSIX commands or CoreFoundation, but not Carbon
	OSErr			myErr = MADNoErr;
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	Handle			myRes;
	
	if (FSPathMakeRef((UInt8*)AlienFileName, &fileRef, NULL) != noErr)
		return MADReadingErr;
	
	switch (order) {
		case MADPlugImport:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = LoadMADH(*myRes, MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADI2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADH2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADFG2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = MADFG2Mad(*myRes, GetHandleSize(myRes), MadFile, init);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		case MADPlugTest:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADk((MADSpec*)*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADI(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MADI Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TESTMADH(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MADH Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TestMADFGFile(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MAD-FG Application", sizeof(info->formatDescription));
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = TestMADFGFile(*myRes);
					HUnlock(myRes);
					DisposeHandle(myRes);
					
					strlcpy(info->formatDescription, "MAD-FG Application", sizeof(info->formatDescription));
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		case MADPlugInfo:
			iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
			if (iFileRefI == -1)
				myErr = MADFileNotSupportedByThisPlug;
			else {
				UseResFile(iFileRefI);
				
				if (Count1Resources('MADK') > 0) {
					myRes = Get1IndResource('MADK', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = INFOMADkApp((MADSpec*)*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADI') > 0) {
					myRes = Get1IndResource('MADI', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADIInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADH') > 0) {
					myRes = Get1IndResource('MADH', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADHInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADF') > 0) {
					myRes = Get1IndResource('MADF', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADFGInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else if (Count1Resources('MADG') > 0) {
					myRes = Get1IndResource('MADG', 1);
					info->fileSize = GetResourceSizeOnDisk(myRes);
					DetachResource(myRes);
					
					HLock(myRes);
					myErr = ExtractMADFGInfo(*myRes, info);
					HUnlock(myRes);
					DisposeHandle(myRes);
				} else
					myErr = MADFileNotSupportedByThisPlug;
				
				CloseResFile(iFileRefI);
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}
#pragma clang diagnostic pop
