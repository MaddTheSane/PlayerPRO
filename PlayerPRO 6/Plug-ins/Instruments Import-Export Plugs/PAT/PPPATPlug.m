//
//  PPPATPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPPATPlug.h"
#include "PAT.h"
#import <PlayerPROKit/PlayerPROKit.h>

@implementation PPPATPlug

+ (MADErr)importPAT:(inout PPInstrumentObject*)insHeader data:(in NSData*)data
{
	const PatchHeader		*PATHeader;
	const PatInsHeader	*PATIns;
	//LayerHeader		*PATLayer;
	const PatSampHeader	*PATSamp;
	int				i, x;
	const void*		PATData = data.bytes;
	const unsigned int	scale_table[200] = {
		16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
		32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
		65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
		130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
		261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
		523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
		1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
		2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
		4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150};
	
	
	// PATCH HEADER
	PATHeader = PATData;
	PATData += 129;
	
	[insHeader resetInstrument];
	
	if (PATHeader->InsNo != 1)
		return MADFileNotSupportedByThisPlug;
	
	int sampleCount = (PATHeader->LoSamp << 8) + PATHeader->HiSamp;
	
	// INS HEADER -- Read only the first instrument
	PATIns = PATData;
	
	int patSize = PATIns->size;
	MADLE32(&patSize);
	PATData += 63;
	
	NSString *insName = [[NSString alloc] initWithBytes:PATIns->name length:strnlen(PATIns->name, sizeof(PATIns->name)) encoding:NSMacOSRomanStringEncoding];
	insHeader.name = insName;
	
	// LAYERS
	for (i = 0; i < PATIns->layer; i++)
		PATData += 47;
	
	// SAMPLES
	for (x = 0; x < sampleCount; x++) {
		PPSampleObject	*curData = [PPSampleObject new];
		Boolean	signedData;
		
		PATSamp = PATData;
		//curData = sample[x] = inMADCreateSample();
		
		curData.name = [[NSString alloc] initWithBytes:PATSamp->name length:strnlen(PATSamp->name, sizeof(PATSamp->name)) encoding:NSMacOSRomanStringEncoding];
		
		int sampSize, sampStartLoop, sampEndLoop;
		unsigned short sampRate = PATSamp->rate;
		sampSize = PATSamp->size;
		sampStartLoop = PATSamp->startLoop;
		sampEndLoop = PATSamp->endLoop;
		MADLE32(&sampSize);
		MADLE32(&sampStartLoop);
		MADLE32(&sampEndLoop);
		MADLE16(&sampRate);
		
		//curData->size = PATSamp->size;
		curData.loopBegin = sampStartLoop;
		curData.loopSize = sampEndLoop - sampStartLoop;
		curData.c2spd = sampRate;
		
		curData.volume = 64;
		curData.loopType = 0;
		
		if (PATSamp->Flag & 0x01)
			curData.amplitude = 16;
		else
			curData.amplitude = 8;
		
		if (PATSamp->Flag & 0x02)
			signedData = true;
		else
			signedData = false;
		
		if (!(PATSamp->Flag & 0x04)) {
			curData.loopBegin = 0;
			curData.loopSize = 0;
		}
		
		if (PATSamp->Flag & 0x08)
			curData.loopType = MADLoopTypePingPong;
		else
			curData.loopType = MADLoopTypeClassic;
		
		///////////////
		PatSampHeader tmpHeader = *PATSamp;
		MADLE32(&tmpHeader.minFreq);
		MADLE32(&tmpHeader.maxFreq);
		MADLE32(&tmpHeader.originRate);
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->originRate) {
				tmpHeader.originRate = i;
				i = 108;
			}
		}
		
		curData.relativeNote = 60 - (12 + PATSamp->originRate);
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->minFreq) {
				tmpHeader.minFreq = i;
				i = 108;
			}
		}
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->maxFreq) {
				tmpHeader.maxFreq = i;
				i = 108;
			}
		}
		
		for (i = PATSamp->minFreq; i < PATSamp->maxFreq; i++) {
			if (i < 96 && i >= 0) {
				insHeader.what[i] = x;
			}
		}
		
		PATData += 96;
		
		
		// DATA
		NSMutableData *aDataObj = [[NSMutableData alloc] initWithLength:sampSize];
		Ptr aData = aDataObj.mutableBytes;
		
		if (aData != NULL) {
			memcpy(aData, PATData, sampSize);
			
			if (curData.amplitude == 16) {
				short *tt = (short*) aData;
				
				dispatch_apply(sampSize / 2, dispatch_get_global_queue(0, 0), ^(size_t tL) {
					MADLE16((Ptr)(tt + tL));
					
					if (signedData)
						*(tt + tL) += 0x8000;
				});
			} else {
				if (signedData) {
					dispatch_apply(sampSize, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
						aData[i] += 0x80;
					});
				}
			}
			curData.data = [aDataObj copy];
		}
		
		PATData += PATSamp->size;
		
		[insHeader addSampleObject:curData];
	}
	
	return MADNoErr;
}

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (BOOL)isInstrument
{
	return YES;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)canImportSampleAtURL:(NSURL *)sampleURL
{
	NSFileHandle *aHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!aHandle) {
		return NO;
	}
	const char headerStr[20] = "GF1PATCH110\0\0";
	NSData *fileData = [aHandle readDataOfLength:12];
	NSData *headerData = [[NSData alloc] initWithBytes:headerStr length:12];
	
	return [fileData isEqual:headerData];
}

- (MADErr)importSampleAtURL:(NSURL *)sampleURL instrument:(inout PPInstrumentObject *)InsHeader sample:(inout PPSampleObject *)sample sampleID:(inout short *)sampleID driver:(PPDriver *)driver
{
	NSData *inData = [[NSData alloc] initWithContentsOfURL:sampleURL];
	if (!inData) {
		return MADReadingErr;
	}
	
	return [PPPATPlug importPAT:InsHeader data:inData];
}

@end
