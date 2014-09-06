//
//  PPXIPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

#import "PPXIPlug.h"
#import <Carbon/Carbon.h>

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

@implementation PPXIPlug

- (MADErr)MAD2KillInstrument:(InstrData *)curIns sample:(sData **)sample
{
	short firstSample;
	
	if (!curIns || !sample)
		return MADParametersErr;
	
	dispatch_apply(curIns->numSamples, dispatch_get_global_queue(0, 0), ^(size_t i) {
		if (sample[i] != NULL) {
			if (sample[i]->data != NULL) {
				free(sample[i]->data);
				sample[i]->data = NULL;
			}
			free(sample[i]);
			sample[i] = NULL;
		}
	});
	
	firstSample = curIns->firstSample;
	memset(curIns, 0, sizeof(InstrData));
	curIns->firstSample = firstSample;
	
	return MADNoErr;
}

+ (BOOL)hasUIConfiguration
{
	return NO;
}

+ (BOOL)isInstrument
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
	
	return [testData isEqual:headerData];
}

- (MADErr)importSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID pluginInfo:(in PPInfoPlug *)info;
{
	Ptr				theXI;
	XMPATCHHEADER	*pth;
	XMWAVHEADER		*wh = NULL;
	short			numSamples;
	
	NSFileHandle *iFileRefI = [NSFileHandle fileHandleForReadingFromURL:sampleURL error:NULL];
	if (iFileRefI != NULL) {
		NSData *xiData = [iFileRefI readDataToEndOfFile];
		size_t inOutCount = xiData.length;
		theXI = alloca(inOutCount);
		memcpy(theXI, xiData.bytes, inOutCount);
		
		//theXI = malloc(inOutCount);
		if (theXI == NULL)
			return MADNeedMemory;
		else {
			
			InsHeader.name = [[sampleURL lastPathComponent] stringByDeletingPathExtension];
			//MAD2KillInstrument(InsHeader, sample);
			
			
			//memset(InsHeader->name, 0, sizeof(InsHeader->name));
			
			//inOutCount = iGetEOF(iFileRefI);
			
			// READ instrument header
			
			[InsHeader resetInstrument];
			
			pth = (XMPATCHHEADER*)(theXI + 0x42);
			
			numSamples = *((short*)(theXI + 0x42 + sizeof(XMPATCHHEADER)));
			MADLE16(&numSamples);
			//InsHeader.countOfSamples = numSamples;
			
			MADLE16(&pth->volfade);
			
			//InsHeader.
			
			//memcpy(InsHeader->what, pth->what, 96);
			//memcpy(InsHeader->volEnv, pth->volenv, 48);
			{
				EnvRec tmpRec[12];
				memcpy(tmpRec, pth->volenv, 48);
				NSMutableArray *envArray = [NSMutableArray new];
				for (int i = 0; i < 12; i++) {
					[envArray addObject:[[PPEnvelopeObject alloc] initWithEnvRec:tmpRec[i]]];
				}
				InsHeader.volumeEnvelope = envArray;
			}
#ifdef __BIG_ENDIAN__
			for (x = 0; x < 12; x++) {
				MADLE16(&InsHeader->volEnv[x].pos);
				MADLE16(&InsHeader->volEnv[x].val);
			}
#endif
			
			InsHeader.volumeSize	= pth->volpts;
			InsHeader.volumeType	= pth->volflg;
			InsHeader.volumeSustain	= pth->volsus;
			InsHeader.volumeBegin	= pth->volbeg;
			InsHeader.volumeEnd	= pth->volend;
			InsHeader.volumeFadeOut	= pth->volfade;
			
			{
				EnvRec tmpRec[12];
				memcpy(tmpRec, pth->panenv, 48);
#ifdef __BIG_ENDIAN__
			for (x = 0; x < 12; x++) {
				MADLE16(&tmpRec[x].pos);
				MADLE16(&tmpRec[x].val);
			}
#endif
				NSMutableArray *envArray = [NSMutableArray new];
				for (int i = 0; i < 12; i++) {
					[envArray addObject:[[PPEnvelopeObject alloc] initWithEnvRec:tmpRec[i]]];
				}
				InsHeader.panningEnvelope = envArray;
			}

			
			InsHeader.panningSize	= pth->panpts;
			InsHeader.panningType	= pth->panflg;
			InsHeader.panningSustain	= pth->pansus;
			InsHeader.panningBegin	= pth->panbeg;
			InsHeader.panningEnd	= pth->panend;
			
			// Read SAMPLE HEADERS
			
			NSMutableArray *tmpSamples = [[NSMutableArray alloc] initWithCapacity:numSamples];
			
			for (int x = 0; x < numSamples; x++) {
				PPSampleObject *curData = [[PPSampleObject alloc] init];
				static const int 	finetune[16] = {
					7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
					8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
				};
				
				wh = (XMWAVHEADER*)(theXI + 0x42 + 0x02 + sizeof(XMPATCHHEADER) + x * sizeof(XMWAVHEADER));
				
				MADLE32(&wh->length);
				MADLE32(&wh->loopstart);
				MADLE32(&wh->looplength);
				
				///////////
				
				//curData = sample[x] = inMADCreateSample();
				
				//curData->size		= wh->length;
				curData.loopBegin 	= wh->loopstart;
				curData.loopSize 	= wh->looplength;
				
				curData.volume		= wh->volume;
				curData.c2spd		= finetune[(wh->finetune + 128) / 16];
				curData.loopType	= 0;
				curData.amplitude		= 8;
				if (wh->type & 0x10) {		// 16 Bits
					curData.amplitude = 16;
				}
				
				if (!(wh->type & 0x3)) {
					curData.loopBegin = 0;
					curData.loopSize = 0;
				}
				
				//curData->panning	= wh->panning;
				curData.relativeNote	= wh->relnote;
				curData.name = @(wh->samplename);
				[tmpSamples addObject:@{@"Sample": curData,
										@"DataLen": @(wh->length)}];
			}
			
			// Read SAMPLE DATA
			{
				Ptr reader = (Ptr)wh;
				
				reader += sizeof(XMWAVHEADER);
				
				for (int x = 0; x < numSamples; x++) {
					NSDictionary * sampleAndSize = tmpSamples[x];
					PPSampleObject *curData = sampleAndSize[@"Sample"];
					NSNumber *dataLen = sampleAndSize[@"DataLen"];
					
					NSMutableData *sampData = [[NSMutableData alloc] initWithBytes:reader length: [dataLen integerValue]];
					Ptr rawSampData = sampData.mutableBytes;
						if (curData.amplitude == 16) {
							short	*tt = (short*)rawSampData;
							
#ifdef __BIG_ENDIAN__
							long	tL;
							
							for (tL = 0; tL < curData->size / 2; tL++) {
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
		}
	} else {
		return MADReadingErr;
	}
	return MADNoErr;
}

- (MADErr)exportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID pluginInfo:(PPInfoPlug *)info
{
	return MADWritingErr;
}


@end
