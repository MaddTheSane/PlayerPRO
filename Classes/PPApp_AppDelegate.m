//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"
#import "PPErrors.h"
#import "OpenPanelViewController.h"
#import "PPInstrumentPlugHandler.h"
#import "PPInstrumentImporterObject.h"
#import "PPPlugInInfo.h"
#import "PPPlugInInfoController.h"
#import "PPDigitalPlugInHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import <PlayerPROKit/PlayerPROKit.h>
#import "PatternHandler.h"
#include <PlayerPROCore/RDriverInt.h>
#include "PPByteswap.h"
#import <QTKit/QTKit.h>
#import <QTKit/QTExportSession.h>
#import <QTKit/QTExportOptions.h>

#define kUnresolvableFile @"Unresolvable files"
#define kUnresolvableFileDescription @"There were %lu file(s) that were unable to be resolved."

static void CocoaDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error", @"Error"),
									  [NSString stringWithFormat:NSLocalizedString(@"MyDebugStr_MainText", @"The Main text to display"), text],
									  NSLocalizedString(@"MyDebugStr_Quit", @"Quit"), NSLocalizedString(@"MyDebugStr_Continue", @"Continue"),
									  NSLocalizedString(@"MyDebugStr_Debug", @"Debug"));
	switch (alert) {
		case NSAlertAlternateReturn:
			break;
		case NSAlertOtherReturn:
			//Debugger();
			NSCAssert(NO, @"Chose to go to debugger.");
			break;
		case NSAlertDefaultReturn:
			NSLog(@"Choosing to fail!");
		default:
			abort();
			break;
	}
}

@interface PPApp_AppDelegate ()
- (void)selectCurrentlyPlayingMusic;
- (void)selectMusicAtIndex:(NSInteger)anIdx;
- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr;
- (void)musicListContentsDidMove;
- (BOOL)musicListWillChange;
- (void)musicListDidChange;
- (void)moveMusicAtIndex:(NSUInteger)from toIndex:(NSUInteger)to;
@property (readwrite, retain) NSString *musicName;
@property (readwrite, retain) NSString *musicInfo;
@property (readonly, strong) NSDictionary *trackerDict;
@property (readwrite, strong) PPLibrary *madLib;
@end

@implementation PPApp_AppDelegate
@synthesize window;
@synthesize exportWindow;
@synthesize musicInfo;
@synthesize musicName;
@synthesize madLib;

@synthesize trackerDict = _trackerDict;
- (NSDictionary *)trackerDict
{
	if (!_trackerDict) {
		NSMutableDictionary *trackerDict = [NSMutableDictionary dictionaryWithDictionary:@{
											NSLocalizedStringWithDefaultValue(@"PPMADKFile", @"InfoPlist",
																			  [NSBundle mainBundle],
																			  @"MADK Tracker", @"MADK Tracker") : @[MADNativeUTI],
											NSLocalizedString(@"Generic MAD tracker", @"Generic MAD tracker"): @[MADGenericUTI],
											NSLocalizedString(@"MAD Package", @"MAD Package"):@[MADPackageUTI]}];
		for (PPLibraryObject *obj in madLib) {
			trackerDict[obj.menuName] = obj.UTItypes;
		}
		
		_trackerDict = [[NSDictionary alloc] initWithDictionary:trackerDict];
	}
	
	return _trackerDict;
}


#if 0
- (void)MADDriverWithPreferences
{
	Boolean madWasReading = false;
	long fullTime = 0, curTime = 0;
	if (madDriver) {
		madWasReading = MADIsPlayingMusic(madDriver);
		MADStopMusic(madDriver);
		MADStopDriver(madDriver);
		if (madWasReading) {
			MADGetMusicStatus(madDriver, &fullTime, &curTime);
		}
		MADDisposeDriver(madDriver);
		madDriver = NULL;
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = [defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	if ([defaults boolForKey:PPOversamplingToggle]) {
		init.oversampling = [defaults integerForKey:PPOversamplingAmount];
	} else {
		init.oversampling = 1;
	}
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = [defaults integerForKey:PPReverbAmount];
	init.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	if ([defaults boolForKey:PPStereoDelayToggle]) {
		init.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	} else {
		init.MicroDelaySize = 0;
	}
	
	init.driverMode = [defaults integerForKey:PPSoundDriver];
	init.repeatMusic = FALSE;
	
	OSErr returnerr = MADCreateDriver(&init, madLib, &madDriver);
	[[NSNotificationCenter defaultCenter] postNotificationName:PPDriverDidChange object:self];
	if (returnerr != noErr) {
		NSError *err = CreateErrorFromMADErrorType(returnerr);
		[[NSAlert alertWithError:err] runModal];
		RELEASEOBJ(err);
		return;
	}
	MADStartDriver(madDriver);
	if (music) {
		MADAttachDriverToMusic(madDriver, music, NULL);
		if (madWasReading) {
			MADSetMusicStatus(madDriver, 0, fullTime, curTime);
			MADPlayMusic(madDriver);
		}
	}
}
#endif

+ (void)initialize
{
	[[NSUserDefaults standardUserDefaults] registerDefaults:@{
											 PPSoundOutBits: @16,
											 PPSoundOutRate: @44100,
											  PPSoundDriver: @(CoreAudioDriver),
										PPStereoDelayToggle: @YES,
											 PPReverbToggle: @NO,
										   PPSurroundToggle: @NO,
									   PPOversamplingToggle: @NO,
										PPStereoDelayAmount: @30,
											 PPReverbAmount: @25,
										   PPReverbStrength: @30,
									   PPOversamplingAmount: @1,
	 
										 PPDEShowInstrument: @YES,
											   PPDEShowNote: @YES,
											 PPDEShowEffect: @YES,
										   PPDEShowArgument: @YES,
											 PPDEShowVolume: @YES,
											PPDEShowMarkers: @YES,
									   PPDEMarkerOffsetPref: @0,
										 PPDEMarkerLoopPref: @3,
										PPDEMarkerColorPref: [[NSColor yellowColor] PPencodeColor],
								  PPDEMouseClickControlPref: @YES,
									PPDEMouseClickShiftPref: @NO,
								  PPDEMouseClickCommandPref: @NO,
								   PPDEMouseClickOptionPref: @NO,
									   PPDELineHeightNormal: @YES,
										   PPDEMusicTraceOn: @YES,
							   PPDEPatternWrappingPartition: @YES,
											 PPDEDragAsPcmd: @YES,
	 
												  PPCColor1: [[NSColor redColor] PPencodeColor],
												  PPCColor2: [[NSColor greenColor] PPencodeColor],
												  PPCColor3: [[NSColor blueColor] PPencodeColor],
												  PPCColor4: [[NSColor cyanColor] PPencodeColor],
												  PPCColor5: [[NSColor yellowColor] PPencodeColor],
												  PPCColor6: [[NSColor magentaColor] PPencodeColor],
												  PPCColor7: [[NSColor orangeColor] PPencodeColor],
												  PPCColor8: [[NSColor purpleColor] PPencodeColor],
	 
										 PPBEMarkersEnabled: @YES,
										  PPBEMarkersOffset: @0,
											PPBEMarkersLoop: @3,
										  PPBEOctaveMarkers: @YES,
										PPBENotesProjection: @NO,
	 
											PPMAddExtension: @YES,
										  PPMMadCompression: @YES,
									PPMNoLoadMixerFromFiles: @NO,
								   PPMOscilloscopeDrawLines: @YES,
	 
										   PPCEShowNotesLen: @NO,
											PPCEShowMarkers: @YES,
										   PPCEMarkerOffset: @0,
											 PPCEMarkerLoop: @3,
											   PPCETempoNum: @4,
											  PPCETempoUnit: @4,
											PPCETrackHeight: @130}];
}

#if 0
- (void)updateMusicStats:(NSTimer*)theTimer
{
	if (music) {
		long fT, cT;
		MADGetMusicStatus(madDriver, &fT, &cT);
		if (MADIsDonePlaying(madDriver) && !self.paused && !MADIsExporting(madDriver)) {
			[self songIsDonePlaying];
			MADGetMusicStatus(madDriver, &fT, &cT);
		}
		[songPos setDoubleValue:cT];
		[songCurTime setIntegerValue:cT];
	}
}

- (void)saveMusicToURL:(NSURL *)tosave
{
	[instrumentController writeInstrumentsBackToMusic];
	int i, x;
	size_t inOutCount;
	MADCleanCurrentMusic(music, madDriver);
	NSMutableData *saveData = [[NSMutableData alloc] initWithCapacity:MADGetMusicSize(music)];
	for( i = 0, x = 0; i < MAXINSTRU; i++)
	{
		music->fid[ i].no = i;
		
		if( music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			x++;
		}
	}

	music->header->numInstru = x;
	{
		MADSpec aHeader;
		aHeader = *music->header;
		ByteSwapMADSpec(&aHeader);
		[saveData appendBytes:&aHeader length:sizeof(MADSpec)];
	}
	{
		BOOL compressMAD = [[NSUserDefaults standardUserDefaults] boolForKey:PPMMadCompression];
		if (compressMAD) {
			for( i = 0; i < music->header->numPat ; i++)
			{
				if (music->partition[i]) {
					PatData *tmpPat = CompressPartitionMAD1(music, music->partition[i]);
					inOutCount = tmpPat->header.patBytes + sizeof(PatHeader);
					tmpPat->header.compMode = 'MAD1';
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		} else {
			for (i = 0; i < music->header->numPat; i++) {
				if (music->partition[i]) {
					inOutCount = sizeof( PatHeader);
					inOutCount += music->header->numChn * music->partition[ i]->header.size * sizeof( Cmd);
					PatData *tmpPat = calloc(inOutCount, 1);
					memcpy(tmpPat, music->partition[i], inOutCount);
					ByteSwapPatHeader(&tmpPat->header);
					[saveData appendBytes:tmpPat length:inOutCount];
					free(tmpPat);
				}
			}
		}
	}
	
	for( i = 0; i < MAXINSTRU; i++)
	{
		if( music->fid[ i].numSamples > 0 || music->fid[ i].name[ 0] != 0)	// Is there something in this instrument?
		{
			music->fid[ i].no = i;
			InstrData instData = music->fid[i];
			ByteSwapInstrData(&instData);
			[saveData appendBytes:&instData length:sizeof( InstrData)];
		}
	}
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		for( x = 0; x < music->fid[i].numSamples; x++)
		{
			sData	curData;
			sData32	copyData;
			curData = *music->sample[ music->fid[i].firstSample + x];
			
			inOutCount = sizeof( sData32);
			ByteSwapsData(&curData);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[saveData appendBytes:&copyData length:inOutCount];
			
			inOutCount = music->sample[ music->fid[i].firstSample + x]->size;
			Ptr dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			if( curData.amp == 16)
			{
				__block short	*shortPtr = (short*) dataCopy;
				
				dispatch_apply(inOutCount / 2, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT , 0), ^(size_t y) {
					PPBE16(&shortPtr[y]);
				});

			}

			[saveData appendBytes:dataCopy length:inOutCount];
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	int alpha = 0;
	for( i = 0; i < 10 ; i++)	// Global Effects
	{
		if( music->header->globalEffect[ i])
		{
			inOutCount = sizeof( FXSets);
			__block FXSets aSet = music->sets[alpha];
			PPBE16(&aSet.id);
			PPBE16(&aSet.noArg);
			PPBE16(&aSet.track);
			PPBE32(&aSet.FXID);
			dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
				PPBE32(&aSet.values[y]);
			});

			[saveData appendBytes:&aSet length:inOutCount];
			alpha++;
		}
	}
	
	for( i = 0; i < music->header->numChn ; i++)	// Channel Effects
	{
		for( x = 0; x < 4; x++)
		{
			if( music->header->chanEffect[ i][ x])
			{
				inOutCount = sizeof( FXSets);
				__block FXSets aSet = music->sets[alpha];
				PPBE16(&aSet.id);
				PPBE16(&aSet.noArg);
				PPBE16(&aSet.track);
				PPBE32(&aSet.FXID);
				dispatch_apply(100, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(size_t y) {
					PPBE32(&aSet.values[y]);
				});
				
				[saveData appendBytes:&aSet length:inOutCount];
				alpha++;
			}
		}
	}
	
	[saveData writeToURL:tosave atomically:YES];
	RELEASEOBJ(saveData);
	
	music->header->numInstru = MAXINSTRU;
	
	music->hasChanged = FALSE;
}

//Yes, the pragma pack is needed
//otherwise the data will be improperly mapped.
#pragma pack(push, 2)
struct Float80i {
	SInt16  exp;
	UInt32  man[2];
};
#pragma pack(pop)

static inline extended80 convertSampleRateToExtended80(unsigned int theNum)
{
	union {
		extended80 shortman;
		struct Float80i intman;
	} toreturn;

	unsigned int shift, exponent;

	
	for(shift = 0U; (theNum >> (31 - shift)) == 0U; ++shift)
		;
	theNum <<= shift;
	exponent = 63U - (shift + 32U); /* add 32 for unused second word */

	toreturn.intman.exp = (exponent+0x3FFF);
	PPBE16(&toreturn.intman.exp);
	toreturn.intman.man[0] = theNum;
	PPBE32(&toreturn.intman.man[0]);
	toreturn.intman.man[1] = 0;
	PPBE32(&toreturn.intman.man[1]);
	
	return toreturn.shortman;
}

- (NSData *)newAIFFDataFromSettings:(MADDriverSettings*)sett data:(NSData*)dat
{
	//TODO: Write a little-endian AIFF exporter
	NSInteger dataLen = [dat length];
	
	ContainerChunk header;
	
	CommonChunk container;

	SoundDataChunk dataChunk;
	
	NSData *nameData;
	NSData *infoData;

	{
#if 0
		TextChunk *nameChunk;
		NSInteger macRomanNameLength = 0;
		NSData *macRomanNameData = [musicName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanNameLength = [macRomanNameData length];
		BOOL isPadded = (macRomanNameLength & 1);
		NSInteger nameChunkLen = sizeof(TextChunk) + macRomanNameLength;

		if (!isPadded) {
			nameChunkLen--;
		}
		
		nameChunk = calloc(nameChunkLen, 1);
		char *firstChar;
		nameChunk->ckID = NameID;
		PPBE32(&nameChunk->ckID);
		nameChunk->ckSize = 1 + macRomanNameLength;
		PPBE32(&nameChunk->ckSize);
		nameChunk->text[0] = macRomanNameLength;
		firstChar = &nameChunk->text[1];
		memcpy(firstChar, [macRomanNameData bytes], macRomanNameLength);
		nameData = [NSData dataWithBytes:nameChunk length:nameChunkLen];
		free(nameChunk);
#else
		ChunkHeader nameChunk;
		NSInteger macRomanNameLength = 0;
		NSData *macRomanNameData = [musicName dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanNameLength = [macRomanNameData length];
		BOOL isPadded = (macRomanNameLength & 1);
		
		nameChunk.ckSize = macRomanNameLength + 1;
		char pStrLen = macRomanNameLength;
		PPBE32(&nameChunk.ckSize);
		
		nameChunk.ckID = NameID;
		PPBE32(&nameChunk.ckID);
		
		NSMutableData *tmpNameDat = [NSMutableData dataWithBytes:&nameChunk length:sizeof(ChunkHeader)];
		[tmpNameDat appendBytes:&pStrLen length:1];
		[tmpNameDat appendData:macRomanNameData];
		
		if (!isPadded) {
			char padbyte = 0;
			[tmpNameDat appendBytes:&padbyte length:1];
		}
		nameData = tmpNameDat;
#endif
	}
	
	{
#if 0
		ApplicationSpecificChunk *infoChunk;
		NSInteger macRomanInfoLength = 0;
		NSData *macRomanInfoData = [musicInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanInfoLength = [macRomanInfoData length];
		BOOL isPadded = (macRomanInfoLength & 1);
		NSInteger infoChunkLen = sizeof(ApplicationSpecificChunk) + macRomanInfoLength;
		
		if (!isPadded) {
			infoChunkLen--;
		}
		
		infoChunk = calloc(infoChunkLen, 0);
		infoChunk->applicationSignature = CommentID;
		PPBE32(&infoChunk->applicationSignature);
		infoChunk->ckID = ApplicationSpecificID;
		PPBE32(&infoChunk->ckID);
		infoChunk->ckSize = macRomanInfoLength + 1;
		PPBE32(&infoChunk->ckSize);
		memcpy(infoChunk->data, [macRomanInfoData bytes], macRomanInfoLength);
		infoData = [NSData dataWithBytes:infoChunk length:infoChunkLen];
		free(infoChunk);
#else
		ChunkHeader infoChunk;
		NSInteger macRomanInfoLength = 0;
		NSData *macRomanInfoData = [musicInfo dataUsingEncoding:NSMacOSRomanStringEncoding allowLossyConversion:YES];
		macRomanInfoLength = [macRomanInfoData length];
		BOOL isPadded = (macRomanInfoLength & 1);
		infoChunk.ckSize = macRomanInfoLength + 1;
		char pStrLen = macRomanInfoLength;
		PPBE32(&infoChunk.ckSize);
		
		infoChunk.ckID = CommentID;
		PPBE32(&infoChunk.ckID);
		NSMutableData *tmpInfoDat = [NSMutableData dataWithBytes:&infoChunk length:sizeof(ChunkHeader)];
		[tmpInfoDat appendBytes:&pStrLen length:1];
		[tmpInfoDat appendData:macRomanInfoData];
		
		if (!isPadded) {
			char padbyte = 0;
			[tmpInfoDat appendBytes:&padbyte length:1];
		}

		infoData = tmpInfoDat;
#endif
	}
	
	NSMutableData *returnData = [[NSMutableData alloc] initWithCapacity:dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + sizeof(ContainerChunk) + [nameData length] + [infoData length]];
	header.ckID = FORMID;
	PPBE32(&header.ckID);
	header.ckSize = dataLen + sizeof(CommonChunk) + sizeof(SoundDataChunk) + 4 + [nameData length] + [infoData length];
	PPBE32(&header.ckSize);
	header.formType = AIFFID;
	PPBE32(&header.formType);
	[returnData appendBytes:&header length:sizeof(ContainerChunk)];
	
	container.ckID = CommonID;
	PPBE32(&container.ckID);
	container.ckSize = /*sizeof(CommonChunk)*/ 18;
	PPBE32(&container.ckSize);
	short chanNums = 0;
	{
		int todiv = sett->outPutBits / 8;

		switch (sett->outPutMode) {
			case DeluxeStereoOutPut:
			case StereoOutPut:
			default:
				todiv *= 2;
				chanNums = 2;
				break;
				
			case PolyPhonic:
				todiv *= 4;
				chanNums = 4;
				break;
				
			case MonoOutPut:
				chanNums = 1;
				break;
		}
		container.numSampleFrames = dataLen / todiv;
	}

	container.numChannels = chanNums;
	PPBE16(&container.numChannels);
	container.sampleSize = sett->outPutBits;
	PPBE16(&container.sampleSize);
	PPBE32(&container.numSampleFrames);
	
	container.sampleRate = convertSampleRateToExtended80(sett->outPutRate);
	[returnData appendBytes:&container length:sizeof(CommonChunk)];
	
	int dataOffset = 0;
	dataChunk.ckID = SoundDataID;
	PPBE32(&dataChunk.ckID);
	dataChunk.blockSize = 0;
	PPBE32(&dataChunk.blockSize);
	dataChunk.ckSize = dataLen + 8 + dataOffset;
	PPBE32(&dataChunk.ckSize);
	dataChunk.offset = dataOffset;
	PPBE32(&dataChunk.offset);
	

	[returnData appendBytes:&dataChunk length:sizeof(SoundDataChunk)];
	if (sett->outPutBits == 16) {
		short swapdata;
		int i;
		for (i = 0; i < dataLen; i += 2) {
			[dat getBytes:&swapdata range:NSMakeRange(i, 2)];
			PPBE16(&swapdata);
			[returnData appendBytes:&swapdata length:2];
		}
	} else {
		[returnData appendData:dat];
	}
	
	[returnData appendData:nameData];
	[returnData appendData:infoData];
	
	return returnData;
}

- (NSData *)getSoundData:(MADDriverSettings*)theSet
{
	MADDriverRec *theRec = NULL;
	
	OSErr err = MADCreateDriver( theSet, madLib, &theRec);
	if (err != noErr) {
		dispatch_async(dispatch_get_main_queue(), ^{
			NSError *NSerr = CreateErrorFromMADErrorType(err);
			[[NSAlert alertWithError:NSerr] runModal];
			RELEASEOBJ(NSerr);
		});
		
		return nil;
	}
	MADCleanDriver( theRec);

	MADAttachDriverToMusic( theRec, music, NULL);
	MADPlayMusic(theRec);
	
	Ptr soundPtr = NULL;
	long full = 0;
	if (theSet->outPutBits == 8) {
		full = MADAudioLength(theRec);
	}else if (theSet->outPutBits == 16) {
		full = MADAudioLength(theRec) * 2;
	} else if (theSet->outPutBits == 20 || theSet->outPutBits == 24 ) {
		full = MADAudioLength(theRec) * 3;
	} else {
		//This is just to make the Static analyzer happy
		full = MADAudioLength(theRec);
	}
	
	switch (theSet->outPutMode) {
		case DeluxeStereoOutPut:
		case StereoOutPut:
			full *= 2;
			break;
			
		case PolyPhonic:
			full *= 4;
			break;
			
		default:
			break;
	}
	
	NSMutableData *mutData = [[NSMutableData alloc] init];
	soundPtr = calloc(full, 1);
	
	while(DirectSave(soundPtr, theSet, theRec))
	{
		[mutData appendBytes:soundPtr length:full];
	}
	NSData *retData = [self newAIFFDataFromSettings:theSet data:mutData];
	RELEASEOBJ(mutData);
	mutData = nil;
	
	MADStopMusic(theRec);
	MADCleanDriver(theRec);
	MADDisposeDriver(theRec);
	free(soundPtr);
	
	return AUTORELEASEOBJ(retData);
}

- (NSInteger)showExportSettings
{
	MADGetBestDriver(&exportSettings);
	exportSettings.driverMode = NoHardwareDriver;
	exportSettings.repeatMusic = FALSE;
	[exportController settingsFromDriverSettings:&exportSettings];
	return [NSApp runModalForWindow:exportWindow];
}

#ifndef PPEXPORT_CREATE_TMP_AIFF
#define PPEXPORT_CREATE_TMP_AIFF 1
#endif

- (IBAction)exportMusicAs:(id)sender
{
	NSInteger tag = [sender tag];
	MADBeginExport(madDriver);

	switch (tag) {
		case -1:
			//AIFF
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:@[@"public.aiff-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as AIFF audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = RETAINOBJ([self getSoundData:&exportSettings]);
						MADEndExport(madDriver);

						[saveData writeToURL:[savePanel URL] atomically:YES];
						RELEASEOBJ(saveData);
						saveData = nil;
						dispatch_async(dispatch_get_main_queue(), ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							} else {
								NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
								if (retVal == NSAlertAlternateReturn) {
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
								}
							}
						});
					});
				} else {
					MADEndExport(madDriver);
				}
			} else {
				MADEndExport(madDriver);
			}
			RELEASEOBJ(savePanel);
		}
			break;
			
		case -2:
			//MP4
		{
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:@[@"com.apple.m4a-audio"]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as MPEG-4 Audio"];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				if ([self showExportSettings] == NSAlertDefaultReturn) {
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						NSData *saveData = RETAINOBJ([self getSoundData:&exportSettings]);
						NSString *oldMusicName = RETAINOBJ(musicName);
						NSString *oldMusicInfo = RETAINOBJ(musicInfo);
						NSURL *oldURL = [[musicList objectInMusicListAtIndex:previouslyPlayingIndex.index] musicUrl];
						MADEndExport(madDriver);
						NSError *expErr = nil;
						dispatch_block_t errBlock = ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							}else {
								NSRunAlertPanel(@"Export failed", @"Export/coversion of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
							}
						};
#if PPEXPORT_CREATE_TMP_AIFF
						NSURL *tmpURL = [[[NSFileManager defaultManager] URLForDirectory:NSItemReplacementDirectory inDomain:NSUserDomainMask appropriateForURL:oldURL create:YES error:nil] URLByAppendingPathComponent:[NSString stringWithFormat:@"%@.aiff", oldMusicName] isDirectory:NO];						
						[saveData writeToURL:tmpURL atomically:NO];
						QTMovie *exportMov = [[QTMovie alloc] initWithURL:tmpURL error:&expErr];
						if (exportMov) {
							[exportMov setAttribute:oldMusicName forKey:QTMovieDisplayNameAttribute];
							[exportMov setAttribute:oldMusicInfo forKey:QTMovieCopyrightAttribute];
						}
#else
						//Attempts of using data directly have resulted in internal assertion failures in the export session initialization code
						QTDataReference *dataRef = [[QTDataReference alloc] initWithReferenceToData:saveData name:oldMusicName MIMEType:@"audio/aiff"];
						
						QTMovie *exportMov = [[QTMovie alloc] initWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys:dataRef, QTMovieDataReferenceAttribute, @NO, QTMovieOpenAsyncOKAttribute, @YES, QTMovieDontInteractWithUserAttribute, @NO, QTMovieOpenForPlaybackAttribute, oldMusicName, QTMovieDisplayNameAttribute, oldMusicInfo, QTMovieCopyrightAttribute, nil] error:&expErr];
#endif
						RELEASEOBJ(oldMusicInfo);
						oldMusicInfo = nil;
						RELEASEOBJ(saveData);
						saveData = nil;
						if (!exportMov) {
							NSLog(@"Init Failed for %@, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
							
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						
						QTExportSession *session = [[QTExportSession alloc] initWithMovie:exportMov exportOptions:[QTExportOptions exportOptionsWithIdentifier:QTExportOptionsAppleM4A] outputURL:[savePanel URL] error:&expErr];
						if (!session) {
							NSLog(@"Export session creation for %@ failed, error: %@", oldMusicName, [expErr localizedDescription]);
#if !PPEXPORT_CREATE_TMP_AIFF
#else
							[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif

							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						[session run];
						BOOL didFinish = [session waitUntilFinished:&expErr];
						if (!didFinish)
						{
							NSLog(@"export of \"%@\" failed, error: %@", oldMusicName, [expErr localizedDescription]);
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
#if !PPEXPORT_CREATE_TMP_AIFF
#else
						[[NSFileManager defaultManager] removeItemAtURL:tmpURL error:NULL];
#endif
						
						if (didFinish) {
							dispatch_async(dispatch_get_main_queue(), ^{
								if (isQuitting) {
									[NSApp replyToApplicationShouldTerminate:YES];
								} else {
									NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
									if (retVal == NSAlertAlternateReturn) {
										[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
									}
								}
							});
						}

					});
				} else {
					MADEndExport(madDriver);
				}
			} else {
				MADEndExport(madDriver);
			}
			RELEASEOBJ(savePanel);
		}
			break;
			
		default:
		{
			if (tag > madLib->TotalPlug || tag < 0) {
				NSBeep();
				MADEndExport(madDriver);
				if (isQuitting) {
					[NSApp replyToApplicationShouldTerminate:YES];
				}
				
				return;
			}
			NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
			[savePanel setAllowedFileTypes:BRIDGE(NSArray*, madLib->ThePlug[tag].UTItypes)];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", BRIDGE(NSString*, madLib->ThePlug[tag].MenuName)]];
			if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
				NSURL *fileURL = RETAINOBJ([savePanel URL]);
				OSErr err = MADMusicExportCFURL(madLib, music, madLib->ThePlug[tag].type, BRIDGE(CFURLRef, fileURL));
				if (err != noErr) {
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSError *aerr = CreateErrorFromMADErrorType(err);
						[[NSAlert alertWithError:aerr] runModal];
						RELEASEOBJ(aerr);
					}
				} else {
					[self addMusicToMusicList:fileURL loadIfPreferencesAllow:NO];
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSInteger retVal = NSRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"Okay", @"Show File", nil, [[savePanel URL] lastPathComponent]);
						if (retVal == NSAlertAlternateReturn) {
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
						}
					}
				}
				RELEASEOBJ(fileURL);
			}
			RELEASEOBJ(savePanel);
		}
			MADEndExport(madDriver);
			break;
	}
}

- (IBAction)okayExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertDefaultReturn];
	[exportWindow close];
}

- (IBAction)cancelExportSettings:(id)sender {
	[NSApp stopModalWithCode:NSAlertAlternateReturn];
	[exportWindow close];
}

- (IBAction)saveMusicAs:(id)sender
{
	MADBeginExport(madDriver);
	
	NSSavePanel * savePanel = RETAINOBJ([NSSavePanel savePanel]);
	[savePanel setAllowedFileTypes:@[MADNativeUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:musicName];
	}
	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		NSURL *saveURL = RETAINOBJ([savePanel URL]);
		[self saveMusicToURL:saveURL];
		[self addMusicToMusicList:saveURL loadIfPreferencesAllow:NO];
		RELEASEOBJ(saveURL);
	}
	RELEASEOBJ(savePanel);
	MADEndExport(madDriver);
}

- (IBAction)saveMusic:(id)sender
{
	MADBeginExport(madDriver);
	
	if (previouslyPlayingIndex.index == -1) {
		// saveMusicAs: will end the exporting when it is done.
		[self saveMusicAs:sender];
	} else {
		NSURL *fileURL = previouslyPlayingIndex.playbackURL;
		NSString *filename = [fileURL path];
		NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
		NSString *utiFile = [sharedWorkspace typeOfFile:filename error:nil];
		if (/*[sharedWorkspace type:utiFile conformsToType:MADNativeUTI]*/ [utiFile isEqualToString:MADNativeUTI]) {
			[self saveMusicToURL:fileURL];
			MADEndExport(madDriver);
		} else {
			// saveMusicAs: will end the exporting when it is done.
			[self saveMusicAs:sender];
		}
	}
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr
{
	if (music != NULL) {
		if (music->hasChanged) {
			NSInteger selection = 0;
			if (previouslyPlayingIndex.index == -1) {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The new music file has unsaved changes. Do you want to save?", @"New unsaved file"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"));
			} else {
				selection = NSRunAlertPanel(NSLocalizedString(@"Unsaved Changes", @"Unsaved Changes"), NSLocalizedString(@"The music file \"%@\" has unsaved changes. Do you want to save?", @"file unsaved"), NSLocalizedString(@"Save", @"Save"), NSLocalizedString(@"Don't Save", @"Don't Save"), NSLocalizedString(@"Cancel", @"Cancel"), [[musicList objectInMusicListAtIndex:previouslyPlayingIndex.index] fileName]);
			}
			switch (selection) {
				case NSAlertDefaultReturn:
					[self saveMusic:nil];
					break;
					
				case NSAlertAlternateReturn:
				default:
					break;
					
				case NSAlertOtherReturn:
					[previouslyPlayingIndex movePlayingIndexToOtherIndex:currentlyPlayingIndex];
					if (theErr) {
						*theErr = AUTORELEASEOBJ([[NSError alloc] initWithDomain:NSCocoaErrorDomain code:NSUserCancelledError userInfo:nil]);
					}
					return NO;
					break;
			}
		}
		MADStopMusic(madDriver);
		MADCleanDriver(madDriver);
		MADDisposeMusic(&music, madDriver);
	}

	char fileType[5];
	OSErr theOSErr = MADMusicIdentifyCFURL(madLib, fileType, BRIDGE(CFURLRef, musicToLoad));
		
	if(theOSErr != noErr)
	{
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	theOSErr = MADLoadMusicCFURLFile(madLib, &music, fileType, BRIDGE(CFURLRef, musicToLoad));
	
	if (theOSErr != noErr) {
		if (theErr) {
			*theErr = AUTORELEASEOBJ(CreateErrorFromMADErrorType(theOSErr));
		}
		self.paused = YES;
		[self clearMusic];
		return NO;
	}
	
	MADAttachDriverToMusic(madDriver, music, NULL);
	MADPlayMusic(madDriver);
	self.paused = NO;
	{
		long fT, cT;
		MADGetMusicStatus(madDriver, &fT, &cT);
		[songPos setMaxValue:fT];
		[songPos setMinValue:0.0];
		[self setTitleForSongLabelBasedOnMusic];
		[songTotalTime setIntegerValue:fT];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPMusicDidChange object:self];

	if (theErr) {
		*theErr = nil;
	}
	return YES;
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad
{
	return [self loadMusicURL:musicToLoad error:NULL];
}
#endif

- (IBAction)showPreferences:(id)sender
{
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[[preferences window] center];
	[preferences showWindow:sender];
}


- (IBAction)exportInstrumentAs:(id)sender
{
    //[instrumentController exportInstrument:sender];
}

- (IBAction)showInstrumentsList:(id)sender
{
    //[instrumentController showWindow:sender];
}

#if 0
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"paused"]) {
		id boolVal = change[NSKeyValueChangeNewKey];
		
		//[pauseButton highlight:[boolVal boolValue]];
		switch ([boolVal boolValue]) {
			case NO:
			default:
				[pauseButton setState:NSOffState];
				[pauseDockMenuItem setState:NSOnState];
				break;
				
			case YES:
				[pauseButton setState:NSOnState];
				[pauseDockMenuItem setState:NSOffState];
				break;
		}
	}
	//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}
#endif

- (IBAction)showPlugInInfo:(id)sender
{
	PPPlugInInfo *inf = plugInInfos[[sender tag]];
	if (!inf) {
		return;
	}
	
	PPPlugInInfoController *infoCont = [[PPPlugInInfoController alloc] initWithPlugInInfo:inf];
	[[infoCont window] center];
	[NSApp runModalForWindow:[infoCont window]];
	//[infoCont showWindow:sender];
}

- (void)updatePlugInInfoMenu
{
	NSInteger i;

	//[plugInInfos removeAllObjects];
	
	for (PPLibraryObject *obj in madLib) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"TrackerPlugName", @"Tracker plug-in name") plugURL:[obj.plugFile bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
	for (PPInstrumentImporterObject *obj in instrumentImporter) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"InstrumentPlugName", @"Instrument plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
	}
	
#if 0
	for (PPDigitalPlugInObject *obj in digitalHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"DigitalPlugName", @"Digital plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
		RELEASEOBJ(tmpInfo);
	}
	
	for (PPFilterPlugObject *obj in filterHandler) {
		PPPlugInInfo *tmpInfo = [[PPPlugInInfo alloc] initWithPlugName:obj.menuName author:obj.authorString plugType:NSLocalizedString(@"FilterPlugName", @"Filter plug-in name") plugURL:[[obj file] bundleURL]];
		if (![plugInInfos containsObject:tmpInfo]) {
			[plugInInfos addObject:tmpInfo];
		}
		RELEASEOBJ(tmpInfo);
	}
#endif
	
	[plugInInfos sortWithOptions:NSSortConcurrent usingComparator:^NSComparisonResult(id obj1, id obj2) {
		NSString *menuNam1 = [obj1 plugName];
		NSString *menuNam2 = [obj2 plugName];
		NSComparisonResult res = [menuNam1 localizedStandardCompare:menuNam2];
		return res;
	}];
	
	[aboutPlugInMenu removeAllItems];
	
	for (i = 0; i < [plugInInfos count]; i++) {
		PPPlugInInfo *pi = plugInInfos[i];
		NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:pi.plugName action:@selector(showPlugInInfo:) keyEquivalent:@""];
		[mi setTag:i];
		[mi setTarget:self];
		[aboutPlugInMenu addItem:mi];
	}
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	isQuitting = NO;
	undoManager = [[NSUndoManager alloc] init];
	srandom(time(NULL) & 0xffffffff);
	PPRegisterDebugFunc(CocoaDebugStr);
	self.madLib = [[PPLibrary alloc] init];

	[self addObserver:self forKeyPath:@"paused" options:NSKeyValueObservingOptionNew context:NULL];
	//self.paused = YES;
	
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	instrumentImporter = [[PPInstrumentPlugHandler alloc] init];
	NSInteger i;
	for (i = 0; i < [instrumentImporter plugInCount]; i++) {
		PPInstrumentImporterObject *obj = [instrumentImporter plugInAtIndex:i];
		if (obj.mode == MADPlugImportExport || obj.mode == MADPlugExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:obj.menuName action:@selector(exportInstrument:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[instrumentExportMenu addItem:mi];
		}
	}
	
	for (i = 0; i < [madLib pluginCount]; i++) {
		PPLibraryObject *obj = [madLib pluginAtIndex:i];
		if (obj.canExport) {
			NSMenuItem *mi = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@...", obj.menuName] action:@selector(exportMusicAs:) keyEquivalent:@""];
			[mi setTag:i];
			[mi setTarget:self];
			[musicExportMenu addItem:mi];
		}
	}
	
	plugInInfos = [[NSMutableArray alloc] init];
	[self updatePlugInInfoMenu];
		
	exportController = [[PPSoundSettingsViewController alloc] init];
	exportController.delegate = self;
	[exportSettingsBox setContentView:[exportController view]];
	
	timeChecker = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0] interval:1/8.0 target:self selector:@selector(updateMusicStats:) userInfo:nil repeats:YES];
	[[NSRunLoop mainRunLoop] addTimer:timeChecker forMode:NSRunLoopCommonModes];
}

#if 0
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	if (MADIsExporting(madDriver)) {
		NSInteger selection = NSRunAlertPanel(@"Exporting", @"PlayerPRO is currently exporting a tracker file.\nQuitting will stop this. Do you really wish to quit?", @"Wait", @"Quit", @"Cancel");
		switch (selection) {
			default:
			case NSAlertOtherReturn:
				return NSTerminateCancel;
				break;
				
			case NSAlertAlternateReturn:
				return NSTerminateNow;
				break;
				
			case NSAlertDefaultReturn:
				//Double-check to make sure we're still exporting
				if (MADIsExporting(madDriver)) {
					isQuitting = YES;
					return NSTerminateLater;
				} else {
					return NSTerminateNow;
				}
				
				break;
		}
	}
	return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[timeChecker invalidate];

	[self removeObserver:self forKeyPath:@"paused"];
	
	RELEASEOBJ(instrumentImporter);
	instrumentImporter = nil;
	
	RELEASEOBJ(digitalHandler);
	digitalHandler = nil;
	
	RELEASEOBJ(filterHandler);
	filterHandler = nil;
	
	RELEASEOBJ(patternHandler);
	patternHandler = nil;
	
	RELEASEOBJ(undoManager);
	undoManager = nil;
	
	MADDisposeLibrary(madLib);	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (IBAction)deleteInstrument:(id)sender
{
    [instrumentController deleteInstrument:sender];
}

- (IBAction)saveInstrumentList:(id)sender
{
	MADBeginExport(madDriver);
	NSSavePanel *savePanel = RETAINOBJ([NSSavePanel savePanel]);
	[savePanel setAllowedFileTypes:@[PPInstrumentListUTI]];
	[savePanel setCanCreateDirectories:YES];
	[savePanel setCanSelectHiddenExtension:YES];
	if (![musicName isEqualToString:@""]) {
		[savePanel setNameFieldStringValue:[NSString stringWithFormat:@"%@'s instruments", musicName]];
	} else {
		[savePanel setNameFieldStringValue:@"Tracker Instruments"];
	}

	if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
		OSErr fileErr = [instrumentController exportInstrumentListToURL:[savePanel URL]];
		if (fileErr) {
			NSError *theErr = CreateErrorFromMADErrorType(fileErr);
			[[NSAlert alertWithError:theErr] runModal];
			RELEASEOBJ(theErr);
		}
	}
	
	MADEndExport(madDriver);
	RELEASEOBJ(savePanel);
}

- (IBAction)showBoxEditor:(id)sender
{
    
}

- (IBAction)showClassicEditor:(id)sender
{
    
}

- (IBAction)showDigitalEditor:(id)sender
{
    
}
#endif

- (BOOL)handleFile:(NSURL *)theURL ofType:(NSString *)theUTI
{
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
	if ([sharedWorkspace type:theUTI conformsToType:MADPackageUTI]) {
		// Do nothing right now
		return NO;
	} else if ([theUTI isEqualToString:MADGenericUTI]) {
		NSInteger retVal = NSRunInformationalAlertPanel(NSLocalizedString(@"Invalid Extension", @"Invalid extension"), NSLocalizedString(@"The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", @"Invalid extension description"), NSLocalizedString(@"Rename", @"rename file"), NSLocalizedString(@"Open", @"Open a file"), NSLocalizedString(@"Cancel", @"Cancel"), [theURL lastPathComponent]);
		switch (retVal) {
			case NSAlertDefaultReturn:
			{
				PPInfoRec rec;
				char ostype[5] = {0};
				//theURL = [theURL fileReferenceURL];
				if ([madLib identifyFileAtURL:theURL type:ostype] != noErr || [madLib getInformationFromFileAtURL:theURL type:ostype info:&rec] != noErr) {
					NSRunCriticalAlertPanel(NSLocalizedString(@"Unknown File", @"unknown file"), NSLocalizedString(@"The file type could not be identified.", @"Unidentified file"), nil, nil, nil);
					return NO;
				}
				OSType2Ptr(rec.signature, ostype);

				NSURL *tmpURL = [theURL URLByDeletingPathExtension];
				tmpURL = [tmpURL URLByAppendingPathExtension:[[NSString stringWithCString:ostype encoding:NSMacOSRomanStringEncoding] lowercaseString]];
				NSError *err = nil;
				if (![[NSFileManager defaultManager] moveItemAtURL:theURL toURL:tmpURL error:&err]) {
					NSLog(@"Could not move file, error: %@", err);
					NSRunInformationalAlertPanel(NSLocalizedString(@"Rename Error", @"Rename Error"), NSLocalizedString(@"The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", @"Could not rename file"), nil, nil, nil, [tmpURL lastPathComponent], [theURL lastPathComponent]);
				} else {
					theURL = tmpURL;
					//TODO: regenerate the UTI
				}
			}
				break;
				
			case NSAlertAlternateReturn:
				break;
				
			case NSAlertOtherReturn:
			default:
				return NO;
				break;
		}
	}
#if 0
	if ([sharedWorkspace type:theUTI conformsToType:PPPCMDUTI]) {
		OSErr theOSErr = [patternHandler importPcmdFromURL:theURL];
		if (theOSErr != noErr) {
			NSError *theErr = CreateErrorFromMADErrorType(theOSErr);
			[[NSAlert alertWithError:theErr] runModal];
			RELEASEOBJ(theErr);
			return NO;
		}
		return YES;
	} else if([sharedWorkspace type:theUTI conformsToType:PPInstrumentListUTI]) {
		NSError *err = nil;
		if (![instrumentController importInstrumentListFromURL:theURL error:&err]) {
			[[NSAlert alertWithError:err] runModal];
		} else return YES;
	} else
#endif
	{
		{
			//TODO: check for valid extension.
			NSDictionary *tracDict = self.trackerDict;
			NSMutableArray *trackerUTIs = [NSMutableArray array];
			for (NSString *key in tracDict) {
				[trackerUTIs addObjectsFromArray:tracDict[key]];
			}
			for (NSString *aUTI in trackerUTIs) {
				if([sharedWorkspace type:theUTI conformsToType:aUTI])
				{
					//[self addMusicToMusicList:theURL];
					return YES;
				}
			}
		}
		{
			NSMutableArray *instrumentArray = [NSMutableArray array];
			for (PPInstrumentImporterObject *obj in instrumentImporter) {
				[instrumentArray addObjectsFromArray:obj.UTITypes];
			}
			
			for (NSString *aUTI in instrumentArray) {
				if ([sharedWorkspace type:theUTI conformsToType:aUTI]) {
#if 0
					if ([instrumentController isWindowLoaded]) {
						NSError *theErr = nil;
						if (![instrumentController importSampleFromURL:theURL makeUserSelectInstrument:YES error:&theErr])
						{
							[[NSAlert alertWithError:theErr] runModal];
							return NO;
						}
						return YES;
					} else return NO;
#endif
				}
			}
		}
	}
	return NO;
}

- (IBAction)openFile:(id)sender {
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSDictionary *trackerDict = self.trackerDict;
		
	NSMutableDictionary *samplesDict = nil;
		NSInteger plugCount = [instrumentImporter plugInCount];
		samplesDict = [[NSMutableDictionary alloc] initWithCapacity:plugCount];
		for (PPInstrumentImporterObject *obj in instrumentImporter) {
			NSArray *tmpArray = obj.UTITypes;
			samplesDict[obj.menuName] = tmpArray;
		}

	NSDictionary *otherDict = @{@"PCMD": @[PPPCMDUTI], @"Instrument List": @[PPInstrumentListUTI]};
		
	OpenPanelViewController *av = [[OpenPanelViewController alloc] initWithOpenPanel:panel trackerDictionary:trackerDict playlistDictionary:nil instrumentDictionary:samplesDict additionalDictionary:otherDict];
	[av setupDefaults];
	if([panel runModal] == NSFileHandlingPanelOKButton)
	{
		NSURL *panelURL = [panel URL];
		NSString *filename = [panelURL path];
		NSError *err = nil;
		NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
		if (err) {
			NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
			return;
		}
		[self handleFile:panelURL ofType:utiFile];
	}
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSError *err = nil;
	NSString *utiFile = [[NSWorkspace sharedWorkspace] typeOfFile:filename error:&err];
	if (err) {
		NSRunAlertPanel(@"Error opening file", @"Unable to open %@: %@", nil, nil, nil, [filename lastPathComponent], [err localizedFailureReason]);
		return NO;
	}
	return [self handleFile:[NSURL fileURLWithPath:filename] ofType:utiFile];
}

#pragma mark PPSoundSettingsViewControllerDelegate methods

- (void)soundOutBitsDidChange:(short)bits
{
	exportSettings.outPutBits = bits;
}

- (void)soundOutRateDidChange:(unsigned int)rat
{
	exportSettings.outPutRate = rat;
}

- (void)soundOutReverbDidChangeActive:(BOOL)isAct
{
	exportSettings.Reverb = isAct;
}

- (void)soundOutOversamplingDidChangeActive:(BOOL)isAct
{
	if (!isAct) {
		exportSettings.oversampling = 1;
	}
}

- (void)soundOutStereoDelayDidChangeActive:(BOOL)isAct
{
	if (!isAct) {
		exportSettings.MicroDelaySize = 0;
	}
}

- (void)soundOutSurroundDidChangeActive:(BOOL)isAct
{
	exportSettings.surround = isAct;
}

- (void)soundOutReverbStrengthDidChange:(short)rev
{
	exportSettings.ReverbStrength = rev;
}

- (void)soundOutReverbSizeDidChange:(short)rev
{
	exportSettings.ReverbSize = rev;
}

- (void)soundOutOversamplingAmountDidChange:(short)ovs
{
	exportSettings.oversampling = ovs;
}

- (void)soundOutStereoDelayAmountDidChange:(short)std
{
	exportSettings.MicroDelaySize = std;
}

@end
