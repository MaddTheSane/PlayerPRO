//
//  PPLibrary.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/24/15.
//
//

import Foundation
import PlayerPROCore
import SwiftAdditions

private let kPlayerPROMADKUTI = "com.quadmation.playerpro.madk"
private let MadIDString = OSTypeToString(MadID)!

private func toDictionary(infoRec: MADInfoRec) -> NSDictionary {
	let aArray: [Int8] = try! arrayFromObject(reflecting: infoRec.internalFileName, appendLastObject: 0)
	let bArray: [Int8] = try! arrayFromObject(reflecting: infoRec.formatDescription, appendLastObject: 0)
	
	return [kPPTotalPatterns:	Int(infoRec.totalPatterns),
		kPPPartitionLength:		Int(infoRec.partitionLength),
		kPPFileSize:			Int(infoRec.fileSize),
		kPPSignature:			OSTypeToString(infoRec.signature, useHexIfInvalid: ()),
		kPPTotalTracks:			Int(infoRec.totalTracks),
		kPPTotalInstruments:	Int(infoRec.totalInstruments),
		kPPInternalFileName:	String(CString: aArray, encoding: String.Encoding.macOSRoman) ?? "",
		kPPFormatDescription:	String(CString: bArray, encoding: String.Encoding.macOSRoman) ?? ""]
}

/// Class that represents the additional tracker types that PlayerPRO can load via plug-ins.
public final class PPLibrary: NSObject, Collection, NSFastEnumeration {
	internal let trackerLibs: [PPLibraryObject]
	private(set) internal var theLibrary: UnsafeMutablePointer<MADLibrary>? = nil
	public struct MusicFileInfo: CustomStringConvertible {
		///The total amount of patterns
		public var totalPatterns: Int
		
		///The length of a partition
		public var partitionLength: Int
		
		///The size of the file
		public var fileSize: Int
		
		///The total number of tracks
		public var totalTracks: Int
		
		///The total number of instruments
		public var totalInstruments: Int
		
		///The title of the tracker file
		public var internalFileName: String
		
		///A simple description of the format
		public var formatDescription: String
		
		///The signature of the file.
		///
		///This may differ from the plug-in's type.
		public var signature: String
		
		///Initializes a `MusicFileInfo` from PlayerPROCore's `MADInfoRec`.
		public init(infoRec: MADInfoRec) {
			let aArray: [Int8] = try! arrayFromObject(reflecting: infoRec.internalFileName, appendLastObject: 0)
			let bArray: [Int8] = try! arrayFromObject(reflecting: infoRec.formatDescription, appendLastObject: 0)
			
			totalPatterns = Int(infoRec.totalPatterns)
			partitionLength = Int(infoRec.partitionLength)
			fileSize = Int(infoRec.fileSize)
			totalTracks = Int(infoRec.totalTracks)
			totalInstruments = Int(infoRec.totalInstruments)
			signature = OSTypeToString(infoRec.signature, useHexIfInvalid: ())
			internalFileName = String(cString: aArray, encoding: String.Encoding.macOSRoman) ?? ""
			formatDescription = String(cString: bArray, encoding: String.Encoding.macOSRoman) ?? ""
		}
		
		private init(infoDict: NSDictionary) {
			totalPatterns = infoDict[kPPTotalPatterns] as! Int
			partitionLength = infoDict[kPPPartitionLength] as! Int
			fileSize = infoDict[kPPFileSize] as! Int
			totalTracks = infoDict[kPPTotalTracks] as! Int
			totalInstruments = infoDict[kPPTotalInstruments] as! Int
			internalFileName = infoDict[kPPInternalFileName] as! String
			formatDescription = infoDict[kPPFormatDescription] as! String
			signature = infoDict[kPPSignature] as! String
		}
		
		public var description: String {
			return "patterns: \(totalPatterns), partition length: \(partitionLength), size: \(fileSize), tracks: \(totalTracks), instruments \(totalInstruments), title: \(internalFileName), format description: \(formatDescription), signature: \(signature)"
		}
	}

	///Resets the debug function called by `MADDebugStr` to the default.
	public class func deregisterDebugFunction() {
		MADRegisterDebugBlock(nil)
	}
	
	///Sets the debug function called by `MADDebugStr` to that of the passed in C function.
	///
	///- parameter newDebugFunc: The debug function to pass in. The first variable is the line number of the code the debug function was called from, the second is the file name of the function called in, the third is the developer-supplied text passed in.
	public class func registerDebugFunction(_ newDebugFunc: @convention(c) (Int16, UnsafePointer<Int8>?, UnsafePointer<Int8>?) -> Void) {
		MADRegisterDebugFunc(newDebugFunc)
	}
	
	///Sets the debug function called by `MADDebugStr` to that of the passed in block.
	///
	///- parameter newDebugFunc: The debug block to pass in. The first variable is the line number of the code the debug function was called from, the second is the file name of the function called in, the third is the developer-supplied text passed in.
	///
	///Swift functions are interchangeable with blocks: use this method to set the debug catcher in Swift code.
	public class func registerDebugBlock(_ newDebugFunc: @convention(block) (Int16, UnsafePointer<Int8>?, UnsafePointer<Int8>?) -> Void) {
		MADRegisterDebugBlock(newDebugFunc)
	}

	public func makeIterator() -> AnyIterator<PPLibraryObject> {
		var index = 0
		return AnyIterator {
			if index < self.trackerLibs.count {
				let idx = self[index]
				index += 1
				return idx
			}
			return nil
		}
	}
	
	@nonobjc public var startIndex: Int {
		return 0
	}
	
	@nonobjc public var endIndex: Int {
		return trackerLibs.count
	}

	public subscript(index: Int) -> PPLibraryObject {
		return trackerLibs[index]
	}
	
	@nonobjc public func index(after: Int) -> Int {
		return after + 1
	}
	
	#if false
	public override convenience init?() {
		self.init(plugInCPath: nil)
	}
	
	#else
	///Init a PPLibrary object without a specific plug-in directory
	///
	///- parameter ignore: unused, needed for Swift-ObjC interop.
	@objc(init:) public convenience init?(_ ignore: Bool = false) {
		self.init(plugInCPath: nil)
	}
	#endif
	
	private init?(plugInCPath cPath: UnsafePointer<Int8>?) {
		if MADInitLibrary(cPath, &theLibrary) != .noErr {
			trackerLibs = []
			super.init()
			return nil
		}
		var tmpArray = [PPLibraryObject]()

		for i in 0 ..< Int(theLibrary!.pointee.TotalPlug) {
			let tmp = PPLibraryObject(plugInfo: theLibrary!.pointee.ThePlug[i])
			tmpArray.append(tmp)
		}
		
		trackerLibs = tmpArray

		super.init()
	}
	
	deinit {
		if theLibrary != nil {
			MADDisposeLibrary(theLibrary)
		}
	}

	///Init a PPLibrary object, including plug-ins from `plugInPath`.
	///
	///- parameter path: The path to a directory that has additional plug-ins.
	public convenience init?(plugInPath path: String) {
		self.init(plugInCPath: (path as NSString).fileSystemRepresentation)
	}
	
	///Init a PPLibrary object, including plug-ins from `plugInURL`.
	///
	///- parameter URL: The file URL to a directory that has additional plug-ins.
	public convenience init?(plugInURL URL: NSURL) {
		self.init(plugInCPath: URL.fileSystemRepresentation)
	}
	
	///The amount of plug-ins registered by the library.
	public var pluginCount: Int {
		return trackerLibs.count
	}
	
	///Attempts to identify the file at the URL passed to it.
	///
	///- parameter apath: The tracker at the file URL to identify.
	///- returns: The identified format of the file, represented as a string.
	///- throws: A `MADErr` wrapped in an `NSError`.
	public func identifyFile(URL apath: NSURL) throws -> String {
		var cType = [Int8](repeating: 0, count: 5)
		
		let aRet = MADMusicIdentifyCFURL(theLibrary, &cType, apath)
		let sRet = String(cString: cType, encoding: String.Encoding.macOSRoman)
		
		if aRet == .noErr {
			return sRet!
		} else {
			throw aRet
		}
	}
	
	///Attempts to identify the file passed to it.
	///
	///- parameter path: The tracker at a POSIX-style path to identify
	///- returns: The identified format of the file, represented as a string.
	///- throws: A `MADErr` wrapped in an `NSError`.
	public func identifyFile(path: String) throws -> String {
		let anURL = NSURL(fileURLWithPath: path)
		return try identifyFile(URL: anURL)
	}
	
	///Attempts to identify the tracker at the specified URL.
	///
	///- parameter apath: The tracker at the file URL to identify.
	///- parameter type: A pointer to an `NSString` object. On return and if successful, set to the tracker type the file is.
	///- returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `identifyFile(URL:)` instead.
	@objc(identifyFileAtURL:stringType:) public func identifyFile(URL apath: NSURL, type: AutoreleasingUnsafeMutablePointer<NSString>?) -> MADErr {
		guard let type = type else {
			return .parametersErr
		}
		
		do {
			type.pointee = try identifyFile(URL: apath)
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		return .unknownErr
	}
	
	///Attempts to identify the tracker at the specified path.
	///
	///- parameter apath: The tracker file to identify.
	///- parameter type: A pointer to an `NSString` object. On return and if successful, set to the tracker type the file is.
	///- returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `identifyFile(path:)` instead.
	@objc(identifyFileAtPath:stringType:) public func identifyFile(path apath: String, type: AutoreleasingUnsafeMutablePointer<NSString>?) -> MADErr {
		guard let type = type else {
			return .parametersErr
		}
		
		do {
			type.pointee = try identifyFile(path: apath)
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		return .unknownErr
	}
	
	private func informationFromFile(URL: NSURL, cType: [Int8]) throws -> MADInfoRec {
		var cStrType = cType
		var infoRec = MADInfoRec()

		let anErr = MADMusicInfoCFURL(theLibrary, &cStrType, URL, &infoRec)
		
		try anErr.throwIfNotNoErr()
		
		return infoRec
	}
	
	/// Gathers information about a tracker at the specified URL.
	///
	/// - parameter apath: A file URL pointing to the tracker.
	/// - parameter type: The tracker type of the file.
	/// - returns: A `MusicFileInfo` struct describing the file pointed to in `URL`.
	/// - throws: A `MADErr` wrapped in an `NSError`.
	public func informationFromFile(URL apath: NSURL, type: String) throws -> MusicFileInfo {
		guard let cStrType = type.cString(using: String.Encoding.macOSRoman) else {
			throw MADErr.parametersErr
		}
		
		let filInfo = try informationFromFile(URL: apath, cType: cStrType)
		let anInfo = MusicFileInfo(infoRec: filInfo)
		return anInfo
	}
	
	/// Gathers information about a tracker at the specified path.
	///
	/// - parameter path: The path pointing to the tracker.
	/// - parameter type: The tracker type of the file.
	/// - returns: A `MusicFileInfo` struct describing the file pointed to in `path`.
	/// - throws: A `MADErr` wrapped in an `NSError`.
	public func informationFromFile(path: String, type: String) throws -> MusicFileInfo {
		let anURL = NSURL(fileURLWithPath: path)
		return try informationFromFile(URL: anURL, type: type)
	}
	
	/// Gathers information about a tracker at the specified URL.
	///
	/// - parameter path: The file URL pointing to the tracker.
	/// - parameter type: The tracker type of the file.
	/// - parameter info: A pointer to an NSDictionary. On return and success, the dictionary is populated with info about the tracker.
	/// - returns: An error value, or `MADNoErr` on success.
	///
	/// This is mainly for Objective-C code. For Swift code, use `informationFromFile(URL:type:)` instead.
	@objc(getInformationFromFileAtURL:stringType:info:) public func getInformationFromFile(URL path: NSURL, type: String, info: AutoreleasingUnsafeMutablePointer<NSDictionary>?) -> MADErr {
		guard let info = info else {
			return .parametersErr
		}
		guard let cStrType = type.cString(using: String.Encoding.macOSRoman) else {
			return .parametersErr
		}
		
		do {
			let aRet = try informationFromFile(URL: path, cType: cStrType)
			let tmpDict = toDictionary(infoRec: aRet)
			info.pointee = tmpDict
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		
		return .unknownErr
	}
	
	/// Gathers information about a tracker at the specified path.
	///
	/// - parameter path: The path pointing to the tracker.
	/// - parameter type: The tracker type of the file.
	/// - parameter info: A pointer to an NSDictionary. On return and success, the dictionary is populated with info about the tracker.
	/// - returns: An error value, or `MADNoErr` on success.
	///
	/// This is mainly for Objective C code. For Swift code, use `informationFromFile(path:type:)` instead.
	@objc(getInformationFromFileAtPath:stringType:info:) public func getInformationFromFile(path: String, type: String, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		let anURL = NSURL(fileURLWithPath: path)
		return getInformationFromFile(URL: anURL, type: type, info: info)
	}
	
	/// Test the tracker at the file URL is actually of type `type`.
	///
	/// - parameter URL: The file URL of the tracker to test.
	/// - parameter type: The type to test for.
	/// - returns: An error value, or `MADNoErr` if the tracker is of the specified type.
	@objc(testFileAtURL:stringType:) public func testFile(URL: NSURL, type: String) -> MADErr {
		guard var cStrType = type.cString(using: String.Encoding.macOSRoman) else {
			return .parametersErr
		}
		
		return MADMusicTestCFURL(theLibrary, &cStrType, URL)
	}
	
	/// Test the tracker at the path is actually of type `type`.
	///
	/// - parameter path: The path of the tracker to test.
	/// - parameter type: The type to test for.
	/// - returns: An error value, or `MADNoErr` if the tracker is of the specified type.
	@objc(testFileAtPath:stringType:) public func testFile(path: String, type: String) -> MADErr {
		let URL = NSURL(fileURLWithPath: path)
		return testFile(URL: URL, type: type)
	}
	
	/// Gets a plug-in type from a UTI
	///
	/// - parameter aUTI: The UTI to find a plug-in type for.
	/// - returns: A plug-in type, four characters long, or `nil` if there's no plug-in that opens the UTI.
	public func typeFromUTI(_ aUTI: String) -> String? {
		if aUTI == kPlayerPROMADKUTI {
			return MadIDString
		}
		
		for obj in trackerLibs {
			for bUTI in obj.UTITypes {
				if aUTI == bUTI {
					return obj.type
				}
			}
		}
		
		return nil
	}
	
	/// Gets the first UTI from a plug-in type.
	///
	/// - parameter aType: the four-character plug-in type to get a UTI for.
	/// - returns: The first UTI in the plug-in's UTI list that corrisponds to the type, or `nil` if the type isn't listed.
	public func typeToUTI(_ aType: String) -> String? {
		if aType == MadIDString {
			return kPlayerPROMADKUTI
		}
		
		for obj in trackerLibs {
			if aType == obj.type {
				return obj.UTITypes.first!
			}
		}
		
		return nil
	}
	
	/// NSFastEnumeration protocol method.
	public func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>!, count len: Int) -> Int {
		return (trackerLibs as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
}

///Deprecated functions
extension PPLibrary {
	///Deprecated: do not use
	@available(*, deprecated, message:"Use -getInformationFromFileAtPath:stringType:info: (Obj-C) or informationFromFile(path:type:) (Swift) instead")
	@objc(getInformationFromFileAtPath:type:info:) public func getInformationFromFile(path: String, type: UnsafeMutablePointer<Int8>, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		let anURL = NSURL(fileURLWithPath: path)
			return getInformationFromFile(URL: anURL, type: type, info: info)
	}

	///Deprecated: do not use
	@available(*, deprecated, message:"Use -getInformationFromFileAtURL:stringType:info: (Obj-C) or informationFromFile(URL:type:) (Swift) instead")
	@objc(getInformationFromFileAtURL:type:info:) public func getInformationFromFile(URL path: NSURL, type: UnsafeMutablePointer<Int8>, info: AutoreleasingUnsafeMutablePointer<NSDictionary>?) -> MADErr {
		guard let info = info else {
			return .parametersErr
		}
		let sLen = strnlen(type, 4)
		assert(sLen != 4, "Even if it's less than four chars long, the rest should be padded")
		
		var cStrType: [Int8] = [0x20, 0x20, 0x20, 0x20, 0]
		for i in 0..<Int(sLen) {
			cStrType[i] = type[i]
		}
		
		do {
			let aRet = try informationFromFile(URL: path, cType: cStrType)
			let tmpDict = toDictionary(infoRec: aRet)
			info.pointee = tmpDict
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		return .unknownErr
	}
	
	///Deprecated: do not use
	@available(*, deprecated, message:"Use -identifyFileAtURL:stringType: (Obj-C) or identifyFile(URL:) (Swift) instead")
	@objc(identifyFileAtURL:type:) public func identifyFile(URL apath: NSURL, type: UnsafeMutablePointer<Int8>?) -> MADErr {
		if type == nil {
			return .parametersErr
		}
		
		do {
			let aRet = try identifyFile(URL: apath)
			guard let typeStr = aRet.cString(using: String.Encoding.macOSRoman) else {
				throw MADErr.parametersErr
			}
			strncpy(type, typeStr, 4)
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		return .unknownErr
	}
	
	///Deprecated: do not use
	@available(*, deprecated, message:"Use -identifyFileAtPath:stringType: (Obj-C) or identifyFile(path:) (Swift) instead")
	@objc(identifyFileAtPath:type:) public func identifyFile(path apath: String, type: UnsafeMutablePointer<Int8>?) -> MADErr {
		guard let type = type else {
			return .parametersErr
		}
		
		do {
			let aStr = try identifyFile(path: apath)
			guard let typeStr = aStr.cString(using: String.Encoding.macOSRoman) else {
				throw MADErr.parametersErr
			}
			strncpy(type, typeStr, 4)
			return .noErr
		} catch let anErr as NSError {
			if anErr.domain == PPMADErrorDomain {
				return MADErr(rawValue: Int16(anErr.code)) ?? .unknownErr
			}
		}
		
		return .unknownErr
	}
}
