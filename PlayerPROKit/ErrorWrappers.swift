//
//  ErrorWrappers.swift
//  PPMacho
//
//  Created by C.W. Betts on 1/22/16.
//
//

import Foundation
import PlayerPROCore
import SwiftAdditions

extension MADErr: CustomNSError, LocalizedError {
	/// PlayerPROKit's `PPMADErrorDomain`
	public static var errorDomain: String {
		return PPMADErrorDomain
	}
	
	/// PlayerPROCore's `MADErr` raw value
	public var errorCode: Int {
		return Int(rawValue)
	}
	
	public var errorDescription: String? {
		return PPLocalizedStringForKeyAndError(NSLocalizedDescriptionKey, self)
	}
	public var failureReason: String? {
		return PPLocalizedStringForKeyAndError(NSLocalizedFailureReasonErrorKey, self)
	}

	public var recoverySuggestion: String? {
		return PPLocalizedStringForKeyAndError(NSLocalizedRecoverySuggestionErrorKey, self)
	}
	
	/// Throws `self` if `self` is anything other than `.NoErr`.
	public func throwIfNotNoErr() throws {
		if self != .noErr {
			throw self
		}
	}
	
	/// Converts to an error to one in the built-in Cocoa error domains, if possible.
	public func convertToCocoaType() -> Error {
		guard let anErr = __PPCreateErrorFromMADErrorTypeConvertingToCocoa(self, true) else {
			return NSError(domain: PPMADErrorDomain, code: 0, userInfo: [NSLocalizedDescriptionKey: "Throwing MADNoErr! This shouldn't happen!"])
		}
		return anErr
	}
	
	/// Creates an `NSError` from `self`, optionally converting the error type to an error in the Cocoa error domain.
	///
	/// - parameter customUserDictionary: A dictionary with additional information. May be `nil`, default is `nil`.
	/// - parameter convertToCocoa: Converts `self` into a comparable error in Cocoa's error types. Default is `true`.
	/// - returns: An `NSError` value, or `nil` if `self` is `.NoErr`
	public func toNSError(customUserDictionary cud: [String: Any]? = nil, convertToCocoa: Bool = true) -> NSError? {
		guard self != .noErr else {
			return nil
		}
		
		func populate(error: NSError) -> NSError {
			if let cud = cud {
				var errDict: [String: Any] = error.userInfo
				errDict[NSLocalizedDescriptionKey] = error.localizedDescription
				if let aFailReason = error.localizedFailureReason {
					errDict[NSLocalizedFailureReasonErrorKey] = aFailReason
				}
				
				if let aRecoverySuggestion = error.localizedRecoverySuggestion {
					errDict[NSLocalizedRecoverySuggestionErrorKey] = aRecoverySuggestion
				}
				
				errDict += cud
				
				return NSError(domain: error.domain, code: error.code, userInfo: errDict)
			} else {
				return error
			}
		}
		
		if convertToCocoa {
			let cocoaErr = __PPCreateErrorFromMADErrorTypeConvertingToCocoa(self, true)!
			return populate(error: cocoaErr as NSError)
		}
		
		return populate(error: self as NSError)
	}
	
	/// Creates a `MADErr` from the provided `NSError`.
	/// Is `nil` if the error isn't in the `PPMADErrorDomain` or
	/// the error code isn't in `MADErr`.
	public init?(error anErr: NSError) {
		guard anErr.domain == PPMADErrorDomain,
			let exact = Int16(exactly: anErr.code),
			let errVal = MADErr(rawValue: exact) else {
				return nil
		}
		
		self = errVal
	}
}

/// Creates an `NSError` from a `MADErr`, optionally converting the error type to an error in the Cocoa error domain.
///
/// - parameter theErr: The `MADErr` to convert to an `NSError`
/// - parameter customUserDictionary: A dictionary with additional information. May be `nil`, defaults to `nil`.
/// - parameter convertToCocoa: Converts the `MADErr` code into a compatible error in Cocoa's error types. defaults to `false`.
/// - returns: An `NSError` value, or `nil` if passed `.noErr`
public func createNSError(from theErr: MADErr, customUserDictionary: [String: Any]? = nil, convertToCocoa: Bool = false) -> NSError? {
	return theErr.toNSError(customUserDictionary: customUserDictionary, convertToCocoa: convertToCocoa)
}
