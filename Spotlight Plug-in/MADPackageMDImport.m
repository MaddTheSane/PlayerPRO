//
//  MADPackageMDImport.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Foundation/Foundation.h>
#import <PlayerPROKit/PPMusicObject.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include "GetMetadataForFile.h"

Boolean GetMetadataForPackage(CFMutableDictionaryRef attributes, CFURLRef pathToFile)
{
	@autoreleasepool {
		NSMutableDictionary *outDict = (__bridge NSMutableDictionary*)attributes;
		NSURL *fileURL = (__bridge NSURL*)pathToFile;
		PPMusicObjectWrapper *musFile = [[PPMusicObjectWrapper alloc] initWithURL:fileURL];
		if (!musFile) {
			return FALSE;
		}
		outDict[(NSString*)kMDItemTitle] = musFile.internalFileName;
		outDict[(NSString*)kMDItemAuthors] = @[musFile.madAuthor];
		{
			OSType codecType = musFile.madType;
			char cType[5];
			OSType2Ptr(codecType, cType);
			NSString *theString = [[NSString alloc] initWithCString:cType encoding:NSMacOSRomanStringEncoding];
			if (!theString) {
				theString = [[NSString alloc] initWithFormat:@"0x%08x", (unsigned int)codecType];
			}
			outDict[(NSString*)kMDItemCodecs] = @[theString];
		}
		outDict[(__bridge NSString*)(kPPMDPartitionLength)] = @(musFile.partitionLength);
		outDict[(__bridge NSString*)kPPMDTotalPatterns] = @(musFile.totalPatterns);
		outDict[(__bridge NSString*)kPPMDPartitionLength] = @(musFile.totalPartitions);
		outDict[(__bridge NSString*)kPPMDTotalInstruments] = @(musFile.totalInstruments);
		outDict[(__bridge NSString*)kPPMDTotalTracks] = @(musFile.totalTracks);
		
		outDict[(__bridge NSString*)kPPMDFormatDescription] = @"MAD Bundle"; //TODO: localize
		outDict[(__bridge NSString*)kPPMDMADKInfo] = musFile.madInfo;
		
		//TODO: fill these out!
		outDict[(__bridge NSString*)kPPMDInstumentsList] = @[];
		outDict[(__bridge NSString*)kPPMDPatternList] = @[];
		outDict[(NSString*)kMDItemDurationSeconds] = @0;
		
		return TRUE;
	}
}


