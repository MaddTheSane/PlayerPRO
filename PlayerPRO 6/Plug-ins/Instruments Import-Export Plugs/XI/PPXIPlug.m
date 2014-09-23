//
//  PPXIPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import "PPXIPlug.h"
#import <Carbon/Carbon.h>
@import PlayerPROKit;

#ifndef WIN32
//Windows Defines
typedef SInt16			WORD;
typedef UInt16			UWORD;
typedef SInt32			HRESULT;
typedef UInt32			ULONG;
typedef void*			LPVOID;
typedef SInt32			LONG;

typedef UInt16			UINT;
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef FourCharCode	FOURCC;
typedef SInt8			BYTE;
#endif

typedef UInt8			UBYTE;

#include "XM.h"

static const int finetune[16] = {
	7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
	8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
};

@implementation PPXIPlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (BOOL)isInstrument
{
	return YES;
}

- (BOOL)canImportSampleAtURL:(NSURL *)sampleURL
{
	NSFileHandle *aHandle = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (!aHandle) {
		return NO;
	}
	const char * header = "Extended Instrument:";
	size_t headerLen = strlen(header);
	
	NSData *headerData = [[NSData alloc] initWithBytes:header length:headerLen];
	NSData *testData = [aHandle readDataOfLength:headerLen];
	[aHandle closeFile];
	
	return [testData isEqual:headerData];
}

- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver *)driver
{
	Ptr				theXI;
	XMPATCHHEADER	*pth;
	XMWAVHEADER		*wh = NULL;
	short			numSamples;
	
	NSData *xiData = [[NSData alloc] initWithContentsOfURL:sampleURL];
	if (xiData != NULL) {
		size_t inOutCount = xiData.length;
		theXI = alloca(inOutCount);
		memcpy(theXI, xiData.bytes, inOutCount);
		
		//theXI = malloc(inOutCount);
		if (theXI == NULL)
			return MADNeedMemory;
		else {
			
			InsHeader.name = [[sampleURL lastPathComponent] stringByDeletingPathExtension];
			
			// READ instrument header
			
			[InsHeader resetInstrument];
			
			pth = (XMPATCHHEADER*)(theXI + 0x42);
			
			numSamples = *((short*)(theXI + 0x42 + sizeof(XMPATCHHEADER)));
			MADLE16(&numSamples);
			
			MADLE16(&pth->volfade);
			
			memcpy([InsHeader what], pth->what, 96);
			{
				EnvRec tmpRec[12] = {0};
				memcpy(tmpRec, pth->volenv, 48);
#ifdef __BIG_ENDIAN__
				for (x = 0; x < 12; x++) {
					MADLE16(&tmpRec[x].pos);
					MADLE16(&tmpRec[x].val);
				}
#endif
				for (int i = 0; i < 12; i++) {
					[InsHeader replaceObjectInVolumeEnvelopeAtIndex:i withObject:[PPEnvelopeObject envelopeWithEnvRec:tmpRec[i]]];
				}
			}
			
			InsHeader.volumeSize	= pth->volpts;
			InsHeader.volumeType	= pth->volflg;
			InsHeader.volumeSustain	= pth->volsus;
			InsHeader.volumeBegin	= pth->volbeg;
			InsHeader.volumeEnd	= pth->volend;
			InsHeader.volumeFadeOut	= pth->volfade;
			
			{
				EnvRec tmpRec[12] = {0};
				memcpy(tmpRec, pth->panenv, 48);
#ifdef __BIG_ENDIAN__
				for (x = 0; x < 12; x++) {
					MADLE16(&tmpRec[x].pos);
					MADLE16(&tmpRec[x].val);
				}
#endif
				for (int i = 0; i < 12; i++) {
					[InsHeader replaceObjectInPanningEnvelopeAtIndex:i withObject:[PPEnvelopeObject envelopeWithEnvRec:tmpRec[i]]];
				}
			}
			
			InsHeader.panningSize		= pth->panpts;
			InsHeader.panningType		= pth->panflg;
			InsHeader.panningSustain	= pth->pansus;
			InsHeader.panningBegin		= pth->panbeg;
			InsHeader.panningEnd		= pth->panend;
			
			// Read SAMPLE HEADERS
			
			NSMutableArray *tmpSamples = [[NSMutableArray alloc] initWithCapacity:numSamples];
			
			for (int x = 0; x < numSamples; x++) {
				PPSampleObject *curData = [[PPSampleObject alloc] init];
				
				wh = (XMWAVHEADER*)(theXI + 0x42 + 0x02 + sizeof(XMPATCHHEADER) + x * sizeof(XMWAVHEADER));
				
				MADLE32(&wh->length);
				MADLE32(&wh->loopstart);
				MADLE32(&wh->looplength);
				
				///////////
				
				//curData->size		= wh->length;
				curData.loopBegin 	= wh->loopstart;
				curData.loopSize 	= wh->looplength;
				
				curData.volume		= wh->volume;
				curData.c2spd		= finetune[(wh->finetune + 128) / 16];
				curData.loopType	= 0;
				curData.amplitude	= 8;
				if (wh->type & 0x10) {		// 16 Bits
					curData.amplitude = 16;
				}
				
				if (!(wh->type & 0x3)) {
					curData.loopBegin	= 0;
					curData.loopSize	= 0;
				}
				
				//curData->panning	= wh->panning;
				curData.relativeNote	= wh->relnote;
				curData.name			= @(wh->samplename);
				[tmpSamples addObject:@{@"Sample": curData,
										@"DataLen": @(wh->length)}];
			}
			
			// Read SAMPLE DATA
			{
				Ptr reader = (Ptr)wh;
				
				reader += sizeof(XMWAVHEADER);
				
				for (int x = 0; x < numSamples; x++) {
					NSDictionary	*sampleAndSize = tmpSamples[x];
					PPSampleObject	*curData = sampleAndSize[@"Sample"];
					NSNumber		*dataLen = sampleAndSize[@"DataLen"];
					
					NSMutableData *sampData = [[NSMutableData alloc] initWithBytes:reader length: [dataLen integerValue]];
					Ptr rawSampData = sampData.mutableBytes;
						if (curData.amplitude == 16) {
							short	*tt = (short*)rawSampData;
							
#ifdef __BIG_ENDIAN__
							for (long tL = 0; tL < curData->size / 2; tL++) {
								MADLE16((Ptr)(tt + tL));
							}
#endif
							
							{
								/* Delta to Real */
								long	oldV = 0, newV;
								long	xL;
								
								for (xL = 0; xL < sampData.length / 2; xL++) {
									newV = tt[xL] + oldV;
									tt[xL] = newV;
									oldV = newV;
								}
							}
						} else {
							/* Delta to Real */
							long	oldV, newV;
							long	xL;
							
							oldV = 0;
							
							for (xL = 0; xL < sampData.length; xL++) {
								newV = rawSampData[xL] + oldV;
								rawSampData[xL] = newV;
								oldV = newV;
							}
						}
					curData.data = [sampData copy];
					reader += curData.data.length;
				}
			}
			for (NSDictionary* ourDict in tmpSamples) {
				[InsHeader addSamplesObject:ourDict[@"Sample"]];
			}
		}
	} else {
		return MADReadingErr;
	}
	return MADNoErr;
}

static NSData *startData()
{
	static NSData *currentData;
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		const char start[0x42] = "Extended Instrument:                       \241FastTracker v2.00   \x02\x01";
		currentData = [[NSData alloc] initWithBytes:start length:0x42];
	});
	
	return currentData;
}

- (MADErr)exportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver *)driver
{
	NSFileHandle *iFileRefI = [NSFileHandle fileHandleForWritingToURL:sampleURL error:NULL];
	MADErr myErr = MADNoErr;
	if (iFileRefI != NULL) {
		// Write instrument header
		
		short			u, i, x;
		long			inOutCount = 0;
		XMPATCHHEADER	pth;
		
		[iFileRefI writeData:startData()];
		
		memcpy(pth.what, [InsHeader what], 96);
		for (i = 0; i < 24; i += 2) {
			pth.volenv[i] = [(PPEnvelopeObject*)InsHeader.volumeEnvelope[i / 2] position];
			pth.volenv[i + 1] = [(PPEnvelopeObject*)InsHeader.volumeEnvelope[i / 2] value];
		}
#if __BIG_ENDIAN__
		for (x = 0; x < 24; x++) {
			MADLE16(&pth.volenv[x]);
		}
#endif
		
		pth.volpts	= InsHeader.volumeSize;
		pth.volflg	= InsHeader.volumeType;
		pth.volsus	= InsHeader.volumeSustain;
		pth.volbeg	= InsHeader.volumeBegin;
		pth.volend	= InsHeader.volumeEnd;
		pth.volfade = InsHeader.volumeFadeOut;
		MADLE16(&pth.volfade);
		
		for (i = 0; i < 24; i += 2) {
			pth.panenv[i] = [(PPEnvelopeObject*)InsHeader.panningEnvelope[i / 2] position];
			pth.panenv[i + 1] = [(PPEnvelopeObject*)InsHeader.panningEnvelope[i / 2] value];
		}
#if __BIG_ENDIAN__
		for (x = 0; x < 24; x++) {
			MADLE16(&pth.panenv[x]);
		}
#endif
		
		pth.panpts = InsHeader.panningSize;
		pth.panflg = InsHeader.panningType;
		pth.pansus = InsHeader.panningSustain;
		pth.panbeg = InsHeader.panningBegin;
		pth.panend = InsHeader.panningEnd;
		
		//inOutCount = sizeof(XMPATCHHEADER);
		[iFileRefI writeData:[NSData dataWithBytes:&pth length:sizeof(XMPATCHHEADER)]];
		
		inOutCount = 2;
		x = InsHeader.countOfSamples;
		MADLE16(&x);
		[iFileRefI writeData:[NSData dataWithBytes:&x length:2]];
		
		/** WRITE samples */
		
		for (u = 0 ; u < InsHeader.countOfSamples ; u++) {
			XMWAVHEADER		wh;
			PPSampleObject	*curData = [InsHeader samplesObjectAtIndex:u];
			
			if (curData.stereo)
				wh.length = (ULONG)(curData.data.length / 2);
			else
				wh.length = (ULONG)curData.data.length;
			
			inOutCount += wh.length;
			
			wh.loopstart	= curData.loopBegin;
			wh.looplength	= curData.loopSize;
			
			if (curData.stereo) {
				wh.loopstart /=2;
				wh.looplength /=2;
			}
			
			wh.volume = curData.volume;
			
			wh.finetune = -128;
			if (curData.c2spd > 8757)
				wh.finetune = 127;
			else {
				while (finetune[(wh.finetune + 128) / 16] < curData.c2spd) {
					wh.finetune += 16;
				}
			}
			wh.type = 0;
			if (curData.amplitude == 16)
				wh.type |= 0x10;
			if (curData.loopSize > 0)
				wh.type |= 0x3;
			
			//	wh.panning = curData->panning;
			wh.relnote = curData.relativeNote;
			strlcpy(wh.samplename, [curData.name cStringUsingEncoding:NSASCIIStringEncoding], sizeof(wh.samplename));
			
			MADLE32(&wh.length);
			MADLE32(&wh.loopstart);
			MADLE32(&wh.looplength);
			
			inOutCount = sizeof(wh);
			[iFileRefI writeData:[NSData dataWithBytes:&wh length:inOutCount]];
		}
		
		for (u = 0 ; u < InsHeader.countOfSamples ; u++) {
			PPSampleObject	*curData = [InsHeader samplesObjectAtIndex:u];
			long	dstSize;
			NSMutableData *tempDat = [curData.data mutableCopy];
			char	*tempPtr = tempDat.mutableBytes;
			
			/// WriteData
			if (tempPtr != NULL) {
				dstSize = curData.data.length;
				
				if (curData.amplitude == 16) {
					short	*tt = (short*)tempPtr;
					long	tL;
					
					/* Real to Delta */
					long oldV, newV;
					long xL;
					
					if (curData.stereo) {
						for (i = 0; i < dstSize; i++) tt[i] = tt[i*2];
						dstSize /= 2;
					}
					
					oldV = 0;
					
					for (xL = 0; xL < dstSize / 2; xL++) {
						newV = tt[xL];
						tt[xL] -= oldV;
						oldV = newV;
					}
					
					for (tL = 0; tL < dstSize / 2; tL++) {
						MADLE16((void*)(tt + tL));
					}
				} else {
					/* Real to Delta */
					long	oldV, newV;
					long	xL;
					char	*tt = (char*) tempPtr;
					
					if (curData.stereo) {
						for (i = 0; i < dstSize; i++)
							tt[i] = tt[i * 2];
						dstSize /= 2;
					}
					
					oldV = 0;
					
					for (xL = 0; xL < dstSize; xL++) {
						newV = tt[xL];
						tt[xL] -= oldV;
						oldV = newV;
					}
				}
				
				[iFileRefI writeData:tempDat];
			} else
				myErr = MADNeedMemory;
		}
	} else {
		myErr = MADWritingErr;
	}
	
	return myErr;
}

@end
