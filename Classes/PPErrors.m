//
//  PPErrors.m
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import "PPErrors.h"
#include <PlayerPROCore/PlayerPROCore.h>

NSString * const PPMADErrorDomain = @"net.sourceforge.playerpro.PlayerPRO_Cocoa.ErrorDomain";

NSError *CreateErrorFromMADErrorType(OSErr theErr)
{
	NSString *ErrorDescription = nil;
	NSString *errorReason = nil;
	NSString *recoverySuggestion = nil;
	
	switch (theErr) {
		case noErr:
			return nil;
			break;
			
		case MADNeedMemory:
			ErrorDescription = NSLocalizedString(@"Not enough memory for operation", @"Not enough memory");
			errorReason = NSLocalizedString(@"Ran out of memory", @"Ran out of memory");
			recoverySuggestion = NSLocalizedString(@"Try closing some applications or closing windows.", @"Close apps");
			break;
			
		case MADReadingErr:
			ErrorDescription = NSLocalizedString(@"Error reading file", @"Error reading file");
			errorReason = NSLocalizedString(@"Could not read file", @"");
			recoverySuggestion = NSLocalizedString(@"check to see if you have read permissions for the file.", @"Can you read it?");
			break;
			
		case MADIncompatibleFile:
			ErrorDescription = NSLocalizedString(@"The file is incompatible with PlayerPRO or one of it's plug-ins", @"Incompatible file");
			errorReason = NSLocalizedString(@"The file is incompatible", @"");
			recoverySuggestion = NSLocalizedString(@"Check the file to make sure it is a valid file.", @"Check file");
			break;
			
		case MADLibraryNotInitialized:
			ErrorDescription = NSLocalizedString(@"The library is not initialized", @"");
			errorReason = NSLocalizedString(@"library not initalized", @"");
			recoverySuggestion = NSLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			break;
			
		case MADParametersErr:
			ErrorDescription = NSLocalizedString(@"Invalid paramaters were sent to a function", @"Invalid parameters");
			errorReason = NSLocalizedString(@"Invalid parameters", @"Invalid parameters");
			recoverySuggestion = NSLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			break;
			
		case MADSoundManagerErr:
			ErrorDescription = NSLocalizedString(@"Error initalizing the sound system", @"Sound system error");
			errorReason = NSLocalizedString(@"Sound system initialization failure", @"");
			recoverySuggestion = NSLocalizedString(@"Make sure that the sound settings are supported by the sound system", @"");
			break;
			
		case MADOrderNotImplemented:
			ErrorDescription = NSLocalizedString(@"The selected order is not implemented in the plug-in", @"");
			errorReason = NSLocalizedString(@"order not implemented", @"");
			recoverySuggestion = NSLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			break;
			
		case MADFileNotSupportedByThisPlug:
			ErrorDescription = NSLocalizedString(@"The file is not supported by this plug-in", @"");
			errorReason = NSLocalizedString(@"Invalid file for plug-in", @"");
			recoverySuggestion = NSLocalizedString(@"Try using another plug-in, or checking to see if the file is okay.", @"try other plug-in");
			break;
			
		case MADCannotFindPlug:
			ErrorDescription = NSLocalizedString(@"Unable to locate a plug-in to play this file", @"unknown plug-in");
			errorReason = NSLocalizedString(@"Unable to locate a plug-in", @"");
			recoverySuggestion = NSLocalizedString(@"Make sure that the file is valid. Also try looking for a compatible plug-in.", @"");
			break;
			
		case MADMusicHasNoDriver:
			ErrorDescription = NSLocalizedString(@"The MAD music has no driver", @"");
			errorReason = NSLocalizedString(@"Music has no driver", @"");
			recoverySuggestion = NSLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			break;
			
		case MADDriverHasNoMusic:
			ErrorDescription = NSLocalizedString(@"The MAD Driver has no music", @"");
			errorReason = NSLocalizedString(@"Driver has no music", @"");
			recoverySuggestion = NSLocalizedString(@"Try attaching music to the driver by selecting the music.", @"");
			break;
			
		case MADSoundSystemUnavailable:
			ErrorDescription = NSLocalizedString(@"The selected sound system is not available", @"");
			errorReason = NSLocalizedString(@"Sound system is unavailable", @"");
			recoverySuggestion = NSLocalizedString(@"Try selecting a different sound system.", @"");
			break;
			
		case MADWritingErr:
			ErrorDescription = NSLocalizedString(@"Unable to write to file", @"");
			errorReason = NSLocalizedString(@"Unable to write", @"");
			recoverySuggestion = NSLocalizedString(@"Make sure you have write permissions at the location you selected.", @"");
			break;
			
		case MADUnknownErr:
		default:
			ErrorDescription = NSLocalizedString(@"An unknown error occured", @"Unknown error");
			errorReason = NSLocalizedString(@"Unknown reason", @"unknown reason");
			recoverySuggestion = NSLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			break;
	}
	
	return [[NSError alloc] initWithDomain:PPMADErrorDomain code:theErr userInfo:[NSDictionary dictionaryWithObjectsAndKeys:ErrorDescription, NSLocalizedDescriptionKey, errorReason, NSLocalizedFailureReasonErrorKey, recoverySuggestion, NSLocalizedRecoverySuggestionErrorKey, nil]];
}

