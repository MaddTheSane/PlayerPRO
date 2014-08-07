//
//  MIDI.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <stdio.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include "PPMIDIImporter.h"


extern MADErr PPImpExpMain(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
{
	__block MADErr theErr = MADOrderNotImplemented;
	__block BOOL isDone = NO;
	NSXPCConnection *conn = [[NSXPCConnection alloc] initWithServiceName:@"net.sourceforge.playerpro.MIDI-Import"];
	conn.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	
	NSURL *ourURL = [[NSURL alloc] initFileURLWithFileSystemRepresentation:AlienFileName isDirectory:NO relativeToURL:nil];
	
	[conn resume];
	switch (order) {
		case MADPlugInfo:
		{
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
				isDone = YES;
			}];
		}
			break;
			
		case MADPlugTest:
		{
			[[conn remoteObjectProxy] canImportMIDIFileAtURL:ourURL withReply:^(MADErr error) {
				theErr = error;
				isDone = YES;
			}];
		}
			break;
			
		default:
			theErr = MADOrderNotImplemented;
			isDone = YES;
			break;
	}
	
	if (!isDone) {
		usleep(100);
	}
	return theErr;
}
