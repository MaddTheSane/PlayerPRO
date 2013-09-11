//
//  MADTypeConversions.c
//  PPMacho
//
//  Created by C.W. Betts on 9/11/13.
//
//

#include "MADTypeConversions.h"
#import <Foundation/Foundation.h>

OSErr ConvertPlugInfoToUnicode(PPInfoRec infoIn, PPInfoRecU *infoOut)
{
	@autoreleasepool {
		infoOut->fileSize = infoIn.fileSize;
		infoOut->partitionLength = infoIn.partitionLength;
		infoOut->signature = infoIn.signature;
		infoOut->totalInstruments = infoIn.totalInstruments;
		infoOut->totalPatterns = infoIn.totalPatterns;
		infoOut->totalTracks = infoIn.totalTracks;
		NSString * toConvString = [NSString stringWithCString:infoIn.formatDescription encoding:NSMacOSRomanStringEncoding];
		if (!toConvString) {
			return MADTextConversionErr;
		}
		if (![toConvString getCString:(char*)infoOut->formatDescription maxLength:sizeof(infoOut->formatDescription) encoding:NSUTF32LittleEndianStringEncoding])
			return MADTextConversionErr;
		toConvString = [NSString stringWithCString:infoIn.internalFileName encoding:NSMacOSRomanStringEncoding];
		if (![toConvString getCString:(char*)infoOut->internalFileName maxLength:sizeof(infoOut->internalFileName) encoding:NSUTF32LittleEndianStringEncoding])
			return MADTextConversionErr;
		
		return noErr;
	}
}

OSErr ConvertMusicToUnicode(MADMusic *musIn, MADMusicUnicode *musOut)
{
	@autoreleasepool {
		return MADOrderNotImplemented;
	}
	
}

OSErr ConvertMusicFromUnicode(MADMusicUnicode *musIn, MADMusic *musOut)
{
	@autoreleasepool {
		return MADOrderNotImplemented;
	}
}

