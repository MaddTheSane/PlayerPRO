/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "FileUtils.h"
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
 
unsigned char* MYC2PStr(Ptr cStr)
{
	long size = strlen(cStr);
	memmove(cStr + 1, cStr, size);
	cStr[ 0] = size;

	return (unsigned char*) cStr;
}

void MYP2CStr(unsigned char *cStr)
{
	long size = cStr[ 0];
	memmove(cStr, cStr + 1, size);
	cStr[ size] = 0;
}

UNFILE iFileOpen(Ptr name)
{
	UNFILE	temp;
	OSErr	iErr;
	FSRef	Ref;
	FSSpec	spec;
	
	HGetVol(NULL, &spec.vRefNum, &spec.parID);
	
	MYC2PStr(name);
	FSMakeFSSpec(spec.vRefNum, spec.parID, (unsigned char*)name, &spec);
	MYP2CStr((unsigned char*)name);
	
	iErr = FSpMakeFSRef(&spec, &Ref);
	if(iErr != noErr) return 0;
	Boolean	UnusedBoolean, UnusedBoolean2;
#if CHECKDATAFORKNAME
	HFSUniStr255 whythis;
	FSGetDataForkName(&whythis);
	FSResolveAliasFile(&Ref, TRUE, &UnusedBoolean, &UnusedBoolean2);
	iErr = FSOpenFork(&Ref, whythis.length, whythis.unicode, fsCurPerm, &temp);
#else
	FSResolveAliasFile(&Ref, TRUE, &UnusedBoolean, &UnusedBoolean2);
	iErr = FSOpenFork(&Ref, 0, 0, fsCurPerm, &temp);
#endif
	if(iErr != noErr) return 0;
	else return temp;
}

long iGetEOF(UNFILE iFileRefI)
{
	SInt64 curEOF;
	
	FSGetForkSize(iFileRefI, &curEOF);
	
	return curEOF;
}

OSErr iRead(long size, Ptr dest, UNFILE iFileRefI)
{
	return FSReadFork(iFileRefI, fsAtMark, 0, size, dest, NULL);
}

OSErr iSeekCur(long size, UNFILE iFileRefI)
{
	return FSSetForkPosition(iFileRefI, fsFromMark, size);
}

void iFileCreate(Ptr name, OSType type)
{
	FSRef	ref;
	FSCatalogInfo	fileCat;
	FileInfo		*fInfo = (FileInfo*)&fileCat.finderInfo;
	memset(&fileCat, 0, sizeof(fileCat));
	
	MYC2PStr(name);
	{
		FSSpec	spec;
		HGetVol(NULL, &spec.vRefNum, &spec.parID);
		Str31 blankName = {0};
		
		FSMakeFSSpec(spec.vRefNum, spec.parID, (unsigned char*)name, &spec);
		if (FSpMakeFSRef(&spec, &ref) == noErr)
		{
			FSDeleteObject(&ref);
		}
		FSMakeFSSpec(spec.vRefNum, spec.parID, blankName, &spec);
		FSpMakeFSRef(&spec, &ref);
	}
	CFStringRef fileName = CFStringCreateWithPascalString(kCFAllocatorDefault, (unsigned char*)name, kCFStringEncodingMacRoman);
	if (!fileName)
	{
		fileName = CFStringCreateWithPascalString(kCFAllocatorDefault, (unsigned char*)name, CFStringGetSystemEncoding());
	}
	
	long uniLen = CFStringGetLength(fileName);
	
	UniChar*		UNICHARThing = calloc(uniLen, sizeof(UniChar));
	CFRange			UNIRange;
	UNIRange.location	= 0;
	UNIRange.length		= uniLen;
	CFStringGetCharacters(fileName, UNIRange, UNICHARThing);
	CFRelease(fileName);
	
	fInfo->fileType		 = type;
	fInfo->fileCreator	 = 'SNPL';
	fInfo->finderFlags	 = 0;
	fInfo->location.h	 = 0;
	fInfo->location.v	 = 0;
	fInfo->reservedField = 0;
	
	FSCreateFileUnicode(&ref, uniLen, UNICHARThing, kFSCatInfoFinderInfo, &fileCat, NULL, NULL);
	free(UNICHARThing);
	
	MYP2CStr((unsigned char*) name);
}

OSErr iWrite(long size, Ptr dest, UNFILE iFileRefI)
{
	return FSWriteFork(iFileRefI, fsAtMark, 0, size, dest, NULL);
}

void iClose(UNFILE iFileRefI)
{
	FSCloseFork(iFileRefI);
}

/////////////////////////////////

EXP void OSType2Ptr(OSType type, Ptr str)
{
	MOT32(&type);
	memcpy(str, &type, 4);
	str[ 4] = 0;
}

EXP OSType Ptr2OSType(char* str)
{
	short 	i;
	OSType	type;
	
	i = strlen(str);
	if (i > 4) i = 4;
	type = '    ';
	memcpy(&type, str, i);
	MOT32(&type);
	
	return type;
}
