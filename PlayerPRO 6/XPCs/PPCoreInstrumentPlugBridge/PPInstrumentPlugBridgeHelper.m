//
//  PPInstrumentPlugBridgeHelper.m
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import "PPInstrumentPlugBridgeHelper.h"
#import "ARCBridge.h"
#import "PPEnvelopeObject32.h"

#if __i386__
typedef PPEnvelopeObject32 PPEnvelopeClass;
#else
typedef PPEnvelopeObject PPEnvelopeClass;
#endif

#pragma mark PlayerPROKit Sample NSCoding keys
#define PPName @"PlayerPROKit Sample Name"
#define PPLocation @"PlayerPROKit Sample Location"
#define PPSampCount @"PlayerPROKit Sample Count"
#define PPMIDI @"PlayerPROKit Sample MIDI"
#define PPMIDIType @"PlayerPROKit Sample MIDI Type"
#define PPVolSize @"PlayerPROKit Sample Volume Size"
#define PPPannSize @"PlayerPROKit Sample Panning Size"
#define PPPitchSize @"PlayerPROKit Sample Pitch Size"
#define PPVolSus @"PlayerPROKit Sample Volume Sustain"
#define PPVolBeg @"PlayerPROKit Sample Volume Begin"
#define PPVolEnd @"PlayerPROKit Sample Volume End"
#define PPPanSus @"PlayerPROKit Sample Panning Sustain"
#define PPPanBeg @"PlayerPROKit Sample Panning Begin"
#define PPPanEnd @"PlayerPROKit Sample Panning End"
#define PPPitchSus @"PlayerPROKit Sample Pitch Sustain"
#define PPPitchBeg @"PlayerPROKit Sample Pitch Begin"
#define PPPitchEnd @"PlayerPROKit Sample Pitch End"
#define PPVibDepth @"PlayerPROKit Sample Vibrato Depth"
#define PPVibRate @"PlayerPROKit Sample Vibrato Rate"
#define PPVolType @"PlayerPROKit Sample Volume Type"
#define PPPannType @"PlayerPROKit Sample Panning Type"
#define PPVolFade @"PlayerPROKit Sample Volume Fade"
#define PPNotes @"PlayerPROKit Sample Notes"
#define PPSamples @"PlayerPROKit Samples"
#define PPWhat @"PlayerPROKit what"

#define LOOPBEGINKEY @"Loop Begin"
#define LOOPSIZEKEY @"Loop Size"
#define VOLUMEKEY @"Volume"
#define C2SPDKEY @"c2spd"
#define LOOPTYPEKEY @"Loop Type"
#define AMPLITUDEKEY @"Amplitude"
#define RELATIVENOTEKEY @"Relative Note"
#define NAMEKEY @"Name"
#define STEREOKEY @"Stereo"
#define DATAKEY @"Data"

#if !__i386__

static NSArray *EncodeSampleObjects(PPInstrumentObject *ourData)
{
	NSMutableArray *ourArray = [[NSMutableArray alloc] initWithCapacity:ourData.countOfSamples];
	for (PPSampleObject *sampObj in ourData.samples) {
		NSMutableDictionary *toRet = [NSMutableDictionary new];
		toRet[LOOPBEGINKEY] = @(sampObj.loopBegin);
		toRet[LOOPSIZEKEY] = @(sampObj.loopSize);
		toRet[VOLUMEKEY] = @(sampObj.volume);
		toRet[C2SPDKEY] = @(sampObj.c2spd);
		toRet[LOOPTYPEKEY] = @(sampObj.loopType);
		toRet[AMPLITUDEKEY] = @(sampObj.amplitude);
		toRet[RELATIVENOTEKEY] = @(sampObj.relativeNote);
		toRet[NAMEKEY] = sampObj.name;
		toRet[STEREOKEY] = @(sampObj.stereo);
		toRet[DATAKEY] = sampObj.data;
		[ourArray addObject:toRet];
	}
	
	return ourArray;
}

NSData *PPInstrumentToData(PPInstrumentObject *ourData)
{
	NSMutableData *ourEncData = [[NSMutableData alloc] init];
	NSKeyedArchiver *ourArchiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:ourEncData];
	[ourArchiver encodeObject:ourData.name forKey:PPName];
	[ourArchiver encodeObject:EncodeSampleObjects(ourData) forKey:PPSamples];
	[ourArchiver encodeInt32:ourData.volumeType forKey:PPVolType];
	[ourArchiver encodeBytes:ourData.what length:96 forKey:PPWhat];
	[ourArchiver encodeInt32:ourData.MIDI forKey:PPMIDI];
	[ourArchiver encodeInt32:ourData.MIDIType forKey:PPMIDIType];
	[ourArchiver encodeInt32:ourData.volumeSize forKey:PPVolSize];
	[ourArchiver encodeInt32:ourData.volumeSustain forKey:PPVolSus];
	[ourArchiver encodeInt32:ourData.volumeBegin forKey:PPVolBeg];
	[ourArchiver encodeInt32:ourData.volumeEnd forKey:PPVolEnd];

	[ourArchiver finishEncoding];
	return ourEncData;
}

static void getSamplesFromArray(PPInstrumentObject *instrument, NSArray *ourArray)
{
	for (NSDictionary *sampleDict in ourArray) {
		PPSampleObject *sampObj = [[PPSampleObject alloc] init];
		sampObj.name = sampleDict[NAMEKEY];
		sampObj.data = sampleDict[DATAKEY];
		sampObj.loopBegin = [(NSNumber*)sampleDict[LOOPBEGINKEY] intValue];
		sampObj.loopSize = [(NSNumber*)sampleDict[LOOPSIZEKEY] intValue];
		sampObj.loopType = [(NSNumber*)sampleDict[LOOPTYPEKEY] unsignedCharValue];
		sampObj.volume = [(NSNumber*)sampleDict[VOLUMEKEY] unsignedCharValue];
		sampObj.c2spd = [(NSNumber*)sampleDict[C2SPDKEY] unsignedShortValue];
		sampObj.relativeNote = [(NSNumber*)sampleDict[RELATIVENOTEKEY] charValue];
		sampObj.stereo = [(NSNumber*)sampleDict[RELATIVENOTEKEY] boolValue];
		[instrument addSamplesObject:sampObj];
	}
}

PPInstrumentObject *PPDataToInstrument(NSData *ourData)
{
	NSKeyedUnarchiver *ourUnarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:ourData];
	PPInstrumentObject *toRet = [PPInstrumentObject new];
	toRet.name = [ourUnarchiver decodeObjectForKey:PPName];
	NSUInteger whatLen = 0;
	const uint8_t *whatData = [ourUnarchiver decodeBytesForKey:PPWhat returnedLength:&whatLen];
	NSCAssert(whatLen == 96, @"How can \"what\" not be 96?");
	memcpy(toRet.what, whatData, whatLen);
	toRet.MIDI = [ourUnarchiver decodeInt32ForKey:PPMIDI];
	switch ([ourUnarchiver decodeInt32ForKey:PPMIDI]) {
		case 0:
			toRet.MIDIOut = NO;
			toRet.soundOut = YES;
			break;
			
		case 1:
			toRet.MIDIOut = YES;
			toRet.soundOut = NO;
			break;
			
		case 2:
			toRet.MIDIOut = YES;
			toRet.soundOut = YES;
			break;
			
		case 3:
			toRet.MIDIOut = NO;
			toRet.soundOut = NO;
			break;
	}
	getSamplesFromArray(toRet, [ourUnarchiver decodeObjectForKey:PPSamples]);
	
	toRet.volumeType = [ourUnarchiver decodeInt32ForKey:PPVolType];
	return toRet;
}

#endif

NSData *InstrumentToData(InstrData* insData, sData ** sampleData)
{
	return nil;
}

InstrData *DataToInstrument(NSData *ourData, sData ***sampleData)
{
	*sampleData = NULL;
	return NULL;
}
