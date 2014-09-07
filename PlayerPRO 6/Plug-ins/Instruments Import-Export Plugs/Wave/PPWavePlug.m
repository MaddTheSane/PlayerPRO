//
//  PPWavePlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import "PPWavePlug.h"

@implementation PPWavePlug

+ (BOOL)hasUIConfiguration
{
	return NO;
}

+ (BOOL)isInstrument
{
	return NO;
}

- (BOOL)canImportSampleAtURL:(NSURL*)sampleURL
{
	return NO;
}

- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID pluginInfo:(in PPInfoPlug *)info
{
	return MADReadingErr;
}

@end
