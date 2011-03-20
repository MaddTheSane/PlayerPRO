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

extern void NSLog(CFStringRef format, ...);
 
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

UNFILEName iFileNameOpen (Ptr name)
{
	UNFILEName temp;
	FSPathMakeRef((UInt8)name, temp, FALSE);
	return temp;
}

UNFILE iFileOpen(UNFILEName name)
{
	UNFILE	temp;
	OSErr	iErr;
	
	Boolean	UnusedBoolean, UnusedBoolean2;
	HFSUniStr255 whythis;
	FSGetDataForkName(&whythis);
	FSResolveAliasFile(name, TRUE, &UnusedBoolean, &UnusedBoolean2);
	iErr = FSOpenFork(name, whythis.length, whythis.unicode, fsCurPerm, &temp);
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

void iFileCreate(UNFILEName folder, Ptr name, OSType type)
{
	CFStringRef		UniCFSTR = CFStringCreateWithCString(kCFAllocatorDefault, name, CFStringGetSystemEncoding());
	CFIndex			UNIcharLen = CFStringGetLength(UniCFSTR);
	UniChar*		UNICHARThing = calloc(UNIcharLen, sizeof(UniChar));
	CFRange			UNIRange;
	UNIRange.location	= 0;
	UNIRange.length		= UNIcharLen;
	CFStringGetCharacters(UniCFSTR, UNIRange, UNICHARThing);

	{
		FSRef maybeRef;

		FSMakeFSRefUnicode(folder, UNIcharLen, UNICHARThing, kTextEncodingDefaultFormat, &maybeRef);
		
		FSDeleteObject(&maybeRef);
	}
	{
		FSCatalogInfo	fileCat;
		FileInfo		*fInfo = (FileInfo*)&fileCat.finderInfo;	
		fInfo->fileType		 = type;
		fInfo->fileCreator	 = 'SNPL';
		fInfo->finderFlags	 = 0;
		fInfo->location.h	 = 0;
		fInfo->location.v	 = 0;
		fInfo->reservedField = 0;

		FSCreateFileUnicode(folder, UNIcharLen, UNICHARThing, kFSCatInfoFinderInfo, &fileCat, NULL, NULL);
	}
		
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
