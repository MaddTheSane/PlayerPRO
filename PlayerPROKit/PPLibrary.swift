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
	init() {
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


public func infoRecToMusicInfo(infoRec: MADInfoRec) -> PPLibrary.MusicFileInfo {
	let tmpDict = infoRecToDictionary(infoRec)
	
	return PPLibrary.MusicFileInfo(infoDict: tmpDict)
}

public class PPLibrary: NSObject, CollectionType, NSFastEnumeration {
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
		
		private init(infoDict: NSDictionary) {
			totalPatterns = infoDict[kPPTotalPatterns] as Int
			partitionLength = infoDict[kPPPartitionLength] as Int
			fileSize = infoDict[kPPFileSize] as Int
			totalTracks = infoDict[kPPTotalTracks] as Int
			totalInstruments = infoDict[kPPTotalInstruments] as Int
			internalFileName = infoDict[kPPInternalFileName] as String
			formatDescription = infoDict[kPPFormatDescription] as String
			signature = infoDict[kPPSignature] as String
		}
		
		public var description: String {
			return "patterns: \(totalPatterns), partition length: \(partitionLength), size: \(fileSize), tracks: \(totalTracks), instruments \(totalInstruments), title: \(internalFileName), format description: \(formatDescription), signature: \(signature)"
		}
	}

	public class func deregisterDebugFunction() {
		MADRegisterDebugFunc(nil)
	}
	
	@objc public class func registerDebugFunction(newDebugFunc: CFunctionPointer<((Int16, UnsafePointer<Int8>, UnsafePointer<Int8>) -> Void)>) {
		MADRegisterDebugFunc(newDebugFunc)
	}
	
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

	public convenience init?(plugInPath path: String) {
		self.init(plugInCPath: path.fileSystemRepresentation())
	}
	
	public convenience init?(plugInURL URL: NSURL) {
		self.init(plugInCPath: URL.fileSystemRepresentation)
	}
	
	
	public var pluginCount: Int {
		return trackerLibs.count
	}
	
	public func identifyFile(URL apath: NSURL) -> (format: String?, error: MADErr) {
		var cType = [Int8](count: 5, repeatedValue: 0)
		
		let aRet = MADMusicIdentifyCFURL(theLibrary, &cType, apath)
		let sRet = String(CString: cType, encoding: NSMacOSRomanStringEncoding)
		
		return (sRet, aRet)
	}
	
	public func identifyFile(#path: String) -> (format: String?, error: MADErr) {
		if let anURL = NSURL(fileURLWithPath: path) {
			return identifyFile(URL: anURL)
		} else {
			return (nil, .ReadingErr)
		}
	}
	
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
	
	public func informationFromFile(URL apath: NSURL, type: String) -> (info: MusicFileInfo?, error: MADErr) {
		let cStrType = type.cStringUsingEncoding(NSMacOSRomanStringEncoding)!
		
		let filInfo = informationFromFile(URL: apath, cType: cStrType)
		
		if filInfo.error == .NoErr {
			let anInfo = infoRecToMusicInfo(filInfo.info)
			return (anInfo, filInfo.error)
		} else {
			return (nil, filInfo.error)
		}
	}
	
	public func informationFromFile(#path: String, type: String) -> (info: MusicFileInfo?, error: MADErr) {
		if let anURL = NSURL(fileURLWithPath: path) {
			return informationFromFile(URL: anURL, type: type)
		} else {
			return (nil, .ReadingErr)
		}
	}
	
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
	
	@objc(getInformationFromFileAtPath:stringType:info:) public func getInformationFromFile(#path: String, type: String, info: AutoreleasingUnsafeMutablePointer<NSDictionary>) -> MADErr {
		if let anURL = NSURL(fileURLWithPath: path) {
			return getInformationFromFile(URL: anURL, type: type, info: info)
		} else {
			return .ReadingErr
		}
	}
	
/**
Gets a plug-in type from a UTI

:param: aUTI The UTI to find a plug-in type for.
:returns: A plug-in type, four characters long, or `nil` if there's no plug-in that opens the UTI
 */
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
	
/**
Gets the first UTI from a plug-in type.

:param: aType the four-character plug-in type to get a UTI for.
:returns: a UTI, or `nil` if the type isn't listed
 */
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
