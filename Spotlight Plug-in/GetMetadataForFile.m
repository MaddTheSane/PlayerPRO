#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#include "MADDefs.h"
#include "MAD.h"
#include "RDriver.h"
#include "MADFileUtils.h"
#include "GetMetadataForFile.h"

static NSString *utf8OrMacRoman(const char *text) NS_RETURNS_RETAINED;
NSString *utf8OrMacRoman(const char *text)
{
	if (!text) {
		return nil;
	}
	if (memcmp(text, "\xEF\xBB\xBF", 3) == 0) {
		NSString *uniStr = [[NSString alloc] initWithUTF8String:text];
		if (uniStr) {
			return uniStr;
		}
	}
	return [[NSString alloc] initWithCString:text encoding:NSMacOSRomanStringEncoding];
}

static NSString *StripStringOfSpaces(NSString *s, BOOL andDashes);
NSString *StripStringOfSpaces(NSString *s, BOOL andDashes)
{
	static NSCharacterSet *ourSet;
	static dispatch_once_t charSetOnce;
	dispatch_once(&charSetOnce, ^{
		NSMutableCharacterSet *tmpSet = [[NSCharacterSet whitespaceCharacterSet] mutableCopy];
		[tmpSet addCharactersInString:@"-."];
		ourSet = [tmpSet copy];
	});
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

static const char *typeBasedOnUTI(NSString* theUTI, MADLibrary *theLib) {
	if ([theUTI isEqualToString:@"com.quadmation.playerpro.madk"]) {
		return "MADK";
	} else if ([theUTI isEqualToString:@"com.quadmation.playerpro.mad"]) {
		// This can happen if a file just ends with .mad
		return MADCouldNotFindPlugValue;
	}
	
	for (int i = 0; i < theLib->TotalPlug; i++) {
		for (NSString *aUTI in (__bridge NSArray*)theLib->ThePlug[i].UTItypes) {
			if ([aUTI isEqualToString:theUTI]) {
				return theLib->ThePlug[i].type;
			}
		}
	}
	
	return MADCouldNotFindPlugValue;
}

static MADDriverRec	*MADDriver = NULL;
static MADLibrary	*MADLib = NULL;


static void SetupMADLib(void) __attribute__((constructor));
void SetupMADLib(void)
{
	MADDriverSettings	init = {0};
	
	MADGetBestDriver(&init);
	init.driverMode = NoHardwareDriver;
	
	if (MADInitLibrary(NULL, &MADLib) != MADNoErr) {
		abort();
	} else if (MADCreateDriver(&init, MADLib, &MADDriver) != MADNoErr) {
		MADDisposeLibrary(MADLib);
		abort();
	}
}

static void closeMADLib(void) __attribute__((destructor));
void closeMADLib(void)
{
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
}

/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForURL(void* thisInterface, CFMutableDictionaryRef attributes,
						  CFStringRef contentTypeUTI, CFURLRef urlForFile)
{
	@autoreleasepool {
		// Before we do anything else, check to make sure it's not the Windows file winoldap.mod
		// This file seems to crash the metadata importer, even though
		// the proper PlayerPRO plug-in (MOD) should say that it can't open it.
		{
			NSString *lastPathName = CFBridgingRelease(CFURLCopyLastPathComponent(urlForFile));
			NSComparisonResult result = [lastPathName compare:@"winoldap.mod" options:NSCaseInsensitiveSearch | NSWidthInsensitiveSearch];
			if (result == NSOrderedSame) {
				return FALSE;
			}
		}
		
		MADMusic			*MADMusic1 = NULL;
		NSMutableDictionary *NSattribs = (__bridge NSMutableDictionary*)attributes;

		{
			char type[5] = {0};
			
			//first, iterate over the the library UTIs.
			const char *whutType = typeBasedOnUTI((__bridge NSString *)(contentTypeUTI), MADLib);
			
			strcpy(type, whutType);
			if (strcmp(type, MADCouldNotFindPlugValue) == 0) {
				OSType info = 0;
				// We couldn't identify it based off of the UTI that way...
				// So try via direct UTI access
				char utiType[5] = {0};
				NSString *ostypes = CFBridgingRelease(UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType));
				
				info = UTGetOSTypeFromString((__bridge CFStringRef)(ostypes));
				if (info)
					OSType2Ptr(info, utiType);
				else
					strcpy(utiType, MADCouldNotFindPlugValue);
				// But is it still valid?
				if (MADPlugAvailable(MADLib, utiType)) {
					strcpy(type, utiType);
				} else {
					// NOW we use MADMusicIdentifyCFURL
					if (MADMusicIdentifyCFURL(MADLib, type, urlForFile) != MADNoErr) {
						// We failed :(
						goto fail1;
					}
				}
			}
			
			if (MADPlugAvailable(MADLib, type)) {
				MADErr err = MADNoErr;
				err = MADLoadMusicCFURLFile(MADLib, &MADMusic1, type, urlForFile);
				if (err != MADNoErr) {
					// wait! The file type might be incorrect!
					err = MADMusicIdentifyCFURL(MADLib, type, urlForFile);
					if (err == MADNoErr) {
						// It says we can open it...
						err = MADLoadMusicCFURLFile(MADLib, &MADMusic1, type, urlForFile);
						if (err == MADNoErr) {
							// ...and we can!
							goto salvageable;
						}
					}
					goto fail1;
				}
			} else {
				goto fail1;
			}
			
		salvageable:
			{
				// Get info
				// Note that most trackers don't have an info field, so most will be "Converted by PlayerPRO..."
				// Hence why we're only letting the MADK tracker show it.
				
				NSString *infoString = utf8OrMacRoman(MADMusic1->header->infos);
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
					}
					if (!NSSig) {
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
					NSattribs[(NSString*)kMDItemTitle] = StripStringOfSpaces(title, NO);
				}
			}
		}
		
		//Set duration metadata
		{
			MADAttachDriverToMusic(MADDriver, MADMusic1, NULL);
			long fT, cT;
			MADGetMusicStatus(MADDriver, &fT, &cT);
			double fTd = fT / 60.0;
			
			NSattribs[(NSString*)kMDItemDurationSeconds] = @(fTd);
		}
		
		// Instrument and sample names
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
			
			NSattribs[kPPMDInstumentsList] = [InstruArray copy];
		}
		
		// Pattern names
		{
			NSMutableArray *PatArray = [[NSMutableArray alloc] initWithCapacity:MAXPATTERN];
			for (int i = 0; i < MAXPATTERN; i++) {
				if (MADMusic1->partition[i] != NULL) {
					NSString *temp = [[NSString alloc] initWithCString:MADMusic1->partition[i]->header.name encoding:NSMacOSRomanStringEncoding];
					if (!StringIsEmpty(temp))
						[PatArray addObject:temp];
				}
			}
			
			if ([PatArray count] > 0) {
				NSattribs[kPPMDPatternList] = [PatArray copy];
			}
		}
		
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&MADMusic1, MADDriver);		// Dispose the current music
		MADStopDriver(MADDriver);					// Stop driver interrupt function
		return TRUE;
		
	fail1:
		MADStopDriver(MADDriver);				// Stop driver interrupt function
		
		return FALSE;
	}
}
