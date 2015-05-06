//
//  PPErrors.m
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import "PPErrors.h"
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPMusicObject.h"

#ifndef __MACERRORS__
#define paramErr (-50)
#endif

NSString * const PPMADErrorDomain = @"net.sourceforge.playerpro.PlayerPROKit.ErrorDomain";

#define PPErrorLocalizedString(theKey, comment) NSLocalizedStringWithDefaultValue(theKey, @"PPErrors", PPKBundle, theKey, comment)

BOOL PPErrorIsUserCancelled(NSError *theErr)
{
	if ([theErr.domain isEqualToString:PPMADErrorDomain]) {
		if (theErr.code == MADUserCanceledErr) {
			return YES;
		}
	} else if([theErr.domain isEqualToString:NSCocoaErrorDomain]) {
		// Also catch Cocoa's user cancelled error.
		if (theErr.code == NSUserCancelledError) {
			return YES;
		}
	}
	
	return NO;
}

NSError *PPCreateErrorFromMADErrorType(MADErr theErr)
{
	return PPCreateErrorFromMADErrorTypeConvertingToCocoa(theErr, NO);
}

NSError *PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa)
{
	static NSBundle *PPKBundle;
	static dispatch_once_t errorOnceToken;
	dispatch_once(&errorOnceToken, ^{
		PPKBundle = [NSBundle bundleForClass:[PPMusicObject class]];
	});
	NSString *ErrorDescription;
	NSString *errorReason;
	NSString *recoverySuggestion;
	NSDictionary *userInfo;
	NSError *cocoaEquiv;
	
	switch (theErr) {
		case MADNoErr:
			return nil;
			break;
			
		case MADNeedMemory:
			ErrorDescription = PPErrorLocalizedString(@"Not enough memory for operation", @"Not enough memory");
			errorReason = PPErrorLocalizedString(@"Ran out of memory", @"Ran out of memory");
			recoverySuggestion = PPErrorLocalizedString(@"Try closing some applications or closing windows.", @"Close apps");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSPOSIXErrorDomain code:ENOMEM
										 userInfo:userInfo];
			break;
			
		case MADReadingErr:
			ErrorDescription = PPErrorLocalizedString(@"Error reading file", @"Error reading file");
			errorReason = PPErrorLocalizedString(@"Could not read file", @"");
			recoverySuggestion = PPErrorLocalizedString(@"check to see if you have read permissions for the file.", @"Can you read it?");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileReadUnknownError
										 userInfo:userInfo];
			break;
			
		case MADIncompatibleFile:
			ErrorDescription = PPErrorLocalizedString(@"The file is incompatible with PlayerPRO or one of it's plug-ins", @"Incompatible file");
			errorReason = PPErrorLocalizedString(@"The file is incompatible", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Check the file to make sure it is a valid file.", @"Check file");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileReadCorruptFileError
										 userInfo:userInfo];
			break;
			
		case MADLibraryNotInitialized:
			ErrorDescription = PPErrorLocalizedString(@"The library is not initialized", @"");
			errorReason = PPErrorLocalizedString(@"library not initalized", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};

			break;
			
		case MADParametersErr:
			ErrorDescription = PPErrorLocalizedString(@"Invalid paramaters were sent to a function", @"Invalid parameters");
			errorReason = PPErrorLocalizedString(@"Invalid parameters", @"Invalid parameters");
			recoverySuggestion = PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSOSStatusErrorDomain code:paramErr
										 userInfo:userInfo];
			break;
			
		case MADSoundManagerErr:
			ErrorDescription = PPErrorLocalizedString(@"Error initalizing the sound system", @"Sound system error");
			errorReason = PPErrorLocalizedString(@"Sound system initialization failure", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Make sure that the sound settings are supported by the sound system", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};

			break;
			
		case MADOrderNotImplemented:
			ErrorDescription = PPErrorLocalizedString(@"The selected order is not implemented in the plug-in", @"");
			errorReason = PPErrorLocalizedString(@"order not implemented", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};

			break;
			
		case MADFileNotSupportedByThisPlug:
			ErrorDescription = PPErrorLocalizedString(@"The file is not supported by this plug-in", @"");
			errorReason = PPErrorLocalizedString(@"Invalid file for plug-in", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Try using another plug-in, or checking to see if the file is okay.", @"try other plug-in");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain code:NSFileReadCorruptFileError
										 userInfo:userInfo];
			break;
			
		case MADCannotFindPlug:
			ErrorDescription = PPErrorLocalizedString(@"Unable to locate a plug-in to play this file", @"unknown plug-in");
			errorReason = PPErrorLocalizedString(@"Unable to locate a plug-in", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Make sure that the file is valid. Also try looking for a compatible plug-in.", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		case MADMusicHasNoDriver:
			ErrorDescription = PPErrorLocalizedString(@"The MAD music has no driver", @"");
			errorReason = PPErrorLocalizedString(@"Music has no driver", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		case MADDriverHasNoMusic:
			ErrorDescription = PPErrorLocalizedString(@"The MAD Driver has no music", @"");
			errorReason = PPErrorLocalizedString(@"Driver has no music", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Try attaching music to the driver by selecting the music.", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		case MADSoundSystemUnavailable:
			ErrorDescription = PPErrorLocalizedString(@"The selected sound system is not available", @"");
			errorReason = PPErrorLocalizedString(@"Sound system is unavailable", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Try selecting a different sound system.", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		case MADWritingErr:
			ErrorDescription = PPErrorLocalizedString(@"Unable to write to file", @"");
			errorReason = PPErrorLocalizedString(@"Unable to write", @"");
			recoverySuggestion = PPErrorLocalizedString(@"Make sure you have write permissions at the location you selected.", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		case MADUserCanceledErr:
			ErrorDescription = PPErrorLocalizedString(@"User Cancelled Action", @"");
			errorReason = PPErrorLocalizedString(@"User Cancelled Action description", @"");
			recoverySuggestion = PPErrorLocalizedString(@"No Recovery needed", @"");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain code:NSUserCancelledError
										 userInfo:userInfo];
			break;
			
		case MADUnknownErr:
			ErrorDescription = PPErrorLocalizedString(@"An unknown error occured", @"Unknown error");
			errorReason = PPErrorLocalizedString(@"Unknown reason", @"unknown reason");
			recoverySuggestion = PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			
			userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
						 NSLocalizedFailureReasonErrorKey: errorReason,
						 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
			break;
			
		default:
			return [NSError errorWithDomain:NSOSStatusErrorDomain code:theErr userInfo:nil];
			break;
	}
	
	if (convertToCocoa && (cocoaEquiv != nil)) {
		return cocoaEquiv;
	}
	
	return [NSError errorWithDomain:PPMADErrorDomain code:theErr userInfo:userInfo];
}

