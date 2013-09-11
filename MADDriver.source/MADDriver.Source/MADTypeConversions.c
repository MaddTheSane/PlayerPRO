//
//  MADTypeConversions.c
//  PPMacho
//
//  Created by C.W. Betts on 9/11/13.
//
//

#include "MADTypeConversions.h"
#include <iconv.h>

OSErr ConvertPlugInfoToUnicode(PPInfoRec infoIn, PPInfoRecU *infoOut)
{
	infoOut->fileSize = infoIn.fileSize;
	infoOut->partitionLength = infoIn.partitionLength;
	infoOut->signature = infoIn.signature;
	infoOut->totalInstruments = infoIn.totalInstruments;
	infoOut->totalPatterns = infoIn.totalPatterns;
	infoOut->totalTracks = infoIn.totalTracks;
	iconv_t convId = iconv_open("UTF-32LE", "MacRoman");
	char *charPtr = infoIn.formatDescription;
	char *wcharPtr = (char*)infoOut->formatDescription;
	size_t charLen = sizeof(infoIn.formatDescription), wcharLen = sizeof(infoOut->formatDescription);
	size_t outVal = iconv(convId, &charPtr, &charLen, &wcharPtr, &wcharLen);
	
	if (outVal == (size_t)-1) {
		iconv_close(convId);
		memset(infoOut, 0, sizeof(PPInfoRecU));
		return MADTextConversionErr;
	}
	
	charPtr = infoIn.internalFileName;
	wcharPtr = (char*)infoOut->internalFileName;
	charLen = sizeof(infoIn.internalFileName), wcharLen = sizeof(infoOut->internalFileName);
	outVal = iconv(convId, &charPtr, &charLen, &wcharPtr, &wcharLen);
	
	if (outVal == (size_t)-1) {
		iconv_close(convId);
		memset(infoOut, 0, sizeof(PPInfoRecU));
		return MADTextConversionErr;
	}
	
	iconv_close(convId);
	return noErr;
}

OSErr ConvertMusicToUnicode(MADMusic *musIn, MADMusicUnicode *musOut)
{
	
}

OSErr ConvertMusicFromUnicode(MADMusicUnicode *musIn, MADMusic *musOut)
{
	
}

