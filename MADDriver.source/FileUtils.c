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
#include <fcntl.h>
#include "RDriver.h"
#include "MADFileUtils.h"
#include "MADPrivate.h"

#ifdef _MAC_H
#include <CoreFoundation/CFURL.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

void iFileCreate(const char *path, MADFourChar type)
{
#if defined _MAC_H && !TARGET_OS_IPHONE
	CFURLRef fileURL;
#endif
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
	
#ifdef WIN32
	{
		//Not the most elegant solution, but it should work.
		FILE *tmpFile = fopen(path, "wb");
		if (tmpFile != NULL) {
			fclose(tmpFile);
		}
	}
#else
	fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd == -1) {
		return;
	}
	close(fd);
#endif
	
#if defined _MAC_H && !TARGET_OS_IPHONE
	fileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8*)path, strlen(path), false);
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

MADErr iRead(long size, void *dest, FILE* iFileRefI)
{
	fread(dest, size, 1, iFileRefI);
	if (ferror(iFileRefI)) {
		return MADReadingErr;
	}
	
	return MADNoErr;
}

MADErr iSeekCur(long size, FILE* iFileRefI)
{
	return fseek(iFileRefI, size, SEEK_CUR) == 0 ? MADNoErr : MADReadingErr;
}

MADErr iWrite(long size, const void *src, FILE* iFileRefI)
{
	fwrite(src, size, 1, iFileRefI);
	if (ferror(iFileRefI)) {
		return MADWritingErr;
	}
	
	return MADNoErr;
}

void iClose(FILE* iFileRefI)
{
	fclose(iFileRefI);
}
