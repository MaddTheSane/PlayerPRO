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

extension MADInfoRec {
	private init() {
		totalPatterns = 0
		partitionLength = 0
		fileSize = 0
		signature = "!!!!"
		
		totalTracks = 0
		totalInstruments = 0
		
		internalFileName = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
		formatDescription = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
	}
}

private func infoRecToDictionary(infoRec: MADInfoRec) -> NSDictionary {
	let aArray: [Int8] = getArrayFromMirror(reflect(infoRec.internalFileName), appendLastObject: 0)
	let bArray: [Int8] = getArrayFromMirror(reflect(infoRec.formatDescription), appendLastObject: 0)
	
	return [kPPTotalPatterns:	Int(infoRec.totalPatterns),
		kPPPartitionLength:		Int(infoRec.partitionLength),
		kPPFileSize:			Int(infoRec.fileSize),
		kPPSignature:			OSTypeToString(infoRec.signature, useHexIfInvalid: ()),
		kPPTotalTracks:			Int(infoRec.totalTracks),
		kPPTotalInstruments:	Int(infoRec.totalInstruments),
		kPPInternalFileName:	String(CString: aArray, encoding: NSMacOSRomanStringEncoding) ?? "",
		kPPFormatDescription:	String(CString: bArray, encoding: NSMacOSRomanStringEncoding) ?? ""]
}

public final class PPLibrary: NSObject, CollectionType, NSFastEnumeration {
	internal let trackerLibs: [PPLibraryObject]
	internal var theLibrary: UnsafeMutablePointer<MADLibrary> = nil
	public struct MusicFileInfo: Printable {
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
			let aArray: [Int8] = getArrayFromMirror(reflect(infoRec.internalFileName), appendLastObject: 0)
			let bArray: [Int8] = getArrayFromMirror(reflect(infoRec.formatDescription), appendLastObject: 0)
			
			totalPatterns = Int(infoRec.totalPatterns)
			partitionLength = Int(infoRec.partitionLength)
			fileSize = Int(infoRec.fileSize)
			totalTracks = Int(infoRec.totalTracks)
			totalInstruments = Int(infoRec.totalInstruments)
			signature = OSTypeToString(infoRec.signature, useHexIfInvalid: ())
			internalFileName = String(CString: aArray, encoding: NSMacOSRomanStringEncoding) ?? ""
			formatDescription = String(CString: bArray, encoding: NSMacOSRomanStringEncoding) ?? ""
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
	///:param: newDebugFunc The debug function to pass in. The first variable is the line number of the code the debug function was called from, the second is the file name of the function called in, the third is the developer-supplied text passed in.
	///
	///Swift functions can't be translated to `CFunctionPointer`s, so use `registerDebugBlock(_:)`, below, if you want to set it via Swift.
	@objc public class func registerDebugFunction(newDebugFunc: CFunctionPointer<((Int16, UnsafePointer<Int8>, UnsafePointer<Int8>) -> Void)>) {
		MADRegisterDebugFunc(newDebugFunc)
	}
	
	///Sets the debug function called by `MADDebugStr` to that of the passed in block.
	///
	///:param: newDebugFunc The debug block to pass in. The first variable is the line number of the code the debug function was called from, the second is the file name of the function called in, the third is the developer-supplied text passed in.
	///
	///Swift functions are interchangeable with blocks: use this method to set the debug catcher in Swift code.
	public class func registerDebugBlock(newDebugFunc: (Int16, UnsafePointer<Int8>, UnsafePointer<Int8>) -> Void) {
		MADRegisterDebugBlock(newDebugFunc)
	}

	typealias Index = Int
	typealias Generator = GeneratorOf<PPLibraryObject>
	
	public func generate() -> GeneratorOf<PPLibraryObject> {
		var index = 0
		return GeneratorOf {
			if index < self.trackerLibs.count {
				return self[index++]
			}
			return nil
		}
	}
	
	public var startIndex: Int {
		return 0
	}
	
	public var endIndex: Int {
		return trackerLibs.count
	}

	public subscript(index: Int) -> PPLibraryObject {
		return trackerLibs[index]
	}
	
	#if false
	public override convenience init?() {
		self.init(plugInCPath: nil)
	}
	
	#else
	///Init a PPLibrary object without a specific plug-in directory
	///
	///:param: ignore unused, needed for Swift-ObjC interaction.
	@objc(init:) public convenience init?(_ ignore: Bool = false) {
		self.init(plugInCPath: nil)
	}
	#endif
	
	private init?(plugInCPath cPath: UnsafePointer<Int8>) {
		if MADInitLibrary(cPath, &theLibrary) != .NoErr {
			trackerLibs = []
			super.init()
			return nil
		}
		var tmpArray = [PPLibraryObject]()

		for i in 0 ..< Int(theLibrary.memory.TotalPlug) {
			let tmp = PPLibraryObject(plugInfo: theLibrary.memory.ThePlug[i])
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
	///:param: path The path to a directory that has additional plug-ins.
	public convenience init?(plugInPath path: String) {
		self.init(plugInCPath: path.fileSystemRepresentation())
	}
	
	///Init a PPLibrary object, including plug-ins from `plugInURL`.
	///
	///:param: URL The file URL to a directory that has additional plug-ins.
	public convenience init?(plugInURL URL: NSURL) {
		self.init(plugInCPath: URL.fileSystemRepresentation)
	}
	
	///The amount of plug-ins registered by the library.
	public var pluginCount: Int {
		return trackerLibs.count
	}
	
	///Attempts to identify the file at the URL passed to it.
	///
	///:param: apath The tracker at the file URL to identify.
	///:returns: A tuple with an optional string identifying the format, and an error value. If the error is `.NoErr`, then format should be non-nil.
	public func identifyFile(URL apath: NSURL) -> (format: String?, error: MADErr) {
		var cType = [Int8](count: 5, repeatedValue: 0)
		
		let aRet = MADMusicIdentifyCFURL(theLibrary, &cType, apath)
		let sRet = String(CString: cType, encoding: NSMacOSRomanStringEncoding)
		
		return (sRet, aRet)
	}
	
	///Attempts to identify the file passed to it.
	///
	///:param: path The tracker at a POSIX-style path to identify
	///:returns: A tuple with an optional string identifying the format, and an error value. If the error is `.NoErr`, then format should be non-nil.
	public func identifyFile(#path: String) -> (format: String?, error: MADErr) {
		if let anURL = NSURL(fileURLWithPath: path) {
			return identifyFile(URL: anURL)
		} else {
			return (nil, .ReadingErr)
		}
	}
	
	///Attempts to identify the tracker at the specified URL.
	///
	///:param: apath The tracker at the file URL to identify.
	///:param: type A pointer to an `NSString` object. On return and if successful, set to the tracker type the file is.
	///:returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `identifyFile(URL:)` instead.
	@objc(identifyFileAtURL:stringType:) public func identifyFile(URL apath: NSURL, type: AutoreleasingUnsafeMutablePointer<NSString>) -> MADErr {
		if type == nil {
			return .ParametersErr
		}
		
		let aRet = identifyFile(URL: apath)
		if aRet.error == .NoErr {
			type.memory = aRet.format!
		}
		
		return aRet.error
	}
	
	///Attempts to identify the tracker at the specified path.
	///
	///:param: apath The tracker file to identify.
	///:param: type A pointer to an `NSString` object. On return and if successful, set to the tracker type the file is.
	///:returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `identifyFile(path:)` instead.
	@objc(identifyFileAtPath:stringType:) public func identifyFile(path apath: String, type: AutoreleasingUnsafeMutablePointer<NSString>) -> MADErr {
		if type == nil {
			return .ParametersErr
		}
		
		let aRet = identifyFile(path: apath)
		if aRet.error == .NoErr {
			type.memory = aRet.format!
		}
		
		return aRet.error
	}
	
	private func informationFromFile(#URL: NSURL, cType: [Int8]) -> (info: MADInfoRec, error: MADErr) {
		var cStrType = cType
		var infoRec = MADInfoRec()

		let anErr = MADMusicInfoCFURL(theLibrary, &cStrType, URL, &infoRec)
		
		return (infoRec, anErr)
	}
	
	///Gathers information about a tracker at the specified URL.
	///
	///:param: apath A file URL pointing to the tracker.
	///:param: type The tracker type of the file.
	///:returns: A tuple with an optional `MusicFileInfo` type and an error value. If `error` is `.NoErr`, the `info` value will not be `nil`.
	public func informationFromFile(URL apath: NSURL, type: String) -> (info: MusicFileInfo?, error: MADErr) {
		let cStrType = type.cStringUsingEncoding(NSMacOSRomanStringEncoding)!
		
		let filInfo = informationFromFile(URL: apath, cType: cStrType)
		
		if filInfo.error == .NoErr {
			let anInfo = MusicFileInfo(infoRec: filInfo.info)
			return (anInfo, filInfo.error)
		} else {
			return (nil, filInfo.error)
		}
	}
	
	///Gathers information about a tracker at the specified path.
	///
	///:param: path The path pointing to the tracker.
	///:param: type The tracker type of the file.
	///:returns: A tuple with an optional `MusicFileInfo` type and an error value. If `error` is `.NoErr`, the `info` value will not be `nil`.
	public func informationFromFile(#path: String, type: String) -> (info: MusicFileInfo?, error: MADErr) {
		if let anURL = NSURL(fileURLWithPath: path) {
			return informationFromFile(URL: anURL, type: type)
		} else {
			return (nil, .ReadingErr)
		}
	}
	
	///Gathers information about a tracker at the specified URL.
	///
	///:param: path The file URL pointing to the tracker.
	///:param: type The tracker type of the file.
	///:param: info A pointer to an NSDictionary. On return and success, the dictionary is populated with info about the tracker.
	///:returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `informationFromFile(URL:type:)` instead.
	@objc(getInformationFromFileAtURL:stringType:info:) public func getInformationFromFile(URL path: NSURL, type: String, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		if info == nil {
			return .ParametersErr
		}
		let cStrType = type.cStringUsingEncoding(NSMacOSRomanStringEncoding)!
		
		let aRet = informationFromFile(URL: path, cType: cStrType)
		
		if aRet.error == .NoErr {
			let tmpDict = infoRecToDictionary(aRet.info)
			
			info.memory = tmpDict
		}
		
		return aRet.error
	}
	
	///Gathers information about a tracker at the specified path.
	///
	///:param: path The path pointing to the tracker.
	///:param: type The tracker type of the file.
	///:param: info A pointer to an NSDictionary. On return and success, the dictionary is populated with info about the tracker.
	///:returns: An error value, or `MADNoErr` on success.
	///
	///This is mainly for Objective C code. For Swift code, use `informationFromFile(path:type:)` instead.
	@objc(getInformationFromFileAtPath:stringType:info:) public func getInformationFromFile(#path: String, type: String, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		if let anURL = NSURL(fileURLWithPath: path) {
			return getInformationFromFile(URL: anURL, type: type, info: info)
		} else {
			return .ReadingErr
		}
	}
	
	///Test the tracker at the file URL is actually of type `type`.
	///
	///:param: URL The file URL of the tracker to test.
	///:param: type The type to test for.
	///:returns: An error value, or `MADNoErr` if the tracker is of the specified type.
	@objc(testFileAtURL:stringType:) public func testFile(#URL: NSURL, type: String) -> MADErr {
		var cStrType = type.cStringUsingEncoding(NSMacOSRomanStringEncoding)!
		
		return MADMusicTestCFURL(theLibrary, &cStrType, URL)
	}
	
	///Test the tracker at the path is actually of type `type`.
	///
	///:param: path The path of the tracker to test.
	///:param: type The type to test for.
	///:returns: An error value, or `MADNoErr` if the tracker is of the specified type.
	@objc(testFileAtPath:stringType:) public func testFile(#path: String, type: String) -> MADErr {
		if let URL = NSURL(fileURLWithPath: path) {
			return testFile(URL: URL, type: type)
		} else {
			return .ReadingErr
		}
	}
	
	///Gets a plug-in type from a UTI
	///
	///:param: aUTI The UTI to find a plug-in type for.
	///:returns: A plug-in type, four characters long, or `nil` if there's no plug-in that opens the UTI.
	public func typeFromUTI(aUTI: String) -> String? {
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
	
	///Gets the first UTI from a plug-in type.
	///
	///:param: aType the four-character plug-in type to get a UTI for.
	///:returns: The first UTI in the plug-in's UTI list that corrisponds to the type, or `nil` if the type isn't listed.
	public func typeToUTI(aType: String) -> String? {
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
	
	public func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (trackerLibs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}

///Deprecated functions
extension PPLibrary {
	///Deprecated: do not use
	@objc(getInformationFromFileAtPath:type:info:) public func getInformationFromFile(#path: String, type: UnsafeMutablePointer<Int8>, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		if let anURL = NSURL(fileURLWithPath: path) {
			return getInformationFromFile(URL: anURL, type: type, info: info)
		} else {
			return .ReadingErr
		}
	}

	///Deprecated: do not use
	@objc(getInformationFromFileAtURL:type:info:) public func getInformationFromFile(URL path: NSURL, type: UnsafeMutablePointer<Int8>, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		if info == nil {
			return .ParametersErr
		}
		let sLen = strnlen(type, 4)
		assert(sLen != 4, "Even if it's less than four chars long, the rest should be padded")
		
		#if false
		var cStrType = [Int8](count: 4, repeatedValue: 0x20)
		cStrType.append(0)
			#else
			var cStrType: [Int8] = [0x20, 0x20, 0x20, 0x20, 0]
			#endif
		for i in 0..<Int(sLen) {
			cStrType[i] = type[i]
		}
		
		let aRet = informationFromFile(URL: path, cType: cStrType)
		
		if aRet.error == .NoErr {
			let tmpDict = infoRecToDictionary(aRet.info)
			
			info.memory = tmpDict
		}
		
		return aRet.error
	}
	
	///Deprecated: do not use
	@objc(identifyFileAtURL:type:) public func identifyFile(URL apath: NSURL, type: UnsafeMutablePointer<Int8>) -> MADErr {
		if type == nil {
			return .ParametersErr
		}
		
		let aRet = identifyFile(URL: apath)
		if aRet.error == .NoErr {
			strncpy(type, aRet.format!.cStringUsingEncoding(NSMacOSRomanStringEncoding)!, 4)
		}
		
		return aRet.error
	}
	
	///Deprecated: do not use
	@objc(identifyFileAtPath:type:) public func identifyFile(path apath: String, type: UnsafeMutablePointer<Int8>) -> MADErr {
		if type == nil {
			return .ParametersErr
		}
		
		let aRet = identifyFile(path: apath)
		if aRet.error == .NoErr {
			strncpy(type, aRet.format!.cStringUsingEncoding(NSMacOSRomanStringEncoding)!, 4)
		}
		
		return aRet.error
	}

}
