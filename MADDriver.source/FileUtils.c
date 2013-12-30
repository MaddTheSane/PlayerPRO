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

//Needed to quiet a few warnings on Windows.
#define _CRT_SECURE_NO_WARNINGS 1
#include <errno.h>
#include "RDriver.h"
#include "FileUtils.h"
#include "PPPrivate.h"

#ifdef _MAC_H
#include <CoreFoundation/CFURL.h>
#endif

void iFileCreate(const char *path, OSType type)
{
	int status = 0;
	int fd = 0;
	errno = 0;
	status = remove(path);
	if (status == -1) {
		switch (errno) {
				//We're fine if there's no file there.
			case ENOENT:
				errno = 0;
				break;
				
			default:
				fprintf(stderr, "Error encountered deleting %s: %s", path, strerror(errno));
				errno = 0;
				return;
				break;
		}
	}
	
	//FIXME: does this work on Windows?
	fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd == -1) {
		return;
	}
	close(fd);

#if defined _MAC_H && !TARGET_OS_IPHONE
	CFURLRef fileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8*)path, strlen(path), false);
	SetOSType(fileURL, type);
	CFRelease(fileURL);
#endif
}

FILE* iFileOpen(const char *name)
{
	return iFileOpenRead(name);
}

FILE* iFileOpenRead(const char *name)
{
	return fopen(name, "rb");
}

FILE* iFileOpenWrite(const char *name)
{
	return fopen(name, "wb");
}

long iGetEOF(FILE* iFileRefI)
{
	long curEOF, oldPos;
	
	oldPos = ftell(iFileRefI);
	fseek(iFileRefI, 0, SEEK_END);
	curEOF = ftell(iFileRefI);
	fseek(iFileRefI, oldPos, SEEK_SET);
	
	return curEOF;
}

OSErr iRead(long size, void *dest, FILE* iFileRefI)
{
	fread(dest, size, 1, iFileRefI);
	
	return noErr;
}

OSErr iSeekCur(long size, FILE* iFileRefI)
{
	return fseek(iFileRefI, size, SEEK_CUR);
}

OSErr iWrite(long size, const void *src, FILE* iFileRefI)
{
	fwrite(src, size, 1, iFileRefI);
	
	return noErr;
}

void iClose(FILE* iFileRefI)
{
	fclose(iFileRefI);
}
