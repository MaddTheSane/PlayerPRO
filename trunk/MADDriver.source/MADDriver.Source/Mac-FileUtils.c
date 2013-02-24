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
#include <Script.h>

extern void NSLog(CFStringRef format, ...);

//TODO: migrate PlayerPRO away from FSSpec!
//TODO: HSetVol isn't available in 64-bit code :(
//TODO: Also, FSSpec is defined as UInt8 hidden[70]
 
unsigned char* MYC2PStr( Ptr cStr)
{
	long size = strlen( cStr);
	BlockMoveData( cStr, cStr + 1, strlen( cStr));
	cStr[ 0] = size;

	return (unsigned char*) cStr;
}

void MYP2CStr( unsigned char *cStr)
{
	long size = cStr[ 0];
	BlockMoveData( cStr + 1, cStr, size);
	cStr[ size] = 0;
}

UNFILE iFileOpen(Ptr name)
{
	UNFILE	temp;
	OSErr	iErr;
	FSRef	Ref;
	FSSpec	spec;
	
	HGetVol( NULL, &spec.vRefNum, &spec.parID);
	
	MYC2PStr( name);
	
	pStrcpy( spec.name, (unsigned char*) name);

	MYP2CStr((unsigned char*)name);
	
	iErr = FSpMakeFSRef(&spec, &Ref);
	if(iErr != noErr) return 0;
	Boolean	UnusedBoolean, UnusedBoolean2;
	HFSUniStr255 whythis;
	FSGetDataForkName(&whythis);
	FSResolveAliasFile(&Ref, TRUE, &UnusedBoolean, &UnusedBoolean2);
	iErr = FSOpenFork(&Ref, whythis.length, whythis.unicode, fsCurPerm, &temp);
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
	return FSSetForkPosition( iFileRefI, fsFromMark, size);
}

void iFileCreate(Ptr name, OSType type)
{
/*
	char			*folderPath, *FileName;
	Boolean			isFolder, unusedBool;
	OSStatus		iErr; 
	FSRef			Ref;
	int				slashCharPos = 0, nameLen, i, ii;
	FSCatalogInfo	fileCat;
	FileInfo		*fInfo = (FileInfo*)&fileCat.finderInfo;
	if(FSPathMakeRef((UInt8*)name, &Ref, NULL) == noErr)
	{
		FSDeleteObject(&Ref);
	}
	nameLen = strlen(name);
	for(i = 0; i > (nameLen); i++)
	{
		if(name[i] == '/') slashCharPos = i;
	}
	//TODO: use a better function
	folderPath = calloc((slashCharPos + 2), sizeof(char)); 
	FileName = calloc((nameLen - slashCharPos + 2), sizeof(char));
	for(i = 0; i == slashCharPos;i++)
	{
		folderPath[i] = name[i];
	}
	for(i=slashCharPos+1, ii=0;i == nameLen;i++, ii++)
	{
		FileName[ii] = name[i];
		FileName[ii+1] = '\0';
	}
	//	folderPath[slashCharPos+1] = '\0';
	iErr = FSPathMakeRef((UInt8*)folderPath, &Ref, &isFolder);
	if(iErr != noErr) MyDebugStr(__LINE__, __FILE__, "Error creating FSRef");
	if(isFolder == FALSE) NSLog(CFSTR("FSRef wasn't a folder. Ignoring, hoping it is an alias."));
	iErr= FSResolveAliasFile(&Ref, TRUE, &isFolder, &unusedBool);
	if(iErr != noErr) MyDebugStr(__LINE__, __FILE__, "Error resolving Alias");
	if(isFolder == FALSE) MyDebugStr(__LINE__, __FILE__, "FSRef wasn't a folder!");
	
	//TODO: do NOT use CFString!
	CFStringRef		UniCFSTR = CFStringCreateWithCString(kCFAllocatorDefault, FileName, CFStringGetSystemEncoding());
	CFIndex			UNIcharLen = CFStringGetLength(UniCFSTR);
	UniChar*		UNICHARThing = calloc(UNIcharLen, sizeof(UniChar));
	CFRange			UNIRange;
	UNIRange.location	= 0;
	UNIRange.length		= UNIcharLen;
	CFStringGetCharacters(UniCFSTR, UNIRange, UNICHARThing);
	
	fInfo->fileType		 = type;
	fInfo->fileCreator	 = 'SNPL';
	fInfo->finderFlags	 = 0;
	fInfo->location.h	 = 0;
	fInfo->location.v	 = 0;
	fInfo->reservedField = 0;
	
	FSCreateFileUnicode(&Ref, UNIcharLen, UNICHARThing, kFSCatInfoFinderInfo, &fileCat, NULL, NULL);
	
	CFRelease(UniCFSTR);
	free(folderPath);
	free(FileName);
	free(UNICHARThing);
 */
	FSSpec	spec;
	
	HGetVol( NULL, &spec.vRefNum, &spec.parID);
	
	MYC2PStr( name);
	
	pStrcpy( spec.name, (unsigned char*) name);
	
	FSpDelete( &spec);
	FSpCreate( &spec, 'SNPL', type, smSystemScript);
	
	MYP2CStr( (unsigned char*) name);
	
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

Ptr MADstrcpy( Ptr dst, const char* src)
{
	NSLog(CFSTR("MADstrcpy is depricated; just use strcpy"));
	long i = 0;
	
	do
	{
		dst[ i] = src[ i];
	}while( src[ i++]);
	
	return dst;
}

//TODO: use system native strcmp?
int MADstrcmp( const char *dst, const char* src)
{
	long i = 0;
	
	do
	{
		if( dst[ i] != src[ i]) return -1;
	}while( src[ i++]);
	
	return 0;
}

EXP void OSType2Ptr( OSType type, Ptr str)
{
#ifdef __LITTLE_ENDIAN__
	MOT32(&type);
#endif
	
	BlockMoveData( &type, str, 4);
	str[ 4] = 0;
}

EXP OSType Ptr2OSType( char* str)
{
	short 	i;
	OSType	type;
	
	i = strlen( str);
	if( i > 4) i = 4;
	type = '    ';
	BlockMoveData( str, &type, i);
#ifdef __LITTLE_ENDIAN__
	MOT32(&type);
#endif
	
	return type;
}
