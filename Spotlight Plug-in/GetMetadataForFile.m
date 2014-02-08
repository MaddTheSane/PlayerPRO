#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#import <PlayerPROKit/PPMusicObject.h>
#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPPatternObject.h>
#include "RDriver.h"
#include "FileUtils.h"
#include "GetMetadataForFile.h"

@interface NSString (PPextras)
+ (BOOL)PPstringIsEmpty:(NSString *)s;
@end

@implementation NSString (PPextras)
+ (BOOL)PPstringIsEmpty:(NSString *)s
{
	NSString *copy;
	NSMutableCharacterSet *mutSet;
	if (s == nil)
		return YES;
	
	if ([s isEqualTo:@""])
		return YES;
	
	mutSet = [[NSCharacterSet whitespaceCharacterSet] mutableCopy];
	[mutSet addCharactersInString:@"-"];
	copy = [s stringByTrimmingCharactersInSet:mutSet];
	
	if ([copy isEqualTo:@""])
		return YES;
	
	return NO;
}
@end

#define kPPMDInstumentsList @"net_sourceforge_playerpro_tracker_instumentlist"
#define kPPMDPatternList @"net_sourceforge_playerpro_tracker_patternlist"
#define kPPMDTotalPatterns @"net_sourceforge_playerpro_tracker_totalpatterns"
#define kPPMDPartitionLength @"net_sourceforge_playerpro_tracker_partitionlength"
#define kPPMDTotalInstruments @"net_sourceforge_playerpro_tracker_totalinstruments"
#define kPPMDTotalTracks @"net_sourceforge_playerpro_tracker_totaltracks"
#define kPPMDFormatDescription @"net_sourceforge_playerpro_tracker_formatdescription"
#define kPPMDMADKInfo @"net_sourceforge_playerpro_tracker_madkinfo"

static Boolean GetMetadataForPackage(NSMutableDictionary *attributes, NSURL *pathToFile);

/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForURL(void* thisInterface, CFMutableDictionaryRef attributes,
						  CFStringRef contentTypeUTI, CFURLRef urlForFile)
{
	@autoreleasepool {
		MADDriverRec		*MADDriver;
		MADMusic			*MADMusic1;
		MADLibrary			*MADLib;
		MADDriverSettings	init;
		NSMutableDictionary *NSattribs = (__bridge NSMutableDictionary*)attributes;
		NSURL *NSFileURL = (__bridge NSURL*)urlForFile;
		
		//Before we do anything else, check to make sure it's not the Windows file winoldap.mod
		//This file seems to crash the metadata importer, even though
		//the proper PlayerPRO plug-in should say that it can't open it.
		{
			NSString *lastPathName = [NSFileURL lastPathComponent];
			NSComparisonResult result = [lastPathName compare:@"winoldap.mod" options:NSCaseInsensitiveSearch | NSWidthInsensitiveSearch];
			if (result == NSOrderedSame)
				return FALSE;
		}
		
		/* Pull any available metadata from the file at the specified path */
		/* Return the attribute keys and attribute values in the dict */
		/* Return TRUE if successful, FALSE if there was no data provided */
		if (UTTypeConformsTo(contentTypeUTI, CFSTR("net.sourceforge.playerpro.mad-bundle"))) {
			return GetMetadataForPackage(NSattribs, NSFileURL);
		}
		
		MADGetBestDriver(&init);
		init.driverMode = NoHardwareDriver;
		
		if (MADInitLibrary(NULL, &MADLib) != noErr) {
			return FALSE;
		} else if (MADCreateDriver(&init, MADLib, &MADDriver) != noErr) {
			MADDisposeLibrary(MADLib);
			return FALSE;
		}
		
		{
			char type[5];
			char utiType[5] = {0};
			OSType info;
			CFStringRef ostypes;
			
			//Try to get the OSType of the UTI.
			ostypes = UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType);
			
			info = UTGetOSTypeFromString(ostypes);
			if (ostypes)
				CFRelease(ostypes);
			if (info)
				OSType2Ptr(info, utiType);
			else
				strcpy(utiType, "!!!!");
			
			if (MADMusicIdentifyCFURL(MADLib, type, urlForFile) != noErr) {
				//Couldn't identify via raw file, try by UTI
				strcpy(type, utiType);
			}
			
#ifdef DEBUG
			if (strcmp(utiType, "!!!!") == 0) {
				NSLog(@"PPImporter: Unable to determine file type based on UTI.");
			} else if (strcmp(utiType, type) != 0) {
				NSLog(@"PPImporter: File types differ, UTI says %s, PlayerPRO says %s.", utiType, type);
			}
#endif
			
			if (MADPlugAvailable(MADLib, type)) {
				OSErr err = noErr;
				err = MADLoadMusicCFURLFile(MADLib, &MADMusic1, type, urlForFile);
				if (err != noErr) {
					goto fail1;
				}
			} else
				goto fail1;
			
			{
				//Get info
				//Note that most trackers don't have an info field, so most will be "Converted by PlayerPRO..."
				//Hence why we're only letting the MADK tracker show it.
				
				NSString *infoString = [[NSString alloc] initWithCString:MADMusic1->header->infos encoding:NSMacOSRomanStringEncoding];
				if (!infoString)
					infoString = @"";
				
				NSattribs[kPPMDMADKInfo] = infoString;
				
			}
			
			NSString *title;
			{
				PPInfoRec rec;
				{
					char sig[5];
					if (MADMusicInfoCFURL(MADLib, type, urlForFile, &rec) != noErr)
						goto skipInfo;
					OSType2Ptr(rec.signature, sig);
					NSString *NSSig = [[NSString alloc] initWithCString:sig encoding:NSMacOSRomanStringEncoding];
					if (!NSSig) {
						NSSig = [[NSString alloc] initWithFormat:@"0x%08X", (unsigned int)rec.signature];
					}
					NSattribs[(NSString*)kMDItemCodecs] = @[NSSig];
				}
				//Set the title metadata
				
				title = [[NSString alloc] initWithCString:rec.internalFileName encoding:NSMacOSRomanStringEncoding];
				NSattribs[(NSString*)kMDItemTitle] = title;
				
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
				NSattribs[(NSString*)kMDItemTitle] = title;
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
				if (![NSString PPstringIsEmpty:temp])
					[InstruArray addObject:temp];
				
				int sDataCount = tempData->firstSample + tempData->numSamples;
				for (int x = tempData->firstSample; x < sDataCount; x++) {
					sData *tempSData = MADMusic1->sample[x];
					temp = [[NSString alloc] initWithCString:tempSData->name encoding:NSMacOSRomanStringEncoding];
					if (![NSString PPstringIsEmpty:temp])
						[InstruArray addObject:temp];
				}
			}
			
			NSattribs[kPPMDInstumentsList] = [InstruArray copy];
		}
		
		{
			NSMutableArray *PatArray = [[NSMutableArray alloc] initWithCapacity:MAXPATTERN];
			for (int i = 0; i < MAXPATTERN; i++) {
				if (MADMusic1->partition != NULL && MADMusic1->partition[i] != NULL) {
					NSString *temp = [[NSString alloc] initWithCString:MADMusic1->partition[i]->header.name encoding:NSMacOSRomanStringEncoding];
					if (![NSString PPstringIsEmpty:temp])
						[PatArray addObject:temp];
				}
			}
			NSattribs[kPPMDPatternList] = [PatArray copy];
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

static Boolean GetMetadataForPackage(NSMutableDictionary *attributes, NSURL *pathToFile)
{
	PPMusicObjectWrapper *musFile = [[PPMusicObjectWrapper alloc] initWithURL:pathToFile];
	if (!musFile) {
		return FALSE;
	}
	attributes[(NSString*)kMDItemTitle] = musFile.internalFileName;
	attributes[(NSString*)kMDItemAuthors] = @[musFile.madAuthor];
	{
		OSType codecType = musFile.madType;
		char cType[5];
		OSType2Ptr(codecType, cType);
		NSString *theString = [[NSString alloc] initWithCString:cType encoding:NSMacOSRomanStringEncoding];
		if (!theString) {
			theString = [[NSString alloc] initWithFormat:@"0x%08X", (unsigned int)codecType];
		}
		attributes[(NSString*)kMDItemCodecs] = @[theString];
	}
	attributes[kPPMDPartitionLength] = @(musFile.partitionLength);
	attributes[kPPMDTotalPatterns] = @(musFile.totalPatterns);
	attributes[kPPMDPartitionLength] = @(musFile.totalPartitions);
	attributes[kPPMDTotalInstruments] = @(musFile.totalInstruments);
	attributes[kPPMDTotalTracks] = @(musFile.totalTracks);
	
	attributes[kPPMDFormatDescription] = @"MAD Bundle"; //TODO: localize?
	attributes[kPPMDMADKInfo] = musFile.madInfo;
	
	NSMutableArray *tmpArray = [[NSMutableArray alloc] initWithCapacity:MAXINSTRU * (MAXSAMPLE + 1)];
	for (PPInstrumentObject *insObj in musFile.instruments) {
		NSString *tmpStr = insObj.name;
		if (![NSString PPstringIsEmpty:tmpStr]) {
			[tmpArray addObject:tmpStr];
		}
		for (PPSampleObject *sampObj in insObj.samples) {
			tmpStr = sampObj.name;
			if (![NSString PPstringIsEmpty:tmpStr]) {
				[tmpArray addObject:tmpStr];
			}
		}
	}
	attributes[kPPMDInstumentsList] = [tmpArray copy];
	
	tmpArray = [[NSMutableArray alloc] initWithCapacity:[musFile.patterns count]];
	for (PPPatternObject *obj in musFile.patterns) {
		NSString *patStr = obj.patternName;
		if (![NSString PPstringIsEmpty:patStr]) {
			[tmpArray addObject:patStr];
		}
	}
	attributes[kPPMDPatternList] = [tmpArray copy];

	//TODO: fill this out!
	attributes[(NSString*)kMDItemDurationSeconds] = @0.0;
	
	return TRUE;
}
