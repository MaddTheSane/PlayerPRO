//
//  PPErrors.m
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>
#import "PPErrors.h"
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPMusicObject.h"

#ifndef __MACERRORS__
#define paramErr (-50)
#define userCanceledErr (-128)
#endif

#ifndef __private_extern
#define __private_extern __attribute__((visibility("hidden")))
#endif

NSString * const PPMADErrorDomain = @"net.sourceforge.playerpro.PlayerPROKit.ErrorDomain";

#define PPErrorLocalizedString(theKey, comment) NSLocalizedStringWithDefaultValue(theKey, @"PPErrors", PPKBundle, theKey, comment)

static NSString *stringForKeyAndError(NSString *userInfoKey, MADErr errCode)
{
	static NSBundle *PPKBundle;
	static dispatch_once_t errorOnceToken;
	dispatch_once(&errorOnceToken, ^{
		PPKBundle = [NSBundle bundleForClass:[PPMusicObject class]];
	});

	switch (errCode) {
			// MADNoErr handled just in case...
		case MADNoErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"You should not be seeing this. File a bug report!", @"No error failure reason");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"There is no error", @"No error description");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"No Recovery needed", @"No error recovery suggestion");
			}
			break;

		case MADNeedMemory:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Not enough memory for operation", @"Not enough memory");;
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Ran out of memory", @"Ran out of memory");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Try closing some applications or closing windows.", @"Close apps");
			}
			break;
			
		case MADReadingErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Error reading file", @"Error reading file");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Could not read file", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"check to see if you have read permissions for the file.", @"Can you read it?");
			}
			break;
			
		case MADIncompatibleFile:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The file is incompatible with PlayerPRO or one of it's plug-ins", @"Incompatible file");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"The file is incompatible", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Check the file to make sure it is a valid file.", @"Check file");
			}
			break;
			
		case MADLibraryNotInitialized:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The library is not initialized", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"library not initalized", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			}
			break;
			
		case MADParametersErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Invalid paramaters were sent to a function", @"Invalid parameters");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Invalid parameters", @"Invalid parameters");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			}
			break;
			
		case MADUnknownErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"An unknown error occured", @"Unknown error");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Unknown reason", @"unknown reason");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			}
			break;
			
		case MADSoundManagerErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Error initalizing the sound system", @"Sound system error");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Sound system initialization failure", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Make sure that the sound settings are supported by the sound system", @"");
			}
			break;
			
		case MADOrderNotImplemented:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The selected order is not implemented in the plug-in", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"order not implemented", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			}
			break;
			
		case MADFileNotSupportedByThisPlug:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The file is not supported by this plug-in", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Invalid file for plug-in", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Try using another plug-in, or checking to see if the file is okay.", @"try other plug-in");
			}
			break;
			
		case MADCannotFindPlug:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Unable to locate a plug-in to play this file", @"unknown plug-in");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Unable to locate a plug-in", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Make sure that the file is valid. Also try looking for a compatible plug-in.", @"");
			}
			break;
			
		case MADMusicHasNoDriver:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The MAD music has no driver", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Music has no driver", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Contact the developer with what you were doing to cause this error.", @"Contact Developer");
			}
			break;
			
		case MADDriverHasNoMusic:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The MAD Driver has no music", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Driver has no music", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Try attaching music to the driver by selecting the music.", @"");
			}
			break;
			
		case MADSoundSystemUnavailable:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"The selected sound system is not available", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Sound system is unavailable", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Try selecting a different sound system.", @"");
			}
			break;
			
		case MADWritingErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"Unable to write to file", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"Unable to write", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"Make sure you have write permissions at the location you selected.", @"");
			}
			break;
			
		case MADUserCancelledErr:
			if ([userInfoKey isEqualToString:NSLocalizedDescriptionKey]) {
				return PPErrorLocalizedString(@"User Cancelled Action", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedFailureReasonErrorKey]) {
				return PPErrorLocalizedString(@"User Cancelled Action description", @"");
			} else if ([userInfoKey isEqualToString:NSLocalizedRecoverySuggestionErrorKey]) {
				return PPErrorLocalizedString(@"No Recovery needed", @"");
			}
			break;

		default:
			break;
	}
	
	return nil;
}

#undef PPErrorLocalizedString

BOOL PPErrorIsUserCancelled(NSError *theErr)
{
	if ([theErr.domain isEqualToString:PPMADErrorDomain]) {
		if (theErr.code == MADUserCancelledErr) {
			return YES;
		}
	} else if([theErr.domain isEqualToString:NSCocoaErrorDomain]) {
		// Also catch Cocoa's user cancelled error.
		if (theErr.code == NSUserCancelledError) {
			return YES;
		}
	} else if([theErr.domain isEqualToString:NSOSStatusErrorDomain]) {
		// Also catch Carbon's user cancelled error.
		if (theErr.code == userCanceledErr) {
			return YES;
		}
	}
	
	return NO;
}

#if defined(__IPHONE_9_0) || defined(__OSX_10_11)
__private_extern @interface PPPrivateErrorLoading : NSObject
@end

@implementation PPPrivateErrorLoading

+ (void)load
{
	if ([[NSError class] respondsToSelector:@selector(setUserInfoValueProviderForDomain:provider:)]) {
		[NSError setUserInfoValueProviderForDomain:PPMADErrorDomain provider:^id (NSError *err, NSString *userInfoKey) {
			return stringForKeyAndError(userInfoKey, err.code);
		}];
	}
}

@end
#endif

NSError *PPCreateErrorFromMADErrorType(MADErr theErr)
{
	return PPCreateErrorFromMADErrorTypeConvertingToCocoa(theErr, NO);
}

NSError *PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa)
{
	NSString *ErrorDescription = stringForKeyAndError(NSLocalizedDescriptionKey, theErr);
	NSString *errorReason = stringForKeyAndError(NSLocalizedFailureReasonErrorKey, theErr);
	NSString *recoverySuggestion = stringForKeyAndError(NSLocalizedRecoverySuggestionErrorKey, theErr);
	NSDictionary *userInfo;
	if (!ErrorDescription) {
		userInfo = nil;
	} else {
		userInfo = @{NSLocalizedDescriptionKey: ErrorDescription,
					 NSLocalizedFailureReasonErrorKey: errorReason,
					 NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion};
	}
	NSError *cocoaEquiv;
	
	switch (theErr) {
		case MADNoErr:
			return nil;
			break;
			
		case MADNeedMemory:
			cocoaEquiv = [NSError errorWithDomain:NSPOSIXErrorDomain
											 code:ENOMEM
										 userInfo:userInfo];
			break;
			
		case MADReadingErr:
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain
											 code:NSFileReadUnknownError
										 userInfo:userInfo];
			break;
			
		case MADIncompatibleFile:
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain
											 code:NSFileReadCorruptFileError
										 userInfo:userInfo];
			break;
			
		case MADLibraryNotInitialized:
			break;
			
		case MADParametersErr:
			cocoaEquiv = [NSError errorWithDomain:NSOSStatusErrorDomain
											 code:paramErr
										 userInfo:userInfo];
			break;
			
		case MADSoundManagerErr:
			break;
			
		case MADOrderNotImplemented:
			break;
			
		case MADFileNotSupportedByThisPlug:
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain
											 code:NSFileReadCorruptFileError
										 userInfo:userInfo];
			break;
			
		case MADCannotFindPlug:
			break;
			
		case MADMusicHasNoDriver:
			break;
			
		case MADDriverHasNoMusic:
			break;
			
		case MADSoundSystemUnavailable:
			break;
			
		case MADWritingErr:
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain
											 code:NSFileWriteUnknownError
										 userInfo:userInfo];
			break;
			
		case MADUserCancelledErr:
			cocoaEquiv = [NSError errorWithDomain:NSCocoaErrorDomain
											 code:NSUserCancelledError
										 userInfo:userInfo];
			break;
			
		case MADUnknownErr:
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

