//
//  PPKSwiftToC.m
//  PPMacho
//
//  Created by C.W. Betts on 1/12/15.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPSampleObject.h"
#import "PPConstants.h"

static NSString* OSTypeToNSString(OSType theOSType)
{
#if !(TARGET_OS_IPHONE || TARGET_OS_TV)
	NSString *checkForValid = CFBridgingRelease(UTCreateStringForOSType(theOSType));
#else
	char ourOSType[5] = {0};
	
	OSType2Ptr(theOSType, ourOSType);
	NSString *checkForValid = [[NSString alloc] initWithBytes:ourOSType length:4 encoding:NSMacOSRomanStringEncoding];
#endif
	
	if (!checkForValid) {
		checkForValid = [[NSString alloc] initWithFormat:@"0x%X", (unsigned int)theOSType];
	}
	
	return checkForValid;
}

short PPNoteFromString(NSString *aNote)
{
	return [PPSampleObject noteFromString:aNote];
}

NSString *PPStringFromNote(short note)
{
	return [PPSampleObject octaveNameFromNote:note];
}

NSDictionary *PPInfoRecToDictionary(MADInfoRec infoRec)
{
	return @{kPPTotalPatterns:		@(infoRec.totalPatterns),
			 kPPPartitionLength:	@(infoRec.partitionLength),
			 kPPFileSize:			@(infoRec.fileSize),
			 kPPSignature:			OSTypeToNSString(infoRec.signature),
			 kPPTotalTracks:		@(infoRec.totalTracks),
			 kPPTotalInstruments:	@(infoRec.totalInstruments),
			 kPPInternalFileName:	[NSString stringWithCString:infoRec.internalFileName encoding:NSMacOSRomanStringEncoding],
			 kPPFormatDescription:	[NSString stringWithCString:infoRec.formatDescription encoding:NSMacOSRomanStringEncoding]};
}
