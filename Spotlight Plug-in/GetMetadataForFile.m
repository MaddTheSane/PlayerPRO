#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#include "MADDefs.h"
#include "MAD.h"
#include "RDriver.h"
#include "MADFileUtils.h"
#include "GetMetadataForFile.h"

static NSCharacterSet *ourSet;
static dispatch_once_t charSetOnce;
static dispatch_block_t charBlock = ^{
	NSMutableCharacterSet *tmpSet = [[NSCharacterSet whitespaceCharacterSet] mutableCopy];
	[tmpSet addCharactersInString:@"-."];
	ourSet = [tmpSet copy];
};

static inline NSString *StripStringOfSpaces(NSString *s, BOOL andDashes) NS_RETURNS_RETAINED;
NSString *StripStringOfSpaces(NSString *s, BOOL andDashes)
{
	dispatch_once(&charSetOnce, charBlock);
	if (s == nil)
		return nil;
	
	return [s stringByTrimmingCharactersInSet: andDashes ? ourSet : [NSCharacterSet whitespaceCharacterSet]];
}

static inline BOOL StringIsEmpty(NSString *s)
{
	NSString *copy;
	if (s == nil)
		return YES;
	
	if ([s isEqualTo:@""])
		return YES;
	
	copy = StripStringOfSpaces(s, YES);
	
	if ([copy isEqualTo:@""])
		return YES;
	
	return NO;
}

#define kPPMDInstumentsList		@"net_sourceforge_playerpro_tracker_instumentlist"
#define kPPMDPatternList		@"net_sourceforge_playerpro_tracker_patternlist"
#define kPPMDTotalPatterns		@"net_sourceforge_playerpro_tracker_totalpatterns"
#define kPPMDPartitionLength	@"net_sourceforge_playerpro_tracker_partitionlength"
#define kPPMDTotalInstruments	@"net_sourceforge_playerpro_tracker_totalinstruments"
#define kPPMDTotalTracks		@"net_sourceforge_playerpro_tracker_totaltracks"
#define kPPMDFormatDescription	@"net_sourceforge_playerpro_tracker_formatdescription"
#define kPPMDMADKInfo			@"net_sourceforge_playerpro_tracker_madkinfo"

/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForURL(void* thisInterface, CFMutableDictionaryRef attributes,
						  CFStringRef contentTypeUTI, CFURLRef urlForFile)
{
	@autoreleasepool {
		MADDriverRec		*MADDriver = NULL;
		MADMusic			*MADMusic1 = NULL;
		MADLibrary			*MADLib = NULL;
		MADDriverSettings	init = {0};
		NSMutableDictionary *NSattribs = (__bridge NSMutableDictionary*)attributes;
		
		//Before we do anything else, check to make sure it's not the Windows file winoldap.mod
		//This file seems to crash the metadata importer, even though
		//the proper PlayerPRO plug-in should say that it can't open it.
		{
			NSURL		*NSFileURL = (__bridge NSURL*)urlForFile;
			NSString	*lastPathName = [NSFileURL lastPathComponent];
			NSComparisonResult result = [lastPathName compare:@"winoldap.mod" options:NSCaseInsensitiveSearch | NSWidthInsensitiveSearch];
			if (result == NSOrderedSame) {
				return FALSE;
			}
		}
		
		/* Pull any available metadata from the file at the specified path */
		/* Return the attribute keys and attribute values in the dict */
		/* Return TRUE if successful, FALSE if there was no data provided */
		
		MADGetBestDriver(&init);
		init.driverMode = NoHardwareDriver;
		
		if (MADInitLibrary(NULL, &MADLib) != MADNoErr) {
			return FALSE;
		} else if (MADCreateDriver(&init, MADLib, &MADDriver) != MADNoErr) {
			MADDisposeLibrary(MADLib);
			return FALSE;
		}
		
		{
			char type[5] = {0};
			char utiType[5] = {0};
			OSType info = 0;
			NSString *ostypes;
			
			//Try to get the OSType of the UTI.
			ostypes = CFBridgingRelease(UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType));
			
			info = UTGetOSTypeFromString((__bridge CFStringRef)(ostypes));
			if (info)
				OSType2Ptr(info, utiType);
			else
				strcpy(utiType, "!!!!");
			
			if (MADMusicIdentifyCFURL(MADLib, type, urlForFile) != MADNoErr) {
				//Couldn't identify via raw file, try by UTI
				strcpy(type, utiType);
			}
			
#ifdef DEBUG
			if (strcmp(utiType, "!!!!") == 0) {
				printf("PPImporter: Unable to determine file type based on UTI.\n");
			} else if (strcmp(utiType, type) != 0) {
				printf("PPImporter: File types differ, UTI says %s, PlayerPRO says %s.", utiType, type);
			}
#endif
			
			if (MADPlugAvailable(MADLib, type)) {
				OSErr err = MADNoErr;
				err = MADLoadMusicCFURLFile(MADLib, &MADMusic1, type, urlForFile);
				if (err != MADNoErr) {
					goto fail1;
				}
			} else {
				goto fail1;
			}
			
			{
				//Get info
				//Note that most trackers don't have an info field, so most will be "Converted by PlayerPRO..."
				//Hence why we're only letting the MADK tracker show it.
				
				NSString *infoString = [[NSString alloc] initWithCString:MADMusic1->header->infos encoding:NSMacOSRomanStringEncoding];
				if (infoString) {
					NSattribs[kPPMDMADKInfo] = infoString;
				}
			}
			
			NSString *title;
			{
				MADInfoRec rec;
				{
					if (MADMusicInfoCFURL(MADLib, type, urlForFile, &rec) != MADNoErr)
						goto skipInfo;
					NSString *NSSig = CFBridgingRelease(UTCreateStringForOSType(rec.signature));
					if (!NSSig) {
						char sig[5];
						OSType2Ptr(rec.signature, sig);
						NSSig = [[NSString alloc] initWithCString:sig encoding:NSMacOSRomanStringEncoding];
					} else if (!NSSig) {
						NSSig = [[NSString alloc] initWithFormat:@"0x%08X", (unsigned int)rec.signature];
					}
					NSattribs[(NSString*)kMDItemCodecs] = @[NSSig];
				}
				//Set the title metadata
				
				title = [[NSString alloc] initWithCString:rec.internalFileName encoding:NSMacOSRomanStringEncoding];
				if (title != nil) {
					NSattribs[(NSString*)kMDItemTitle] = StripStringOfSpaces(title, NO);
				}
				
				NSattribs[kPPMDTotalPatterns] = @(rec.totalPatterns);
				NSattribs[kPPMDPartitionLength] = @(rec.partitionLength);
				NSattribs[kPPMDTotalInstruments] = @(rec.totalInstruments);
				NSattribs[kPPMDTotalTracks] = @(rec.totalTracks);
				{
					NSString *FormatDes = [[NSString alloc] initWithCString:rec.formatDescription encoding:NSMacOSRomanStringEncoding];
					NSattribs[kPPMDFormatDescription] = FormatDes;
				}
			}
		skipInfo:
			if (!title) {
				title = [[NSString alloc] initWithCString:MADMusic1->header->name encoding:NSMacOSRomanStringEncoding];
				if (title != nil) {
					NSattribs[(NSString*)kMDItemTitle] = title;
				}
			}
		}
		
		{
			//Set duration metadata
			MADAttachDriverToMusic(MADDriver, MADMusic1, NULL);
			long fT, cT;
			MADGetMusicStatus(MADDriver, &fT, &cT);
			double fTd = fT / 60.0;
			
			NSattribs[(NSString*)kMDItemDurationSeconds] = @(fTd);
		}
		
		{
			NSMutableArray *InstruArray = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU * (MAXSAMPLE + 1)];
			
			for (int i = 0; i < MAXINSTRU; i++) {
				InstrData *tempData = &MADMusic1->fid[i];
				NSString *temp = [[NSString alloc] initWithCString:tempData->name encoding:NSMacOSRomanStringEncoding];
				if (!StringIsEmpty(temp)) {
					[InstruArray addObject:temp];
				}
				
				int sDataCount = tempData->firstSample + tempData->numSamples;
				for (int x = tempData->firstSample; x < sDataCount; x++) {
					sData *tempSData = MADMusic1->sample[x];
					temp = [[NSString alloc] initWithCString:tempSData->name encoding:NSMacOSRomanStringEncoding];
					if (!StringIsEmpty(temp)) {
						[InstruArray addObject:temp];
					}
				}
			}
			
			NSattribs[kPPMDInstumentsList] = InstruArray;
		}
		
		{
			NSMutableArray *PatArray = [[NSMutableArray alloc] initWithCapacity:MAXPATTERN];
			for (int i = 0; i < MAXPATTERN; i++) {
				if (MADMusic1->partition != NULL && MADMusic1->partition[i] != NULL) {
					NSString *temp = [[NSString alloc] initWithCString:MADMusic1->partition[i]->header.name encoding:NSMacOSRomanStringEncoding];
					if (!StringIsEmpty(temp))
						[PatArray addObject:temp];
				}
			}
			
			if ([PatArray count] > 0) {
				NSattribs[kPPMDPatternList] = PatArray;
			}
		}
		
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&MADMusic1, MADDriver);		// Dispose the current music
		MADStopDriver(MADDriver);					// Stop driver interrupt function
		MADDisposeDriver(MADDriver);				// Dispose music driver
		MADDisposeLibrary(MADLib);					// Close music library
		return TRUE;
		
	fail1:
		MADStopDriver(MADDriver);				// Stop driver interrupt function
		MADDisposeDriver(MADDriver);			// Dispose music driver
		MADDisposeLibrary(MADLib);				// Close music library
		
		return FALSE;
	}
}
