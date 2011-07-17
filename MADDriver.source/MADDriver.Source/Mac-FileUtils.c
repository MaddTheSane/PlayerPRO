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
	memmove( cStr, cStr + 1, strlen( cStr));
	cStr[ 0] = size;

	return (unsigned char*) cStr;
}

void MYP2CStr( unsigned char *cStr)
{
	long size = cStr[ 0];
	memmove( cStr + 1, cStr, size);
	cStr[ size] = 0;
}

UNFILE iFileOpen(Ptr name)
{
	UNFILE	temp;
	FSRef	ref;
	OSErr	iErr;
	FSPathMakeRef((UInt8*)name, &ref, FALSE);

	
	Boolean	UnusedBoolean, UnusedBoolean2;
	HFSUniStr255 whythis;
	FSGetDataForkName(&whythis);
	FSResolveAliasFile(&ref, TRUE, &UnusedBoolean, &UnusedBoolean2);
	iErr = FSOpenFork(&ref, whythis.length, whythis.unicode, fsCurPerm, &temp);
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

void iFileCreate(Ptr path, OSType type)
{
	char parentDir[PATH_MAX];
	char fileName[FILENAME_MAX];

	char *c;
	strncpy(parentDir, path, sizeof(parentDir));
	//strncpy(fileName, path, sizeof(fileName));

	c = (char*)parentDir;
	
	while (*c != '\0')     /* go to end */
	{
		c++;
	}
	
	while (*c != '/')      /* back up to parent */
	{
		c--;
	}
	strncpy(fileName, c, sizeof(fileName));
	*c++ = '\0';           /* cut off last part (binary name) */
	
	
	
	iFileCreateName(parentDir, fileName, type);

}

void iFileCreateName(Ptr folder, Ptr name, OSType type)
{
	FSRef			temp;
	CFStringRef		UniCFSTR = CFStringCreateWithCString(kCFAllocatorDefault, name, CFStringGetSystemEncoding());
	CFIndex			UNIcharLen = CFStringGetLength(UniCFSTR);
	UniChar*		UNICHARThing = calloc(UNIcharLen, sizeof(UniChar));
	CFRange			UNIRange;
	UNIRange.location	= 0;
	UNIRange.length		= UNIcharLen;
	CFStringGetCharacters(UniCFSTR, UNIRange, UNICHARThing);
	FSPathMakeRef((UInt8*)folder, &temp, FALSE);

	{
		FSRef maybeRef;

		FSMakeFSRefUnicode(&temp, UNIcharLen, UNICHARThing, kTextEncodingDefaultFormat, &maybeRef);
		
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

		FSCreateFileUnicode(&temp, UNIcharLen, UNICHARThing, kFSCatInfoFinderInfo, &fileCat, NULL, NULL);
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
	
	memmove( &type, str, 4);
	str[ 4] = 0;
}

EXP OSType Ptr2OSType( char* str)
{
	short 	i;
	OSType	type;
	
	i = strlen( str);
	if( i > 4) i = 4;
	type = '    ';
	memmove( str, &type, i);
#ifdef __LITTLE_ENDIAN__
	MOT32(&type);
#endif
	
	return type;
}
