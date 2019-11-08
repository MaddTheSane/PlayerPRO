//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#ifndef __PLAYERPROKIT_PPERRORS_H__
#define __PLAYERPROKIT_PPERRORS_H__

#import <Foundation/NSString.h>
#include <PlayerPROCore/MADDefs.h>

#ifndef MTS_ERROR_ENUM
#define __MTS_ERROR_ENUM_GET_MACRO(_0, _1, _2, NAME, ...) NAME
#if ((__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))) && __has_attribute(ns_error_domain)
#define __MTS_NAMED_ERROR_ENUM(_type, _domain, _name)     enum _name : _type _name; enum __attribute__((ns_error_domain(_domain))) _name : _type
#define __MTS_ANON_ERROR_ENUM(_type, _domain)             enum __attribute__((ns_error_domain(_domain))) : _type
#else
#define __MTS_NAMED_ERROR_ENUM(_type, _domain, _name) NS_ENUM(_type, _name)
#define __MTS_ANON_ERROR_ENUM(_type, _domain) NS_ENUM(_type)
#endif

#define MTS_ERROR_ENUM(...) __MTS_ERROR_ENUM_GET_MACRO(__VA_ARGS__, __MTS_NAMED_ERROR_ENUM, __MTS_ANON_ERROR_ENUM)(__VA_ARGS__)
#endif

__BEGIN_DECLS

//! The error domain of \c MADErr errors in <code>NSError</code>s.
extern NSErrorDomain __nonnull const PPMADErrorDomain;

/*!
 *	@enum		PPMADError
 *	@abstract	Swift-friendly way of wrapping \c MADErr
 */
typedef MTS_ERROR_ENUM(short, PPMADErrorDomain, PPMADError) {
	/// No error was encountered
	PPMADErrorNone = MADNoErr,
	/// There isn't enough memory to execute the command.
	PPMADErrorNeedsMemory = MADNeedMemory,
	/// An error occured when trying to reading a file.
	PPMADErrorReading = MADReadingErr,
	/// The file isn't compatible with PlayerPROCore.
	PPMADErrorIncompatibleFile = MADIncompatibleFile,
	/// A library function was called without an initialized library.
	PPMADErrorLibraryNotInitialized = MADLibraryNotInitialized,
	/// Bad paramaters were sent to a function.
	PPMADErrorParameters = MADParametersErr,
	/// An unknown error occured.
	PPMADErrorUnknown = MADUnknownErr,
	/// An error occured when trying to initialize the sound system.
	PPMADErrorSoundManager = MADSoundManagerErr,
	/// The plug-in doesn't implement the order specified.
	PPMADErrorOrderNotImplemented = MADOrderNotImplemented,
	/// The file that a plug-in attempted to load was incompatible
	/// with said plug-in
	PPMADErrorFileNotSupportedByThisPlug = MADFileNotSupportedByThisPlug,
	/// PlayerPRO couldn't find the plug-in specified.
	PPMADErrorCannotFindPlug = MADCannotFindPlug,
	/// Attempted to use a music function that wasn't attached to a driver.
	PPMADErrorMusicHasNoDriver = MADMusicHasNoDriver,
	/// Attempted to use a driver function that requires the use of a loaded music file.
	PPMADErrorDriverHasNoMusic = MADDriverHasNoMusic,
	/// The sound system requested isn't available for the current architecture.
	PPMADErrorSoundSystemUnavailable = MADSoundSystemUnavailable,
	/// An error occured when trying to write to a file
	PPMADErrorWriting = MADWritingErr,
	/// The user cancelled an action. This shouldn't be seen by the user.
	PPMADErrorUserCancelled = MADUserCancelledErr,
};

/*!
 *	@function	PPCreateErrorFromMADErrorTypeConvertingToCocoaWithDictionary
 *	@abstract	Create an \c NSError object from a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate or convert.
 *	@param		convertToCocoa
 *				If <code>YES</code>, attempts to convert the error value into an error
 *				value native to Cocoa.
 *	@param		addlInfo
 *				Additional info to put into the <code>NSError</code>'s \c userInfo dictionary.
 *	@return		A retained \c NSError object, or \c nil if passed <code>MADNoErr</code>.
 *	@discussion	If \c convertToCocoa is <code>YES</code>, Attempts to create an \c NSError
 *	object in an error domain that is native to the Foundation framework, such as
 *	<code>NSPOSIXErrorDomain</code>. If there isn't an equivalent, creates an \c NSError
 *	object in the <code>PPMADErrorDomain</code>. It will also fill out the \c NSError \c userInfo keys: <code>NSLocalizedDescriptionKey</code>,
 *	<code>NSLocalizedFailureReasonErrorKey</code>, and
 *	<code>NSLocalizedRecoverySuggestionErrorKey</code>.
 *
 *	If the value in \c theErr isn't a value in \c MADErr, it returns an \c NSError in the
 *	\c NSOSStatusErrorDomain with nothing in the \c userInfo dictionary except what was in
 *	the \c addlInfo parameter.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorTypeConvertingToCocoaWithDictionary(MADErr theErr, BOOL convertToCocoa, NSDictionary<NSErrorUserInfoKey,id>* __nullable addlInfo) NS_RETURNS_RETAINED NS_REFINED_FOR_SWIFT;

/*!
 *	@function	PPCreateErrorFromMADErrorType
 *	@abstract	Create an \c NSError object encapulating a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate.
 *	@return		A retained \c NSError object, or \c nil if passed <code>MADNoErr</code>.
 *	@discussion	Creates an \c NSError object in the \c PPMADErrorDomain with 
 *				\c NSLocalizedDescriptionKey,
 *				\c NSLocalizedFailureReasonErrorKey, and 
 *				\c NSLocalizedRecoverySuggestionErrorKey set.
 *
 *				If the value in \c theErr isn't a value in <code>MADErr</code>, 
 *				it returns an \c NSError in the \c NSOSStatusErrorDomain with 
 *				nothing in the \c userInfo dictionary.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED NS_REFINED_FOR_SWIFT;

/*!
 *	@function	PPCreateErrorFromMADErrorTypeConvertingToCocoa
 *	@abstract	Create an \c NSError object from a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate or convert.
 *	@param		convertToCocoa
 *				If <code>YES</code>, attempts to convert the error value into an error
 *				value native to Cocoa.
 *	@return		A retained \c NSError object, or \c nil if passed <code>MADNoErr</code>.
 *	@discussion	Attempts to create an \c NSError object in an error domain that is native to the
 *				Foundation framework, such as \c NSPOSIXErrorDomain. If there isn't an equivalent,
 *				creates an \c NSError object in the \c PPMADErrorDomain with \c NSLocalizedDescriptionKey,
 *				\c NSLocalizedFailureReasonErrorKey, and \c NSLocalizedRecoverySuggestionErrorKey set.<br>
 *				If the value in \c theErr isn't a value in \c MADErr, it returns an \c NSError in the
 *				\c NSOSStatusErrorDomain with nothing in the \c userInfo dictionary.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa) NS_RETURNS_RETAINED NS_REFINED_FOR_SWIFT;

/*!
 *	@function	PPCreateErrorFromMADErrorTypeWithDictionary
 *	@abstract	Create an \c NSError object from a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate.
 *	@param		addlInfo
 *				Additional info to put into the <code>NSError</code>'s \c userInfo dictionary.
 *	@return		A retained \c NSError object, or \c nil if passed <code>MADNoErr</code>.
 *	@discussion	Creates an \c NSError object in the \c PPMADErrorDomain with
 *				<code>NSLocalizedDescriptionKey</code>,
 *				<code>NSLocalizedFailureReasonErrorKey</code> and
 *				\c NSLocalizedRecoverySuggestionErrorKey set.
 *
 *	If the value in \c theErr isn't a value in <code>MADErr</code>,
 *	it returns an \c NSError in the \c NSOSStatusErrorDomain with
 *	nothing in the \c userInfo dictionary except what was in
 *	the \c addlInfo parameter.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorTypeWithDictionary(MADErr theErr, NSDictionary<NSErrorUserInfoKey,id>* __nullable addlInfo) NS_RETURNS_RETAINED NS_REFINED_FOR_SWIFT;

/*!
 *	@function	PPErrorIsUserCancelled
 *	@abstract	Checks to see if the error type was cancelled by the user.
 *	@param		theErr
 *				The \c NSError object to check.
 *	@return		\c YES if the value was user cancelled, \c NO otherwise.
 *	@discussion	Checks if the error sent to it is a user cancelled error.
 *
 *	This checks for \c MADUserCancelledErr in the <code>PPMADErrorDomain</code>,
 *	\c NSUserCancelledError in the <code>NSCocoaErrorDomain</code>, and \c userCanceledErr
 *	in the <code>NSOSStatusErrorDomain</code>.
 */
extern BOOL PPErrorIsUserCancelled(NSError * __nonnull theErr);

/*!
 *	@function	PPLocalizedStringForKeyAndError
 *	@abstract	Returns the localized value of the specified \c NSErrorUserInfoKey from the
 *	specified \c MADErr
 *	@param userInfoKey The specified user key to get the localized version.
 *	@param errCode The \c MADErr to get the localized string from.
 *	@return A localized value, or \c nil if there isn't a localized value
 *	@discussion This function gets info from the \c PPErrors.strings file, which it locates
 *	as being in the same bundle as the \c PPMusicObject class. This is also used by \c NSError \c -setUserInfoValueProviderForDomain:provider
 */
extern NSString * __nullable PPLocalizedStringForKeyAndError(NSErrorUserInfoKey __nonnull userInfoKey, MADErr errCode);

__END_DECLS

#endif
