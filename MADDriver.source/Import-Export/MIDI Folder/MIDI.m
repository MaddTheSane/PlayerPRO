//
//  MIDI.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import "PPMIDIImporter.h"

extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	__block MADErr theErr = MADOrderNotImplemented;
	NSXPCConnection *conn = [[NSXPCConnection alloc] initWithServiceName:@"net.sourceforge.playerpro.MIDI-Import"];
	conn.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	
	NSURL *ourURL = [[NSURL alloc] initFileURLWithFileSystemRepresentation:AlienFileName isDirectory:NO relativeToURL:nil];
	
	[conn resume];
	switch (order) {
		case MADPlugInfo:
		{
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
		}
			break;
			
		case MADPlugTest:
		{
			dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
			[[conn remoteObjectProxy] canImportMIDIFileAtURL:ourURL withReply:^(MADErr error) {
				theErr = error;
				[conn invalidate];
				dispatch_semaphore_signal(sessionWaitSemaphore);
			}];
			dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
		}
			break;
			
		case MADPlugImport:
		{
			dispatch_semaphore_t sessionWaitSemaphore = dispatch_semaphore_create(0);
			[[conn remoteObjectProxy] importMIDIFileAtURL:ourURL withReply:^(NSData *fileData, MADErr error) {
				
				[conn invalidate];
				dispatch_semaphore_signal(sessionWaitSemaphore);
			}];
			dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
		}
			
		default:
			theErr = MADOrderNotImplemented;
			[conn invalidate];
			break;
	}
	
	return theErr;
}
