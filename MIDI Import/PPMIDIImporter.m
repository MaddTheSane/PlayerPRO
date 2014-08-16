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

void ConvertMidiFile(const char *src, MADMusic *theMAD, MADDriverSettings *init);

@implementation PPMIDIImporter

+ (MADErr)fillData:(NSMutableData*)theData withMusic:(MADMusic*)music
{
	int		alpha = 0;
	int		i, x;
	size_t	inOutCount;
	MADErr	theErr = MADNoErr;
	MADSpec aHeader;
	
	if (music->musicUnderModification)
		return MADWritingErr;
	
	//TODO: error-checking
	
	music->musicUnderModification = TRUE;
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		music->fid[i].no = i;
		
		// Is there something in this instrument?
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {
			x++;
		}
	}
	
	music->header->numInstru = x;
	
	aHeader = *music->header;
	[theData appendBytes:&aHeader length:sizeof(MADSpec)];
	
	for (i = 0; i < music->header->numPat; i++) {
		if (music->partition[i]) {
			PatData *tmpPat;
			inOutCount = sizeof(PatHeader);
			inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
			tmpPat = calloc(inOutCount, 1);
			memcpy(tmpPat, music->partition[i], inOutCount);
			tmpPat->header.compMode = 'NONE';
			[theData appendBytes:tmpPat length:inOutCount];
			free(tmpPat);
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			InstrData instData;
			music->fid[i].no = i;
			instData = music->fid[i];
			[theData appendBytes:&instData length:sizeof(InstrData)];
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			void	*dataCopy = NULL;
			curData = *music->sample[music->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[theData appendBytes:&copyData length:inOutCount];
			
			inOutCount = music->sample[music->fid[i].firstSample + x]->size;
			dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			[theData appendBytes:dataCopy length:inOutCount];
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	for (i = 0; i < 10 ; i++) {	// Global Effects
		if (music->header->globalEffect[i]) {
			FXSets aSet;
			inOutCount = sizeof(FXSets);
			aSet = music->sets[alpha];
			[theData appendBytes:&aSet length:inOutCount];
			alpha++;
		}
	}
	
	for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				FXSets aSet;
				inOutCount = sizeof(FXSets);
				aSet = music->sets[alpha];
				[theData appendBytes:&aSet length:inOutCount];
				alpha++;
			}
		}
	}
	
	music->header->numInstru = MAXINSTRU;
	
	return theErr;
}

- (void)importMIDIFileAtURL:(NSURL*)theURL withReply:(void (^)(NSData *, MADErr error))reply
{
	MADErr theErr = MADNoErr;
	MADDriverSettings init = {0};
	MADMusic MadFile = {0};
	NSMutableData *madData = [[NSMutableData alloc] initWithCapacity:128];
	NSData *fileData = [[NSData alloc] initWithContentsOfURL:theURL options:NSDataReadingMappedIfSafe error:NULL];
	if (!fileData) {
		reply(nil, theErr);
		return;
	}
	const void *AlienFile = [fileData bytes];
	theErr = TestMIDIFile(AlienFile);
	if (theErr != MADNoErr) {
		[fileData release];
		reply(nil, theErr);
		return;
	}
	ConvertMidiFile(AlienFile, &MadFile, &init);
	[fileData release];
	
	theErr = [PPMIDIImporter fillData:madData withMusic:&MadFile];
	
	reply(madData, theErr);
	[madData autorelease];
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
		PPInfoDict = [@{@kPPTotalPatterns: @(theInfo.totalPatterns),
						@kPPPartitionLength: @(theInfo.partitionLength),
						@kPPFileSize: @(theInfo.fileSize),
						@kPPSignature: @(theInfo.signature),
						@kPPTotalTracks: @(theInfo.totalTracks),
						@kPPTotalInstruments: @(theInfo.totalInstruments),
						@kPPInternalFileName: [NSString stringWithCString:theInfo.internalFileName encoding:NSMacOSRomanStringEncoding],
						@kPPFormatDescription: [NSString stringWithCString:theInfo.formatDescription encoding:NSMacOSRomanStringEncoding]} retain];
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


