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
#define PPVolEnv @"PlayerPROKit Instrument Volume Envelope"
#define PPPannEnv @"PlayerPROKit Instrument Panning Envelope"
#define PPPitchEnv @"PlayerPROKit Instrument Pitch Envelope"

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

static NSDictionary *sampleToDictionary(sData *sampObj) NS_RETURNS_RETAINED;
static sData *dictionaryToSample(NSDictionary *sampleDict);


#if !__i386__
static PPSampleObject *newSampleFromDictionary(NSDictionary *sampleDict) NS_RETURNS_RETAINED;
static NSDictionary *EncodeSampleObject(PPSampleObject *sampObj);
static NSArray *EncodeSamplesObjects(PPInstrumentObject *ourData);
static void getSamplesFromArray(PPInstrumentObject *instrument, NSArray *ourArray);

NSDictionary *EncodeSampleObject(PPSampleObject *sampObj)
{
	NSMutableDictionary *toRet = [[NSMutableDictionary alloc] initWithCapacity:10];
	toRet[LOOPBEGINKEY] = @(sampObj.loopBegin);
	toRet[LOOPSIZEKEY] = @(sampObj.loopSize);
	toRet[VOLUMEKEY] = @(sampObj.volume);
	toRet[C2SPDKEY] = @(sampObj.c2spd);
	toRet[LOOPTYPEKEY] = @(sampObj.loopType);
	toRet[AMPLITUDEKEY] = @(sampObj.amplitude);
	toRet[RELATIVENOTEKEY] = @(sampObj.realNote);
	toRet[NAMEKEY] = sampObj.name;
	toRet[STEREOKEY] = @(sampObj.stereo);
	toRet[DATAKEY] = sampObj.data;

	return toRet;
}

NSData *PPSampleToData(PPSampleObject *sampObj)
{
	NSMutableData *ourEncData = [[NSMutableData alloc] init];
	NSKeyedArchiver *ourArchiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:ourEncData];
	NSDictionary *ourRet = EncodeSampleObject(sampObj);
	[ourArchiver encodeObject:ourRet forKey:PPSamples];
	[ourArchiver finishEncoding];

	return [ourEncData copy];
}

NSArray *EncodeSamplesObjects(PPInstrumentObject *ourData)
{
	NSMutableArray *ourArray = [[NSMutableArray alloc] initWithCapacity:ourData.countOfSamples];
	for (PPSampleObject *sampObj in ourData.samples) {
		[ourArray addObject:EncodeSampleObject(sampObj)];
	}
	
	return ourArray;
}

NSData *PPInstrumentToData(PPInstrumentObject *ourData)
{
	NSMutableData *ourEncData = [[NSMutableData alloc] init];
	NSKeyedArchiver *ourArchiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:ourEncData];
	[ourArchiver encodeObject:ourData.name forKey:PPName];
	[ourArchiver encodeObject:EncodeSamplesObjects(ourData) forKey:PPSamples];
	[ourArchiver encodeInt32:ourData.volumeType forKey:PPVolType];
	[ourArchiver encodeBytes:ourData.what length:96 forKey:PPWhat];
	[ourArchiver encodeInt32:ourData.MIDI forKey:PPMIDI];
	[ourArchiver encodeInt32:ourData.MIDIType forKey:PPMIDIType];
	[ourArchiver encodeInt32:ourData.volumeSize forKey:PPVolSize];
	[ourArchiver encodeInt32:ourData.volumeSustain forKey:PPVolSus];
	[ourArchiver encodeInt32:ourData.volumeBegin forKey:PPVolBeg];
	[ourArchiver encodeInt32:ourData.volumeEnd forKey:PPVolEnd];

	[ourArchiver encodeObject:ourData.volumeEnvelope forKey:PPVolEnv];
	[ourArchiver encodeObject:ourData.panningEnvelope forKey:PPPannEnv];
	[ourArchiver encodeObject:ourData.pitchEnvelope forKey:PPPitchEnv];
	
	[ourArchiver encodeInt32:ourData.panningSustain forKey:PPPanSus];
	[ourArchiver encodeInt32:ourData.panningBegin forKey:PPPanBeg];
	[ourArchiver encodeInt32:ourData.panningEnd forKey:PPPanEnd];
	
	[ourArchiver encodeInt32:ourData.pitchSustain forKey:PPPitchSus];
	[ourArchiver encodeInt32:ourData.pitchBegin forKey:PPPitchBeg];
	[ourArchiver encodeInt32:ourData.pitchEnd forKey:PPPitchEnd];
	
	[ourArchiver encodeInt32:ourData.volumeType forKey:PPVolType];
	[ourArchiver encodeInt32:ourData.panningType forKey:PPPannType];
	
	[ourArchiver encodeInt32:ourData.vibratoDepth forKey:PPVibDepth];
	[ourArchiver encodeInt32:ourData.vibratoRate forKey:PPVibRate];
	
	[ourArchiver encodeInt32:ourData.panningSize forKey:PPPannSize];
	[ourArchiver encodeInt32:ourData.pitchSize forKey:PPPitchSize];

	[ourArchiver finishEncoding];
	return [ourEncData copy];
}

PPSampleObject *newSampleFromDictionary(NSDictionary *sampleDict)
{
	PPSampleObject *sampObj = [[PPSampleObject alloc] init];
	sampObj.name = sampleDict[NAMEKEY];
	sampObj.data = sampleDict[DATAKEY];
	sampObj.loopBegin = [(NSNumber*)sampleDict[LOOPBEGINKEY] intValue];
	sampObj.loopSize = [(NSNumber*)sampleDict[LOOPSIZEKEY] intValue];
	sampObj.loopType = [(NSNumber*)sampleDict[LOOPTYPEKEY] unsignedCharValue];
	sampObj.volume = [(NSNumber*)sampleDict[VOLUMEKEY] unsignedCharValue];
	sampObj.c2spd = [(NSNumber*)sampleDict[C2SPDKEY] unsignedShortValue];
	sampObj.realNote = [(NSNumber*)sampleDict[RELATIVENOTEKEY] charValue];
	sampObj.stereo = [(NSNumber*)sampleDict[STEREOKEY] boolValue];

	return sampObj;
}

PPSampleObject *PPDataToSample(NSData *ourData)
{
	NSKeyedUnarchiver * ourUnarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:ourData];
	NSDictionary *ourDict = [ourUnarchiver decodeObjectForKey:PPSamples];
	
	return newSampleFromDictionary(ourDict);
}

void getSamplesFromArray(PPInstrumentObject *instrument, NSArray *ourArray)
{
	for (NSDictionary *sampleDict in ourArray) {
		[instrument addSampleObject:newSampleFromDictionary(sampleDict)];
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
	switch ([ourUnarchiver decodeInt32ForKey:PPMIDIType]) {
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
	
	NSArray *volEnvArray = [ourUnarchiver decodeObjectForKey:PPVolEnv];
	NSArray *panEnvArray = [ourUnarchiver decodeObjectForKey:PPPannEnv];
	NSArray *pitchEnvArray = [ourUnarchiver decodeObjectForKey:PPPitchEnv];
	
	for (int i = 0; i < 12; i++) {
		PPEnvelopeObject *aVolEnv = volEnvArray[i];
		PPEnvelopeObject *aPannEnv = panEnvArray[i];
		PPEnvelopeObject *aPitchEnv = pitchEnvArray[i];

		[toRet replaceObjectInPanningEnvelopeAtIndex:i withObject:aPannEnv];
		[toRet replaceObjectInPitchEnvelopeAtIndex:i withObject:aPitchEnv];
		[toRet replaceObjectInVolumeEnvelopeAtIndex:i withObject:aVolEnv];
	}
	toRet.volumeType = [ourUnarchiver decodeInt32ForKey:PPVolType];
	//toRet.MIDIType = [ourUnarchiver decodeInt32ForKey:PPMIDIType];
	toRet.volumeSize = [ourUnarchiver decodeInt32ForKey:PPVolSize];
	toRet.volumeSustain = [ourUnarchiver decodeInt32ForKey:PPVolSus];
	toRet.volumeBegin = [ourUnarchiver decodeInt32ForKey:PPVolBeg];
	toRet.volumeEnd = [ourUnarchiver decodeInt32ForKey:PPVolEnd];
	
	toRet.panningSustain = [ourUnarchiver decodeInt32ForKey:PPPanSus];
	toRet.panningBegin = [ourUnarchiver decodeInt32ForKey:PPPanBeg];
	toRet.panningEnd = [ourUnarchiver decodeInt32ForKey:PPPitchEnv];
	
	toRet.pitchSustain = [ourUnarchiver decodeInt32ForKey:PPPitchSus];
	toRet.pitchBegin = [ourUnarchiver decodeInt32ForKey:PPPitchBeg];
	toRet.pitchEnd = [ourUnarchiver decodeInt32ForKey:PPPitchEnd];
	
	toRet.panningType = [ourUnarchiver decodeInt32ForKey:PPPannType];
	
	toRet.vibratoRate = [ourUnarchiver decodeInt32ForKey:PPVibRate];
	toRet.vibratoDepth = [ourUnarchiver decodeInt32ForKey:PPVibDepth];
	
	toRet.panningSize = [ourUnarchiver decodeInt32ForKey:PPPannSize];
	toRet.pitchSize = [ourUnarchiver decodeInt32ForKey:PPPitchSize];
	
	return toRet;
}

#endif

NSData *MADInstrumentToData(InstrData* insData, sData ** sampleData)
{
	NSMutableData *ourEncData = nil;
	@autoreleasepool {
		ourEncData = [[NSMutableData alloc] init];
		NSKeyedArchiver *ourArchiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:ourEncData];
		NSMutableArray *sampArray = [[NSMutableArray alloc] init];
		for (int i = 0; i < insData->numSamples; i++) {
			NSDictionary *sampDict = sampleToDictionary(sampleData[i]);
			[sampArray addObject:sampDict];
			DESTROYOBJ(sampDict);
		}
		[ourArchiver encodeObject:sampArray forKey:PPSamples];
		DESTROYOBJ(sampArray);
		[ourArchiver encodeInt32:insData->volType forKey:PPVolType];
		[ourArchiver encodeBytes:insData->what length:96 forKey:PPWhat];
		[ourArchiver encodeInt32:insData->MIDI forKey:PPMIDI];
		[ourArchiver encodeInt32:insData->MIDIType forKey:PPMIDIType];
		[ourArchiver encodeInt32:insData->volSize forKey:PPVolSize];
		[ourArchiver encodeInt32:insData->volSus forKey:PPVolSus];
		[ourArchiver encodeInt32:insData->volBeg forKey:PPVolBeg];
		[ourArchiver encodeInt32:insData->volEnd forKey:PPVolEnd];
		[ourArchiver encodeObject:[NSString stringWithCString:insData->name encoding:NSMacOSRomanStringEncoding] forKey:PPName];
		NSMutableArray *volumeEnvelope = [[NSMutableArray alloc] init];
		NSMutableArray *panningEnvelope = [[NSMutableArray alloc] init];
		NSMutableArray *pitchEnvelope = [[NSMutableArray alloc] init];
		for (int i = 0; i < 12; i++) {
			PPEnvelopeClass *aVolEnv = [[PPEnvelopeClass alloc] initWithEnvRec:insData->volEnv[i]];
			PPEnvelopeClass *aPannEnv = [[PPEnvelopeClass alloc] initWithEnvRec:insData->pannEnv[i]];
			PPEnvelopeClass *aPitchEnv = [[PPEnvelopeClass alloc] initWithEnvRec:insData->pitchEnv[i]];
			[volumeEnvelope addObject:aVolEnv];
			[panningEnvelope addObject:aPannEnv];
			[pitchEnvelope addObject:aPitchEnv];
			RELEASEOBJ(aVolEnv); RELEASEOBJ(aPannEnv); RELEASEOBJ(aPitchEnv);
		}
		[ourArchiver encodeObject:volumeEnvelope forKey:PPVolEnv];
		[ourArchiver encodeObject:panningEnvelope forKey:PPPannEnv];
		[ourArchiver encodeObject:pitchEnvelope forKey:PPPitchEnv];
		DESTROYOBJ(volumeEnvelope);
		DESTROYOBJ(panningEnvelope);
		DESTROYOBJ(pitchEnvelope);
		
		[ourArchiver encodeInt32:insData->volSize forKey:PPVolSize];
		[ourArchiver encodeInt32:insData->pannSize forKey:PPPannSize];
		[ourArchiver encodeInt32:insData->pitchSize forKey:PPPitchSize];
		
		[ourArchiver encodeInt32:insData->pannSus forKey:PPPanSus];
		[ourArchiver encodeInt32:insData->pannBeg forKey:PPPanBeg];
		[ourArchiver encodeInt32:insData->pannEnd forKey:PPPanEnd];
		
		[ourArchiver encodeInt32:insData->pitchSus forKey:PPPitchSus];
		[ourArchiver encodeInt32:insData->pitchBeg forKey:PPPitchBeg];
		[ourArchiver encodeInt32:insData->pitchEnd forKey:PPPitchEnd];
		
		[ourArchiver encodeInt32:insData->pannType forKey:PPPannType];
		
		[ourArchiver encodeInt32:insData->vibDepth forKey:PPVibDepth];
		[ourArchiver encodeInt32:insData->vibRate forKey:PPVibRate];
		
		[ourArchiver finishEncoding];
		RELEASEOBJ(ourArchiver);
	}
#if __has_feature(objc_arc)
	return [ourEncData copy];
#else
	NSData *toRet = [ourEncData copy];
	[ourEncData release];
	return toRet;
#endif
}

InstrData *MADDataToInstrument(NSData *ourData, sData ***sampleData)
{
	InstrData *toRet = calloc(sizeof(InstrData), 1);
	*sampleData = calloc(sizeof(sData*), MAXSAMPLE);
	NSKeyedUnarchiver *ourUnarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:ourData];
	toRet->MIDI = [ourUnarchiver decodeInt32ForKey:PPMIDI];
	toRet->MIDIType = [ourUnarchiver decodeInt32ForKey:PPMIDIType];
	NSUInteger whatLen = 0;
	const uint8_t *whatData = [ourUnarchiver decodeBytesForKey:PPWhat returnedLength:&whatLen];
	NSCAssert(whatLen == 96, @"How can \"what\" not be 96?");
	NSString *aNam = [ourUnarchiver decodeObjectForKey:PPName];
	if (aNam && [aNam lengthOfBytesUsingEncoding:NSMacOSRomanStringEncoding] != 0) {
		strlcpy(toRet->name, [aNam cStringUsingEncoding:NSMacOSRomanStringEncoding], sizeof(toRet->name));
	}
	
	memcpy(toRet->what, whatData, 96);
	
	NSArray *volEnvArray = [ourUnarchiver decodeObjectForKey:PPVolEnv];
	NSArray *panEnvArray = [ourUnarchiver decodeObjectForKey:PPPannEnv];
	NSArray *pitchEnvArray = [ourUnarchiver decodeObjectForKey:PPPitchEnv];
	
	toRet->volType = [ourUnarchiver decodeInt32ForKey:PPVolType];
	
	for (int i = 0; i < 12; i++) {
		PPEnvelopeClass *aVolEnv = volEnvArray[i];
		PPEnvelopeClass *aPannEnv = panEnvArray[i];
		PPEnvelopeClass *aPitchEnv = pitchEnvArray[i];
	 
		toRet->pannEnv[i] = aPannEnv.envelopeRec;
		toRet->volEnv[i] = aVolEnv.envelopeRec;
		toRet->pitchEnv[i] = aPitchEnv.envelopeRec;
	}

	toRet->volType = [ourUnarchiver decodeInt32ForKey:PPVolType];
	toRet->volSize = [ourUnarchiver decodeInt32ForKey:PPVolSize];
	toRet->volSus = [ourUnarchiver decodeInt32ForKey:PPVolSus];
	toRet->volBeg = [ourUnarchiver decodeInt32ForKey:PPVolBeg];
	toRet->volEnd = [ourUnarchiver decodeInt32ForKey:PPVolEnd];
	
	toRet->pannSus = [ourUnarchiver decodeInt32ForKey:PPPanSus];
	toRet->pannBeg = [ourUnarchiver decodeInt32ForKey:PPPanBeg];
	toRet->pannEnd = [ourUnarchiver decodeInt32ForKey:PPPitchEnv];
	
	toRet->pitchSus = [ourUnarchiver decodeInt32ForKey:PPPitchSus];
	toRet->pitchBeg = [ourUnarchiver decodeInt32ForKey:PPPitchBeg];
	toRet->pitchEnd = [ourUnarchiver decodeInt32ForKey:PPPitchEnd];
	
	toRet->pannType = [ourUnarchiver decodeInt32ForKey:PPPannType];
	
	toRet->vibRate = [ourUnarchiver decodeInt32ForKey:PPVibRate];
	toRet->vibDepth = [ourUnarchiver decodeInt32ForKey:PPVibDepth];
	
	toRet->pannSize = [ourUnarchiver decodeInt32ForKey:PPPannSize];
	toRet->pitchSize = [ourUnarchiver decodeInt32ForKey:PPPitchSize];

	NSArray *ourSamps = [ourUnarchiver decodeObjectForKey:PPSamples];
	
	for (NSInteger i = 0; i < ourSamps.count; i++) {
		(*sampleData)[i] = dictionaryToSample(ourSamps[i]);
	}
	
	toRet->numSamples = ourSamps.count;
	toRet->firstSample = 0;
	
	RELEASEOBJ(ourUnarchiver);
	return toRet;
}

NSDictionary *sampleToDictionary(sData *sampObj)
{
	NSMutableDictionary *toRet = [[NSMutableDictionary alloc] initWithCapacity:10];
	@autoreleasepool {
		toRet[LOOPBEGINKEY] = @(sampObj->loopBeg);
		toRet[LOOPSIZEKEY] = @(sampObj->loopSize);
		toRet[VOLUMEKEY] = @(sampObj->vol);
		toRet[C2SPDKEY] = @(sampObj->c2spd);
		toRet[LOOPTYPEKEY] = @(sampObj->loopType);
		toRet[AMPLITUDEKEY] = @(sampObj->amp);
		toRet[RELATIVENOTEKEY] = @(sampObj->realNote);
		toRet[NAMEKEY] = [NSString stringWithCString:sampObj->name encoding:NSMacOSRomanStringEncoding] ?: [NSString string];
		toRet[STEREOKEY] = @(sampObj->stereo);
		toRet[DATAKEY] = sampObj->size != 0 ? [NSData dataWithBytes:sampObj->data length:sampObj->size] : [NSData data];
	}
	
	return toRet;
}

NSData *MADSampleToData(sData *sampObj)
{
	NSMutableData *ourEncData = nil;
	@autoreleasepool {
		NSDictionary *toRet = sampleToDictionary(sampObj);
		
		ourEncData = [[NSMutableData alloc] init];
		NSKeyedArchiver *ourArchiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:ourEncData];
		[ourArchiver encodeObject:toRet forKey:PPSamples];
		[ourArchiver finishEncoding];
		
		RELEASEOBJ(toRet);
		RELEASEOBJ(ourArchiver);
	}
#if __has_feature(objc_arc)
	return [ourEncData copy];
#else
	NSData *toRet = [ourEncData copy];
	[ourEncData release];
	return toRet;
#endif
}

sData *dictionaryToSample(NSDictionary *sampleDict)
{
	sData *toRet = calloc(sizeof(sData), 1);
	@try {
		strlcpy(toRet->name, [sampleDict[NAMEKEY] cStringUsingEncoding:NSMacOSRomanStringEncoding], sizeof(toRet->name));
		NSData *aData = sampleDict[DATAKEY];
		if (aData && [aData length] != 0) {
			toRet->size = (int)aData.length;
			toRet->data = malloc(toRet->size);
			memcpy(toRet->data, aData.bytes, toRet->size);
		}
		toRet->loopBeg = [(NSNumber*)sampleDict[LOOPBEGINKEY] intValue];
		toRet->loopSize = [(NSNumber*)sampleDict[LOOPSIZEKEY] intValue];
		toRet->loopType = [(NSNumber*)sampleDict[LOOPTYPEKEY] unsignedCharValue];
		toRet->vol = [(NSNumber*)sampleDict[VOLUMEKEY] unsignedCharValue];
		toRet->c2spd = [(NSNumber*)sampleDict[C2SPDKEY] unsignedShortValue];
		toRet->realNote = [(NSNumber*)sampleDict[RELATIVENOTEKEY] charValue];
		toRet->stereo = [(NSNumber*)sampleDict[STEREOKEY] boolValue];
		return toRet;
	}
	@catch (NSException *exception) {
		if (toRet->data) {
			free(toRet->data);
		}
		free(toRet);
		return NULL;
	}
}

sData *MADDataToSample(NSData *ourData)
{
	sData *toRet = NULL;
	@autoreleasepool {
		NSKeyedUnarchiver *ourUnarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:ourData];
		NSDictionary *sampleDict = [ourUnarchiver decodeObjectForKey:PPSamples];

		toRet = dictionaryToSample(sampleDict);
		
		RELEASEOBJ(ourUnarchiver);
	}
	return toRet;
}
