//
//  PPMIDIImportHelper.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import "PPMIDIImporter.h"

#include <PlayerPROCore/PlayerPROCore.h>
#include <string.h>

#ifdef __LP64__
#error this code will only work on 32-bit mode
#error this code needs to be rewritten.
#endif

/**************************************************************************
 **************************************************************************/

static unsigned char* MYC2PStr(Ptr cStr)
{
	long length = strlen(cStr);
	memmove(cStr + 1, cStr, length);
	cStr[0] = length;
	return (unsigned char*)cStr;
}

static inline bool compMem(const void *a, const void *b, size_t s)
{
	return memcmp(a, b, s) == 0;
}

static MADErr TestMIDIFile(const void *AlienFile)
{
	if (compMem(AlienFile, "MThd", 4))
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static MADErr ExtractMIDIInfo(PPInfoRec *info, const void *theMIDI)
{
	info->signature = 'Midi';
	strcpy(info->internalFileName, "");
	info->totalPatterns = 0;
	info->partitionLength = 0;
	info->totalInstruments = 0;
	info->totalTracks = 0;
	strcpy(info->formatDescription, "Midi Plug");
	
	return MADNoErr;
}

void CreateResult(Ptr aPtr)
{
	MYC2PStr(aPtr);
	DebugStr((unsigned char *) aPtr);
}

void ConvertMidiFile(char *src, MADMusic *theMAD, MADDriverSettings *init);


@implementation PPMIDIImporter

- (void)importMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(NSData *, MADErr error))reply
{
	
}

- (void)getMIDIInfoFromFileAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary *, MADErr error))reply
{
	PPInfoRec theInfo = {0};
	MADErr theErr = MADNoErr;
	NSDictionary *fileInfo = [theURL resourceValuesForKeys:@[NSURLFileSizeKey] error:NULL];
	if (!fileInfo) {
		reply(nil, MADReadingErr);
		return;
	}
	NSData *fileData = [[NSData alloc] initWithContentsOfURL:theURL options:NSDataReadingMappedIfSafe error:NULL];
	if (!fileData) {
		reply(nil, MADReadingErr);
		return;
	}
	theErr = ExtractMIDIInfo(&theInfo, [fileData bytes]);
	[fileData release];
	if (theErr) {
		reply(nil, theErr);
		return;
	}
	theInfo.fileSize = [[fileInfo objectForKey:NSURLFileSizeKey] longValue];
	NSDictionary *PPInfoDict;
	@autoreleasepool {
		PPInfoDict = [@{kPPTotalPatterns: @(theInfo.totalPatterns),
						kPPPartitionLength: @(theInfo.partitionLength),
						kPPFileSize: @(theInfo.fileSize),
						kPPSignature: @(theInfo.signature),
						kPPTotalTracks: @(theInfo.totalTracks),
						kPPTotalInstruments: @(theInfo.totalInstruments),
						kPPInternalFileName: [NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding],
						kPPFormatDescription: [NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]} retain];
	}
	reply(PPInfoDict, MADNoErr);
	[PPInfoDict autorelease];
}

- (void)canImportMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(MADErr error))reply
{
	MADErr myErr = noErr;
	NSData *fileData = [[NSData alloc] initWithContentsOfURL:theURL options:NSDataReadingMappedIfSafe error:NULL];

	myErr = TestMIDIFile([fileData bytes]);
	[fileData release];
	reply(myErr);
}

@end
