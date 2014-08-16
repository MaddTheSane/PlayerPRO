//
//  MIDI.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <Cocoa/Cocoa.h>
#include <xpc/xpc.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#import "PPMIDIImporter.h"

static MADErr MADReadMAD(MADMusic *MDriver, const void* MADPtr)
{
	short		i, x;
	size_t		inOutCount, OffSetToSample = 0;
	PatHeader	tempPatHeader;
	
	MDriver->musicUnderModification = false;
	
	/**** HEADER ****/
	inOutCount = sizeof(MADSpec);
	
	MDriver->header = (MADSpec*)calloc(inOutCount, 1);
	if (MDriver->header == NULL) {
		return MADNeedMemory;
	}
	
	memcpy(MDriver->header, MADPtr, inOutCount);
	OffSetToSample += inOutCount;
	
	if (MDriver->header->MAD != 'MADK' || MDriver->header->numInstru > MAXINSTRU) {
		free(MDriver->header);
		return MADIncompatibleFile;
	}
	
	if (MDriver->header->MultiChanNo == 0)
		MDriver->header->MultiChanNo = 48;
	
	/**** PARTITION ****/
	//TODO: dispatch this
	dispatch_apply(MAXPATTERN - MDriver->header->numPat, dispatch_get_global_queue(0, 0), ^(size_t iTmp) {
		size_t i = iTmp + MDriver->header->numPat;
		MDriver->partition[i] = NULL;
	});
	
	for (i = 0; i < MDriver->header->numPat; i++) {
		/** Lecture du header de la partition **/
		inOutCount = sizeof(PatHeader);
		
		memcpy(&tempPatHeader, MADPtr + OffSetToSample, inOutCount);
		
		inOutCount = sizeof(PatHeader) + MDriver->header->numChn * tempPatHeader.size * sizeof(Cmd);
		
		MDriver->partition[i] = (PatData*)malloc(inOutCount);
		if (MDriver->partition[i] == NULL) {
			for (x = 0; x < i; x++) {
				if (MDriver->partition[x] != NULL)
					free(MDriver->partition[x]);
			}
			free(MDriver->header);
			
			return MADNeedMemory;
		}
		
		memcpy(MDriver->partition[i], MADPtr + OffSetToSample, inOutCount);
		OffSetToSample += inOutCount;
	}
	
	/**** INSTRUMENTS ****/
	MDriver->fid = (InstrData*)calloc(sizeof(InstrData), MAXINSTRU);
	if (!MDriver->fid) {
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		
		return MADNeedMemory;
	}
	
	inOutCount = sizeof(InstrData) * MDriver->header->numInstru;
	memcpy(MDriver->fid, MADPtr + OffSetToSample, inOutCount);
	OffSetToSample += inOutCount;
	
	for (i = MDriver->header->numInstru - 1; i >= 0 ; i--) {
		InstrData *curIns = &MDriver->fid[i];
		
		if (i != curIns->no) {
			MDriver->fid[curIns->no] = *curIns;
			MADResetInstrument(curIns);
		}
	}
	MDriver->header->numInstru = MAXINSTRU;
	
	/**** SAMPLES ****/
	MDriver->sample = (sData**) calloc(sizeof(sData*), MAXINSTRU * MAXSAMPLE);
	if (!MDriver->sample) {
		for (x = 0; x < MAXINSTRU ; x++)
			MADKillInstrument(MDriver, x);
		
		for (x = 0; x < MDriver->header->numPat; x++) {
			if (MDriver->partition[x] != NULL)
				free(MDriver->partition[x]);
		}
		free(MDriver->header);
		
		return MADNeedMemory;
	}
	
	for (i = 0; i < MAXINSTRU ; i++) {
		for (x = 0; x < MDriver->fid[i].numSamples ; x++) {
			sData	 *curData;
			
			// ** Read Sample header **
			
			curData = MDriver->sample[i * MAXSAMPLE + x] = (sData*)malloc(sizeof(sData));
			if (curData == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = sizeof(sData32);
			
			memcpy(curData, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
			
			// ** Read Sample DATA
			
			curData->data = (char*)malloc(curData->size);
			if (curData->data == NULL) {
				for (x = 0; x < MAXINSTRU ; x++)
					MADKillInstrument(MDriver, x);
				
				for (x = 0; x < MDriver->header->numPat; x++) {
					if (MDriver->partition[x] != NULL)
						free(MDriver->partition[x]);
				}
				free(MDriver->header);
				
				return MADNeedMemory;
			}
			
			inOutCount = curData->size;
			memcpy(curData->data, MADPtr + OffSetToSample, inOutCount);
			OffSetToSample += inOutCount;
		}
	}
	dispatch_apply(MAXINSTRU, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
		MDriver->fid[ixi].firstSample = ixi * MAXSAMPLE;
	});
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	{
		short	alpha;
		
		MDriver->sets = (FXSets*)calloc(sizeof(FXSets), MAXTRACK);
		
		alpha = 0;
		
		for (i = 0; i < 10 ; i++) {	// Global Effects
			if (MDriver->header->globalEffect[i]) {
				inOutCount = sizeof(FXSets);
				memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
				OffSetToSample += inOutCount;
				alpha++;
			}
		}
		
		for (i = 0; i < MDriver->header->numChn ; i++) {	// Channel Effects
			for (x = 0; x < 4; x++) {
				if (MDriver->header->chanEffect[i][x]) {
					inOutCount = sizeof(FXSets);
					memcpy(&MDriver->sets[alpha], MADPtr + OffSetToSample, inOutCount);
					OffSetToSample += inOutCount;
					alpha++;
				}
			}
		}
	}
	
	MDriver->header->MAD = 'MADK';
	
	return MADNoErr;
}

extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	__block MADErr theErr = MADOrderNotImplemented;
	xpc_connection_t conn = xpc_connection_create("net.sourceforge.playerpro.MIDI-Import", NULL);
	//NSXPCConnection *conn = [[NSXPCConnection alloc] initWithServiceName:@"net.sourceforge.playerpro.MIDI-Import"];
	//conn.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	
	NSURL *ourURL = [[NSURL alloc] initFileURLWithFileSystemRepresentation:AlienFileName isDirectory:NO relativeToURL:nil];
	xpc_object_t xpcURL = xpc_string_create([ourURL fileSystemRepresentation]);
	const char * const keys[] = {"filePath"};
	xpc_object_t values[] = {xpcURL};
	xpc_object_t theDict = xpc_dictionary_create(keys, values, 1);
	
	xpc_object_t retVal;

	xpc_connection_resume(conn);
	switch (order) {
		case MADPlugInfo:
		{
			xpc_dictionary_set_int64(theDict, kPPMIDICall, PPInfoMIDI);
			retVal = xpc_connection_send_message_with_reply_sync(conn, theDict);
			theErr = xpc_dictionary_get_int64(retVal, kPPMIDIErr);
			xpc_object_t ppInfo = xpc_dictionary_get_value(retVal, kPPMIDIInfo);
#if 0
			dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
			[[conn remoteObjectProxy] getMIDIInfoFromFileAtURL:ourURL withReply:^(NSDictionary * ppInfo, MADErr error) {
				if (error) {
					theErr = error;
					return;
				}
				info->fileSize = [ppInfo[kPPFileSize] longValue];
				info->totalPatterns = [ppInfo[kPPTotalPatterns] intValue];
				info->partitionLength = [ppInfo[kPPPartitionLength] intValue];
				info->signature = [ppInfo[kPPSignature] unsignedIntValue];
				info->totalTracks = [ppInfo[kPPTotalTracks] shortValue];
				info->totalInstruments = [ppInfo[kPPTotalInstruments] shortValue];
				{
					NSString *internalFileName = ppInfo[kPPInternalFileName];
					NSString *formatDescription = ppInfo[kPPFormatDescription];
					[internalFileName getCString:info->internalFileName maxLength:60 encoding:NSMacOSRomanStringEncoding];
					[formatDescription getCString:info->internalFileName maxLength:60 encoding:NSMacOSRomanStringEncoding];
				}
				dispatch_semaphore_signal(sessionWaitSemaphore);
				[conn invalidate];
			}];
			dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
#endif
		}
			break;
			
		case MADPlugTest:
		{
			xpc_dictionary_set_int64(theDict, kPPMIDICall, PPTestMIDI);
			retVal = xpc_connection_send_message_with_reply_sync(conn, theDict);
			theErr = xpc_dictionary_get_int64(retVal, kPPMIDIErr);
#if 0
			dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
			[[conn remoteObjectProxy] canImportMIDIFileAtURL:ourURL withReply:^(MADErr error) {
				theErr = error;
				[conn invalidate];
				dispatch_semaphore_signal(sessionWaitSemaphore);
			}];
			dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
#endif
		}
			break;
			
		case MADPlugImport:
		{
			xpc_dictionary_set_int64(theDict, kPPMIDICall, PPImportMIDI);
			retVal = xpc_connection_send_message_with_reply_sync(conn, theDict);
			theErr = xpc_dictionary_get_int64(retVal, kPPMIDIErr);
#if 0
			dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
			[[conn remoteObjectProxy] importMIDIFileAtURL:ourURL withReply:^(NSData *fileData, MADErr error) {
				if (error == MADNoErr) {
					NSData *aTmpData = fileData;
					theErr = MADReadMAD(MadFile, [aTmpData bytes]);
					aTmpData = nil;
				} else {
					theErr = error;
				}
				[conn invalidate];
				dispatch_semaphore_signal(sessionWaitSemaphore);
			}];
			dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
#endif
		}
			
		default:
			theErr = MADOrderNotImplemented;
			break;
	}
	
	return theErr;
}
