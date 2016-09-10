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

extension MADErr: ErrorType {
	/// PlayerPROKit's `PPMADErrorDomain`
	public var _domain: String {
		return PPMADErrorDomain
	}
	
	/// PlayerPROCore's `MADErr` raw value
	public var _code: Int {
		return Int(rawValue)
	}
	
	/// Throws `self` if `self` is anything other than `.NoErr`.
	public func throwIfNotNoErr() throws {
		if self != .NoErr {
			throw self
		}
	}
	
	/// Converts to an error to one in the built-in Cocoa error domains, if possible.
	@warn_unused_result public func convertToCocoaType() -> NSError {
		guard let anErr = PPCreateErrorFromMADErrorTypeConvertingToCocoa(self, true) else {
			return NSError(domain: PPMADErrorDomain, code: 0, userInfo: [NSLocalizedDescriptionKey: "Throwing MADNoErr! This shouldn't happen!"])
		}
		return anErr
	}
	
	/// Creates an `NSError` from `self`, optionally converting the error type to an error in the Cocoa error domain.
	///
	/// - parameter customUserDictionary: A dictionary with additional information. May be `nil`, default is `nil`.
	/// - parameter convertToCocoa: Converts `self` into a comparable error in Cocoa's error types. Default is `true`.
	/// - returns: An `NSError` value, or `nil` if `self` is `.NoErr`
	@warn_unused_result public func toNSError(customUserDictionary cud: [String: NSObject]? = nil, convertToCocoa: Bool = true) -> NSError? {
		if self == .NoErr {
			return nil
		}
		
		func populateErrors(error: NSError) -> NSError {
			if let cud = cud {
				var errDict: [NSObject: AnyObject] = error.userInfo
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
			let cocoaErr = PPCreateErrorFromMADErrorTypeConvertingToCocoa(self, true)!
			return populateErrors(cocoaErr)
		}
		
		do {
			throw self
		} catch let error as NSError {
			return populateErrors(error)
		}
	}
}

/// Creates an `NSError` from a `MADErr`, optionally converting the error type to an error in the Cocoa error domain.
///
/// - parameter theErr: The `MADErr` to convert to an `NSError`
/// - parameter customUserDictionary: A dictionary with additional information. May be `nil`, defaults to `nil`.
/// - parameter convertToCocoa: Converts the `MADErr` code into a compatible error in Cocoa's error types. defaults to `false`.
/// - returns: An `NSError` value, or `nil` if passed `.NoErr`
@warn_unused_result
public func createNSErrorFromMADErrorType(theErr: MADErr, customUserDictionary: [String: NSObject]? = nil, convertToCocoa: Bool = false) -> NSError? {
	return theErr.toNSError(customUserDictionary: customUserDictionary, convertToCocoa: convertToCocoa)
}
